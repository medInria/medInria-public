/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkImageView2D.cxx 1367 2009-11-30 11:23:21Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-11-30 12:23:21 +0100 (lun, 30 nov 2009) $
Version:   $Revision: 1367 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkImageView2D.h"


#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkImageMapToColors.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkScalarBarActor.h"
#include "vtkCornerAnnotation.h"
#include "vtkTextProperty.h"
#include "vtkLookupTable.h"
#include "vtkMath.h"
#include "vtkPlane.h"
#include "vtkCutter.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkProp3DCollection.h"
#include "vtkDataSetCollection.h"
#include "vtkPoints.h"
#include "vtkIdList.h"
#include "vtkOutlineSource.h"
#include "vtkMatrixToLinearTransform.h"
#include "vtkPointData.h"
#include "vtkUnsignedCharArray.h"
#include "vtkIntArray.h"
#include "vtkImageAccumulate.h"
#include "vtkCoordinate.h"
#include "vtkTextActor.h"
#include "vtkAxisActor2D.h"
#include "vtkProperty.h"
#include <vtkOrientationAnnotation.h>
#include <vtkImageView2DCommand.h>
#include <vtkProperty2D.h>
#include <vtkSmartPointer.h>
#include <vtkAxisActor2D.h>
#include <vtkAxes2DWidget.h>
#include <vtkRulerWidget.h>
#include "vtkDistanceWidget.h"
#include "vtkDistanceRepresentation2D.h"
#include "vtkAngleWidget.h"
#include "vtkAngleRepresentation2D.h"
#include "vtkLeaderActor2D.h"
#include "vtkProperty2D.h"
#include <vtkPointHandleRepresentation2D.h>
#include <vtkDataSet2DWidget.h>
#include <vtkImageViewCornerAnnotation.h>
#include <vtkImageCast.h>
#include "vtkLookupTableManager.h"
#include <vtkColorTransferFunction.h>
#include <vtkImageReslice.h>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>


vtkCxxRevisionMacro(vtkImageView2D, "$Revision: 3 $");
vtkStandardNewMacro(vtkImageView2D);



class vtkImage2DDisplay : public vtkObject
{
    public:
        static vtkImage2DDisplay * New();
        vtkImage2DDisplay();
        ~vtkImage2DDisplay();

        virtual void SetInput(vtkImageData * image);
        virtual vtkImageData * GetInput();
        // virtual void SetWindowLevel(vtkImageMapToColors * color);
        virtual void SetLookupTable(vtkLookupTable * lut);
        virtual vtkImageActor * GetImageActor();
        vtkGetObjectMacro(WindowLevel, vtkImageMapToColors);


    private :
        vtkImageMapToColors*            WindowLevel;
        vtkLookupTable*                 LUT;
        vtkImageData*                   Input;
        vtkImageActor*                  ImageActor;
};

//vtkCxxRevisionMacro(vtkImage2DDisplay, "$Revision: 3 $");
vtkStandardNewMacro(vtkImage2DDisplay);

vtkImage2DDisplay::vtkImage2DDisplay()
{
    this->ImageActor        = vtkImageActor::New();
    this->WindowLevel       = vtkImageMapToColors::New();
    this->WindowLevel->SetOutputFormatToRGBA();
    this->LUT               = vtkLookupTable::New();
}

vtkImage2DDisplay::~vtkImage2DDisplay()
{
    this->LUT->Delete();
    this->WindowLevel->Delete();
    this->ImageActor->Delete();
    this->Input->Delete();
}

vtkImageData * vtkImage2DDisplay::GetInput()
{
    return this->Input;
}

void
vtkImage2DDisplay::SetInput(vtkImageData * image)
{
    this->Input = image;

    if (image)
        image->UpdateInformation(); // must be called before GetSliceForWorldCoordinates()

    this->ImageActor->SetInput( this->WindowLevel->GetOutput() );
    this->WindowLevel->SetInput(image);
}

/*
void
vtkImage2DDisplay::SetWindowLevel(vtkImageMapToColors * color)
{
    this->WindowLevel = color;
}*/

void
vtkImage2DDisplay::SetLookupTable(vtkLookupTable * lut)
{
    this->LUT = lut;
    this->WindowLevel->SetLookupTable(this->LUT);
//    this->WindowLevel->Update();
}

vtkImageActor *
vtkImage2DDisplay::GetImageActor()
{
    return this->ImageActor;
}
// End vtkImage2DDisplay




//----------------------------------------------------------------------------
vtkImageView2D::vtkImageView2D()
{
  this->ImageActor          = vtkImageActor::New();
    this->SecondImageActor  = vtkImageActor::New();
    this->SecondImageActor->SetOpacity(0.5);
    //this->ImageActor->SetOpacity(0.5);
  this->Axes2DWidget        = vtkAxes2DWidget::New();
  this->RulerWidget         = vtkRulerWidget::New();
  this->DistanceWidget      = vtkDistanceWidget::New();
  this->AngleWidget         = vtkAngleWidget::New();
  this->ConventionMatrix    = vtkMatrix4x4::New();
  this->SlicePlane          = vtkPolyData::New();
  this->Command             = vtkImageView2DCommand::New();
  this->OrientationAnnotation = vtkOrientationAnnotation::New();

  this->ImageDisplayMap.insert( std::pair<int, vtkImage2DDisplay *> (0, vtkImage2DDisplay::New()) );

  this->SliceOrientation = vtkImageView2D::SLICE_ORIENTATION_XY;

  // this->ImageActor->SetInput(this->WindowLevel->GetOutput()); // do not set input to prevent VTK warning
    
  this->Command->SetViewer (this);

  this->OrientationAnnotation->SetNonlinearFontScaleFactor (0.25);
  this->OrientationAnnotation->SetTextProperty ( this->TextProperty );

  this->ViewConvention  = vtkImageView2D::VIEW_CONVENTION_RADIOLOGICAL;
  this->ViewOrientation = vtkImageView2D::VIEW_ORIENTATION_AXIAL;
  this->ViewCenter[0] = this->ViewCenter[1] = this->ViewCenter[2] = 0;
  this->Pan[0] = this->Pan[1] = 0.0;
  this->Slice = 0;
  
  this->ConventionMatrix->Zero();
  this->SetViewConvention (vtkImageView2D::VIEW_CONVENTION_RADIOLOGICAL);
  
  this->InitializeSlicePlane();
  this->InitialParallelScale = 1.0;
  this->ShowRulerWidget      = 1;
  this->ShowImageAxis        = 1;
  this->ShowDistanceWidget   = 0;
  this->ShowAngleWidget      = 0;
  this->AnnotationStyle      = AnnotationStyle1;
  this->CursorFollowMouse    = 0;

  this->CornerAnnotation->SetImageActor (this->ImageActor);
  this->CornerAnnotation->ShowSliceAndImageOn();

  this->RulerWidget->KeyPressActivationOff();

  this->DistanceWidget->KeyPressActivationOn();
  this->DistanceWidget->CreateDefaultRepresentation();
  this->DistanceWidget->SetKeyPressActivationValue ('d');
  vtkDistanceRepresentation2D* rep1 = vtkDistanceRepresentation2D::SafeDownCast (this->DistanceWidget->GetRepresentation());
  rep1->GetAxis()->SetTickLength (6);

  this->AngleWidget->KeyPressActivationOn();
  this->AngleWidget->SetKeyPressActivationValue ('o');
  this->AngleWidget->CreateDefaultRepresentation();
  vtkAngleRepresentation2D* rep2 = vtkAngleRepresentation2D::SafeDownCast (this->AngleWidget->GetRepresentation());
  rep2->GetRay1()->GetProperty()->SetColor (0,1,0);
  rep2->GetRay2()->GetProperty()->SetColor (0,1,0);
  rep2->GetArc()->GetProperty()->SetColor (0,1,0);

  this->SetAnnotationsFromOrientation();
  
  vtkInteractorStyle* style = vtkInteractorStyleImageView2D::New();
  this->SetInteractorStyle ( style );
  style->Delete();

  this->SetTransferFunctions(NULL, NULL);

}

//----------------------------------------------------------------------------
vtkImageView2D::~vtkImageView2D()
{
  this->ImageActor->Delete();
  this->SecondImageActor->Delete();
  this->Axes2DWidget->Delete();
  this->RulerWidget->Delete();
  this->DistanceWidget->Delete();
  this->AngleWidget->Delete();
  this->ConventionMatrix->Delete();
  this->SlicePlane->Delete();
  this->Command->Delete();
  this->OrientationAnnotation->Delete();
  for (unsigned int i=0; i<this->DataSetWidgets.size(); i++)
  {
    this->DataSetWidgets[i]->Delete();
  }

}

//----------------------------------------------------------------------------
void vtkImageView2D::GetSliceRange(int &min, int &max)
{
  vtkImageData *input = this->GetInput();
  if (input)
  {
    input->UpdateInformation();
    int *w_ext = input->GetWholeExtent();
    min = w_ext[this->SliceOrientation * 2];
    max = w_ext[this->SliceOrientation * 2 + 1];
  }
}

//----------------------------------------------------------------------------
int* vtkImageView2D::GetSliceRange()
{
  vtkImageData *input = this->GetInput();
  if (input)
  {
    input->UpdateInformation();
    return input->GetWholeExtent() + this->SliceOrientation * 2;
  }
  return NULL;
}

//----------------------------------------------------------------------------
int vtkImageView2D::GetSliceMin() 
{
  int *range = this->GetSliceRange();
  if (range)
  {
    return range[0];
  }
  return 0;
}

//----------------------------------------------------------------------------
int vtkImageView2D::GetSliceMax() 
{
  int *range = this->GetSliceRange();
  if (range)
  {
    return range[1];
  }
  return 0;
}

int vtkImageView2D::GetSlice (void)
{
  return this->Slice;
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetSlice(int slice)
{
  if (!this->GetInput())
  {
    vtkWarningMacro (<<"Input should be set before setting a slice");
    return;
  }
  
  // Stay within range
  int *range = this->GetSliceRange();
  if (range)
  {
    slice = std::max (slice, range[0]);
    slice = std::min (slice, range[1]);
  }

  // Verify slice is actually changing.
  int old_slice = this->GetSlice();
  
  // Estimate the displacement
  double displacement[4] = {0,0,0,0};
  double* spacing = this->GetInput()->GetSpacing();
  displacement[this->SliceOrientation] = (slice - old_slice) * spacing[this->SliceOrientation];
  this->GetOrientationMatrix()->MultiplyPoint (displacement, displacement);

  vtkSmartPointer < vtkTransform > pointTransform = vtkSmartPointer < vtkTransform >::New ();

  pointTransform->Translate ( displacement );

  // Update the instances according to displacement.
  double pos[3];
  pointTransform->TransformPoint ( this->CurrentPoint , pos );
//  for (unsigned int i=0; i<3; i++)
//    pos[i] = this->CurrentPoint[i] + displacement[i];
  // NB: Modified() and InvokeEvent( vtkImageView::CurrentPointChangedEvent ) are
  // callled in SetCurrentPoint()

  // Shift the camera with the slice, so that the slice does not go behind the camera.
  vtkCamera *camera = this->Renderer->GetActiveCamera();
  if (camera)
  {
      // This shifts both the camera position and the focal point. 
      camera->ApplyTransform ( pointTransform );
  }
	
  this->SetCurrentPoint (pos);
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetSliceOrientation(int orientation)
{
  if (orientation < vtkImageView2D::SLICE_ORIENTATION_YZ ||
      orientation > vtkImageView2D::SLICE_ORIENTATION_XY)
  {
    vtkErrorMacro("Error - invalid slice orientation " << orientation);
    return;
  }
  
  if (this->SliceOrientation == orientation)
    return;

  this->SliceOrientation = orientation;

  this->UpdateOrientation();

  // The slice might have changed in the process
  if (this->Input)
  {
    this->Slice = this->GetSliceForWorldCoordinates (this->CurrentPoint);
    this->UpdateDisplayExtent();
    // this->UpdateCenter();
    this->UpdateSlicePlane();
    this->InvokeEvent (vtkImageView2D::SliceChangedEvent);
  }
      
  this->InvokeEvent (vtkImageView2D::OrientationChangedEvent);
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkImageView2D::UpdateOrientation()
{
  // Store zoom and pan :
  double zoom = this->GetZoom();
  double pan[2];
  this->GetPan (pan);
  // Set the camera according to the orientation
  int axis = this->SetCameraFromOrientation ();  
  // The ViewOrientation is "estimated" from SliceOrientation
  this->ViewOrientation = axis;
  // Change Annotations according to orientation
  this->SetAnnotationsFromOrientation();
  // Change axes colors according to orientation
  this->SetSlicePlaneFromOrientation();
  // fix some internal camera settings, do not comment
  this->ResetCamera();
  // finally update the extent as it might have changed
  this->UpdateDisplayExtent();
  // Restore zoom and pan :
  this->SetZoom (zoom);
  this->SetPan (pan);
}

//----------------------------------------------------------------------------
bool CompareExtents ( const int * extentA, const int *extentB )
{
    return ( extentA [0] == extentB [0] ) &&
        ( extentA [1] == extentB [1] ) &&
        ( extentA [2] == extentB [2] ) &&
        ( extentA [3] == extentB [3] ) &&
        ( extentA [4] == extentB [4] ) &&
        ( extentA [5] == extentB [5] ) ;
}
//----------------------------------------------------------------------------
void vtkImageView2D::UpdateDisplayExtent()
{
  if (ImageDisplayMap.size() == 0)
      return;

  vtkImageData * input = ImageDisplayMap.at(0)->GetInput();
  if (!input)
      return;

  input->UpdateInformation();
  int *w_ext = input->GetWholeExtent();
    
    int *w_ext2;
    if (this->SecondImageActor->GetInput()) {
        this->SecondImageActor->GetInput()->UpdateInformation();
        w_ext2 = this->SecondImageActor->GetInput()->GetWholeExtent();
    }

  int slice = this->Slice;
  int *range = this->GetSliceRange();
  if (range)
  {
    slice = std::max (slice, range[0]);
    slice = std::min (slice, range[1]);
  }

  if (slice != this->Slice)
  {
    // vtkWarningMacro (<<"WARNING: asking to display an out of bound extent"<<endl);
  }

  std::map<int,vtkImage2DDisplay *>::iterator it;
  for (it = ImageDisplayMap.begin(); it != ImageDisplayMap.end(); it++)
  {
      switch (this->SliceOrientation)
       {
           case vtkImageView2D::SLICE_ORIENTATION_XY:
               (*it).second->GetImageActor()->SetDisplayExtent(w_ext[0], w_ext[1], w_ext[2], w_ext[3], slice, slice);
               break;

           case vtkImageView2D::SLICE_ORIENTATION_XZ:
               (*it).second->GetImageActor()->SetDisplayExtent(w_ext[0], w_ext[1], slice, slice, w_ext[4], w_ext[5]);
               break;

           case vtkImageView2D::SLICE_ORIENTATION_YZ:
           default:
               (*it).second->GetImageActor()->SetDisplayExtent(slice, slice, w_ext[2], w_ext[3], w_ext[4], w_ext[5]);
               break;
       }

       if ( ! CompareExtents ( input->GetUpdateExtent (), (*it).second->GetImageActor()->GetDisplayExtent () ) ) {

         input->SetUpdateExtent((*it).second->GetImageActor()->GetDisplayExtent ());

         // SetUpdateExtent does not call modified. There is a comment relating to this in
         // vtkDataObject::SetUpdateExtent
         input->Modified ();
         input->PropagateUpdateExtent();
       }
  }
       // Figure out the correct clipping range
       if (this->Renderer)
       {
         if (this->InteractorStyle &&
             this->InteractorStyle->GetAutoAdjustCameraClippingRange())
         {
           this->Renderer->ResetCameraClippingRange();
         }
         else
         {
           vtkCamera *cam = this->Renderer->GetActiveCamera();
           if (cam)
           {
             double bounds[6];
             ImageDisplayMap.at(0)->GetImageActor()->GetBounds(bounds);
             double spos = bounds[this->SliceOrientation * 2];
             double cpos = cam->GetPosition()[this->SliceOrientation];
             double range = fabs(spos - cpos);
             double *spacing = input->GetSpacing();
             double avg_spacing = (spacing[0] + spacing[1] + spacing[2]) / 3.0;
             cam->SetClippingRange(range - avg_spacing * 3.0, range + avg_spacing * 3.0);
           }
         }
  }


//  vtkImageData *input = this->GetInput();
//  if (!input)
//    return;
//
//  input->UpdateInformation();
//  int *w_ext = input->GetWholeExtent();
//
//  // get and clamp the slice if necessary
//  int  slice = this->Slice;
//  // Stay within range
//  int *range = this->GetSliceRange();
//  if (range)
//  {
//    slice = std::max (slice, range[0]);
//    slice = std::min (slice, range[1]);
//  }
//
//  if (slice != this->Slice)
//  {
//    // vtkWarningMacro (<<"WARNING: asking to display an out of bound extent"<<endl);
//  }
//
//  switch (this->SliceOrientation)
//  {
//      case vtkImageView2D::SLICE_ORIENTATION_XY:
//          this->ImageActor->SetDisplayExtent(w_ext[0], w_ext[1], w_ext[2], w_ext[3], slice, slice);
//          this->SecondImageActor->SetDisplayExtent(w_ext[0], w_ext[1], w_ext[2], w_ext[3], slice, slice);
//          break;
//
//      case vtkImageView2D::SLICE_ORIENTATION_XZ:
//          this->ImageActor->SetDisplayExtent(w_ext[0], w_ext[1], slice, slice, w_ext[4], w_ext[5]);
//          this->SecondImageActor->SetDisplayExtent(w_ext[0], w_ext[1], slice, slice, w_ext[4], w_ext[5]);
//          break;
//
//      case vtkImageView2D::SLICE_ORIENTATION_YZ:
//      default:
//          this->ImageActor->SetDisplayExtent(slice, slice, w_ext[2], w_ext[3], w_ext[4], w_ext[5]);
//          this->SecondImageActor->SetDisplayExtent(slice, slice, w_ext[2], w_ext[3], w_ext[4], w_ext[5]);
//          break;
//  }
//
//  if ( ! CompareExtents ( input->GetUpdateExtent (), this->ImageActor->GetDisplayExtent () ) ) {
//
//    input->SetUpdateExtent(this->ImageActor->GetDisplayExtent ());
//
//    // SetUpdateExtent does not call modified. There is a comment relating to this in
//    // vtkDataObject::SetUpdateExtent
//    input->Modified ();
//    input->PropagateUpdateExtent();
//  }
//
//  // Figure out the correct clipping range
//  if (this->Renderer)
//  {
//    if (this->InteractorStyle &&
//        this->InteractorStyle->GetAutoAdjustCameraClippingRange())
//    {
//      this->Renderer->ResetCameraClippingRange();
//    }
//    else
//    {
//      vtkCamera *cam = this->Renderer->GetActiveCamera();
//      if (cam)
//      {
//        double bounds[6];
//        this->ImageActor->GetBounds(bounds);
//        double spos = bounds[this->SliceOrientation * 2];
//        double cpos = cam->GetPosition()[this->SliceOrientation];
//        double range = fabs(spos - cpos);
//        double *spacing = input->GetSpacing();
//        double avg_spacing = (spacing[0] + spacing[1] + spacing[2]) / 3.0;
//        cam->SetClippingRange(range - avg_spacing * 3.0, range + avg_spacing * 3.0);
//      }
//    }
//  }

}


//----------------------------------------------------------------------------
void vtkImageView2D::SetViewConvention(int convention)
{
  if ( (convention < vtkImageView2D::VIEW_CONVENTION_RADIOLOGICAL))
    return;

  this->ViewConvention = convention;

  this->ConventionMatrix->SetElement (2,0, 1);
  this->ConventionMatrix->SetElement (2,1, 1);
  this->ConventionMatrix->SetElement (1,2, -1);

  int x_watcher, y_watcher, z_watcher;

  switch(convention)
  {
      case vtkImageView2D::VIEW_CONVENTION_RADIOLOGICAL:
      default:
	x_watcher =  1;
	y_watcher = -1;
	z_watcher = -1;
	break;
      case vtkImageView2D::VIEW_CONVENTION_NEUROLOGICAL:
	x_watcher = 1;
	y_watcher = 1;
	z_watcher = 1;
	break;
	/// \todo We can define oblique convention points of view
	/// that would match cardiologic conventions.
	/// i.e. - A cardiologist looks a heart from short axis (R.V in the righ of plane)
	/// i.e. - A cardiologist looks a heart from 4 chamber  (R.V in the left of plane)
	/// ...
  }
  this->ConventionMatrix->SetElement (0,3, x_watcher);
  this->ConventionMatrix->SetElement (1,3, y_watcher);
  this->ConventionMatrix->SetElement (2,3, z_watcher);

  this->UpdateOrientation();  
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetViewOrientation(int orientation)
{
  /// \todo: in terms of view orientation here we can add some cardiac specific: short axis, long axis, and 4-chambers !!! exiting !
  if ( (orientation < vtkImageView2D::VIEW_ORIENTATION_SAGITTAL) || orientation == this->ViewOrientation)
    return;

  unsigned int sliceorientation = 0;
  double dot = 0;

  if ( this->GetOrientationMatrix() )
  {
    for (unsigned int i=0; i<3; i++)
    {
      if (dot < std::abs (this->GetOrientationMatrix()->GetElement (orientation, i)))
      {
	dot = std::abs (this->GetOrientationMatrix()->GetElement (orientation, i));
	sliceorientation = i;
      }
    }
  }
  
  this->SetSliceOrientation (sliceorientation);
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetOrientationMatrix (vtkMatrix4x4* matrix)
{
  this->Superclass::SetOrientationMatrix (matrix);
  this->ImageDisplayMap.at(0)->GetImageActor()->SetUserMatrix (this->OrientationMatrix);
  this->UpdateOrientation();
  
  // The slice might have changed in the process
  if (this->Input)
  {
    this->Slice = this->GetSliceForWorldCoordinates (this->CurrentPoint);
    this->UpdateDisplayExtent();
    // this->UpdateCenter();
    this->UpdateSlicePlane();
    this->InvokeEvent (vtkImageView2D::SliceChangedEvent);
  }
  
  this->InvokeEvent (vtkImageView2D::OrientationChangedEvent);
}

//----------------------------------------------------------------------------
void vtkImageView2D::InitializeSlicePlane(void)
{
  vtkPoints* points = vtkPoints::New();
  this->SlicePlane->SetPoints (points);
  points->InsertNextPoint (0, 0, 0);
  points->InsertNextPoint (1, 0, 0);
  points->InsertNextPoint (0, 1, 0);
  points->InsertNextPoint (1, 1, 0);
  this->SlicePlane->Allocate (4);

  vtkIdType pts[3];
  pts[0] = 0; pts[1] = 1; pts[2] = 2;
  this->SlicePlane->InsertNextCell (VTK_TRIANGLE, 3, pts);
  pts[1] = 1; pts[2] = 2; pts[2] = 3;
  this->SlicePlane->InsertNextCell (VTK_TRIANGLE, 3, pts);
  pts[0] = 0; pts[1] = 1; pts[2] = 3;
  this->SlicePlane->InsertNextCell (VTK_TRIANGLE, 3, pts);
  pts[0] = 0; pts[1] = 2; pts[2] = 3;
  this->SlicePlane->InsertNextCell (VTK_TRIANGLE, 3, pts);
  points->Delete();

  vtkUnsignedCharArray* array = vtkUnsignedCharArray::New();
  array->SetName ("Colors");
  array->SetNumberOfComponents (3);
  unsigned char vals[3];
  vals[0] = 255; vals[1] = 0; vals[2] = 0;
  array->InsertNextTupleValue (vals);
  array->InsertNextTupleValue (vals);
  array->InsertNextTupleValue (vals);
  array->InsertNextTupleValue (vals);

  this->SlicePlane->GetPointData()->SetScalars (array);
  array->Delete();
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetCurrentPoint(double pos[3])
{
  this->Superclass::SetCurrentPoint (pos);
		
  int old_slice = this->Slice;
  int new_slice = this->GetSliceForWorldCoordinates (pos);

  if(new_slice != old_slice)
  {
    this->Slice = new_slice;
    this->UpdateDisplayExtent();
    // this->UpdateCenter();
    this->UpdateSlicePlane();
    this->InvokeEvent (vtkImageView2D::SliceChangedEvent);
  }
}

//----------------------------------------------------------------------------
int vtkImageView2D::SetCameraFromOrientation(void)
{

  // We entirely rely on the slice orientation this->SliceOrientation
  // The ViewOrientation is "estimated", returned as id
  
  vtkCamera *cam = this->Renderer ? this->Renderer->GetActiveCamera() : NULL;
  if (!cam)
    return -1;
  
  double position[4] = {0,0,0,0}, focalpoint[4] = {0,0,0,0}, viewup[4];
  double conventionposition[4];
  double conventionview[4];
  double focaltoposition[3];
  std::vector<double*> viewupchoices;
  double first[3], second[3], third[3], fourth[3];
  bool inverseposition;
  
  // At first, we initialize the cam focal point to 0,0,0. (we should do so to the image-center / or the current point)
  // The viewup and the cam position are set according to the convention matrix.
  for (unsigned int i=0; i<3; i++)
    viewup[i]   = this->ConventionMatrix->GetElement (i, this->SliceOrientation);
  position[this->SliceOrientation] = this->ConventionMatrix->GetElement (this->SliceOrientation, 3);
  
  // Recover also information from the convention matrix
  for (unsigned int i=0; i<3; i++)
  {
    conventionposition[i] = this->ConventionMatrix->GetElement (i,3);
    conventionview[i] = this->ConventionMatrix->GetElement (i, this->SliceOrientation);
  }

  // Points VS vectors: uniform coordinates
  position[3] = 1;
  focalpoint[3] = 1;
  conventionposition[3] = 1;
  conventionview[3] = 0;
  viewup[3] = 0;
  
  // Apply the orientation matrix to all this information
  if ( this->OrientationMatrix )
  {
    this->OrientationMatrix->MultiplyPoint (position, position);
    this->OrientationMatrix->MultiplyPoint (focalpoint, focalpoint);
    this->OrientationMatrix->MultiplyPoint (conventionview, conventionview);
    this->OrientationMatrix->MultiplyPoint (conventionposition, conventionposition);
  }

  // Compute the vector perpendicular to the view
  for (unsigned int i=0; i<3; i++)
    focaltoposition[i] = position[i] - focalpoint[i];

  // Deal with the position :
  // invert it if necessary (symetry among the focal point)
  inverseposition = (vtkMath::Dot (focaltoposition, conventionposition) < 0);
  if (inverseposition)
    for (unsigned int i=0; i<3; i++)
      position[i] -= 2*focaltoposition[i];

  // Now we now we have 4 choices for the View-Up information
  for(unsigned int i=0; i<3; i++)
  {
    first[i] = conventionview[i];
    second[i] = -conventionview[i];
  }

  vtkMath::Cross (first, focaltoposition, third);
  vtkMath::Cross (second, focaltoposition, fourth);
  vtkMath::Normalize (third);
  vtkMath::Normalize (fourth);

  viewupchoices.push_back (first);
  viewupchoices.push_back (second);
  viewupchoices.push_back (third);
  viewupchoices.push_back (fourth);

  // To choose between these choices, first we find the axis
  // the closest to the focaltoposition vector
  unsigned int id = 0;
  double dot = 0;
  for (unsigned int i=0; i<3; i++)
  {
    if (dot < std::abs (focaltoposition[i]))
    {
      dot = std::abs (focaltoposition[i]);
      id = i;
    }
  }

  // Then we choose the convention matrix vector correspondant to the
  // one we just found
  for (unsigned int i=0; i<3; i++)
    conventionview[i] = this->ConventionMatrix->GetElement (i, id);

  // Then we pick from the 4 solutions the closest to the
  // vector just found
  unsigned int id2 = 0;
  double dot2 = 0;
  for (unsigned int i=0; i<viewupchoices.size(); i++)
    if (dot2 < vtkMath::Dot (viewupchoices[i], conventionview))
    {
      dot2 = vtkMath::Dot (viewupchoices[i], conventionview);
      id2 = i;
    }

  // We found the solution
  for (unsigned int i=0; i<3; i++)
    viewup[i] = viewupchoices[id2][i];

  cam->SetPosition(position[0], position[1], position[2]);
  cam->SetFocalPoint(focalpoint[0], focalpoint[1], focalpoint[2]);
  cam->SetViewUp(viewup[0], viewup[1], viewup[2]);
  
  this->InvokeEvent (vtkImageView::CameraChangedEvent);

  return id;
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetAnnotationStyle(unsigned int arg)
{
  this->AnnotationStyle = arg;
  this->SetAnnotationsFromOrientation();
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetAnnotationsFromOrientation(void)
{

  // This method has to be called after the camera
  // has been set according to orientation and convention.
  // We rely on the camera settings to compute the oriention
  // annotations.

  std::string solution[4]={"L","P","R","A"};

  vtkCamera *cam = this->Renderer ? this->Renderer->GetActiveCamera() : NULL;
  if (cam)
  {

    std::string matrix[3][2];
    matrix[0][0] = "R";matrix[0][1] = "L";
    matrix[1][0] = "A";matrix[1][1] = "P";
    matrix[2][0] = "I";matrix[2][1] = "S";

    ///\todo surely there is a simpler way to do all of that !
    
    double* viewup = cam->GetViewUp();
    double* normal = cam->GetViewPlaneNormal();
    double rightvector[3];
    vtkMath::Cross (normal, viewup, rightvector);
    
    unsigned int id1 = 0;
    unsigned int id2 = 0;
    unsigned int id3 = 0;
    double dot1 = 0;
    double dot2 = 0;
    double dot3 = 0;
    
    for (unsigned int i=0; i<3; i++)
    {
      if (dot1 <= std::abs (viewup[i]))
      {
	dot1 = std::abs (viewup[i]);
	id1 = i;
      }
      if (dot2 <= std::abs (rightvector[i]))
      {
	dot2 = std::abs (rightvector[i]);
	id2 = i;
    }
      if (dot3 <= std::abs (normal[i]))
      {
	dot3 = std::abs (normal[i]);
	id3 = i;
      }
    }
    
    if (viewup[id1] > 0)
    {
      solution[3] = matrix[id1][0];
      solution[1] = matrix[id1][1];
    } else {
      solution[3] = matrix[id1][1];
      solution[1] = matrix[id1][0];
    }
    if (rightvector[id2] > 0)
    {
      solution[0] = matrix[id2][0];
      solution[2] = matrix[id2][1];
    } else {
      solution[0] = matrix[id2][1];
      solution[2] = matrix[id2][0];
    }
  }
  
  for (unsigned int i=0; i<4; i++)
    this->OrientationAnnotation->SetText (i, solution[i].c_str());
  
  std::ostringstream osNW;
  std::ostringstream osSW;
  std::ostringstream osSE;
  std::ostringstream osNE;

  switch(this->AnnotationStyle)
  {
      case AnnotationStyle2:
	osNW << "<size>\n"
	     << "<spacing>\n"
	     << "<xyz>\n"
	     << "<value>\n";
	osSW << "<zoom>";
	osNE << "<slice_and_max>\n<window>\n<level>";
	break;

      case AnnotationStyle1:
      default:
	
	osSW << "<zoom>\n";
	osSW << "<slice_and_max>\n";
	osNW<< "Image size: " << "<size_x>x<size_y>\n";
	osNW<< "Voxel size: " << "<spacing_x>x<spacing_y>\n";
	osNW<< "X: " << "<coord_x>" << " px Y: " << "<coord_y>" << " px " << "<value>\n";
	osNW<< "X: " << "<pos_x>" << " mm Y: " << "<pos_y> mm\n";
	osSW<< "Location: " << "<pos_z>" << " mm";
	osNW<< "<window_level>";
	osNE << "<patient>\n<study>\n<series>";
  
	switch( this->ViewOrientation )
	{
	    case vtkImageView2D::VIEW_ORIENTATION_AXIAL:
	      osSE << "Axial View";
	      break;
	    case vtkImageView2D::VIEW_ORIENTATION_CORONAL:
	      osSE << "Coronal View";
	      break;
	    case vtkImageView2D::VIEW_ORIENTATION_SAGITTAL:
	      osSE << "Sagittal View";
	      break;
	}
	break;
  }

  this->GetCornerAnnotation()->SetText (2, osNW.str().c_str());
  this->GetCornerAnnotation()->SetText (1, osSE.str().c_str());
  this->GetCornerAnnotation()->SetText (0, osSW.str().c_str());
  this->GetCornerAnnotation()->SetText (3, osNE.str().c_str());
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetSlicePlaneFromOrientation(void)
{
  if (this->ViewOrientation<VIEW_ORIENTATION_SAGITTAL || this->ViewOrientation>VIEW_ORIENTATION_AXIAL)
    return;
  /**
     These lines tell the slice plane which color it should be
     ///\todo We should allow more colors...
  */
  unsigned char vals[3] = {0,0,0};
  vals[this->ViewOrientation] = 255;
  vtkUnsignedCharArray* array = vtkUnsignedCharArray::SafeDownCast (this->SlicePlane->GetPointData()->GetScalars());
  if (!array)
    return;
  array->SetTupleValue (0, vals);
  array->SetTupleValue (1, vals);
  array->SetTupleValue (2, vals);
  array->SetTupleValue (3, vals);

  this->UpdateSlicePlane();

}

//----------------------------------------------------------------------------
void vtkImageView2D::UpdateSlicePlane (void)
{

  if( !this->GetInput() ) // if input is not set yet, no way we can now the display bounds
    return;
  
  vtkPoints* oldpoints = vtkPoints::New();
  vtkPoints* points = vtkPoints::New();
  double x[3];
  double* bounds = this->ImageActor->GetDisplayBounds ();
  unsigned int added1;
  unsigned int added2;

  for (unsigned int i=0; i<4; i++)
  {
    added1 = (!(i%2)) ? 1 : 0;
    added2 = (i<2) ? 1 : 0;
    x[(this->SliceOrientation+1)%3] = bounds[2*((this->SliceOrientation+1)%3) + added1];
    x[(this->SliceOrientation+2)%3] = bounds[2*((this->SliceOrientation+2)%3) + added2];
    x[this->SliceOrientation] = bounds[2*this->SliceOrientation];
    oldpoints->InsertPoint(i,x);
  }
  this->OrientationTransform->TransformPoints (oldpoints, points);
  this->SlicePlane->SetPoints (points);
  oldpoints->Delete();
  points->Delete();
}

//----------------------------------------------------------------------------
void vtkImageView2D::UpdateCenter (void)
{
  if (!this->GetInput())
    return;
  vtkCamera *cam = this->Renderer ? this->Renderer->GetActiveCamera() : NULL;
  if (!cam)
    return;
  int* dimensions = this->GetInput()->GetDimensions();

  int indices[3] = {0,0,0};
  for (unsigned int i=0; i<3; i++)
  {
    indices[i] = (int)((double)dimensions[i] / 2.0);
  }
  indices[this->SliceOrientation] = this->GetSlice();

  this->GetWorldCoordinatesFromImageCoordinates (indices, this->ViewCenter);
}

//----------------------------------------------------------------------------
int vtkImageView2D::GetSliceForWorldCoordinates(double pos[3])
{
  int indices[3];
  this->GetImageCoordinatesFromWorldCoordinates (pos, indices);
  return indices[this->SliceOrientation];
}

//----------------------------------------------------------------------------
void vtkImageView2D::GetWorldCoordinatesForSlice(int slice, double* position)
{
  int indices[3] = {0,0,0};
  indices[this->SliceOrientation] = slice;
  this->GetWorldCoordinatesFromImageCoordinates (indices, position);
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetPan (double* arg)
{
  this->Pan[0] = arg[0];
  this->Pan[1] = arg[1];
  
  // If WorldToDisplay is called when RenderWindow has never been rendered, nans are returned
  // Reason: worldtodisplay needs to now the window size, which detected when the vtk renderwindow
  // has been rendered at least once
  vtkCamera *camera = this->Renderer ? this->Renderer->GetActiveCamera() : NULL;
  if (!camera || !this->Renderer || !this->RenderWindow || this->RenderWindow->GetNeverRendered())
  {
    return;
  }
  
  double focaldepth, focalpoint[4], position[4], motion[4];
  double bounds[6];
  double center[3];

  camera->GetFocalPoint (focalpoint);
  camera->GetPosition(position);

  this->Renderer->SetWorldPoint (focalpoint[0], focalpoint[1], focalpoint[2], 1.0);
  this->Renderer->WorldToDisplay();
  focaldepth = this->Renderer->GetDisplayPoint()[2];
    
  this->Renderer->ComputeVisiblePropBounds (bounds);
  center[0] = (bounds[0] + bounds[1])/2.0;
  center[1] = (bounds[2] + bounds[3])/2.0;
  center[2] = (bounds[4] + bounds[5])/2.0;
    
  this->Renderer->SetWorldPoint (center[0], center[1], center[2], 1.0);
  this->Renderer->WorldToDisplay();
  this->Renderer->GetDisplayPoint (center);
  center[0] += this->Pan[0];
  center[1] += this->Pan[1];
    
  this->Renderer->SetDisplayPoint (center[0], center[1], focaldepth);  
  this->Renderer->DisplayToWorld();
    
  // Camera motion is reversed    
  motion[0] = focalpoint[0] - this->Renderer->GetWorldPoint()[0];
  motion[1] = focalpoint[1] - this->Renderer->GetWorldPoint()[1];
  motion[2] = focalpoint[2] - this->Renderer->GetWorldPoint()[2];
    
  camera->SetFocalPoint(- motion[0] + focalpoint[0],
			- motion[1] + focalpoint[1],
			- motion[2] + focalpoint[2]);
  camera->SetPosition(- motion[0] + position[0],
		      - motion[1] + position[1],
		      - motion[2] + position[2]);
         
  if (this->Interactor && this->Interactor->GetLightFollowCamera()) 
  {
    this->Renderer->UpdateLightsGeometryToFollowCamera();
  }

  this->InvokeEvent (vtkImageView::PanChangedEvent);
  this->Modified();
}

//----------------------------------------------------------------------------
double* vtkImageView2D::GetPan ()
{
  this->GetPan (this->Pan);
  return this->Pan;
}

//----------------------------------------------------------------------------
void vtkImageView2D::GetPan (double pan[2])
{
  vtkCamera *camera = this->Renderer ? this->Renderer->GetActiveCamera() : NULL;
  if (!camera || !this->Renderer || !this->RenderWindow || this->RenderWindow->GetNeverRendered())
  {
    pan[0] = this->Pan[0];
    pan[1] = this->Pan[1];
    return;
  }

  double bounds[6], center[3], viewFocus[4];
  
  camera->GetFocalPoint(viewFocus);
  this->Renderer->ComputeVisiblePropBounds (bounds);
  
  center[0] = (bounds[0] + bounds[1])/2.0;
  center[1] = (bounds[2] + bounds[3])/2.0;
  center[2] = (bounds[4] + bounds[5])/2.0;
  
  this->Renderer->SetWorldPoint (center[0], center[1], center[2], 0.0);
  this->Renderer->WorldToDisplay();
  this->Renderer->GetDisplayPoint (center);
  
  this->Renderer->SetWorldPoint (viewFocus[0], viewFocus[1], viewFocus[2], 1.0);
  this->Renderer->WorldToDisplay();
  this->Renderer->GetDisplayPoint (viewFocus);
  
  pan[0] = -center[0] + viewFocus[0];
  pan[1] = -center[1] + viewFocus[1];
}

//----------------------------------------------------------------------------
void vtkImageView2D::ResetCamera (void)
{
  this->Superclass::ResetCamera();
  this->SetZoom (1.0);
  this->Pan[0] = this->Pan[1] = 0.0;
  this->SetPan (this->Pan); // not sure this is needed
}

//----------------------------------------------------------------------------
void vtkImageView2D::Reset (void)
{
  this->Superclass::Reset();
  this->UpdateDisplayExtent();
}
//----------------------------------------------------------------------------
void vtkImageView2D::SetShowAnnotations (int val)
{
  this->Superclass::SetShowAnnotations (val);
  this->OrientationAnnotation->SetVisibility (val);
}

//----------------------------------------------------------------------------
void vtkImageView2D::GetWorldCoordinatesFromDisplayPosition (int xy[2], double* position)
{
  vtkCamera *cam = this->Renderer ? this->Renderer->GetActiveCamera() : NULL;
  if (!cam)
  {
    position[0] = position[1] = position[2] = 0;
    return;
  }
  
  this->GetRenderer()->SetDisplayPoint (xy[0], xy[1], 0);
  this->GetRenderer()->DisplayToWorld();
  vtkPlane::ProjectPoint(this->Renderer->GetWorldPoint(), this->CurrentPoint, cam->GetViewPlaneNormal(), position);
}

//----------------------------------------------------------------------------
void vtkImageView2D::InstallPipeline()
{
  this->Superclass::InstallPipeline();

  if (this->Renderer)
  {
    this->Renderer->AddViewProp( this->ImageActor );
    this->Renderer->AddViewProp( this->OrientationAnnotation );
    this->Renderer->GetActiveCamera()->ParallelProjectionOn();
  }

  if( this->InteractorStyle )
  {
    if ( !this->InteractorStyle->HasObserver (vtkImageView2DCommand::SliceMoveEvent, this->Command) )
      this->InteractorStyle->AddObserver (vtkImageView2DCommand::SliceMoveEvent, this->Command, 10);

	if ( !this->InteractorStyle->HasObserver (vtkImageView2DCommand::TimeChangeEvent, this->Command) )
	  this->InteractorStyle->AddObserver (vtkImageView2DCommand::TimeChangeEvent, this->Command, 10);

    if ( !this->InteractorStyle->HasObserver (vtkImageView2DCommand::RequestedPositionEvent, this->Command) )
      this->InteractorStyle->AddObserver (vtkImageView2DCommand::RequestedPositionEvent, this->Command, 10);

    if ( !this->InteractorStyle->HasObserver (vtkImageView2DCommand::ResetViewerEvent, this->Command) )
      this->InteractorStyle->AddObserver (vtkImageView2DCommand::ResetViewerEvent, this->Command, 10);

    if ( !this->InteractorStyle->HasObserver (vtkImageView2DCommand::StartWindowLevelEvent, this->Command) )
      this->InteractorStyle->AddObserver (vtkImageView2DCommand::StartWindowLevelEvent, this->Command, 10);

    if ( !this->InteractorStyle->HasObserver (vtkImageView2DCommand::WindowLevelEvent, this->Command) )
      this->InteractorStyle->AddObserver (vtkImageView2DCommand::WindowLevelEvent, this->Command, 10);

    if ( !this->InteractorStyle->HasObserver (vtkImageView2DCommand::CharEvent, this->Command) )
      this->InteractorStyle->AddObserver (vtkImageView2DCommand::CharEvent, this->Command, 10);
    
    if( this->CursorFollowMouse && !this->InteractorStyle->HasObserver (vtkImageView2DCommand::DefaultMoveEvent, this->Command) )
      this->InteractorStyle->AddObserver (vtkImageView2DCommand::DefaultMoveEvent, this->Command, 10);

    if( !this->InteractorStyle->HasObserver (vtkImageView2DCommand::CameraZoomEvent, this->Command) )
      this->InteractorStyle->AddObserver (vtkImageView2DCommand::CameraZoomEvent, this->Command, 10);

    // if( !this->InteractorStyle->HasObserver (vtkImageView2DCommand::CameraPanEvent, this->Command) )
    //   this->InteractorStyle->AddObserver (vtkImageView2DCommand::CameraPanEvent, this->Command, 10);
  }

}

//----------------------------------------------------------------------------
void vtkImageView2D::UnInstallPipeline()
{
  if ( this->GetRenderer() )
  {
    this->GetRenderer()->RemoveViewProp ( this->ImageActor );
    this->GetRenderer()->RemoveViewProp ( this->OrientationAnnotation );
    //this->ImageActor->SetInput (NULL);
  }

  if( this->InteractorStyle )
  {/*
    this->InteractorStyle->RemoveObservers(vtkImageView2DCommand::SliceMoveEvent);
    this->InteractorStyle->RemoveObservers(vtkImageView2DCommand::RequestedPositionEvent);
    this->InteractorStyle->RemoveObservers(vtkImageView2DCommand::ResetViewerEvent);
    this->InteractorStyle->RemoveObservers(vtkImageView2DCommand::StartWindowLevelEvent);
    this->InteractorStyle->RemoveObservers(vtkImageView2DCommand::WindowLevelEvent);
    this->InteractorStyle->RemoveObservers(vtkImageView2DCommand::CharEvent);
    this->InteractorStyle->RemoveObservers(vtkImageView2DCommand::DefaultMoveEvent);
    this->InteractorStyle->RemoveObservers(vtkImageView2DCommand::CameraZoomEvent);
    this->InteractorStyle->RemoveObservers(vtkImageView2DCommand::CameraPanEvent);
   */
    this->InteractorStyle->RemoveObserver ( this->Command );
  }
  

  this->Superclass::UnInstallPipeline();
}

//----------------------------------------------------------------------------
void vtkImageView2D::InstallInteractor()
{
  if (this->Interactor && this->InteractorStyle)
  {
    this->Interactor->SetInteractorStyle ( this->InteractorStyle );
  }

  if( this->Interactor && this->RenderWindow)
  {    
    this->Interactor->SetRenderWindow   (this->RenderWindow);
    this->RulerWidget->SetInteractor    (this->GetInteractor());
    this->DistanceWidget->SetInteractor (this->GetInteractor());
    this->AngleWidget->SetInteractor    (this->GetInteractor());
  }

  
  if ( this->RulerWidget->GetInteractor() && this->ShowRulerWidget)
    this->RulerWidget->On();
  if ( this->DistanceWidget->GetInteractor() && this->ShowDistanceWidget)
    this->DistanceWidget->On();
  if ( this->AngleWidget->GetInteractor() && this->ShowAngleWidget)
    this->AngleWidget->On();

  this->Axes2DWidget->SetImageView (this);
  if( this->ShowImageAxis && this->RenderWindow && this->Renderer)
    this->Axes2DWidget->On();
  
  for (unsigned int i=0; i<this->DataSetWidgets.size(); i++)
  {
    this->DataSetWidgets[i]->SetImageView(this);
    this->DataSetWidgets[i]->On();
  }
	
  this->IsInteractorInstalled = 1;
}


//----------------------------------------------------------------------------
void vtkImageView2D::UnInstallInteractor()
{
  this->RulerWidget->SetInteractor ( NULL );
  this->DistanceWidget->SetInteractor ( NULL );
  this->AngleWidget->SetInteractor ( NULL );
  this->Axes2DWidget->SetImageView ( NULL );

  if (this->Interactor)
  {
    this->Interactor->SetRenderWindow (NULL);
    this->Interactor->SetInteractorStyle (NULL);
  }
	
  for (unsigned int i=0; i<this->DataSetWidgets.size(); i++)
  {
    this->DataSetWidgets[i]->SetImageView(0);
  }
	
  this->IsInteractorInstalled = 0;
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetInterpolate(int val)
{
  this->ImageActor->SetInterpolate (val);
  this->Modified();
}

//----------------------------------------------------------------------------
int vtkImageView2D::GetInterpolate(void)
{
  return this->ImageActor->GetInterpolate();
}

void vtkImageView2D::SetTransferFunctions(vtkColorTransferFunction* color, vtkPiecewiseFunction *opacity)
{
    this->Superclass::SetTransferFunctions(color, opacity);

    if (this->ImageDisplayMap.size() != 0)
    {
        this->ImageDisplayMap.at(0)->GetWindowLevel()->SetLookupTable(this->GetColorTransferFunction());
    }
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetInput (vtkImageData *image, vtkMatrix4x4 *matrix, int layer)
{
    if (layer==0) {
        this->Superclass::SetInput( image, matrix, layer);

        if (image)
            image->UpdateInformation(); // must be called before GetSliceForWorldCoordinates()

//        this->ImageDisplayMap.insert(std::pair<int, vtkImage2DDisplay*>(layer, vtkImage2DDisplay::New()));
//        ImageDisplayMap.at(0)->SetInput(image);
//        // The slice might have changed in the process
//        if (this->ImageDisplayMap.at(0)->GetInput())
//        {
//            this->ImageDisplayMap.at(0)->GetInput()->UpdateInformation();
//            this->Slice = this->GetSliceForWorldCoordinates (this->CurrentPoint);
//            this->UpdateDisplayExtent();
//            // this->UpdateCenter();
//            this->UpdateSlicePlane();
//            this->InvokeEvent (vtkImageView2D::SliceChangedEvent);
//        }
    }


//    if (ImageDisplayMap.size() == 0)
//        ImageDisplayMap.insert(std::pair<int, vtkImage2DDisplay*>(layer, vtkImage2DDisplay::New()));

//    if ( !ImageDisplayMap.at(layer))
        ImageDisplayMap.insert(std::pair<int, vtkImage2DDisplay*>(layer, vtkImage2DDisplay::New()));

//        ImageDisplayMap.insert(layer, vtkImage2DDisplay::New());

//    std::map<int,vtkImage2DDisplay *>::iterator it;
  //  for (it = ImageDisplayMap.begin(); it != ImageDisplayMap.end(); it++)
    //{
        vtkRenderer * renderer = 0;
        if (layer == 0)
        {
            ImageDisplayMap.at(layer)->SetInput(image);
            renderer = this->GetRenderer(); //vtkRenderer::New();
//            vtkLookupTable *lut = vtkLookupTableManager::GetGrayRainbowLookupTable();
//            lut->SetTableRange(0.0, 3.0);
//            ImageDisplayMap.at(layer)->SetLookupTable(lut);
        }
        else
        {
            vtkMatrix4x4 *auxMatrix = vtkMatrix4x4::New();
            if (matrix)
                auxMatrix->DeepCopy(matrix);
            else {
                auxMatrix->Identity();
            }
            
            
            vtkMatrix4x4::Multiply4x4(this->InvertOrientationMatrix, auxMatrix, auxMatrix);
            
            double origin[4], origin2[4];
            this->GetInput()->GetOrigin(origin);
            image->GetOrigin(origin2);
            
            for (int i=0; i<3; i++)
                origin2[i] -= origin[i];
            origin2[3] = 0.0;
            
            this->InvertOrientationMatrix->MultiplyPoint (origin2, origin2);
            
            for (int i=0; i<3; i++)
            {
                auxMatrix->SetElement(i, 3, origin2[i]);
            }
            
            int *w_ext = image->GetWholeExtent();
            double ext_min[4], ext_max[4];
            for (int i=0; i<3; i++)
            {
                ext_min[i] = static_cast<double> (w_ext[i*2]);
                ext_max[i] = static_cast<double> (w_ext[i*2+1]);
            }
            ext_min[3] = 1.0;
            ext_max[3] = 1.0;
            
            auxMatrix->MultiplyPoint(ext_min, ext_min);
            auxMatrix->MultiplyPoint(ext_max, ext_max);
            
            int new_extent[6];
            
            int *eextent = this->GetInput()->GetWholeExtent();
            
            for (int i=0; i<3; i++)
            {
                new_extent[i*2] = vtkMath::Round(ext_min[i]);
                new_extent[i*2+1] = vtkMath::Round(ext_max[i]);
                
                if (new_extent[i*2]>eextent[i*2])
                    new_extent[i*2] = eextent[i*2];
                
                if (new_extent[i*2+1]<eextent[i*2+1])
                    new_extent[i*2+1] = eextent[i*2+1];
            }
            
            vtkImageReslice *reslice = vtkImageReslice::New();
            reslice->SetInput(image);
            reslice->SetResliceAxes(auxMatrix);
            reslice->SetOutputOrigin(this->GetInput()->GetOrigin());
            reslice->SetOutputSpacing(this->GetInput()->GetSpacing());
            reslice->SetOutputExtent(new_extent);
            
            auxMatrix->Delete();
            //reslice->Update();
            
            ImageDisplayMap.at(layer)->SetInput(reslice->GetOutput());
            
            this->GetRenderWindow()->SetNumberOfLayers(ImageDisplayMap.size());
            renderer = vtkRenderer::New();
            renderer->SetLayer(layer);
            this->GetRenderWindow()->AddRenderer(renderer);
            renderer->SetActiveCamera(this->GetRenderer()->GetActiveCamera());
            vtkLookupTable *lut = vtkLookupTableManager::GetSpectrumLookupTable();
            lut->SetTableRange(0.0, 1000.0);
            lut->SetAlphaRange(0.0, 1.0);
            ImageDisplayMap.at(layer)->SetLookupTable(lut);
            ImageDisplayMap.at(layer)->GetImageActor()->SetOpacity(1.0);
            ImageDisplayMap.at(layer)->GetImageActor()->SetUserMatrix (this->OrientationMatrix);
        }
    
    renderer->AddViewProp(ImageDisplayMap.at(layer)->GetImageActor());

    ImageDisplayMap.at(layer)->GetInput()->UpdateInformation();
    this->Slice = this->GetSliceForWorldCoordinates (this->CurrentPoint);
    this->UpdateDisplayExtent();
    // this->UpdateCenter();
    this->UpdateSlicePlane();
    this->InvokeEvent (vtkImageView2D::SliceChangedEvent);

//    if (layer==0) {
//        this->Superclass::SetInput( image );
//
//        if (image)
//            image->UpdateInformation(); // must be called before GetSliceForWorldCoordinates()
//
//        this->ImageActor->SetInput( this->WindowLevel->GetOutput() );
//
//        // The slice might have changed in the process
//        if (this->Input)
//        {
//            this->Input->UpdateInformation();
//            this->Slice = this->GetSliceForWorldCoordinates (this->CurrentPoint);
//            this->UpdateDisplayExtent();
//            // this->UpdateCenter();
//            this->UpdateSlicePlane();
//            this->InvokeEvent (vtkImageView2D::SliceChangedEvent);
//        }
//    }
//    else { // layer >0
//        if (this->GetRenderWindow())
//        {
//            /*
//            this->GetRenderWindow()->SetNumberOfLayers( layer+1 );
//            vtkRenderer *renderer = vtkRenderer::New();
//            renderer->SetLayer(layer);
//            renderer->SetActiveCamera( this->GetRenderer()->GetActiveCamera() );
//
//            this->GetRenderWindow()->AddRenderer( renderer );
//            renderer->Delete();
//            */
//            vtkImageMapToColors *color = vtkImageMapToColors::New();
//            color->SetInput(image);
//            color->SetOutputFormatToRGB();
//            vtkLookupTable *lut = vtkLookupTableManager::GetSpectrumLookupTable();
//            lut->SetTableRange(0.0, 3.0);
//            color->SetLookupTable(lut);
//            color->Update();
//
//            this->SecondImageActor->SetInput( color->GetOutput() );
//            //renderer->AddViewProp( this->SecondImageActor );
//            this->Renderer->AddViewProp( this->SecondImageActor );
//            this->UpdateDisplayExtent();
//            color->Delete();
//        }
//    }
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetInputConnection (vtkAlgorithmOutput *input, vtkMatrix4x4 *matrix, int layer)
{
  this->Superclass::SetInputConnection( input, matrix, layer);
  this->ImageActor->SetInput( this->WindowLevel->GetOutput() );

  // The slice might have changed in the process
  if (this->Input)
  {
    this->Slice = this->GetSliceForWorldCoordinates (this->CurrentPoint);
    this->UpdateDisplayExtent();
    // this->UpdateCenter();
    this->UpdateSlicePlane();
    this->InvokeEvent (vtkImageView2D::SliceChangedEvent);
  }
}

vtkImageData *vtkImageView2D::GetImageInput(int layer) const
{
    std::map<int, vtkImage2DDisplay*>::const_iterator it = this->ImageDisplayMap.begin();
    while(it!=this->ImageDisplayMap.end())
    {
        if (it->first==layer)
            return it->second->GetInput();
        ++it;
    }
    
    return NULL;
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetShowRulerWidget (int val)
{
  this->ShowRulerWidget = val;
  if ( this->RulerWidget->GetInteractor() )
  {
    if( this->ShowRulerWidget )
      this->RulerWidget->On();
    else
      this->RulerWidget->Off();

    this->Modified();
  }
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetShowDistanceWidget (int val)
{
  this->ShowDistanceWidget = val;
  if ( this->DistanceWidget->GetInteractor() )
  {
    if( this->ShowDistanceWidget )
      this->DistanceWidget->On();
    else
      this->DistanceWidget->Off();

    this->Modified();
  }
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetShowAngleWidget (int val)
{
  this->ShowAngleWidget = val;
  if ( this->AngleWidget->GetInteractor() )
  {
    if( this->ShowAngleWidget )
      this->AngleWidget->On();
    else
      this->AngleWidget->Off();

    this->Modified();
  }
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetShowImageAxis (int val)
{
  this->ShowImageAxis = val;
  if (this->Axes2DWidget->GetImageView() && this->RenderWindow && this->Renderer)
  {
    this->Axes2DWidget->SetEnabled(val);
    this->Modified();
  }
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetCursorFollowMouse (int val)
{
  this->CursorFollowMouse = val;
  if( this->GetInteractorStyle() )
  {
    if( val && !this->GetInteractorStyle()->HasObserver (vtkImageView2DCommand::DefaultMoveEvent, this->Command))
      this->InteractorStyle->AddObserver (vtkImageView2DCommand::DefaultMoveEvent, this->Command);
    else
      this->InteractorStyle->RemoveObservers (vtkImageView2DCommand::DefaultMoveEvent);
  }
  this->Modified();
}
  
//----------------------------------------------------------------------------
void vtkImageView2D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "RenderWindow:\n";
  this->RenderWindow->PrintSelf(os,indent.GetNextIndent());
  os << indent << "Renderer:\n";
  this->Renderer->PrintSelf(os,indent.GetNextIndent());
  os << indent << "ImageActor:\n";
  this->ImageActor->PrintSelf(os,indent.GetNextIndent());
  os << indent << "WindowLevel:\n" << endl;
  this->WindowLevel->PrintSelf(os,indent.GetNextIndent());
  os << indent << "Slice: " << this->GetSlice() << endl;
  os << indent << "SliceOrientation: " << this->SliceOrientation << endl;
  os << indent << "InteractorStyle: " << endl;
  if (this->InteractorStyle)
    {
    os << "\n";
    this->InteractorStyle->PrintSelf(os,indent.GetNextIndent());
    }
  else
    {
    os << "None";
    }
}
  
//----------------------------------------------------------------------------
vtkActor* vtkImageView2D::AddDataSet(vtkPointSet* arg, vtkProperty* prop)
{
  vtkDataSet2DWidget* widget = vtkDataSet2DWidget::New();
  widget->GetActor()->SetProperty (prop);
  widget->SetSource (arg);
  widget->SetImageView (this);

  if ( this->GetIsInteractorInstalled () )  {

    widget->On();
  }
  this->DataSetWidgets.push_back( widget );

  return widget->GetActor();
}
