#ifndef _vtk_InteractorStyleTrackballCamera2_h_
#define _vtk_InteractorStyleTrackballCamera2_h_

// This class behaves like vtkInteractorStyleTrackballCamera except that the right click
// is canceled (no effect). This is particularly usefull when one wants a menu to popup
// when right clikcking on the  window. The wheel has the same effect as the right click.

#include "vtkInteractorStyleTrackballCamera.h"

#include "vtkINRIA3DConfigure.h"

class VTK_WIDGETSADDON_EXPORT vtkInteractorStyleTrackballCamera2 : public vtkInteractorStyleTrackballCamera
{
 public:
  static vtkInteractorStyleTrackballCamera2 *New();
  vtkTypeRevisionMacro(vtkInteractorStyleTrackballCamera2,vtkInteractorStyleTrackballCamera);

  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();

 protected:
  vtkInteractorStyleTrackballCamera2();
  ~vtkInteractorStyleTrackballCamera2();

 private:
  vtkInteractorStyleTrackballCamera2(const vtkInteractorStyleTrackballCamera2&);  // Not implemented.
  void operator=(const vtkInteractorStyleTrackballCamera2&);  // Not implemented.
};



#endif
