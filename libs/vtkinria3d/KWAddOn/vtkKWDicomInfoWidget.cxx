/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWDicomInfoWidget.cxx 1302 2009-10-27 21:57:16Z ntoussaint $
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
#include <vtkKWDicomInfoWidget.h>
#include "vtkObjectFactory.h"

#include <vtksys/SystemTools.hxx>

#include <vtkTimerLog.h>
#include <vtkKWScale.h>
#include <vtkKWScaleSet.h>
#include <vtkKWPushButton.h>
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWCheckButton.h>
#include <vtkKWFrame.h>
#include <vtkKWApplication.h>
#include <vtkKWLabel.h>
#include <vtkKWSimpleEntryDialog.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWEntry.h>
#include <vtkKWTkUtilities.h>

#include <pixmap/KWAddOnResources.h>
#include <vtkKWMultiColumnList.h>
#include <vtkKWMultiColumnListWithScrollbars.h>
#include <vtkKWDialog.h>
#include <vtkKWIcon.h>
#include <vtkKWNotebook.h>
#include <vtkKWToolBox.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWDicomInfoWidget );
vtkCxxRevisionMacro( vtkKWDicomInfoWidget, "$Revision: 1302 $");

//----------------------------------------------------------------------------
vtkKWDicomInfoWidget::vtkKWDicomInfoWidget()
{
  
  this->EditableOn();

  this->SearchDialog = vtkKWSimpleEntryDialog::New();
  this->SearchEntry = vtkKWEntry::New();
}

//----------------------------------------------------------------------------
vtkKWDicomInfoWidget::~vtkKWDicomInfoWidget()
{
  this->DicomTagList.clear();

  this->SearchDialog->Delete();
  this->SearchEntry->Delete();
}


//----------------------------------------------------------------------------
void vtkKWDicomInfoWidget::CreateWidget()
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

  vtkKWFrame* temp = vtkKWFrame::New();
  temp->SetParent (this);
  temp->Create();
  vtkKWLabel* label = vtkKWLabel::New();
  label->SetParent (temp);
  label->Create();
  label->SetText ("DICOM Data");
  vtkKWPushButton* button = vtkKWPushButton::New();
  button->SetParent (temp);
  button->Create();
  button->SetImageToPixels(image_find, image_find_width, image_find_height, image_find_pixel_size, image_find_length); 
  button->SetCommand (this, "SearchCallback");
  button->SetBalloonHelpString("Search data");

  this->SearchEntry->SetParent (temp);
  this->SearchEntry->Create();
  this->SearchEntry->SetWidth(22);
  this->SearchEntry->SetValue("search...");
  this->SearchEntry->SetCommand (this, "SearchEntryCallback");
  this->SearchEntry->SetBackgroundColor(1,1,1);
  this->SearchEntry->ReadOnlyOff();
  this->SearchEntry->SetCommandTrigger(vtkKWEntry::TriggerOnReturnKey);
  
  this->Script("pack %s -side left -anchor nw -expand n", 
		 label->GetWidgetName());
  this->Script("pack %s -side right -anchor ne -expand n", 
		 button->GetWidgetName());
  this->Script("pack %s -side right -anchor se -expand n", 
		 this->SearchEntry->GetWidgetName());
  this->Script("pack %s -side top -fill x -anchor nw -expand n", 
		 temp->GetWidgetName());
  label->Delete();
  temp->Delete();
  button->Delete();



  vtkKWMultiColumnListWithScrollbars* listframe = vtkKWMultiColumnListWithScrollbars::New();
  listframe->SetParent (this);
  listframe->Create();
  listframe->VerticalScrollbarVisibilityOn();
  listframe->HorizontalScrollbarVisibilityOn();
  
  this->DicomInfoList = listframe->GetWidget();
  
  if (listframe)
  {
   this->Script("pack %s -fill both -side top -expand t", 
	       listframe->GetWidgetName());
  }

  this->DicomInfoList->MovableColumnsOn();
  this->DicomInfoList->SetWidth(67);
  this->DicomInfoList->SetSelectionBackgroundColor(1,0.9647,0.6314);  
  this->DicomInfoList->SetSelectionForegroundColor (0,0,0);
  this->DicomInfoList->SetSelectionModeToBrowse ();

  int col_index;
  
  // Add the columns (make some of them editable)
  
  col_index = this->DicomInfoList->AddColumn("Name");
  this->DicomInfoList->SetColumnEditable(col_index, 0);  
  
  col_index = this->DicomInfoList->AddColumn("Value");
  this->DicomInfoList->SetColumnEditable(col_index, 1);
  
  col_index = this->DicomInfoList->AddColumn("Type");
  this->DicomInfoList->SetColumnEditable(col_index, 0);

  this->DicomInfoList->SetSelectionChangedCommand (this, "SelectionChangedCallback");
  this->DicomInfoList->SetUneditableCellDoubleClickCommand(this, "DoubleClickCallback");
  this->DicomInfoList->SetCellUpdatedCommand (this, "CellUpdateCallback");

  this->Pack();
  
  this->SearchDialog->SetMasterWindow (this);
  
  this->SearchDialog->Create();
  this->SearchDialog->SetTitle ("Search data");
  this->SearchDialog->GetEntry()->SetLabelPositionToTop();
  this->SearchDialog->GetEntry()->SetLabelText ("use this tool to search for a specific tag.\nNote that the tool is case sensitive");
  this->SearchDialog->SetStyleToOkCancel();
  this->SearchDialog->GetEntry()->GetWidget()->SetBackgroundColor (1,1,1);
  this->SearchDialog->GetEntry()->GetWidget()->SetCommand(this, "ValidateSearchCallback");
  this->SearchDialog->GetEntry()->GetWidget()->SetCommandTrigger(vtkKWEntry::TriggerOnReturnKey);
  this->SearchDialog->GetOKButton()->SetCommand(this->SearchDialog, "OK");
}

//----------------------------------------------------------------------------
void vtkKWDicomInfoWidget::SetDicomEntryList(DicomEntryList list)
{
  for (unsigned int i=0; i<list.size(); i++)
    this->DicomTagList.push_back (list[i]);
  this->Update();
}

//----------------------------------------------------------------------------
void vtkKWDicomInfoWidget::Update()
{
   // Check if already created
  if (!this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }

  this->DicomInfoList->DeleteAllRows();
  
  for (unsigned int i=0; i<this->DicomTagList.size(); i++)
  {
    DicomEntry tag = this->DicomTagList[i];
    
    this->DicomInfoList->InsertCellText(i, 0, tag.first.c_str());
    this->DicomInfoList->InsertCellText(i, 1, tag.second.c_str());    
    this->DicomInfoList->InsertCellText(i, 2, "");
  }
}
  
//----------------------------------------------------------------------------
void vtkKWDicomInfoWidget::Pack (void)
{
}

//----------------------------------------------------------------------------
void vtkKWDicomInfoWidget::SelectionChangedCallback()
{
}

//----------------------------------------------------------------------------
void vtkKWDicomInfoWidget::DoubleClickCallback()
{

}

//----------------------------------------------------------------------------
void vtkKWDicomInfoWidget::SearchCallback()
{
  // Check if already created
  if (!this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  this->SearchEntryCallback(this->SearchEntry->GetValue());
}


//----------------------------------------------------------------------------
void vtkKWDicomInfoWidget::ValidateSearchCallback(const char* NotUsed)
{
  this->SearchDialog->OK();
}


//----------------------------------------------------------------------------
void vtkKWDicomInfoWidget::SearchEntryCallback(const char* value)
{
  std::string tosearch = value;
  
  if (!tosearch.size())
    return;
  
  this->DicomInfoList->ClearSelection();

  bool seen = false;

  for (int i=0; i<this->DicomInfoList->GetNumberOfRows(); i++)
  {
    std::string name = vtksys::SystemTools::LowerCase (this->DicomInfoList->GetCellText(i, 0));
    std::string type = vtksys::SystemTools::LowerCase (this->DicomInfoList->GetCellText(i, 1));
    std::string value = vtksys::SystemTools::LowerCase (this->DicomInfoList->GetCellText(i, 2));

    std::string::size_type pos1, pos2, pos3;
    pos1 = name.find (tosearch);
    pos2 = type.find (tosearch);
    pos3 = value.find (tosearch);
    if (pos1 != std::string::npos ||
	pos2 != std::string::npos ||
	pos3 != std::string::npos)
    {
      this->DicomInfoList->SelectRow(i);
      if (!seen)
      {
	this->DicomInfoList->SeeRow (i);
	seen = true;
      }
      
    }
  }
}

//----------------------------------------------------------------------------
void vtkKWDicomInfoWidget::CellUpdateCallback(int row, int col, const char* text)
{
  if (col != 1)
    return;

  this->DicomTagList[row].second = text;
}
