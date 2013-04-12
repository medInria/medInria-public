#pragma once

// This class behaves like vtkInteractorStyleTrackballCamera except that the right click
// is canceled (no effect). This is particularly usefull when one wants a menu to popup
// when right clikcking on the  window. The wheel has the same effect as the right click.

#include "vtkInteractorStyleTrackballCamera.h"

#include "vtkWidgetsAddOnExport.h"

class vtkTransform;

class VTK_WIDGETSADDON_EXPORT vtkInteractorStyleTrackballCamera2 : public vtkInteractorStyleTrackballCamera
{
 public:
  static vtkInteractorStyleTrackballCamera2 *New();
  vtkTypeRevisionMacro(vtkInteractorStyleTrackballCamera2,vtkInteractorStyleTrackballCamera);

  virtual void OnLeftButtonDown();
  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();
  virtual void Rotate();

  void Azimuth(double angle);
  void Elevation(double angle);

 protected:
  vtkInteractorStyleTrackballCamera2();
  ~vtkInteractorStyleTrackballCamera2();

  double UpAxis[3];
  double RightAxis[3];
  vtkTransform *Transform;

 private:
  vtkInteractorStyleTrackballCamera2(const vtkInteractorStyleTrackballCamera2&);  // Not implemented.
  void operator=(const vtkInteractorStyleTrackballCamera2&);  // Not implemented.
};




