/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkViewImage2DCommand.h 1223 2009-08-06 07:21:42Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-08-06 09:21:42 +0200 (Thu, 06 Aug 2009) $
Version:   $Revision: 1223 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// version vtkRenderingAddOn
#ifndef _vtkViewImage2DCommand_h_
#define _vtkViewImage2DCommand_h_

#include "vtkINRIA3DConfigure.h"

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
  void EndPicking (void);
  void Zoom (vtkInteractorStyleImage2D* p_isi);
  void DefaultMove (vtkInteractorStyle*);

  void PropagateWindowLevel (void);
  void PropagateResetWindowLevel (void);
  void PropagateStartWindowing (void);
  void PropagateEndWindowing (void);
  
  void PropagateCurrentPoint (void);
  void PropagateResetCurrentPoint (void);

  void PropagateZoom (void);
  void PropagateResetZoom (void);
  
  
 private:
  vtkViewImage2D* View;
  double          InitialWindow;
  double          InitialLevel;

  bool            WindowEventStatus;
  
};

#endif
