/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkInteractorStyleImage2D.h 1223 2009-08-06 07:21:42Z filus $
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
#ifndef _vtk_InteractorStyleImage2D_h_
#define _vtk_InteractorStyleImage2D_h__

#include "vtkRenderingAddOnExport.h"

#include <vtkInteractorStyleImage.h>
#include <vtkRenderingAddOn/vtkViewImage2D.h>

// Motion flags (See VTKIS_PICK and so on in vtkInteractorStyleImage.h)
#define VTKIS_MEASURE      5050
#define VTKIS_ZSLICE_MOVE  5051

 
class VTK_RENDERINGADDON_EXPORT vtkInteractorStyleImage2D : public vtkInteractorStyleImage
{
 public:
  static vtkInteractorStyleImage2D *New();
  vtkTypeRevisionMacro (vtkInteractorStyleImage2D, vtkInteractorStyleImage);
  
  virtual void OnMouseMove();
  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();
  virtual void OnMiddleButtonDown();
  virtual void OnMiddleButtonUp();
  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();
  virtual void OnMouseWheelForward();
  virtual void OnMouseWheelBackward();
  virtual void OnChar();
  
  void SetView (vtkViewImage2D* view)
  {
    this->View = view;
  }
  
  vtkGetObjectMacro (View, vtkViewImage2D);
  vtkSetMacro (ZSliceStep, int);
  vtkGetMacro (ZSliceStep, int);

  vtkSetMacro (WindowStep, double);
  vtkGetMacro (WindowStep, double);

  vtkSetMacro (LevelStep,  double);
  vtkGetMacro (LevelStep,  double);

  vtkSetMacro (ZoomFactor, double);
  vtkGetMacro (ZoomFactor, double);

  virtual void StartZSliceMove();
  virtual void ZSliceMove();
  virtual void ZSliceWheelForward();
  virtual void ZSliceWheelBackward();
  virtual void EndZSliceMove();
  virtual void FullPage();
  virtual void StartMeasure();
  virtual void Measure();
  virtual void EndMeasure();
  virtual void WindowLevel ();
  virtual void WindowLevelWheelForward ();
  virtual void WindowLevelWheelBackward ();
  virtual void Dolly();
  virtual void DollyWheelForward();
  virtual void DollyWheelBackward();
  //virtual void Pan();
  virtual void DefaultMoveAction();
  
 protected:
  vtkInteractorStyleImage2D();
  ~vtkInteractorStyleImage2D();

  void PropagateCameraFocalAndPosition();

 private:
  
  vtkInteractorStyleImage2D(const vtkInteractorStyleImage2D&);  // Not implemented.
  void operator=(const vtkInteractorStyleImage2D&);  // Not implemented.
  
 private:
  vtkViewImage2D*  View;
  int              ZSliceStep;
  double           WindowStep;
  double           LevelStep;
  double           ZoomFactor;
  
};

#endif
