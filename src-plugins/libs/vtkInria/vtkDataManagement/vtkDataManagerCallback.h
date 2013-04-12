/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkDataManagerCallback.h 608 2008-01-14 08:21:23Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-01-14 09:21:23 +0100 (Mon, 14 Jan 2008) $
Version:   $Revision: 608 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#pragma once

#include <vtkCommand.h>
#include <vtkObject.h>
class vtkDataManager;

  
class vtkDataManagerCallback : public vtkCommand
{    
 public:
  
  static  vtkDataManagerCallback* New() 
    { return new vtkDataManagerCallback; }



  void SetDataManager (vtkDataManager* manager);
  vtkDataManager* GetDataManager() const
  { return this->DataManager; }
  

  
  void Execute(vtkObject *   caller, 
               unsigned long event, 
               void *        callData);

  
 protected:
  vtkDataManagerCallback();
  ~vtkDataManagerCallback();
  
 private:

  vtkDataManager* DataManager;
  
  
  
};



