/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWGenericToolBox.cxx 477 2007-11-20 17:46:10Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2007-11-20 18:46:10 +0100 (Tue, 20 Nov 2007) $
Version:   $Revision: 477 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <vtkKWGenericToolBox.h>
#include "vtkObjectFactory.h"

#include <vtkMetaDataSet.h>
#include <vtkKWPushButton.h>
#include <vtkKWLoadSaveDialog.h>
#include <vtkKWIcon.h>

#include <vtkKWMessageDialog.h>


//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWGenericToolBox );
vtkCxxRevisionMacro( vtkKWGenericToolBox, "$Revision: 477 $");

//----------------------------------------------------------------------------
vtkKWGenericToolBox::vtkKWGenericToolBox()
{
  this->ButtonSave = vtkKWPushButton::New();
  this->ButtonDuplicate = vtkKWPushButton::New();
  this->ButtonAttributes = vtkKWPushButton::New();
  this->ButtonLoadArray = vtkKWPushButton::New();
}

//----------------------------------------------------------------------------
vtkKWGenericToolBox::~vtkKWGenericToolBox()
{
  this->ButtonSave->Delete();
  this->ButtonDuplicate->Delete();
  this->ButtonAttributes->Delete();
  this->ButtonLoadArray->Delete();
  
}


//----------------------------------------------------------------------------
void vtkKWGenericToolBox::CreateWidget()
{
  // Check if already created
  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();

  //this->SetLabelText ("Generic");
  this->CreateButtons();
  
  
  
  // Update according to the current property and metadataset
  this->Update();

  // Pack
  this->Pack();


}


//----------------------------------------------------------------------------
void vtkKWGenericToolBox::CreateButtons()
{
  vtkKWIcon *icon = vtkKWIcon::New();

  icon->SetImage(vtkKWIcon::IconFloppyDrive);
  this->ButtonSave->SetParent(this);
  this->ButtonSave->Create();
  this->ButtonSave->SetImageToIcon (icon);
  this->ButtonSave->SetCommand(this, "ButtonSaveCallBack");
  this->ButtonSave->SetBalloonHelpString("save...");

  icon->SetImage(vtkKWIcon::IconFloppyDrive);
  this->ButtonDuplicate->SetParent(this);
  this->ButtonDuplicate->Create();
  this->ButtonDuplicate->SetImageToIcon (icon);
  this->ButtonDuplicate->SetCommand(this, "ButtonDuplicateCallBack");
  this->ButtonDuplicate->SetBalloonHelpString("duplicate...");

  icon->SetImage(vtkKWIcon::IconFloppyDrive);
  this->ButtonAttributes->SetParent(this);
  this->ButtonAttributes->Create();
  this->ButtonAttributes->SetImageToIcon (icon);
  this->ButtonAttributes->SetCommand(this, "ButtonAttributesCallBack");
  this->ButtonAttributes->SetBalloonHelpString("meta information...");

  icon->SetImage(vtkKWIcon::IconFloppyDrive);
  this->ButtonLoadArray->SetParent(this);
  this->ButtonLoadArray->Create();
  this->ButtonLoadArray->SetImageToIcon (icon);
  this->ButtonLoadArray->SetCommand(this, "ButtonLoadArrayCallBack");
  this->ButtonLoadArray->SetBalloonHelpString("load array...");
    
  icon->Delete();
  
}

//----------------------------------------------------------------------------
void vtkKWGenericToolBox::ButtonSaveCallBack()
{

  if (!this->MetaDataSet)
    return;
  
  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
  
  dialog->SetParent(this);
  dialog->Create();
  dialog->MultipleSelectionOff();
  dialog->SaveDialogOn ();

  dialog->RetrieveLastPathFromRegistry("DataPath");
  dialog->SetTitle ("Save the MetaDataSet");
  dialog->SetFileTypes ("{{All} {.*}}");

  if ( dialog->Invoke () == 0 )
  {
    dialog->Delete();
    return ;
  }

     
  const char* filename = dialog->GetFileName();
  try
  {
    this->MetaDataSet->Write (filename);
  }
  
  catch (...)
  {
    vtkKWMessageDialog::PopupMessage(this->GetApplication(),
				     this->GetParentTopLevel(),
				     "Error",
				     "Cannot save file !\nPlease check the log window for details.",
				     vtkKWMessageDialog::ErrorIcon);
    
  }
}

//----------------------------------------------------------------------------
void vtkKWGenericToolBox::ButtonDuplicateCallBack()
{}

//----------------------------------------------------------------------------
void vtkKWGenericToolBox::ButtonAttributesCallBack()
{}

//----------------------------------------------------------------------------
void vtkKWGenericToolBox::ButtonLoadArrayCallBack()
{}


//----------------------------------------------------------------------------
void vtkKWGenericToolBox::Pack()
{
  if (this->ButtonSave->IsCreated())
  {
    this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
		 this->ButtonSave->GetWidgetName());
  }
  if (this->ButtonDuplicate->IsCreated())
  {
    this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
		 this->ButtonDuplicate->GetWidgetName());
  }
  if (this->ButtonAttributes->IsCreated())
  {
    this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
		 this->ButtonAttributes->GetWidgetName());
  }
  if (this->ButtonLoadArray->IsCreated())
  {
    this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
		 this->ButtonLoadArray->GetWidgetName());
  }
}


//----------------------------------------------------------------------------
void vtkKWGenericToolBox::Update()
{
  
}

