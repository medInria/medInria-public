/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkViewImage2DFullCommand.h 1080 2009-02-18 13:27:02Z acanale $
Language:  C++
Author:    $Author: acanale $
Date:      $Date: 2009-02-18 14:27:02 +0100 (Wed, 18 Feb 2009) $
Version:   $Revision: 1080 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// version vtkRenderingAddOn
#ifndef _vtkViewImage2DFullCommand_h_
#define _vtkViewImage2DFullCommand_h_

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

#endif
