/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWDataSelectorWidget.cxx 1 2007-11-21 09:03:23Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-11-21 10:03:23 +0100 (Wed, 21 Nov 2007) $
Version:   $Revision: 1 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <vtkKWDataSelectorWidget.h>
#include "vtkObjectFactory.h"

#include <vtkKWPushButton.h>
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWCheckButton.h>
#include <vtkKWFrame.h>
#include <vtkDataManager.h>
#include <vtkMetaDataSet.h>

#include <pixmap/KWAddOnResources.h>
#include <vtkKWMultiColumnList.h>
#include <vtkKWMultiColumnListWithScrollbars.h>
#include <vtkKWScrollbar.h>
#include <vtkKWIcon.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWDataSelectorWidget );
vtkCxxRevisionMacro( vtkKWDataSelectorWidget, "$Revision: 1 $");

//----------------------------------------------------------------------------
vtkKWDataSelectorWidget::vtkKWDataSelectorWidget()
{
  this->MultiColumnList = NULL;
  this->DataManager = NULL;
  this->SelectAllCheckbox = NULL;

  for (unsigned long i=0; i<vtkMetaDataSet::VTK_META_DATATYPE_NUMBER; i++)
    this->SetAllowDataType (i, true);

  this->SingleSelectionOff();
  
  this->Initialize();
}

//----------------------------------------------------------------------------
vtkKWDataSelectorWidget::~vtkKWDataSelectorWidget()
{
  
}


//----------------------------------------------------------------------------
void vtkKWDataSelectorWidget::CreateWidget()
{
  // Check if already created
  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();
  
  this->SetBorderWidth(2);
  this->SetReliefToFlat();


  vtkKWCheckButtonWithLabel* selectallcheckbox = vtkKWCheckButtonWithLabel::New();
  selectallcheckbox->SetParent (this);
  selectallcheckbox->Create();
  selectallcheckbox->SetLabelText ("Select All");
  selectallcheckbox->SetLabelPositionToRight();
  
  this->SelectAllCheckbox = selectallcheckbox->GetWidget();
  this->SelectAllCheckbox->Deselect();
  this->SelectAllCheckbox->SetCommand(this, "SelectAllCallback");  

  this->Script( "pack %s -side top -expand false -anchor nw -fill none -padx 5 -pady 5", 
		selectallcheckbox->GetWidgetName());

  selectallcheckbox->Delete();
  
  vtkKWMultiColumnListWithScrollbars* listframe = vtkKWMultiColumnListWithScrollbars::New();
  listframe->SetParent (this);
  listframe->Create();
  listframe->VerticalScrollbarVisibilityOn();
  listframe->HorizontalScrollbarVisibilityOff();
  listframe->GetVerticalScrollbar()->SetWidth(13);
  
  this->MultiColumnList = listframe->GetWidget();
  this->MultiColumnList->MovableColumnsOff();
  this->MultiColumnList->SetWidth(60);

  this->MultiColumnList->SetSelectionBackgroundColor(1,0.9647,0.6314);  
  this->MultiColumnList->SetSelectionForegroundColor (0,0,0);
  

  int col_index;
  
  // Add the columns (make some of them editable)

  col_index = this->MultiColumnList->AddColumn("Selected");
  this->MultiColumnList->SetColumnEditable(col_index, 0);
  this->MultiColumnList->SetColumnFormatCommandToEmptyOutput(col_index);
  this->MultiColumnList->SetColumnAlignmentToCenter(col_index);  
  
  col_index = this->MultiColumnList->AddColumn("Name");
  this->MultiColumnList->SetColumnEditable(col_index, 0);
  this->MultiColumnList->SetColumnWidth(col_index, 28);

  col_index = this->MultiColumnList->AddColumn("Type");
  this->MultiColumnList->SetColumnEditable(col_index, 0);
  this->MultiColumnList->SetColumnAlignmentToCenter(col_index);
  
  this->MultiColumnList->SetSelectionChangedCommand (this, "SelectionChangedCallback");
  this->MultiColumnList->SetCellUpdatedCommand (this, "CellUpdateCallback");
  this->MultiColumnList->SetUneditableCellDoubleClickCommand(this, "DoubleClickCallback");


  this->Script("pack %s -fill both -side top -expand t", 
	       listframe->GetWidgetName());
  
  listframe->Delete();


  

  
}


//----------------------------------------------------------------------------
void vtkKWDataSelectorWidget::Initialize()
{

  if (!this->GetDataManager() || !this->MultiColumnList)
    return;
  
  this->MultiColumnList->DeleteAllRows();
  
  std::vector<vtkMetaDataSet*> metadatasetlist = this->GetDataManager()->GetMetaDataSetList();
  bool selectall = this->SelectAllCheckbox->GetSelectedState();
  
  for (unsigned int i=0; i<metadatasetlist.size(); i++)
  {
    vtkMetaDataSet* metadataset = metadatasetlist[i];

    if (this->AllowDataTypeList[metadataset->GetType()] == true)
    {
      int rowid = this->MultiColumnList->GetNumberOfRows();
      
      this->MultiColumnList->InsertCellTextAsInt(rowid, 0, selectall);    
      this->MultiColumnList->SetCellWindowCommandToCheckButton(rowid, 0);
      
      this->MultiColumnList->InsertCellText(rowid, 1, metadataset->GetName());
      this->MultiColumnList->InsertCellText(rowid, 2, metadataset->GetDataSetType());

      
    }
    
  }
  
  
}
  

//----------------------------------------------------------------------------
std::vector<vtkMetaDataSet*> vtkKWDataSelectorWidget::GetSelection (void)
{
  std::vector<vtkMetaDataSet*> ret;

  if (!this->GetDataManager())
    return ret;
  
  for (int i=0; i<this->MultiColumnList->GetNumberOfRows(); i++)
  {
    int selected = this->MultiColumnList->GetCellTextAsInt(i, 0);
    if (!selected)
      continue;
    std::string     name = this->MultiColumnList->GetCellText (i, 1);
    vtkMetaDataSet* metadataset = this->DataManager->GetMetaDataSet (name.c_str());
    
    if (metadataset)
      ret.push_back(metadataset);
    else
      vtkWarningMacro(<<"could not find "<<name.c_str()<<" in datamanager !");
  }


  return ret;
  
  
  
}


//----------------------------------------------------------------------------
void vtkKWDataSelectorWidget::SelectAllCallback(int state)
{
  for (int i=0; i<this->MultiColumnList->GetNumberOfRows(); i++)
  {    
    this->MultiColumnList->SetCellTextAsInt(i, 0, state);
    this->MultiColumnList->SetCellWindowCommandToCheckButton(i, 0);
      
  }
}


//----------------------------------------------------------------------------
vtkMetaDataSet* vtkKWDataSelectorWidget::GetHighlightedDataSet (void)
{
  
  int row = this->MultiColumnList->GetIndexOfFirstSelectedRow();
  int name_col = 1;

  const char* cname = this->MultiColumnList->GetCellText (row, name_col);

  if (!cname || !(*cname))
    return NULL;
  
  std::string name = cname;
  
  vtkMetaDataSet* metadataset = this->GetDataManager()->GetMetaDataSet(name.c_str());
  return metadataset;
  
}



//----------------------------------------------------------------------------
void vtkKWDataSelectorWidget::SelectionChangedCallback()
{
}

//----------------------------------------------------------------------------
void vtkKWDataSelectorWidget::CellUpdateCallback(int row, int col, int state)
{
  if (col != 0)
    return;
  
  if (this->SingleSelection)
  {
    for (int i=0; i<this->MultiColumnList->GetNumberOfRows(); i++)
    {
      if ( i != row)
      {
	this->MultiColumnList->SetCellTextAsInt(i, 0, 0);
	this->MultiColumnList->SetCellWindowCommandToCheckButton(i, 0);
      }
      
    }
  }
  
}

//----------------------------------------------------------------------------
void vtkKWDataSelectorWidget::DoubleClickCallback()
{
}

  
