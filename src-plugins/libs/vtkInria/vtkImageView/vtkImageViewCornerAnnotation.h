#pragma once

#include "vtkCornerAnnotation.h"

#include "vtkImageViewExport.h"

class vtkImageView;


/**
   Notes on Nicolas Toussaint changes

   A) RenderOpaqueGeometry()
   we actually absolutely need to re-write this method in order
   to update properly the information as the mouse move over the window.
   
*/

/**
   flags :

   
*/


class VTK_IMAGEVIEW_EXPORT vtkImageViewCornerAnnotation : public vtkCornerAnnotation
{

 public:
  vtkTypeRevisionMacro(vtkImageViewCornerAnnotation, vtkCornerAnnotation);
  static vtkImageViewCornerAnnotation *New();

  
  void SetImageView (vtkImageView* arg)
  { this->ImageView = arg; }
  vtkGetObjectMacro (ImageView, vtkImageView);
  // Description:
  // Draw the scalar bar and annotation text to the screen.
  int RenderOpaqueGeometry(vtkViewport* viewport);

 protected:
  vtkImageViewCornerAnnotation();
  ~vtkImageViewCornerAnnotation();

  virtual void TextReplace( vtkImageActor *ia, vtkImageMapToWindowLevelColors *wl);
  
 private:
  vtkImageViewCornerAnnotation(const vtkImageViewCornerAnnotation&);  // Not implemented.
  void operator=(const vtkImageViewCornerAnnotation&);  // Not implemented.

  vtkImageView *ImageView;


};



