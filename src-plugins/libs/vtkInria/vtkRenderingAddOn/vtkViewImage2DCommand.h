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


class vtkInteractorStyle;
class vtkInteractorStyleImage;
class vtkInteractorStyleImage2D;
class vtkViewImage2D;
  
class VTK_RENDERINGADDON_EXPORT vtkViewImage2DCommand : public vtkCommand
{    
 public:
  vtkViewImage2DCommand(); 
  
  static  vtkViewImage2DCommand* New() 
    { return new vtkViewImage2DCommand; }
  
  void Execute(vtkObject *   caller, 
               unsigned long event, 
               void *        callData);    
  
  void SetView(vtkViewImage2D *p_view);

  //BTX
  enum EventIds
  {
    ZSliceMoveEvent=(vtkCommand::UserEvent+1),
    StartZSliceMoveEvent,
    EndZSliceMoveEvent,
    StartMeasureEvent,
    MeasureEvent,
    EndMeasureEvent,
    FullPageEvent,
    ResetPositionEvent,
    ZoomEvent,
    ResetZoomEvent,
    DefaultMoveEvent
  };
  //ETX
    
 protected:
  void ChangeZSlice(vtkInteractorStyleImage2D* p_isi);
  void EndWindowing();
  void Windowing(vtkInteractorStyleImage2D* p_isi);
  void StartWindowing();  
  void StartPicking(vtkInteractorStyle* p_isi);
  void EndPicking();
  void Zoom (vtkInteractorStyleImage2D* p_isi);
  void DefaultMove (vtkInteractorStyle*);

  void PropagateWindowLevel();
  void PropagateResetWindowLevel();
  void PropagateStartWindowing();
  void PropagateEndWindowing();
  
  void PropagateCurrentPoint();
  void PropagateResetCurrentPoint();

  void PropagateZoom();
  void PropagateResetZoom();
  
  
 private:
  vtkViewImage2D* View;
  double          InitialWindow;
  double          InitialLevel;
  
};


