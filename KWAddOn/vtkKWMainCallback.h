/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWMainCallback.h 612 2008-01-14 10:59:37Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-01-14 11:59:37 +0100 (Mon, 14 Jan 2008) $
Version:   $Revision: 612 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtkKWMainCallback_h_
#define _vtkKWMainCallback_h_

#include "vtkINRIA3DConfigure.h"

#include <vtkCommand.h>
#include <vtkObject.h>

class vtkKWApplication;

class KW_ADDON_EXPORT vtkKWMainCallback : public vtkCommand
{    
 public:
  
  static  vtkKWMainCallback* New() 
    { return new vtkKWMainCallback; }
  
  void Execute(vtkObject *   caller, 
               unsigned long event, 
               void *        callData);

  void SetApplication (vtkKWApplication* application);
  vtkKWApplication* GetTopLevel (void) const
  { return this->Application; }
  
  
  
 protected:
  vtkKWMainCallback();
  ~vtkKWMainCallback();
  
 private:

  vtkKWApplication* Application;
  
  
  
};

#endif
