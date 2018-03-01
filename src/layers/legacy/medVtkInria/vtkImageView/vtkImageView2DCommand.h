/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkInriaExport.h>

#include <vtkCommand.h>
#include <vtkObject.h>

class vtkImageView2D;


/**
   Notes on Nicolas Toussaint changes


   A) Methods, API
   A command must have only the execute() method and a bunch of
   events to deal with. the rest should be dealt with in the interactorStyle

   B) The Zoom and Pan events as separated in Pierre's version are still work
   in progress to be separated here as well. Zoom is done but pan is still to be done.

*/

class MEDVTKINRIA_EXPORT vtkImageView2DCommand : public vtkCommand
{
 public:
  static  vtkImageView2DCommand* New()
  { return new vtkImageView2DCommand; }

  //BTX
  enum EventIds
  {
    SliceMoveEvent=(vtkCommand::UserEvent+1),
    StartSliceMoveEvent,
    EndSliceMoveEvent,
    TimeChangeEvent,
    StartTimeChangeEvent,
    EndTimeChangeEvent,
    RequestedPositionEvent,
    ResetViewerEvent,
    CameraMoveEvent,
    CameraZoomEvent,
    CameraPanEvent,
    DefaultMoveEvent,
    RequestedCursorInformationEvent
  };
  //ETX

  // Description:
  // Satisfy the superclass API for callbacks. Recall that the caller is
  // the instance invoking the event; eid is the event id (see
  // vtkCommand.h); and calldata is information sent when the callback
  // was invoked (e.g., progress value in the vtkCommand::ProgressEvent).
  virtual void Execute(vtkObject *caller,unsigned long event, void *vtkNotUsed(callData));

  virtual void SetViewer(vtkImageView2D *viewer)
  { this->Viewer = viewer; }


 protected:
  vtkImageView2DCommand();
  ~vtkImageView2DCommand();

 private:
  vtkImageView2D* Viewer;
  double InitialWindow;
  double InitialLevel;

};


