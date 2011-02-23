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

class vtkImage2DDisplay : public vtkObject
{
public:
  static vtkImage2DDisplay * New();
  vtkTypeRevisionMacro (vtkImage2DDisplay, vtkObject);
  
  virtual void SetInput(vtkImageData * image);
  vtkGetObjectMacro(Input, vtkImageData);
  
  virtual void SetLookupTable(vtkLookupTable * lut);
  
  vtkGetObjectMacro(ImageActor, vtkImageActor);
  
  vtkGetObjectMacro(WindowLevel, vtkImageMapToColors);
  
protected:
  vtkImage2DDisplay();
  ~vtkImage2DDisplay();    
  
private:
  vtkImageMapToColors*            WindowLevel;
  vtkImageData*                   Input;
  vtkImageActor*                  ImageActor;

  vtkImage2DDisplay (const vtkImage2DDisplay&);
  void operator=(const vtkImage2DDisplay&);
};

vtkCxxRevisionMacro(vtkImage2DDisplay, "$Revision: $");
vtkStandardNewMacro(vtkImage2DDisplay);

vtkImage2DDisplay::vtkImage2DDisplay()
{
  this->Input = 0;
  this->ImageActor        = vtkImageActor::New();
  this->WindowLevel       = vtkImageMapToColors::New();
  this->WindowLevel->SetOutputFormatToRGBA();
}

vtkImage2DDisplay::~vtkImage2DDisplay()
{
  this->WindowLevel->Delete();
  this->ImageActor->Delete();
}

void vtkImage2DDisplay::SetInput(vtkImageData * image)
{
  this->Input = image;
  
  if (image)
    image->UpdateInformation();
  
  this->ImageActor->SetInput( this->WindowLevel->GetOutput() );
  this->WindowLevel->SetInput(image);
}

void vtkImage2DDisplay::SetLookupTable(vtkLookupTable * lut)
{
  this->WindowLevel->SetLookupTable(lut);
}



vtkCxxRevisionMacro(vtkImageView2D, "$Revision: 3 $");
vtkStandardNewMacro(vtkImageView2D);

//----------------------------------------------------------------------------
vtkImageView2D::vtkImageView2D()
{
  this->Axes2DWidget        = vtkAxes2DWidget::New();
  this->RulerWidget         = vtkRulerWidget::New();
  this->DistanceWidget      = vtkDistanceWidget::New();
  this->AngleWidget         = vtkAngleWidget::New();
  this->ConventionMatrix    = vtkMatrix4x4::New();
  this->SlicePlane          = vtkPolyData::New();
  this->Command             = vtkImageView2DCommand::New();
  this->OrientationAnnotation = vtkOrientationAnnotation::New();
  
  this->ImageDisplayMap.insert (std::pair<int, vtkImage2DDisplay *> (0, vtkImage2DDisplay::New()));
  this->RendererMap.insert     (std::pair<int, vtkRenderer*>        (0, NULL));
  
  this->SliceOrientation = vtkImageView2D::SLICE_ORIENTATION_XY;
  
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
  
  this->CornerAnnotation->SetImageActor (this->GetImageActor(0));
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
  
  this->SetTransferFunctions(NULL, NULL, 0);
}

//----------------------------------------------------------------------------
vtkImageView2D::~vtkImageView2D()
{
  std::map<int, vtkImage2DDisplay*>::iterator it = this->ImageDisplayMap.begin();
  while (it!=this->ImageDisplayMap.end())
  {
    if (it->second)
    {
      it->second->Delete();
    }
    ++it;
  }
  this->ImageDisplayMap.clear();

  std::map<int, vtkRenderer*>::iterator it2 = this->RendererMap.begin();
  while (it2!=this->RendererMap.end())
  {
    if (vtkRenderer *renderer = it2->second)
    {
      renderer->Delete();
    }
    ++it2;
  }
  this->RendererMap.clear();

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

void vtkImageView2D::SetVisibility(int visible, int layer)
{
  if (this->HasLayer(layer))
    this->ImageDisplayMap.at(layer)->GetImageActor()->SetVisibility(visible);
}

int vtkImageView2D::GetVisibility(int layer)
{
  if (this->HasLayer(layer))
    return this->ImageDisplayMap.at(layer)->GetImageActor()->GetVisibility();
  
  return 0;
}

void vtkImageView2D::SetOpacity(double opacity, int layer)
{
  if (this->HasLayer(layer))
    this->ImageDisplayMap.at(layer)->GetImageActor()->SetOpacity(opacity);
}

double vtkImageView2D::GetOpacity(int layer)
{
  if (this->HasLayer(layer))
    return this->ImageDisplayMap.at(layer)->GetImageActor()->GetOpacity();
  
  return 0.0;
}

//----------------------------------------------------------------------------
unsigned long vtkImageView2D::GetMTime()
{
    typedef unsigned long MTimeType;

    MTimeType mTime = Superclass::GetMTime();

    vtkObject * objectsToInclude[] = {
        this->GetImageActor(0),
        this->Axes2DWidget,
        this->RulerWidget,
        this->DistanceWidget,
        this->AngleWidget,
        this->ConventionMatrix,
        this->SlicePlane,
        this->OrientationAnnotation };

        const int numObjects = sizeof(objectsToInclude) / sizeof(vtkObject *);

        for ( int i(0); i<numObjects; ++i ) {
            vtkObject * object = objectsToInclude[i];
            if (object) {
                const MTimeType testMtime = object->GetMTime();
                if ( testMtime > mTime )
                    mTime = testMtime;
            }
        }
        return mTime;
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

//----------------------------------------------------------------------------
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
  if (this->ImageDisplayMap.size()==0)
    return;
  
  vtkImageData * input = this->ImageDisplayMap.at(0)->GetInput();
  if (!input)
    return;
  
  input->UpdateInformation();
  
  int *w_ext = input->GetWholeExtent();
  
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
  for (it = this->ImageDisplayMap.begin(); it != this->ImageDisplayMap.end(); it++)
  {
    vtkImageData *imageInput = (*it).second->GetInput();
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
    
    if ( ! CompareExtents ( imageInput->GetUpdateExtent (), (*it).second->GetImageActor()->GetDisplayExtent () ) ) {
      
      imageInput->SetUpdateExtent((*it).second->GetImageActor()->GetDisplayExtent ());
      
      // SetUpdateExtent does not call modified. There is a comment relating to this in
      // vtkDataObject::SetUpdateExtent
      imageInput->Modified ();
      imageInput->PropagateUpdateExtent();
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
        this->ImageDisplayMap.at(0)->GetImageActor()->GetBounds(bounds);
        double spos = bounds[this->SliceOrientation * 2];
        double cpos = cam->GetPosition()[this->SliceOrientation];
        double range = fabs(spos - cpos);
        double *spacing = input->GetSpacing();
        double avg_spacing = (spacing[0] + spacing[1] + spacing[2]) / 3.0;
        cam->SetClippingRange(range - avg_spacing * 3.0, range + avg_spacing * 3.0);
      }
    }
  }    
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
  double* bounds = this->GetImageActor(0)->GetDisplayBounds ();
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
  
  camera->SetFocalPoint(this->Renderer->GetWorldPoint()[0],
                        this->Renderer->GetWorldPoint()[1],
                        this->Renderer->GetWorldPoint()[2]);
  
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
    //this->Renderer->AddViewProp( this->ImageActor );
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
    //this->GetRenderer()->RemoveViewProp ( this->ImageActor );
    this->GetRenderer()->RemoveViewProp ( this->OrientationAnnotation );          
    //this->ImageActor->SetInput (NULL);
  }
  
  if( this->InteractorStyle )
  {
    /*
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
  
  if (this->RenderWindow)
  {
    std::map<int, vtkRenderer*>::iterator it = this->RendererMap.begin();
    while (it!=this->RendererMap.end()) 
    {
      if (vtkRenderer *renderer = it->second)
      {
        this->RenderWindow->AddRenderer(renderer);
      }
      ++it;
    }
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
  
  if (this->RenderWindow)
   {
    std::map<int, vtkRenderer*>::iterator it = this->RendererMap.begin();
    while (it!=this->RendererMap.end())
     {
      if (vtkRenderer *renderer = it->second)
       {
        this->RenderWindow->RemoveRenderer(renderer);
       }
       ++it;
     } 
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
  this->GetImageActor(0)->SetInterpolate (val);
  this->Modified();
}

//----------------------------------------------------------------------------
int vtkImageView2D::GetInterpolate(void)
{
  return this->GetImageActor(0)->GetInterpolate();
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetTransferFunctions(vtkColorTransferFunction* color, vtkPiecewiseFunction *opacity, int layer)
{
  if (layer==0)
  {
    this->Superclass::SetTransferFunctions(color, opacity);
    this->ImageDisplayMap.at(0)->GetWindowLevel()->SetLookupTable(this->GetColorTransferFunction());
  }
  else if (color && this->HasLayer(layer))
  {
    double *range = this->ImageDisplayMap.at(layer)->GetInput()->GetScalarRange();
    this->SetTransferFunctionRangeFromWindowSettings(color, 0, range[0], range[1]);
    this->ImageDisplayMap.at(layer)->GetWindowLevel()->SetLookupTable(color);
  }
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetLookupTable(vtkLookupTable* lut, int layer)
{
  if (layer==0)
  {
    this->Superclass::SetLookupTable(lut);
    this->ImageDisplayMap.at(0)->GetWindowLevel()->SetLookupTable(lut);
  }
  else if (lut && this->HasLayer(layer))
  {
    // this->ImageDisplayMap.at(layer)->GetInput()->Update();
    double *range = this->ImageDisplayMap.at(layer)->GetInput()->GetScalarRange();
    lut->SetTableRange(range[0], range[1]);
    this->ImageDisplayMap.at(layer)->GetWindowLevel()->SetLookupTable(lut);
  }
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetInput (vtkImageData *image, vtkMatrix4x4 *matrix, int layer)
{
  if (image)
    image->UpdateInformation(); // must be called before GetSliceForWorldCoordinates()
  
  if (layer==0) 
  {
    this->ImageDisplayMap.at(0)->SetInput(image);
    this->Superclass::SetInput (image, matrix, layer);
  }
  
  vtkRenderer *renderer = 0;
  if (layer == 0)
  {
    renderer = this->GetRenderer();
  }
  else // layer >0
  {
    if (!this->GetInput())
    {
      vtkErrorMacro (<< "Set input prior to adding layers");
      return;
    }
    
    vtkImageData *reslicedImage = this->ResliceImageToInput(image, matrix);
    if (!reslicedImage)
    {
      vtkErrorMacro (<< "Could not reslice image to input");
      return;
    }

    this->AddLayer(layer);

    // AddLayer must have created the proper vtkRenderer
    renderer = this->RendererMap.at(layer);

    // determine the scalar range. Copy the update extent to match the input's one
    double range[2];
    reslicedImage->SetUpdateExtent (this->GetInput()->GetUpdateExtent());
    reslicedImage->PropagateUpdateExtent();
    reslicedImage->Update();
    reslicedImage->GetScalarRange (range);

    vtkLookupTable *lut = vtkLookupTableManager::GetLookupTable("Default");
    lut->SetTableRange (range[0], range[1]);
    
    this->ImageDisplayMap.at(layer)->SetInput (reslicedImage);
    this->ImageDisplayMap.at(layer)->GetWindowLevel()->SetLookupTable (lut);
    this->ImageDisplayMap.at(layer)->GetImageActor()->SetOpacity(0.5);
    this->ImageDisplayMap.at(layer)->GetImageActor()->SetUserMatrix (this->OrientationMatrix);
    // this->ImageDisplayMap.at(layer)->GetInput()->UpdateInformation();
    
    reslicedImage->Delete();
    lut->Delete();
  }
  
  if (!renderer)
    return;
  
  renderer->AddViewProp (this->ImageDisplayMap.at(layer)->GetImageActor());
  
  this->Slice = this->GetSliceForWorldCoordinates (this->CurrentPoint);
  this->UpdateDisplayExtent();
  // this->UpdateCenter();
  this->UpdateSlicePlane();
  this->InvokeEvent (vtkImageView2D::SliceChangedEvent);
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetInputConnection (vtkAlgorithmOutput *input, vtkMatrix4x4 *matrix, int layer)
{
  this->Superclass::SetInputConnection( input, matrix, layer);
  // this->ImageActor->SetInput( this->WindowLevel->GetOutput() );
  
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

vtkImageActor *vtkImageView2D::GetImageActor(int layer) const
{
  if (!this->HasLayer(layer))
    return NULL;
  
  return this->ImageDisplayMap.at(layer)->GetImageActor();
}

vtkImageData *vtkImageView2D::GetImageInput(int layer) const
{
  if (!this->HasLayer(layer))
    return NULL;
  
  return this->ImageDisplayMap.at(layer)->GetInput();
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

//----------------------------------------------------------------------------
void vtkImageView2D::AddLayer(int layer)
{
  if (this->HasLayer(layer))
    return;

  this->ImageDisplayMap.insert(std::pair<int, vtkImage2DDisplay*>(layer, vtkImage2DDisplay::New()));
  this->RendererMap.insert(std::pair<int, vtkRenderer*>(layer, vtkRenderer::New()));
  
  vtkRenderer *renderer = this->RendererMap.at(layer);
  renderer->SetLayer(layer);
  if (this->GetRenderWindow())
  {
    this->GetRenderWindow()->SetNumberOfLayers (this->GetNumberOfLayers());
    this->GetRenderWindow()->AddRenderer(renderer);
  }
  
  if (this->GetRenderer())
    renderer->SetActiveCamera (this->GetRenderer()->GetActiveCamera());
}

//----------------------------------------------------------------------------
void vtkImageView2D::RemoveLayer(int layer)
{
  if (!this->HasLayer(layer))
    return;
  
  if (layer==0) // do not remove layer 0
    return;
  
  vtkRenderer *renderer = this->RendererMap.at(layer);
  vtkImage2DDisplay *display  = this->ImageDisplayMap.at(layer);
  
  if (this->GetRenderWindow())
  {
    this->GetRenderWindow()->RemoveRenderer(renderer);
  }
  
  renderer->Delete();
  display->Delete();
  
  this->RendererMap.erase(layer);
  this->ImageDisplayMap.erase(layer);
}

//----------------------------------------------------------------------------
bool vtkImageView2D::HasLayer(int layer) const
{
  std::map<int, vtkRenderer*>::const_iterator it = this->RendererMap.begin();
  while (it!=this->RendererMap.end())
   {
    if ((*it).first==layer)
      return true;
    ++it;
   }
  return false;
}

//----------------------------------------------------------------------------
int vtkImageView2D::GetNumberOfLayers(void) const
{
  return int(this->RendererMap.size());
}
