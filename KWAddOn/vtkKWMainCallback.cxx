/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWMainCallback.cxx 1302 2009-10-27 21:57:16Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-10-27 22:57:16 +0100 (Tue, 27 Oct 2009) $
Version:   $Revision: 1302 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkKWMainCallback.h"
#include <vtkObjectFactory.h>

#include <vtkKWApplication.h>

#include <vtkDataManager.h>
#include <vtkMetaDataSet.h>

#include <vtkKWDialog.h>


vtkKWMainCallback::vtkKWMainCallback()
{
  this->Application = NULL;
}

vtkKWMainCallback::~vtkKWMainCallback()
{
  if (this->Application)
    this->Application->UnRegister (this);
}


void vtkKWMainCallback::SetApplication (vtkKWApplication* application)
{
  if (!application)
    return;
  if (this->Application)
  {
    if (application==this->Application)
      return;
    this->Application->UnRegister (this);
    this->Application = NULL;
  }

  this->Application = application;
  this->Application->Register(this);
}


void vtkKWMainCallback::Execute(vtkObject* caller, unsigned long event, void* callData)
{
  if (event == vtkDataManager::MetaDataSetPickEvent )
  {
    // disabled
  }
  
}

