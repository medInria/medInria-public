/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWToolBox.cxx 888 2008-07-02 14:23:59Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-07-02 15:23:59 +0100 (Wed, 02 Jul 2008) $
Version:   $Revision: 888 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <vtkKWToolBox.h>
#include "vtkObjectFactory.h"

#include <vtkMetaDataSet.h>
#include <vtkKWLabel.h>
#include <vtkKWToolbar.h>
#include <vtkKWToolbarSet.h>
#include <vtkKWMainWindowInteractor.h>
#include <vtkKWIcon.h>


//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWToolBox );
vtkCxxRevisionMacro( vtkKWToolBox, "$Revision: 888 $");

//----------------------------------------------------------------------------
vtkKWToolBox::vtkKWToolBox()
{
  this->MetaDataSet = NULL;
  //this->DataManager = NULL;

  this->MetaDataSetChangedCommand = NULL;
  this->MetaDataSetChangingCommand = NULL;
  this->MetaDataSetModifiedCommand = NULL;

  
  this->ParentObject = NULL;
  this->ToolType = TOOL_VERTICAL;
  this->Toolbar = vtkKWToolbar::New();

  this->Icon = NULL;
  
  
  vtkKWIcon* icon = vtkKWIcon::New();
  icon->SetImage (vtkKWIcon::IconFolderXP);
  this->SetIcon (icon);
  icon->Delete();

  
  this->Name = "m-toolbox";
  
}

//----------------------------------------------------------------------------
vtkKWToolBox::~vtkKWToolBox()
{
  std::cout<<"deleting vtkKWToolBox "<<this->GetName()<<std::endl;

  if (this->MetaDataSet)
  {
    this->MetaDataSet->Delete();
  }

  if (this->Icon)
    this->Icon->Delete();
  
  this->Toolbar->Delete();
  

}

//----------------------------------------------------------------------------
void vtkKWToolBox::CreateWidget()
{
  this->Superclass::CreateWidget();


}

//----------------------------------------------------------------------------
void vtkKWToolBox::InvokeObjectMethodCommand(const char *command)
{
  this->Superclass::InvokeObjectMethodCommand(command);
}

//----------------------------------------------------------------------------
void vtkKWToolBox::SetMetaDataSetChangedCommand(
  vtkObject *object, const char *method)
{
  this->SetObjectMethodCommand(
    &this->MetaDataSetChangedCommand, object, method);
}

//----------------------------------------------------------------------------
void vtkKWToolBox::InvokeMetaDataSetChangedCommand()
{
  this->InvokeObjectMethodCommand(this->MetaDataSetChangedCommand);
}

//----------------------------------------------------------------------------
void vtkKWToolBox::SetMetaDataSetChangingCommand(
  vtkObject *object, const char *method)
{
  this->SetObjectMethodCommand(
    &this->MetaDataSetChangingCommand, object, method);
}

//----------------------------------------------------------------------------
void vtkKWToolBox::InvokeMetaDataSetChangingCommand()
{
  this->InvokeObjectMethodCommand(this->MetaDataSetChangingCommand);
}


//----------------------------------------------------------------------------
void vtkKWToolBox::SetMetaDataSetModifiedCommand(
  vtkObject *object, const char *method)
{
  this->SetObjectMethodCommand(
    &this->MetaDataSetModifiedCommand, object, method);
}

//----------------------------------------------------------------------------
void vtkKWToolBox::InvokeMetaDataSetModifiedCommand()
{
  this->InvokeObjectMethodCommand(this->MetaDataSetModifiedCommand);
}




//----------------------------------------------------------------------------
void vtkKWToolBox::Pack()
{}

//----------------------------------------------------------------------------
void vtkKWToolBox::Update()
{


}

//----------------------------------------------------------------------------
void vtkKWToolBox::SetMetaDataSets (std::vector<vtkMetaDataSet*> metadatasets)
{
  this->MetaDataSets.clear();
  
  for (unsigned int i=0; i<metadatasets.size(); i++)
    this->MetaDataSets.push_back (metadatasets[i]);

  if (metadatasets.size())
  {
    this->SetMetaDataSet (metadatasets[0]);
  }
  else
    this->SetMetaDataSet (NULL);
  
}


//----------------------------------------------------------------------------
void vtkKWToolBox::SetMetaDataSet (vtkMetaDataSet* metadataset)
{
  if (this->MetaDataSet == metadataset)
  {
    return;
  }
  
  if (this->MetaDataSet)
  {
    this->MetaDataSet->UnRegister(this);
  }
    
  this->MetaDataSet = metadataset;
  
  if (this->MetaDataSet)
  {
    this->MetaDataSet->Register(this);
  }

  if (!this->MetaDataSets.size())
    this->MetaDataSets.push_back (metadataset);

  this->Modified();

  this->Update();


}


//----------------------------------------------------------------------------
void vtkKWToolBox::SetIcon (vtkKWIcon* icon)
{
  if ((this->Icon) && (this->Icon == icon))
    {
    return;
    }

  if (this->Icon)
    {
    this->Icon->UnRegister(this);
    }
    
  this->Icon = icon;

  if (this->Icon)
    {
    this->Icon->Register(this);
    }


  this->Modified();

}

