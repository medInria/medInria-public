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
  //this->Update();
  this->DicomInfoList = NULL;

  this->SearchDialog = vtkKWSimpleEntryDialog::New();
  this->SearchEntry = vtkKWEntry::New();
  
  
  
}

//----------------------------------------------------------------------------
vtkKWDicomInfoWidget::~vtkKWDicomInfoWidget()
{
  this->DicomTagList.clear();
  //this->DicomInfoList->Delete();

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
  listframe->HorizontalScrollbarVisibilityOff();
  
  this->DicomInfoList = listframe->GetWidget();
  
  if (listframe)
  {
   this->Script("pack %s -fill y -side top -expand t", 
	       listframe->GetWidgetName());
  }

  //listframe->Delete();





  
//   this->DicomInfoList->SetParent(this);
//   this->DicomInfoList->Create();

  this->DicomInfoList->MovableColumnsOn();
  this->DicomInfoList->SetWidth(67);
  this->DicomInfoList->SetSelectionBackgroundColor(1,0.9647,0.6314);  
  this->DicomInfoList->SetSelectionForegroundColor (0,0,0);
  this->DicomInfoList->SetSelectionModeToBrowse ();
  

  int col_index;
  
  // Add the columns (make some of them editable)
  
  col_index = this->DicomInfoList->AddColumn("Name");
  this->DicomInfoList->SetColumnEditable(col_index, 0);  
  
  col_index = this->DicomInfoList->AddColumn("Type");
  this->DicomInfoList->SetColumnEditable(col_index, 0);
  this->DicomInfoList->SetColumnAlignmentToCenter(col_index);

  col_index = this->DicomInfoList->AddColumn("Value");
  this->DicomInfoList->SetColumnEditable(col_index, 1);
  this->DicomInfoList->SetColumnWidth(col_index, 30);
  
  this->DicomInfoList->SetSelectionChangedCommand (this, "SelectionChangedCallback");
  this->DicomInfoList->SetUneditableCellDoubleClickCommand(this, "DoubleClickCallback");
  this->DicomInfoList->SetCellUpdatedCommand (this, "CellUpdateCallback");
//   if (this->DicomInfoList)
//   {
//    this->Script("pack %s -fill both -side top -expand t", 
// 	       this->DicomInfoList->GetWidgetName());
//   }

  this->Pack();
  
  this->SearchDialog->SetMasterWindow (this);
  
//   this->SearchDialog->SetApplication (this->GetApplication());
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
void vtkKWDicomInfoWidget::SetDicomTagList(DicomTagListType list)
{
  
  this->DicomTagList.clear();
  for (unsigned int i=0; i<list.size(); i++)
  {
    this->DicomTagList.push_back(list[i]);
  }
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
    DicomTagType tag = this->DicomTagList[i];
    
    this->DicomInfoList->InsertCellText(i, 0, tag.name.c_str());
    this->DicomInfoList->InsertCellText(i, 1, tag.NotUsed.c_str());
    this->DicomInfoList->InsertCellText(i, 2, tag.value.c_str());    
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
//   int row = this->DicomInfoList->GetIndexOfFirstSelectedRow();

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

//     // Get the position of the mouse, the size of the top level window.

//   int px, py, tw, th, sw, sh;

//   vtkKWTkUtilities::GetMousePointerCoordinates(this, &px, &py);
//   vtkKWTkUtilities::GetWidgetRequestedSize(this->SearchDialog, &tw, &th);
//   vtkKWTkUtilities::GetScreenSize(this->SearchDialog, &sw, &sh);

//   px -= tw / 2;
//   if (px + tw > sw)
//     {
//     px -= (px + tw - sw);
//     }
//   if (px < 0)
//     {
//     px = 0;
//     }

//   py -= th / 2;
//   if (py + th > sh)
//     {
//     py -= (py + th - sh);
//     }
//   if (py < 0)
//     {
//     py = 0;
//     }

//   this->SearchDialog->SetPosition(px, py);
//   this->SearchDialog->Raise();
//   this->SearchDialog->Focus();

//   this->SearchDialog->Invoke ();

//   if (this->SearchDialog->GetStatus() != vtkKWDialog::StatusOK)
//     return;
  
  
//   std::string tosearch = vtksys::SystemTools::LowerCase (this->SearchDialog->GetEntry()->GetWidget()->GetValue());

//   if (!tosearch.size())
//     return;
  
//   this->DicomInfoList->ClearSelection();

//   bool seen = false;
  

//   for (unsigned int i=0; i<this->DicomInfoList->GetNumberOfRows(); i++)
//   {
//     std::string name = vtksys::SystemTools::LowerCase (this->DicomInfoList->GetCellText(i, 0));
//     std::string type = vtksys::SystemTools::LowerCase (this->DicomInfoList->GetCellText(i, 1));
//     std::string value = vtksys::SystemTools::LowerCase (this->DicomInfoList->GetCellText(i, 2));

//     std::string::size_type pos1, pos2, pos3;
//     pos1 = name.find (tosearch);
//     pos2 = type.find (tosearch);
//     pos3 = value.find (tosearch);
//     if (pos1 != std::string::npos ||
// 	pos2 != std::string::npos ||
// 	pos3 != std::string::npos)
//     {
//       this->DicomInfoList->SelectRow(i);
//       if (!seen)
//       {
// 	this->DicomInfoList->SeeRow (i);
// 	seen = true;
//       }
      
//     }
    

//   }
  

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
void vtkKWDicomInfoWidget::SetDicomTagListAsDictionary (itk::MetaDataDictionary dictionary)
{
  itk::DicomTagManager::Pointer tagmanager = itk::DicomTagManager::New();
  std::vector< std::string> keys = dictionary.GetKeys();

  for (unsigned int i=0; i<keys.size(); i++)
  {
    itk::DicomTag tag;  
    std::string value;
    itk::ExposeMetaData<std::string> ( dictionary, keys[i], value );
    tag = itk::DicomTagManager::FindTagByIdentifier (keys[i].c_str());
    tag.value = value.c_str();
    tagmanager->AddTag (tag);
  }

  this->SetDicomTagList(tagmanager->GetTagList());

}

  

//----------------------------------------------------------------------------
itk::MetaDataDictionary vtkKWDicomInfoWidget::GetDicomTagListAsDictionary (void)
{


  itk::MetaDataDictionary dictionary;
  
  std::vector<itk::DicomTag> taglist = this->GetDicomTagList();
  for (unsigned int i=0; i<taglist.size(); i++)
  {
    itk::EncapsulateMetaData<std::string>(dictionary,taglist[i].identifier,std::string(taglist[i].value));
  }      

  return dictionary;
  
}

  
//----------------------------------------------------------------------------
void vtkKWDicomInfoWidget::CellUpdateCallback(int row, int col, const char* text)
{
  if (col != 2)
    return;

  this->DicomTagList[row].value = text;
  
}
