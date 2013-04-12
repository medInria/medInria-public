/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkLandmarkManagerCallback.h 1282 2009-10-08 17:00:40Z acanale $
Language:  C++
Author:    $Author: acanale $
Date:      $Date: 2009-10-08 19:00:40 +0200 (Thu, 08 Oct 2009) $
Version:   $Revision: 1282 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

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




