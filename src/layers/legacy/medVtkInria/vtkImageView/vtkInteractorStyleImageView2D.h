/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkInriaExport.h>

#include <vtkInteractorStyleImage.h>

/**
   Notes on Nicolas Toussaint changes


   A) XXButtonInteractionStyle became XXButtonInteraction : shorter

   B) Differentiation between zoom and pan is handled here, Pan is still work in progress

   C) PropagateCameraFocalAndPosition() is equivalent to sync. panning, so panning should be an event
   and synchronization of it should be in the collection.
*/

// Motion flags (See VTKIS_PICK and so on in vtkInteractorStyleImage.h)
#define VTKIS_SLICE_MOVE  5050
#define VTKIS_TIME_CHANGE 5051

 
class MEDVTKINRIA_EXPORT vtkInteractorStyleImageView2D : public vtkInteractorStyleImage
{
 public:
  static vtkInteractorStyleImageView2D *New();
  vtkTypeMacro (vtkInteractorStyleImageView2D, vtkInteractorStyleImage);

 
  //BTX
  enum InteractionTypeIds
  {
    InteractionTypeNull = 0,
    InteractionTypeSlice,
    InteractionTypeTime,
    InteractionTypeWindowLevel,
    InteractionTypeZoom,
    InteractionTypePan,
  };
  //ETX

  vtkSetClampMacro (LeftButtonInteraction, int, InteractionTypeNull, InteractionTypePan);
  vtkGetMacro (LeftButtonInteraction, int);
  vtkSetClampMacro (RightButtonInteraction, int, InteractionTypeNull, InteractionTypePan);
  vtkGetMacro (RightButtonInteraction, int);
  vtkSetClampMacro (MiddleButtonInteraction, int, InteractionTypeNull, InteractionTypePan);
  vtkGetMacro (MiddleButtonInteraction, int);  
  vtkSetClampMacro (WheelButtonInteraction, int, InteractionTypeNull, InteractionTypePan);
  vtkGetMacro (WheelButtonInteraction, int);  
  vtkSetClampMacro (KeyboardInteraction, int, InteractionTypeNull, InteractionTypePan);
  vtkGetMacro (KeyboardInteraction, int);  
  
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
  virtual void OnKeyDown();
  virtual void OnKeyUp();

  virtual void StartSliceMove();
  virtual void SliceMove();
  virtual void EndSliceMove();

  virtual void StartTimeChange();
  virtual void TimeChange();
  virtual void EndTimeChange();
	
  virtual void DefaultMoveAction();
  
  vtkGetMacro (SliceStep, int);
  vtkGetVector2Macro (RequestedPosition, int);
  
protected:
  vtkInteractorStyleImageView2D();
  ~vtkInteractorStyleImageView2D();
  
  int SliceStep;
  int RequestedPosition[2];

 private: 
  vtkInteractorStyleImageView2D(const vtkInteractorStyleImageView2D&);  // Not implemented.
  void operator=(const vtkInteractorStyleImageView2D&);  // Not implemented.


  int LeftButtonInteraction;
  int RightButtonInteraction;
  int MiddleButtonInteraction;
  int WheelButtonInteraction;
  int KeyboardInteraction;
  
};


