/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkLandmarkManagementExport.h"

#include <vtkCommand.h>

class vtkImageView;
class vtkPolyData;
class vtkImageViewCollection;
class vtkLandmark;
class vtkLandmarkManager;

class VTK_LANDMARKMANAGEMENT_EXPORT vtkLandmarkManagerCallback : public vtkCommand
{    
 public:
  
  static  vtkLandmarkManagerCallback* New() 
    { return new vtkLandmarkManagerCallback; }

  void Execute(vtkObject *   caller, 
               unsigned long event, 
               void *        callData);


  //BTX
  enum EventIds
  {
    LandmarkHasMovedEvent = (vtkCommand::UserEvent+100),
    LandmarkAddedEvent,
    LandmarkPickedEvent,
    LandmarkRemovingEvent,
    LandmarkRemovedEvent
  };
  //ETX
  
  
  void SetViewList (vtkImageViewCollection* collection)
  {
    this->ViewList = collection;
  }
  vtkImageViewCollection* GetViewList()
  {
    return this->ViewList;
  }
  vtkLandmarkManager* GetLandmarkManager()
  {
    return this->LandmarkManager;
  }
  void SetLandmarkManager (vtkLandmarkManager* manager)
  {
    this->LandmarkManager = manager;
  }

  vtkLandmark* GetSelectedLandmark() const
  { return this->SelectedLandmark; }
  
  
 protected:
  vtkLandmarkManagerCallback()
  {
    this->ViewList = NULL;
    this->SelectedLandmark = NULL;
    this->LandmarkManager = NULL;
    
  };
  ~vtkLandmarkManagerCallback(){};  
  
 private:

  vtkImageViewCollection* ViewList;
  vtkLandmark* SelectedLandmark;
  vtkLandmarkManager* LandmarkManager;
  

};




