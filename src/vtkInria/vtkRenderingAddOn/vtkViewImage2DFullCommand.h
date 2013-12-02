/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkRenderingAddOnExport.h"

#include<vtkCommand.h>
#include<vtkObjectFactory.h>

class vtkViewImage2D;
class VTK_RENDERINGADDON_EXPORT vtkViewImage2DFullCommand : public vtkCommand
{
    
public:

  static vtkViewImage2DFullCommand* New() 
  { return new vtkViewImage2DFullCommand; }
    
  void Execute(vtkObject *   caller, 
               unsigned long event, 
               void *        callData);    
    
  vtkViewImage2D*          IV;
  double                   InitialWindow;
  double                   InitialLevel;

  bool                     WindowEventStatus;

 protected:
  vtkViewImage2DFullCommand()
  {
    this->IV = 0;
    this->InitialWindow = 0;
    this->InitialLevel = 0;
    this->WindowEventStatus = false;
  }
};


