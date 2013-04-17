/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

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



