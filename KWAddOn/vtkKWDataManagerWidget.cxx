/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWDataManagerWidget.cxx 1302 2009-10-27 21:57:16Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-10-27 21:57:16 +0000 (Tue, 27 Oct 2009) $
Version:   $Revision: 1302 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <vtkKWDataManagerWidget.h>
#include "vtkObjectFactory.h"

#include <vtkTimerLog.h>
#include <vtkKWScale.h>
#include <vtkKWScaleSet.h>
#include <vtkKWPushButton.h>
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWCheckButton.h>
#include <vtkKWFrame.h>
#include <vtkDataManager.h>
#include <vtkMetaDataSet.h>

#include <vtkKWMainWindowInteractor.h>
#include <vtkKWPageView.h>
#include <vtkKWApplication.h>

#include <pixmap/KWAddOnResources.h>
#include <vtkKWMultiColumnListWithScrollbars.h>
#include <vtkKWMultiColumnList.h>
#include <vtkKWDialog.h>
#include <vtkKWIcon.h>
#include <vtkKWNotebook.h>
#include <vtkKWToolBox.h>
#include <vtkImageView3D.h>
#include <vtkInteractorStyle.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWDataManagerWidget );
vtkCxxRevisionMacro( vtkKWDataManagerWidget, "$Revision: 1302 $");

//----------------------------------------------------------------------------
vtkKWDataManagerWidget::vtkKWDataManagerWidget()
{
  
  this->DataSetList = 0;
  this->ParentObject = NULL;
  
  this->Update();

  this->Lock = false;
}

//----------------------------------------------------------------------------
vtkKWDataManagerWidget::~vtkKWDataManagerWidget()
{
  
  
}


//----------------------------------------------------------------------------
void vtkKWDataManagerWidget::CreateWidget()
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

  vtkKWMultiColumnListWithScrollbars* mcl = vtkKWMultiColumnListWithScrollbars::New();
  mcl->HorizontalScrollbarVisibilityOff();
  mcl->VerticalScrollbarVisibilityOn();
  
  mcl->SetParent(this);
  mcl->Create();

  this->DataSetList = mcl->GetWidget();
  
  this->DataSetList->MovableColumnsOn();
  this->DataSetList->SetWidth(60);
  this->DataSetList->SetSelectionModeToExtended ();
  this->DataSetList->SetSelectionBackgroundColor(1,0.9647,0.6314);  
  this->DataSetList->SetSelectionForegroundColor (0,0,0);
  
  int col_index;
  
  // Add the columns (make some of them editable)
  
  col_index = this->DataSetList->AddColumn("Name");
  this->DataSetList->SetColumnEditable(col_index, 0);  
  this->DataSetList->SetColumnWidth(col_index, 24);

  col_index = this->DataSetList->AddColumn(NULL);
  this->DataSetList->SetColumnEditable(col_index, 0);
  this->DataSetList->SetColumnFormatCommandToEmptyOutput(col_index);
  this->DataSetList->SetColumnLabelImageToPredefinedIcon(col_index, vtkKWIcon::IconEye);
  this->DataSetList->SetColumnAlignmentToCenter(col_index);  

  col_index = this->DataSetList->AddColumn("Type");
  this->DataSetList->SetColumnEditable(col_index, 0);
  this->DataSetList->SetColumnAlignmentToCenter(col_index);
  
  this->DataSetList->SetSelectionChangedCommand (this, "SelectionChangedCallback");
  this->DataSetList->SetCellUpdatedCommand (this, "VisibilityChangedCallback");
  this->DataSetList->SetUneditableCellDoubleClickCommand(this, "DoubleClickCallback");

   this->Script("pack %s -fill both -side top -expand t", 
	       mcl->GetWidgetName());

  mcl->Delete();
}


//----------------------------------------------------------------------------
void vtkKWDataManagerWidget::Update()
{

  if (!this->ParentObject ||
      !this->ParentObject->GetDataManager()
      )
    return;

  
  this->Lock = true;

  unsigned int N_selected = this->DataSetList->GetNumberOfSelectedRows();
  int* selectedids = new int[N_selected];
  this->DataSetList->GetSelectedRows(selectedids);
  
  this->DataSetList->DeleteAllRows();
  
  std::vector<vtkMetaDataSet*> metadatasetlist = this->ParentObject->GetDataManager()->GetMetaDataSetList();
  for (unsigned int i=0; i<metadatasetlist.size(); i++)
  {
    vtkMetaDataSet* metadataset = metadatasetlist[i];
    this->DataSetList->InsertCellText(i, 0, metadataset->GetName());
    bool visible = false;
    if (this->ParentObject->GetPage(metadataset->GetTag()))
      visible = (bool)(this->ParentObject->GetPage(metadataset->GetTag())->GetVisibility (metadataset->GetDataSet()));
    this->DataSetList->InsertCellTextAsInt(i, 1, visible);    
    this->DataSetList->SetCellWindowCommandToCheckButton(i, 1);
    this->DataSetList->InsertCellText(i, 2, metadataset->GetDataSetType());
  }

  for (unsigned int i=0; i<N_selected; i++)
    this->DataSetList->SelectRow (selectedids[i]);
  
  std::vector<vtkKWToolBox*> tblist = this->ParentObject->GetToolBoxList();
  
  for (unsigned int j=0; j<tblist.size(); j++)
    tblist[j]->Update();
  
  this->Lock = false;
  
  
}
  
//----------------------------------------------------------------------------
void vtkKWDataManagerWidget::Pack (void)
{


}
 

//----------------------------------------------------------------------------
void vtkKWDataManagerWidget::SelectionChangedCallback()
{
  if (this->Lock)
    return;
  
  std::vector<vtkMetaDataSet*> metadatasets = this->GetSelectedMetaDataSets();
  std::vector<vtkKWToolBox*> tblist = this->ParentObject->GetToolBoxList();  

  vtkKWPageView* page = NULL;
  if (metadatasets.size())
  {
    page = this->ParentObject->GetPage(metadatasets[0]->GetTag());      
    if (page)
    {
      this->ParentObject->GetViewNotebook()->RaisePage(metadatasets[0]->GetTag());
      this->ParentObject->GetViewNotebook()->RaiseCallback (this->ParentObject->GetViewNotebook()->GetRaisedPageId());
    }
    
  }
  
  for (unsigned int j=0; j<tblist.size(); j++)
  {
    if (page)
    {
      tblist[j]->SetMetaDataSetModifiedCommand(page, "Render");
    }
    tblist[j]->SetMetaDataSets (metadatasets);
    //tblist[j]->Update();
  }
  
}

//----------------------------------------------------------------------------
void vtkKWDataManagerWidget::VisibilityChangedCallback(int row, int col, int state)
{
  if (this->Lock)
    return;
  
  if (col != 1)
    return;

  
  std::string name = this->DataSetList->GetCellText (row, 0);
  vtkMetaDataSet* metadataset = this->ParentObject->GetDataManager()->GetMetaDataSet(name.c_str());

  if (!metadataset)
    return;

  metadataset->SetVisibility(static_cast<bool>(state));

  vtkKWPageView* page = this->ParentObject->GetPage(metadataset->GetTag());
  if (!page)
    return;
  
  page->SetVisibility (metadataset->GetDataSet(), static_cast<bool>(state));
  page->Render();
}

//----------------------------------------------------------------------------
void vtkKWDataManagerWidget::DoubleClickCallback()
{
  // disabled
}

  

//----------------------------------------------------------------------------
vtkMetaDataSet* vtkKWDataManagerWidget::GetSelectedMetaDataSet (void)
{
  
  if (!this->ParentObject)
    return NULL;
  
  int row = this->DataSetList->GetIndexOfFirstSelectedRow();
  int name_col = 0;

  const char* cname = this->DataSetList->GetCellText (row, name_col);

  if (!cname || !(*cname))
    return NULL;
  
  std::string name = cname;
  
  
  
  vtkMetaDataSet* metadataset = this->ParentObject->GetDataManager()->GetMetaDataSet(name.c_str());
  return metadataset;
  
}

//----------------------------------------------------------------------------
std::vector<vtkMetaDataSet*> vtkKWDataManagerWidget::GetSelectedMetaDataSets (void)
{

  std::vector<vtkMetaDataSet*> list;
  
  if (!this->ParentObject)
    return list;


  const unsigned int Number_Of_Selected = this->DataSetList->GetNumberOfSelectedRows();
  int* indices = new int[Number_Of_Selected];
  this->DataSetList->GetSelectedRows(indices);

  int name_col = 0;

  for (unsigned int i=0; i<Number_Of_Selected; i++)
  {
    const char* cname = this->DataSetList->GetCellText (indices[i], name_col);
    
    if (!cname || !(*cname))
      continue;
    
    std::string name = cname;
    
    vtkMetaDataSet* metadataset = this->ParentObject->GetDataManager()->GetMetaDataSet(name.c_str());
    if (metadataset)
      list.push_back (metadataset);
  }

  delete [] indices;
  

  return list;
  
}



//----------------------------------------------------------------------------
void vtkKWDataManagerWidget::SetSelectedMetaDataSet (vtkMetaDataSet* metadataset)
{

  if (!metadataset)
    return;
  
  unsigned int N = this->DataSetList->GetNumberOfRows();
  for (unsigned int i=0; i<N; i++)
  {
    const char* cname = this->DataSetList->GetCellText (i, 0);

    if (strcmp(cname, metadataset->GetName())==0)
    {
      this->Lock = true;
      this->DataSetList->SelectRow (i);
      this->Lock = false;
      return;
    }
  }
  

}

  
