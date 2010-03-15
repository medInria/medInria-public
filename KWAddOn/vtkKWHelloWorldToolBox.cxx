/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWHelloWorldToolBox.cxx 591 2008-01-08 15:58:17Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-01-08 16:58:17 +0100 (Tue, 08 Jan 2008) $
Version:   $Revision: 591 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <vtkKWHelloWorldToolBox.h>
#include "vtkObjectFactory.h"

#include <vtkMetaDataSet.h>
#include <vtkKWPushButton.h>
#include <vtkKWLoadSaveDialog.h>
#include <vtkKWIcon.h>



//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWHelloWorldToolBox );
vtkCxxRevisionMacro( vtkKWHelloWorldToolBox, "$Revision: 591 $");

//----------------------------------------------------------------------------
vtkKWHelloWorldToolBox::vtkKWHelloWorldToolBox()
{
  this->Button = vtkKWPushButton::New();
}

//----------------------------------------------------------------------------
vtkKWHelloWorldToolBox::~vtkKWHelloWorldToolBox()
{
  this->Button->Delete();
}


//----------------------------------------------------------------------------
void vtkKWHelloWorldToolBox::CreateWidget()
{
  // Check if already created
  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();

  //this->SetLabelText ("hello !");
  this->CreateButton();
  
  
  
  // Update according to the current property and metadataset
  this->Update();

  // Pack
  this->Pack();


}


//----------------------------------------------------------------------------
void vtkKWHelloWorldToolBox::CreateButton()
{
  // Check if already created
  if (this->Button->IsCreated())
    return;
  this->Button->SetParent(this);
  this->Button->Create();
  
  this->Button->SetWidth (20);
  this->Button->SetHeight (20);
  vtkKWIcon *icon = vtkKWIcon::New();
  icon->SetImage(vtkKWIcon::IconFloppyDrive);
  this->Button->SetImageToIcon (icon);
  icon->Delete();
  
  this->Button->SetCommand(this, "ButtonCallBack");
  this->Button->SetBalloonHelpString("Save...");
  
  
  
}

//----------------------------------------------------------------------------
void vtkKWHelloWorldToolBox::ButtonCallBack()
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
  this->MetaDataSet->Write (filename);  
}



//----------------------------------------------------------------------------
void vtkKWHelloWorldToolBox::Pack()
{
  if (this->Button->IsCreated())
  {
    this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
		 this->Button->GetWidgetName());
  }
}


//----------------------------------------------------------------------------
void vtkKWHelloWorldToolBox::Update()
{
  
  this->Superclass::Update();
  
}

