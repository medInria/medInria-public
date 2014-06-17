/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <vtkImplicitPlaneWidget.h>
#include <vtkImageData.h>
#include <vtkCommand.h>
#include <vtkObject.h>

class vtkRenderWindowInteractor;
class vtkImageData;
class vtkImageClippingPlane;


//BTX
class VTK_EXPORT vtkImageClippingPlaneCallback : public vtkCommand
{

 public:
  static vtkImageClippingPlaneCallback* New()
  { return new vtkImageClippingPlaneCallback; }

  /// Main method of the callback
  virtual void Execute (vtkObject* caller, unsigned long event, void*);

  /// Set image to clip
  void SetImage(vtkImageData* img);

  /// Set ImplicitPlaneWidget
  void SetImplicitPlaneWidget(vtkImplicitPlaneWidget* wdg)
  { this->ImplicitPlaneWidget = wdg; }

  /// Set the render window interactor where the plane widget must be added
  void SetRenderWindowInteractor1(vtkRenderWindowInteractor* iren)
  { this->IRen1 = iren; }
  void SetRenderWindowInteractor2(vtkRenderWindowInteractor* iren)
  { this->IRen2 = iren; }
  void SetRenderWindowInteractor3(vtkRenderWindowInteractor* iren)
  { this->IRen3 = iren; }
  void SetRenderWindowInteractor4(vtkRenderWindowInteractor* iren)
  { this->IRen4 = iren; }


 private:

  /// Constructor
  vtkImageClippingPlaneCallback();

  /// Destructor
  ~vtkImageClippingPlaneCallback();

  vtkImageData* Image;
  vtkImageData* BackupImage;

  vtkImplicitPlaneWidget* ImplicitPlaneWidget;

  vtkRenderWindowInteractor* IRen1;
  vtkRenderWindowInteractor* IRen2;
  vtkRenderWindowInteractor* IRen3;
  vtkRenderWindowInteractor* IRen4;
  
  bool FirstRender;
};
//ETX



class VTK_EXPORT vtkImageClippingPlane : public vtkObject
{

 public:

  static vtkImageClippingPlane* New();
  vtkTypeRevisionMacro(vtkImageClippingPlane, vtkObject);

  /// Set the render window interactor where the plane widget must be added
  void SetRenderWindowInteractor1(vtkRenderWindowInteractor* iren)
  { this->IRen1 = iren; }
  void SetRenderWindowInteractor2(vtkRenderWindowInteractor* iren)
  { this->IRen2 = iren; }
  void SetRenderWindowInteractor3(vtkRenderWindowInteractor* iren)
  { this->IRen3 = iren; }
  void SetRenderWindowInteractor4(vtkRenderWindowInteractor* iren)
  { this->IRen4 = iren; }

  /// Set the image to clip
  void SetImage(vtkImageData* image)
  { this->Image = image; }

  void GetMask(vtkImageData* image);

  /// Create the clipping plane
  void CreateClippingPlane();

 private:

  /// Constructor
  vtkImageClippingPlane();

  /// Destructor
  ~vtkImageClippingPlane();

  /// 3D clipping plane
  vtkImplicitPlaneWidget* PlaneWidget;

  /// Renderer windows to interact with
  vtkRenderWindowInteractor* IRen1;
  vtkRenderWindowInteractor* IRen2;
  vtkRenderWindowInteractor* IRen3;
  vtkRenderWindowInteractor* IRen4;

  /// Image to clip
  vtkImageData* Image;
	
  /// Callback that will perform the clipping
  vtkImageClippingPlaneCallback* Callback;

  
};



