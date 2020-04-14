#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkCornerAnnotation.h>
#include <vtkImageView2D.h>

#include <medVtkInriaExport.h>

class vtkImageView;

/**
   Notes on Nicolas Toussaint changes

   A) RenderOpaqueGeometry()
   we actually absolutely need to re-write this method in order
   to update properly the information as the mouse move over the window.

*/

class MEDVTKINRIA_EXPORT vtkImageViewCornerAnnotation : public vtkCornerAnnotation
{

 public:
  vtkTypeMacro(vtkImageViewCornerAnnotation, vtkCornerAnnotation)
  static vtkImageViewCornerAnnotation *New();

  void SetImageView (vtkImageView* arg);

   vtkGetObjectMacro (ImageView, vtkImageView)

  // Description:
  //Draw the scalar bar and annotation text to the screen.
  int RenderOpaqueGeometry(vtkViewport* viewport);

 protected:
  vtkImageViewCornerAnnotation();
  ~vtkImageViewCornerAnnotation();

  virtual void TextReplace(vtkImageActor *imageActor, vtkImageMapToWindowLevelColors *);

 private:
  vtkImageViewCornerAnnotation(const vtkImageViewCornerAnnotation&);  // Not implemented.
  void operator=(const vtkImageViewCornerAnnotation&);  // Not implemented.

  vtkImageView *ImageView;
  vtkImageView2D *view2d;
};
