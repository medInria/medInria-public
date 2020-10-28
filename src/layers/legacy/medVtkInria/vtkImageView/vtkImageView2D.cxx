/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkImage2DDisplay.h"
#include "vtkImageView2D.h"

#include <vtkAlgorithmOutput.h>
#include <vtkAngleWidget.h>
#include <vtkAngleRepresentation2D.h>
#include <vtkAxes2DWidget.h>
#include <vtkAxisActor2D.h>
#include <vtkCamera.h>
#include <vtkDataSet2DWidget.h>
#include <vtkDataSetCollection.h>
#include <vtkDistanceWidget.h>
#include <vtkDistanceRepresentation2D.h>
#include <vtkImageView2DCommand.h>
#include <vtkImageViewCornerAnnotation.h>
#include <vtkInformation.h>
#include <vtkLeaderActor2D.h>
#include <vtkMatrixToLinearTransform.h>
#include <vtkOrientationAnnotation.h>
#include <vtkPlane.h>
#include <vtkPointData.h>
#include <vtkPointSet.h>
#include <vtkPolyData.h>
#include <vtkProp3DCollection.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRulerWidget.h>
#include <vtkScalarBarActor.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkTransform.h>

#include <sstream>

vtkStandardNewMacro(vtkImageView2D)

//----------------------------------------------------------------------------
vtkImageView2D::vtkImageView2D()
{
  this->qtSignalHandler = new vtkImageView2DQtSignals();
  this->Axes2DWidget        = vtkAxes2DWidget::New();
  this->RulerWidget         = vtkRulerWidget::New();
  this->DistanceWidget      = vtkDistanceWidget::New();
  this->AngleWidget         = vtkAngleWidget::New();
  this->ConventionMatrix    = vtkMatrix4x4::New();
  this->SlicePlane          = vtkPolyData::New();
  this->Command             = vtkImageView2DCommand::New();
  this->OrientationAnnotation = vtkOrientationAnnotation::New();
  this->m_vtkImageFromBoundsSourceGenerator = nullptr;

  this->CurrentLayer = 0;

  this->LayerInfoVec.resize(1);
  this->LayerInfoVec[0].ImageDisplay = vtkSmartPointer<vtkImage2DDisplay>::New();
  this->LayerInfoVec[0].Renderer = vtkSmartPointer<vtkRenderer>::New();

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

  this->InitialParallelScale = 1.0;
  this->ShowRulerWidget      = 1;
  this->ShowImageAxis        = 1;
  this->ShowDistanceWidget   = 0;
  this->ShowAngleWidget      = 0;
  this->AnnotationStyle      = AnnotationStyle1;
  this->CursorFollowMouse    = 0;

  this->SetViewConvention (vtkImageView2D::VIEW_CONVENTION_RADIOLOGICAL);

  this->InitializeSlicePlane();

  this->CornerAnnotation->SetImageActor (this->GetImageActor());
  this->CornerAnnotation->ShowSliceAndImageOn();
  this->CornerAnnotation->GetProperty()->SetDisplayLocationToForeground();

  this->RulerWidget->KeyPressActivationOff();

  this->DistanceWidget->KeyPressActivationOff();
  this->DistanceWidget->CreateDefaultRepresentation();
  this->DistanceWidget->SetKeyPressActivationValue ('d');
  vtkDistanceRepresentation2D* rep1 = vtkDistanceRepresentation2D::SafeDownCast (this->DistanceWidget->GetRepresentation());
  rep1->GetAxis()->SetTickLength (6);

  this->AngleWidget->KeyPressActivationOff();
  this->AngleWidget->SetKeyPressActivationValue ('o');
  this->AngleWidget->CreateDefaultRepresentation();
  vtkAngleRepresentation2D* rep2 = vtkAngleRepresentation2D::SafeDownCast (this->AngleWidget->GetRepresentation());
  rep2->GetRay1()->GetProperty()->SetColor (0,1,0);
  rep2->GetRay2()->GetProperty()->SetColor (0,1,0);
  rep2->GetArc()->GetProperty()->SetColor (0,1,0);

  // Orientation
  matrixOrientation[0][0] = "R"; matrixOrientation[0][1] = "L";
  matrixOrientation[1][0] = "A"; matrixOrientation[1][1] = "P";
  matrixOrientation[2][0] = "I"; matrixOrientation[2][1] = "S";
  this->SetAnnotationsFromOrientation();

  vtkInteractorStyle* style = vtkInteractorStyleImageView2D::New();
  this->SetInteractorStyle ( style );
  style->Delete();

  this->SetTransferFunctions(nullptr, nullptr, 0);

  SetRenderer(this->LayerInfoVec[0].Renderer);
}

//----------------------------------------------------------------------------
vtkImageView2D::~vtkImageView2D()
{
// Deletion of objects in the LayerInfoMap is handled by the SmartPointers.
  this->LayerInfoVec.clear();

  this->Axes2DWidget->SetImageView(nullptr);

  this->Axes2DWidget->Delete();
  this->RulerWidget->Delete();
  this->DistanceWidget->Delete();
  this->AngleWidget->Delete();
  this->ConventionMatrix->Delete();
  this->SlicePlane->Delete();
  this->Command->Delete();
  this->OrientationAnnotation->Delete();


  for (std::list<vtkDataSet2DWidget*>::iterator it3 = this->DataSetWidgets.begin();
      it3!=this->DataSetWidgets.end(); ++it3)
  {
    (*it3)->Off();
    (*it3)->SetImageView (nullptr);
    (*it3)->Delete();
  }
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetVisibility(int visible, int layer)
{
  if (this->HasLayer(layer))
  {
    this->GetImage2DDisplayForLayer(layer)->GetImageActor()->SetVisibility(visible);
    this->Modified();
  }
}

//----------------------------------------------------------------------------
int vtkImageView2D::GetVisibility(int layer) const
{
  if (this->HasLayer(layer))
  {
    return this->GetImage2DDisplayForLayer(layer)->GetImageActor()->GetVisibility();
  }

  return 0;
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetOpacity(double opacity, int layer)
{
  if (this->HasLayer(layer))
  {
   this->GetImage2DDisplayForLayer(layer)->GetImageActor()->SetOpacity(opacity);
    this->Modified();
  }
}

//----------------------------------------------------------------------------
double vtkImageView2D::GetOpacity(int layer) const
{
  if (this->HasLayer(layer))
  {
   return this->GetImage2DDisplayForLayer(layer)->GetImageActor()->GetOpacity();
  }

  return 0.0;
}

//----------------------------------------------------------------------------
/** Override vtkObject - return the maximum mtime of this and any objects owned by this. */
vtkMTimeType vtkImageView2D::GetMTime()
{
    vtkMTimeType mTime = Superclass::GetMTime();

    vtkObject * objectsToInclude[] = {
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
                const vtkMTimeType testMtime = object->GetMTime();
                if ( testMtime > mTime )
                    mTime = testMtime;
            }
        }

        const int numLayer = this->GetNumberOfLayers();
        for ( int i(0); i<numLayer; ++i ) {
            vtkObject * object = this->GetImage2DDisplayForLayer(i)->GetImageActor();
            if (object) {
                const vtkMTimeType testMtime = object->GetMTime();
                if ( testMtime > mTime )
                    mTime = testMtime;
            }
        }

        return mTime;
}

//----------------------------------------------------------------------------
void vtkImageView2D::GetSliceRange(int &min, int &max) const
{
    if (this->GetMedVtkImageInfo() && this->GetMedVtkImageInfo()->initialized)
    {
        min = 0;
        max = 0;
        
        if(this->Get2DDisplayMapperInputAlgorithm())
        {
            this->Get2DDisplayMapperInputAlgorithm()->UpdateInformation();
        }
        auto pAlgo = this->Get2DDisplayMapperInputAlgorithm();
        if (pAlgo != nullptr)
        {
            auto pInfo = pAlgo->GetOutputInformation(0);
           if (pInfo != nullptr)
           {
               int* w_ext = pInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());
               if (w_ext != nullptr)
               {
                   min = w_ext[this->SliceOrientation * 2];
                   max = w_ext[this->SliceOrientation * 2 + 1];
               }
           }
        }
    }
}

//----------------------------------------------------------------------------
int* vtkImageView2D::GetSliceRange() const
{
  if (this->GetMedVtkImageInfo() && this->GetMedVtkImageInfo()->initialized)
  {
      int* w_ext = this->GetMedVtkImageInfo()->extent;
      return w_ext + this->SliceOrientation * 2;
  }
  return nullptr;
}

//----------------------------------------------------------------------------
/** Return the minimum and maximum slice values (depending on the orientation
 this can be in X, Y or Z). */
int vtkImageView2D::GetSliceMin() const
{
  int *range = this->GetSliceRange();
  if (range)
  {
    return range[0];
  }
  return 0;
}

//----------------------------------------------------------------------------
int vtkImageView2D::GetSliceMax() const
{
  int *range = this->GetSliceRange();
  if (range)
  {
    return range[1];
  }
  return 0;
}

//----------------------------------------------------------------------------
/** Set/Get the current slice to display (depending on the orientation
 this can be in X, Y or Z).
 */
int vtkImageView2D::GetSlice() const
{
  return this->Slice;
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetSlice(int slice)
{
  if (!this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized)
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
  double* spacing = this->GetMedVtkImageInfo()->spacing;
  displacement[this->SliceOrientation] = (slice - old_slice) * spacing[this->SliceOrientation];
  this->GetOrientationMatrix()->MultiplyPoint (displacement, displacement);

  vtkSmartPointer < vtkTransform > pointTransform = vtkSmartPointer < vtkTransform >::New ();

  pointTransform->Translate ( displacement );

  // Update the instances according to displacement.
  double pos[3];
  pointTransform->TransformPoint ( this->CurrentPoint , pos );
  // NB: Modified() and InvokeEvent( vtkImageView::CurrentPointChangedEvent ) are
  // called in SetCurrentPoint()

  // Shift the camera with the slice, so that the slice does not go behind the camera.
  vtkCamera *camera = this->GetRenderer()->GetActiveCamera();
  if (camera)
  {
    // This shifts both the camera position and the focal point.
    camera->ApplyTransform ( pointTransform );
  }

  this->SetCurrentPoint (pos);

  qtSignalHandler->emitSliceChanged(slice, this->SliceOrientation);
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
  if (this->m_poInternalImageFromInput)
  {
    this->Slice = this->GetSliceForWorldCoordinates (this->CurrentPoint);
    this->UpdateDisplayExtent();
    this->UpdateSlicePlane();
    this->InvokeEvent (vtkImageView2D::SliceChangedEvent);
  }

  this->InvokeEvent (vtkImageView2D::OrientationChangedEvent);
  this->Modified();
}

//----------------------------------------------------------------------------
/**
After the orientation has changed, it is crucial to adapt
a couple of things according to new orientation.
In UpdateOrientation() the SlicePlane, the Camera settings,
the CornerAnnotation are modified.
*/
void vtkImageView2D::UpdateOrientation()
{
  // Store zoom and pan
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

  // Restore zoom and pan
  this->SetZoom (zoom);
  this->SetPan (pan);
}

//----------------------------------------------------------------------------
/** Update the display extent manually so that the proper slice for the
 given orientation is displayed. It will also try to set a
 reasonable camera clipping range.
 This method is called automatically when the Input is changed, but
 most of the time the input of this class is likely to remain the same,
 i.e. connected to the output of a filter, or an image reader. When the
 input of this filter or reader itself is changed, an error message might
 be displayed since the current display extent is probably outside
 the new whole extent. Calling this method will ensure that the display
 extent is reset properly.
 */
void vtkImageView2D::UpdateDisplayExtent()
{
  if (this->LayerInfoVec.empty())
    return;

  if (!this->GetImage2DDisplayForLayer(GetFirstLayer())->isInputSet())
  {
    return;
  }


  int* w_ext = this->GetMedVtkImageInfo()->extent;

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

  for (LayerInfoVecType::iterator it = this->LayerInfoVec.begin(); it != this->LayerInfoVec.end(); it++)
  {
    vtkImage2DDisplay * imageDisplay = it->ImageDisplay;
    if (!imageDisplay->isInputSet())
    {
        continue;
    }

    switch (this->SliceOrientation)
    {
      case vtkImageView2D::SLICE_ORIENTATION_XY:
        imageDisplay->GetImageActor()->SetDisplayExtent(w_ext[0], w_ext[1], w_ext[2], w_ext[3], slice, slice);
        break;

      case vtkImageView2D::SLICE_ORIENTATION_XZ:
        imageDisplay->GetImageActor()->SetDisplayExtent(w_ext[0], w_ext[1], slice, slice, w_ext[4], w_ext[5]);
        break;

      case vtkImageView2D::SLICE_ORIENTATION_YZ:
      default:
        imageDisplay->GetImageActor()->SetDisplayExtent(slice, slice, w_ext[2], w_ext[3], w_ext[4], w_ext[5]);
        break;
    }

  }

  // Figure out the correct clipping range
  if (this->GetRenderer())
  {
      // the clipping range is adjusted so that it englobles the slice +/- 0.5 * spacing in the
      // direction given by ViewOrientation. It allows to automatically clip polygonal datasets
      // without having to use a time consuming vtkCutter. In case of oblique slices (i.e., when
      // a non-identity OrientationTransform is used, a large margin is used
      // resulting in a too large clipping range. To be futher investigated.
      vtkCamera *cam = this->GetRenderer()->GetActiveCamera();
      if (cam)
      {
        double pos[3] = {.0,.0,.0};
        this->GetWorldCoordinatesForSlice (this->GetSlice(), pos);

        double vn[3] = {.0,.0,.0}, position[3] = {.0,.0,.0};
        cam->GetViewPlaneNormal(vn);
        cam->GetPosition(position);

        double distance = 0.0;
        for (int i=0; i<3; i++)
            distance += (pos[i]-position[i]) * -vn[i];

        double avg_spacing = this->GetMedVtkImageInfo()->spacing[this->ViewOrientation] * 0.5;
        cam->SetClippingRange(distance - avg_spacing, distance + avg_spacing);
    }
  }
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetViewConvention(int convention)
{
  if ( (convention < vtkImageView2D::VIEW_CONVENTION_RADIOLOGICAL))
    return;

  this->ViewConvention = convention;
  // first vector is the view up for the yz plane (sagittal), directed towards the +z direction
  this->ConventionMatrix->SetElement (2,0, 1);
  // first vector is the view up for the xz plane (coronal),  directed towards the +z direction
  this->ConventionMatrix->SetElement (2,1, 1);
  // first vector is the view up for the xy plane (axial),    directed towards the -y direction
  this->ConventionMatrix->SetElement (1,2, -1);

  int x_watcher, y_watcher, z_watcher;

  switch(convention)
  {
    case vtkImageView2D::VIEW_CONVENTION_RADIOLOGICAL:
    default:
      // for sagittal view, you look from +x point
      x_watcher =  1;
      // for coronal view,  you look from -y point
      y_watcher = -1;
      // for axial view,    you look from -z point
      z_watcher = -1;
      break;
    case vtkImageView2D::VIEW_CONVENTION_NEUROLOGICAL:
      // for sagittal view, you look from +x point
      x_watcher =  1;
      // for coronal view,  you look from +y point
      y_watcher =  1;
      // for axial view,    you look from +z point
      z_watcher =  1;
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
  int slice_orientation = -1;

  /**
     In cases the acquisition is strongly oblique, it can happen that
     their is NO slice-orientation corresponding to the desired
     view-orientation (redundant view-orientation for 2 slice-orientations)

     We are not doomed though, the hack is to give priority to the native
     acquisition plane (xy) and derive the other orientations from it.
  */
  double dot = 0;
  double projection, p1, p2;

  /// Proiority to the xy plane :
  int viewtoslice[3] = {-1,-1,-1};

  for (unsigned int i=0; i<3; i++)
  {
    // take the third column of the orientation matrix, check the i-est projection
    projection  = std::abs (this->GetOrientationMatrix()->GetElement (i,       2));
    // take the read-out acquisition axis, check the other projections
    p1          = std::abs (this->GetOrientationMatrix()->GetElement ((i+1)%3, 0));
    p2          = std::abs (this->GetOrientationMatrix()->GetElement ((i+2)%3, 0));

    // check to which axis this vector is the closest
    if (projection > dot)
    {
      dot = projection;
      viewtoslice[i] = 2;
      // choose between the 2 options according to respective projections.
      viewtoslice[(i+1)%3] = (p1 >= p2) ? 0 : 1;
      viewtoslice[(i+2)%3] = (p1 >= p2) ? 1 : 0;
    }
  }

  slice_orientation = viewtoslice[orientation];



#if 0 // comment code above and replace it by the one below if you
      // want to test the behavior of this function as it was
      // previous to the fix...

  /**
     but normally we should be able to use this bit of code,
     it works and is stable for most cases, when acquisition is
     not too oblique.
  */
  int view_orientation = -1;
  for (unsigned int i=0; i<3; i++)
  {
    view_orientation = this->GetViewOrientationFromSliceOrientation(i);
    if (view_orientation == orientation)
    {
      slice_orientation = i;
      break;
    }
  }

  if (slice_orientation == -1)
  {
    vtkErrorMacro("Error - No slice-orientation corresponding to the required view-orientation "<< orientation
          <<"\n Please consider switch ON vtkINRIA3D_HACK_OBLIQUE_ORIENTATION ");
    slice_orientation = orientation;
  }

#endif //

  this->SetSliceOrientation (slice_orientation);
}

//----------------------------------------------------------------------------
void vtkImageView2D::SetOrientationMatrix (vtkMatrix4x4* matrix)
{
  this->Superclass::SetOrientationMatrix (matrix);
  if(this->GetImage2DDisplayForLayer(GetFirstLayer()))
  {
        this->GetImage2DDisplayForLayer(GetFirstLayer())->GetImageActor()->SetUserMatrix (this->OrientationMatrix);
  }
  this->UpdateOrientation();

  // The slice might have changed in the process
  if (this->m_poInternalImageFromInput)
  {
    this->Slice = this->GetSliceForWorldCoordinates (this->CurrentPoint);
    this->UpdateDisplayExtent();
    this->UpdateSlicePlane();
    this->InvokeEvent (vtkImageView2D::SliceChangedEvent);
  }

  this->InvokeEvent (vtkImageView2D::OrientationChangedEvent);
}

//----------------------------------------------------------------------------
/**
This method is called each time the orientation changes (SetViewOrientation())
and sets the appropriate color to the slice plane.

Red: R-L direction --> sagittal orientation

Green: A-P direction --> coronal orientation

Blue: I-S direction --> axial orientation
*/
void vtkImageView2D::InitializeSlicePlane()
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
  array->InsertNextTypedTuple (vals);
  array->InsertNextTypedTuple (vals);
  array->InsertNextTypedTuple (vals);
  array->InsertNextTypedTuple (vals);

  this->SlicePlane->GetPointData()->SetScalars (array);
  array->Delete();
}

//----------------------------------------------------------------------------
/**
The wolrd is not always what we think it is ...

Use this method to move the viewer slice such that the position
(in world coordinates) given by the arguments is contained by
the slice plane. If the given position is outside the bounds
of the image, then the slice will be as close as possible.
*/
void vtkImageView2D::SetCurrentPoint(double pos[3])
{
  int old_slice = this->Slice;
  int new_slice = this->GetSliceForWorldCoordinates (pos);

  int *range = this->GetSliceRange();
  if (range)
  {
    new_slice = std::max (new_slice, range[0]);
    new_slice = std::min (new_slice, range[1]);
  }

  bool sliceChanged = false;

  if(new_slice != old_slice)
  {
    this->Slice = new_slice;
    sliceChanged = true;
  }

  this->Superclass::SetCurrentPoint (pos);

  if (sliceChanged)
  {
      this->UpdateDisplayExtent();
      this->UpdateSlicePlane();
      this->InvokeEvent (vtkImageView2D::SliceChangedEvent);
  }
}



//----------------------------------------------------------------------------
/**
Returns the estimated View-Orientation corresponding
to a given Slice-Orientation, and outputs the camera's parameters
This is crucial when choosing to impose the View-Orientation
instead of imposing the Slice-Orientation.
*/
int vtkImageView2D::GetViewOrientationFromSliceOrientation(int sliceorientation, double* cam_pos, double* cam_focus)
{
  double position[4] = {0,0,0,0}, focalpoint[4] = {0,0,0,0};
  double focaltoposition[3]={0,0,0};
  double origin[3] = {0,0,0};

  if (this->GetMedVtkImageInfo() && this->GetMedVtkImageInfo()->initialized)
    this->GetMedVtkImageInfo()->GetOrigin(origin);

  // At first, we initialize the cam focal point to {0,0,0}, so nothing to do
  // (after re-orientation, it will become the origin of the image)
  position[sliceorientation] = this->ConventionMatrix->GetElement (sliceorientation, 3);

  // Points VS vectors: homogeneous coordinates
  focalpoint[3] = 1;
  position[3]   = 1;

  // Apply the orientation matrix to all this information
  if ( this->OrientationMatrix )
  {
    /**
       The origin in ITK pipeline is taken into account in a different
       way than in the VTK equivalent.
       A first hack would be to force the vtkImageData instance to have
       a null origin, and put the ITK origin in the 4th column of the
       OrientationMatrix instance. BUT, when the ITK pipeline is updated,
       then the ITK origin is put back in place in the vtkImageData instance.

       Therefore we need to keep the vtkImageData's origin the same as the
       ITK one. And, we need to correct for this misbehaviour through a hack
       in the OrientationMatrix 4th column, a sort of corrected origin.

       THIS FOR-LOOP HAS TO BE REMOVED IF THE ORIGIN OF THE
       VTKIMAGEDATA IS FORCED TO ZERO (AND PASSED AS-IS IN THE ORIENTATIONMATRIX)
    */
    for (unsigned int i=0; i<3; i++)
    {
      position[i] += origin[i];
      focalpoint[i] += origin[i];
    }

    this->OrientationMatrix->MultiplyPoint  (position,   position);
    this->OrientationMatrix->MultiplyPoint  (focalpoint, focalpoint);
  }

  // Compute the vector normal to the view
  for (unsigned int i=0; i<3; i++)
    focaltoposition[i] = position[i] - focalpoint[i];

  // we find the axis the closest to the focaltoposition vector
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

  if (cam_pos && cam_focus)
  {
    for (unsigned int i=0; i<3; i++)
    {
      cam_pos[i] = position[i];
      cam_focus[i] = focalpoint[i];
    }
  }

  // return the view-orientation found by those principles.
  // this should then be set as this->ViewOrientation.
  return id;
}



//----------------------------------------------------------------------------
/**
After the orientation has changed, it is crucial to adapt
a couple of things according to new orientation.
In UpdateOrientation() the SlicePlane, the Camera settings,
the CornerAnnotation are modified.
*/
int vtkImageView2D::SetCameraFromOrientation()
{
  // We entirely rely on the slice orientation this->SliceOrientation
  // The ViewOrientation is "estimated", returned as id

  vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : nullptr;
  if (!cam)
    return -1;

  double position[4] = {0,0,0,0}, focalpoint[4] = {0,0,0,0}, viewup[4] = {0,0,0,0};
  double focaltoposition[3]={0,0,0};
  std::vector<double*> viewupchoices;
  double first[3]={0,0,0}, second[3]={0,0,0}, third[3]={0,0,0}, fourth[3]={0,0,0};
  bool inverseposition = false;

  // The viewup and the cam position are set according to the convention matrix.
  for (unsigned int i=0; i<3; i++)
    viewup[i]   = this->ConventionMatrix->GetElement (i, this->SliceOrientation);

  // Apply the orientation matrix to all this information
  if ( this->OrientationMatrix )
    this->OrientationMatrix->MultiplyPoint  (viewup, viewup);

  // first we find the axis the closest to the focaltoposition vector
  unsigned int id = this->GetViewOrientationFromSliceOrientation (this->SliceOrientation, position, focalpoint);

  // Compute the vector normal to the view
  for (unsigned int i=0; i<3; i++)
    focaltoposition[i] = position[i] - focalpoint[i];

  // Now we now we have 4 choices for the View-Up information
  for(unsigned int i=0; i<3; i++)
  {
    first[i]  = viewup[i];
    second[i] = -viewup[i];
  }

  vtkMath::Cross (first, focaltoposition, third);
  vtkMath::Cross (second, focaltoposition, fourth);
  vtkMath::Normalize (third);
  vtkMath::Normalize (fourth);

  viewupchoices.push_back (first);
  viewupchoices.push_back (second);
  viewupchoices.push_back (third);
  viewupchoices.push_back (fourth);

  double conventionviewup[4]={0,0,0,0};
  // Then we choose the convention matrix vector correspondent to the
  // one we just found
  for (unsigned int i=0; i<3; i++)
    conventionviewup[i] = this->ConventionMatrix->GetElement (i, id);

  // Then we pick from the 4 solutions the closest to the
  // vector just found
  unsigned int id2 = 0;
  double dot2 = 0;
  for (unsigned int i=0; i<viewupchoices.size(); i++)
    if (dot2 < vtkMath::Dot (viewupchoices[i], conventionviewup))
    {
      dot2 = vtkMath::Dot (viewupchoices[i], conventionviewup);
      id2 = i;
    }

  // We found the solution
  for (unsigned int i=0; i<3; i++)
    viewup[i] = viewupchoices[id2][i];

  double conventionposition[4]={0,0,0,1};
  // Then we check if we are on the right side of the image
  conventionposition[id] = this->ConventionMatrix->GetElement (id, 3);
  inverseposition = (vtkMath::Dot (focaltoposition, conventionposition) < 0 );

  // invert the cam position if necessary (symmetry along the focal point)
  if (inverseposition)
    for (unsigned int i=0; i<3; i++)
      position[i] -= 2*focaltoposition[i];

  // finally set the camera with all iinformation.
  cam->SetPosition(position[0], position[1], position[2]);
  cam->SetFocalPoint(focalpoint[0], focalpoint[1], focalpoint[2]);
  cam->SetViewUp(viewup[0], viewup[1], viewup[2]);

  this->InvokeEvent (vtkImageView::CameraChangedEvent);

  // return the view-orientation found by those principles.
  // this should then be set as this->ViewOrientation.
  return id;
}

//----------------------------------------------------------------------------
/**
 Switch between annotation style.
 AnnotationStyle1 is there by default, it shows the X-Y position (in-plane),
 and specify the orientation on lower-right corner
 AnnotationStyle1 shows the global physical x-y-z position (scanner coordinates),
 and leaves lower-right corner empty for user to fill.
*/
void vtkImageView2D::SetAnnotationStyle(unsigned int arg)
{
    this->AnnotationStyle = arg;
    this->SetAnnotationsFromOrientation();
}

unsigned int vtkImageView2D::GetAnnotationStyle2D()
{
    return this->AnnotationStyle;
}

//----------------------------------------------------------------------------
/**
After the orientation has changed, it is crucial to adapt
a couple of things according to new orientation.
In UpdateOrientation() the SlicePlane, the Camera settings,
the CornerAnnotation are modified.
*/
void vtkImageView2D::SetAnnotationsFromOrientation()
{
    // This method has to be called after the camera
    // has been set according to orientation and convention.
    // We rely on the camera settings to compute the oriention
    // annotations.

    std::string solution[4]={"L","P","R","A"};

    vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : nullptr;
    if (cam)
    {
        double* viewup = cam->GetViewUp();
        double* normal = cam->GetViewPlaneNormal();
        double rightvector[3];
        vtkMath::Cross (normal, viewup, rightvector);

        unsigned int id1 = 0;
        unsigned int id2 = 0;

        double dot1 = 0;
        double dot2 = 0;

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
        }

        if (viewup[id1] > 0)
        {
            solution[3] = matrixOrientation[id1][0];
            solution[1] = matrixOrientation[id1][1];
        }
        else
        {
            solution[3] = matrixOrientation[id1][1];
            solution[1] = matrixOrientation[id1][0];
        }
        if (rightvector[id2] > 0)
        {
            solution[0] = matrixOrientation[id2][0];
            solution[2] = matrixOrientation[id2][1];
        }
        else
        {
            solution[0] = matrixOrientation[id2][1];
            solution[2] = matrixOrientation[id2][0];
        }
    }

    for (unsigned int i=0; i<4; i++)
    {
        this->OrientationAnnotation->SetText(i, solution[i].c_str());
    }

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
            osSW<< "<pos_z>";

            osNW<< "<image_size>\n";
            osNW<< "<voxel_size>\n";
            osNW<< "<coord_xy_and_value>\n";
            osNW<< "<pos_xy>\n";
            osNW<< "<window_level>";

            osNE << "<patient>\n<study>\n<series>";

            switch( this->ViewOrientation )
            {
                case vtkImageView2D::VIEW_ORIENTATION_AXIAL:
                default:
                    osSE << "Axial View";
                    break;
                case vtkImageView2D::VIEW_ORIENTATION_CORONAL:
                    osSE << "Coronal View";
                    break;
                case vtkImageView2D::VIEW_ORIENTATION_SAGITTAL:
                    osSE << "Sagittal View";
            }
    }

    if (osSW.str().size())
    {
        this->GetCornerAnnotation()->SetText (0, osSW.str().c_str());
    }
    if (osSE.str().size())
    {
        this->GetCornerAnnotation()->SetText (1, osSE.str().c_str());
    }
    if (osNW.str().size())
    {
        this->GetCornerAnnotation()->SetText (2, osNW.str().c_str());
    }
    if (osNE.str().size())
    {
        this->GetCornerAnnotation()->SetText (3, osNE.str().c_str());
    }
}

//----------------------------------------------------------------------------
/**
After the orientation has changed, it is crucial to adapt
a couple of things according to new orientation.
In UpdateOrientation() the SlicePlane, the Camera settings,
the CornerAnnotation are modified.
*/
void vtkImageView2D::SetSlicePlaneFromOrientation()
{
  if (this->ViewOrientation < VIEW_ORIENTATION_SAGITTAL || this->ViewOrientation > VIEW_ORIENTATION_AXIAL)
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
  array->SetTypedTuple (0, vals);
  array->SetTypedTuple (1, vals);
  array->SetTypedTuple (2, vals);
  array->SetTypedTuple (3, vals);

  this->UpdateSlicePlane();

}

//----------------------------------------------------------------------------
/**
The SlicePlane instance (GetSlicePlane()) is the polygonal
square corresponding to the slice plane,
it is color-coded according to conventions.

UpdateSlicePlane() is thus called each time we change slice
or change orientation.
*/
void vtkImageView2D::UpdateSlicePlane()
{
  if( !this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized) // if input is not set yet, no way we can now the display bounds
    return;

  vtkPoints* oldpoints = vtkPoints::New();
  vtkPoints* points = vtkPoints::New();
  double x[3];
  double* bounds = this->GetImageActor()->GetDisplayBounds ();
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
/**
The ViewCenter instance follows the center of the view
in world coordinates. It is updated UpdateCenter() each
time the slice or the orientation changes.

CAUTION: for the moment it is de-activated to speed up the
visualization. (The ViewCenter is not used anywhere else).
*/
void vtkImageView2D::UpdateCenter()
{
  if (!this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized)
    return;
  vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : nullptr;
  if (!cam)
    return;
  int* dimensions = this->GetMedVtkImageInfo()->dimensions;

  int indices[3] = {0,0,0};
  for (unsigned int i=0; i<3; i++)
  {
    indices[i] = (int)((double)dimensions[i] / 2.0);
  }
  indices[this->SliceOrientation] = this->GetSlice();

  this->GetWorldCoordinatesFromImageCoordinates (indices, this->ViewCenter);
}

//----------------------------------------------------------------------------
/**
Convert a world coordinate point into an image indices coordinate point
*/
int vtkImageView2D::GetSliceForWorldCoordinates(double pos[3]) const
{
  int indices[3];
  this->GetImageCoordinatesFromWorldCoordinates (pos, indices);
  return indices[this->SliceOrientation];
}

//----------------------------------------------------------------------------
/**
Convert an indices coordinate point (image coordinates) into a world coordinate point
*/
void vtkImageView2D::GetWorldCoordinatesForSlice(int slice, double* position)
{
  int indices[3] = {0,0,0};
  indices[this->SliceOrientation] = slice;
  this->GetWorldCoordinatesFromImageCoordinates (indices, position);
}

//----------------------------------------------------------------------------
/**
Get/Set the pan factor of the view
*/
void vtkImageView2D::SetPan (double* arg)
{
  this->Pan[0] = arg[0];
  this->Pan[1] = arg[1];

  // If mapWorldToDisplayCoordinates is called when RenderWindow has never been rendered, nans are returned
  // Reason: mapWorldToDisplayCoordinates needs to now the window size, which detected when the vtk renderwindow
  // has been rendered at least once
  vtkCamera *camera = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : nullptr;
  if (!camera || !this->GetRenderer() || !this->RenderWindow || this->RenderWindow->GetNeverRendered())
  {
    return;
  }

  double focaldepth, focalpoint[4], position[4], motion[4];
  double bounds[6];
  double center[3];

  camera->GetFocalPoint (focalpoint);
  camera->GetPosition(position);

  this->GetRenderer()->SetWorldPoint (focalpoint[0], focalpoint[1], focalpoint[2], 1.0);
  this->GetRenderer()->WorldToDisplay();
  focaldepth = this->GetRenderer()->GetDisplayPoint()[2];

  this->GetRenderer()->ComputeVisiblePropBounds (bounds);
  center[0] = (bounds[0] + bounds[1])/2.0;
  center[1] = (bounds[2] + bounds[3])/2.0;
  center[2] = (bounds[4] + bounds[5])/2.0;

  this->GetRenderer()->SetWorldPoint (center[0], center[1], center[2], 1.0);
  this->GetRenderer()->WorldToDisplay();
  this->GetRenderer()->GetDisplayPoint (center);
  center[0] += this->Pan[0];
  center[1] += this->Pan[1];

  this->GetRenderer()->SetDisplayPoint (center[0], center[1], focaldepth);
  this->GetRenderer()->DisplayToWorld();

  // Camera motion is reversed
  motion[0] = focalpoint[0] - this->GetRenderer()->GetWorldPoint()[0];
  motion[1] = focalpoint[1] - this->GetRenderer()->GetWorldPoint()[1];
  motion[2] = focalpoint[2] - this->GetRenderer()->GetWorldPoint()[2];

  camera->SetFocalPoint(this->GetRenderer()->GetWorldPoint()[0],
                        this->GetRenderer()->GetWorldPoint()[1],
                        this->GetRenderer()->GetWorldPoint()[2]);

  camera->SetPosition(- motion[0] + position[0],
                      - motion[1] + position[1],
                      - motion[2] + position[2]);

  if (this->Interactor && this->Interactor->GetLightFollowCamera())
  {
    this->GetRenderer()->UpdateLightsGeometryToFollowCamera();
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
  vtkCamera *camera = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : nullptr;
  if (!camera || !this->GetRenderer() || !this->RenderWindow || this->RenderWindow->GetNeverRendered())
  {
    pan[0] = this->Pan[0];
    pan[1] = this->Pan[1];
    return;
  }

  double bounds[6], center[3], viewFocus[4];

  camera->GetFocalPoint(viewFocus);
  this->GetRenderer()->ComputeVisiblePropBounds (bounds);

  center[0] = (bounds[0] + bounds[1])/2.0;
  center[1] = (bounds[2] + bounds[3])/2.0;
  center[2] = (bounds[4] + bounds[5])/2.0;

  this->GetRenderer()->SetWorldPoint (center[0], center[1], center[2], 0.0);
  this->GetRenderer()->WorldToDisplay();
  this->GetRenderer()->GetDisplayPoint (center);

  this->GetRenderer()->SetWorldPoint (viewFocus[0], viewFocus[1], viewFocus[2], 1.0);
  this->GetRenderer()->WorldToDisplay();
  this->GetRenderer()->GetDisplayPoint (viewFocus);

  pan[0] = -center[0] + viewFocus[0];
  pan[1] = -center[1] + viewFocus[1];
}

//----------------------------------------------------------------------------
/**
Reset the camera in a nice way for the 2D view
*/
void vtkImageView2D::ResetCamera()
{
  this->Superclass::ResetCamera();

  this->Pan[0] = this->Pan[1] = 0.0;
  this->SetPan (this->Pan); // not sure this is needed
}

//----------------------------------------------------------------------------
/**
Reset position - zoom - window/level to default
*/
void vtkImageView2D::Reset()
{
  this->Superclass::Reset();
  this->UpdateDisplayExtent();
}
//----------------------------------------------------------------------------
/**
 Show/Hide the annotations (CornerAnnotation AND OrientationAnnotation)
*/
void vtkImageView2D::SetShowAnnotations (int val)
{
  this->Superclass::SetShowAnnotations (val);
  this->OrientationAnnotation->SetVisibility (val);
}

/**
Change the interaction triggered by the mouse buttons.
Choices are listed in vtkInteractorStyleImageView2D class:
InteractionTypeSlice : changes the slice number.
InteractionTypeWindowLevel : changes the window-level values.
InteractionTypeZoom : changes the zoom level.
InteractionTypePan : translate the view in-plane.
*/
void vtkImageView2D::SetLeftButtonInteractionStyle (int arg)
{
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
        t->SetLeftButtonInteraction (arg);
}
int vtkImageView2D::GetLeftButtonInteractionStyle()
{
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
        return t->GetLeftButtonInteraction ();
    return -1;
}

/**
Change the interaction triggered by the keyboard.
Choices are listed in vtkInteractorStyleImageView2D class:
InteractionTypeSlice : changes the slice number.
InteractionTypeWindowLevel : changes the window-level values.
InteractionTypeZoom : changes the zoom level.
InteractionTypePan : translate the view in-plane.
*/
void vtkImageView2D::SetKeyboardInteractionStyle (int arg)
{
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
        t->SetKeyboardInteraction (arg);
}
int vtkImageView2D::GetKeyboardInteractionStyle()
{
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
        return t->GetKeyboardInteraction ();
    return -1;
}

/**
Change the interaction triggered by the mouse buttons.
Choices are listed in vtkInteractorStyleImageView2D class:
InteractionTypeSlice : changes the slice number.
InteractionTypeWindowLevel : changes the window-level values.
InteractionTypeZoom : changes the zoom level.
InteractionTypePan : translate the view in-plane.
*/
void vtkImageView2D::SetRightButtonInteractionStyle (int arg)
{
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
        t->SetRightButtonInteraction (arg);
}
int vtkImageView2D::GetRightButtonInteractionStyle()
{
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
        return t->GetRightButtonInteraction ();
    return -1;
}
/**
Change the interaction triggered by the mouse buttons.
Choices are listed in vtkInteractorStyleImageView2D class:
InteractionTypeSlice : changes the slice number.
InteractionTypeWindowLevel : changes the window-level values.
InteractionTypeZoom : changes the zoom level.
InteractionTypePan : translate the view in-plane.
*/
void vtkImageView2D::SetMiddleButtonInteractionStyle (int arg)
{
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
        t->SetMiddleButtonInteraction (arg);
}
int vtkImageView2D::GetMiddleButtonInteractionStyle()
{
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
        return t->GetMiddleButtonInteraction ();
    return -1;
}
/**
Change the interaction triggered by the mouse buttons.
Choices are listed in vtkInteractorStyleImageView2D class:
InteractionTypeSlice : changes the slice number.
InteractionTypeWindowLevel : changes the window-level values.
InteractionTypeZoom : changes the zoom level.
InteractionTypePan : translate the view in-plane.
*/
void vtkImageView2D::SetWheelInteractionStyle (int arg)
{
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
        t->SetWheelButtonInteraction (arg);
}
int vtkImageView2D::GetWheelInteractionStyle()
{
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
        return t->GetWheelButtonInteraction ();
    return -1;
}

/**
Change the interaction triggered by the mouse buttons.
Choices are listed in vtkInteractorStyleImageView2D class:
InteractionTypeSlice : changes the slice number.
InteractionTypeWindowLevel : changes the window-level values.
InteractionTypeZoom : changes the zoom level.
InteractionTypePan : translate the view in-plane.
*/
void vtkImageView2D::SetInteractionStyle (int arg)
{
    this->SetLeftButtonInteractionStyle (arg);
    this->SetRightButtonInteractionStyle (arg);
    this->SetMiddleButtonInteractionStyle (arg);
    this->SetWheelInteractionStyle (arg);
}
/**
Change the interaction triggered by the mouse buttons.
Choices are listed in vtkInteractorStyleImageView2D class:
InteractionTypeSlice : changes the slice number.
InteractionTypeWindowLevel : changes the window-level values.
InteractionTypeZoom : changes the zoom level.
InteractionTypePan : translate the view in-plane.
*/
int vtkImageView2D::GetInteractionStyle()
{
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
        return t->GetLeftButtonInteraction();
    else
        return 0;
}

//----------------------------------------------------------------------------
/**
Useful method that transform a display position into a world corrdinate point
*/
void vtkImageView2D::GetWorldCoordinatesFromDisplayPosition (int xy[2], double* position)
{
  vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : nullptr;
  if (!cam)
  {
    position[0] = position[1] = position[2] = 0;
    return;
  }

  this->GetRenderer()->SetDisplayPoint (xy[0], xy[1], 0);
  this->GetRenderer()->DisplayToWorld();
  vtkPlane::ProjectPoint(this->GetRenderer()->GetWorldPoint(), this->CurrentPoint, cam->GetViewPlaneNormal(), position);
}

//----------------------------------------------------------------------------
/**
Overwrite of the Superclass InstallPipeline() method in order to set up the
home made InteractorStyle, and make it observe all events we need
*/
void vtkImageView2D::InstallPipeline()
{
  this->Superclass::InstallPipeline();

  if (this->GetRenderer())
  {
    this->GetRenderer()->AddViewProp( this->OrientationAnnotation );
    this->GetRenderer()->GetActiveCamera()->ParallelProjectionOn();
  }

  if( this->InteractorStyle )
  {
    if ( !this->InteractorStyle->HasObserver (vtkImageView2DCommand::SliceMoveEvent, this->Command) )
      this->InteractorStyle->AddObserver (vtkImageView2DCommand::SliceMoveEvent, this->Command, 10);

    if ( !this->InteractorStyle->HasObserver (vtkImageView2DCommand::TimeChangeEvent, this->Command) )
      this->InteractorStyle->AddObserver (vtkImageView2DCommand::TimeChangeEvent, this->Command, 10);

    if ( !this->InteractorStyle->HasObserver (vtkImageView2DCommand::RequestedPositionEvent, this->Command) )
      this->InteractorStyle->AddObserver (vtkImageView2DCommand::RequestedPositionEvent, this->Command, 10);

    if( !this->InteractorStyle->HasObserver (vtkCommand::LeaveEvent, this->Command) )
      this->InteractorStyle->AddObserver (vtkCommand::LeaveEvent, this->Command, 10);

    if ( !this->InteractorStyle->HasObserver (vtkImageView2DCommand::RequestedCursorInformationEvent, this->Command) )
      this->InteractorStyle->AddObserver (vtkImageView2DCommand::RequestedCursorInformationEvent, this->Command, 10);

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
  }
}

//----------------------------------------------------------------------------
/**
Overwrite of the Superclass UnInstallPipeline() method in order to set up the
home made InteractorStyle, and make it observe all events we need
*/
void vtkImageView2D::UnInstallPipeline()
{
    if ( this->GetRenderer() )
    {
        this->GetRenderer()->RemoveViewProp ( this->OrientationAnnotation );
    }

    if( this->InteractorStyle )
    {
        this->InteractorStyle->RemoveObserver ( this->Command );
    }

    this->Superclass::UnInstallPipeline();
}

//----------------------------------------------------------------------------
/** Start/Stop the interactor relation with the view.
 it basically plug or unplug the interactor.
 */
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
      for (auto it : LayerInfoVec)
      {
          if (vtkRenderer *renderer = it.Renderer)
          {
              this->RenderWindow->AddRenderer(renderer);
          }
      }
  }

  this->Axes2DWidget->SetImageView (this);
  this->Axes2DWidget->SetRenderWindow(this->RenderWindow);

  if ( this->RulerWidget->GetInteractor() && this->ShowRulerWidget)
    this->RulerWidget->On();

  if ( this->DistanceWidget->GetInteractor() && this->ShowDistanceWidget)
    this->DistanceWidget->On();

  if ( this->AngleWidget->GetInteractor() && this->ShowAngleWidget)
    this->AngleWidget->On();

  if( this->ShowImageAxis && this->RenderWindow && this->GetRenderer())
    this->Axes2DWidget->On();

  for (std::list<vtkDataSet2DWidget*>::iterator it = this->DataSetWidgets.begin();
      it!=this->DataSetWidgets.end(); ++it)
  {
    (*it)->SetImageView(this);
    (*it)->On();
  }

  this->IsInteractorInstalled = 1;
}

//----------------------------------------------------------------------------
void vtkImageView2D::UnInstallInteractor()
{
    this->RulerWidget->SetInteractor ( nullptr );
    this->DistanceWidget->SetInteractor ( nullptr );
    this->AngleWidget->SetInteractor ( nullptr );
    this->Axes2DWidget->SetImageView ( nullptr );

    if (this->Interactor)
    {
        this->Interactor->SetRenderWindow (nullptr);
        this->Interactor->SetInteractorStyle (nullptr);
    }

    if (this->RenderWindow)
    {
        for (auto it : LayerInfoVec)
        {
            if (vtkRenderer *renderer = it.Renderer)
            {
                this->RenderWindow->RemoveRenderer(renderer);
            }
        }
    }

    for (std::list<vtkDataSet2DWidget*>::iterator it = this->DataSetWidgets.begin(); it!=this->DataSetWidgets.end(); ++it )
    {
        (*it)->SetImageView(nullptr);
        (*it)->Off();
    }

    this->IsInteractorInstalled = 0;
}

//----------------------------------------------------------------------------
/**
 Get/Set weither or not the interpolation between pixels should be activated.
 It is On by default
*/
void vtkImageView2D::SetInterpolate(int val, int layer)
{
    if (this->HasLayer(layer))
    {
        this->GetImage2DDisplayForLayer(layer)->GetImageActor()->SetInterpolate (val);
        this->qtSignalHandler->emitInterpolate((bool)val, layer);
        this->Modified();
    }
}

//----------------------------------------------------------------------------
int vtkImageView2D::GetInterpolate(int layer) const
{
    int iRes = 0;
    if (this->HasLayer(layer))
    {
        iRes = this->GetImage2DDisplayForLayer(layer)->GetImageActor()->GetInterpolate();
    }

  return iRes;
}

//----------------------------------------------------------------------------
void vtkImageView2D::ApplyColorTransferFunction(vtkScalarsToColors * colors, int layer)
{
  vtkImage2DDisplay * imageDisplay = this->GetImage2DDisplayForLayer(layer);
  if (!imageDisplay)
      return;
  imageDisplay->GetWindowLevel()->SetLookupTable(colors);
}

void vtkImageView2D::SetFirstLayer(vtkAlgorithmOutput *pi_poInputAlgoImg, vtkMatrix4x4 *matrix, int layer)
{
    vtkImage2DDisplay *imageDisplay = this->GetImage2DDisplayForLayer(layer);
    imageDisplay->SetInputProducer(pi_poInputAlgoImg);
    this->Superclass::SetInput (pi_poInputAlgoImg, matrix, 0);
    imageDisplay->SetInputData(m_poInternalImageFromInput);
}

/**
 @brief If the layer numbered "layer" is the first of the stack of layers to contain an image then it is the first layer.
 @return True if is the first layer to contain an image.
 False in other cases.
*/
bool vtkImageView2D::IsFirstLayer(int layer) const
{
    bool firstLayer = true;
    for ( size_t i(0); i<this->LayerInfoVec.size() && i<(unsigned int)layer; ++i )
    {
        if (this->LayerInfoVec[i].ImageDisplay && this->LayerInfoVec[i].ImageDisplay->isInputSet())
        {
            firstLayer = false;
            break;
        }
    }
    return firstLayer;
}

int vtkImageView2D::GetFirstLayer() const
{
    for ( size_t i(0); i<this->LayerInfoVec.size(); ++i )
    {
        if (this->LayerInfoVec[i].ImageDisplay && this->LayerInfoVec[i].ImageDisplay->isInputSet())
        {
            return static_cast<int>(i);
        }
    }
    return 0;
}

//----------------------------------------------------------------------------
/** Set/Get the input image to the viewer. */
void vtkImageView2D::SetInput(vtkAlgorithmOutput* pi_poVtkAlgoOutput, vtkMatrix4x4 *matrix /*= 0*/, int layer /*= 0*/)
{
    if (pi_poVtkAlgoOutput)
    {
        if (layer == 0 || IsFirstLayer(layer))
        {
            SetFirstLayer(pi_poVtkAlgoOutput, matrix, layer);
        }
        else // layer > 0
        {
            SetInputLayer(pi_poVtkAlgoOutput, matrix, layer);
        }

        SetInputCommon(pi_poVtkAlgoOutput, layer);
    }
}

void vtkImageView2D::SetInputLayer(vtkAlgorithmOutput* pi_poVtkAlgoOutput, vtkMatrix4x4 *matrix /*= 0*/, int layer /*= 0*/)
{
    // layer > 0
    this->AddLayer(layer);
    pi_poVtkAlgoOutput = this->ResliceImageToInput(pi_poVtkAlgoOutput, matrix);
    vtkImage2DDisplay *imageDisplay = this->GetImage2DDisplayForLayer(layer);
    if (imageDisplay)
    {
        imageDisplay->SetInputProducer(pi_poVtkAlgoOutput);
        imageDisplay->SetInputData(static_cast<vtkImageAlgorithm*>(pi_poVtkAlgoOutput->GetProducer())->GetOutput());
        imageDisplay->GetImageActor()->SetUserMatrix (this->OrientationMatrix);
        if(imageDisplay->GetMedVtkImageInfo())
        {
            this->SetColorRange(imageDisplay->GetMedVtkImageInfo()->scalarRange, layer);
        }
    }
}

void vtkImageView2D::SetInputCommon(vtkAlgorithmOutput* pi_poVtkAlgoOutput, int layer /*= 0*/)
{
    this->LayerInfoVec[layer].ImageAlgo = static_cast<vtkImageAlgorithm*>(pi_poVtkAlgoOutput->GetProducer());

    vtkRenderer *renderer = this->GetRendererForLayer(layer);
    if (renderer)
    {
        renderer->AddViewProp (this->GetImage2DDisplayForLayer(layer)->GetImageActor());

        this->SetCurrentLayer(layer);
        this->Slice = this->GetSliceForWorldCoordinates (this->CurrentPoint);
        this->UpdateDisplayExtent();
        this->UpdateSlicePlane();
        this->InvokeEvent (vtkImageView2D::SliceChangedEvent);

        renderer->AddViewProp(this->CornerAnnotation);
        renderer->AddViewProp(this->ScalarBar);
        renderer->AddViewProp(this->OrientationAnnotation);

        if(this->ShowRulerWidget)
        {
            this->ShowRulerWidgetOff();
            this->ShowRulerWidgetOn();
        }
    }
}

void vtkImageView2D::SetInput (vtkActor *actor, int layer, vtkMatrix4x4 *matrix, const int imageSize[], const double imageSpacing[], const double imageOrigin[])
{
    vtkRenderer *renderer = 0;

    this->AddLayer(layer);

    renderer = this->GetRendererForLayer(layer);
    if (!renderer)
      return;

    renderer->AddActor(actor);

    double bounds[6];
    actor->GetBounds(bounds);

    // these bounds are used by vtkImageFromBoundsSource to generate a background image in case there is none
    // vtkImageFromBoundsSource output image size is actually [boundsXMax-boundXMin]...,
    // so we need to increase bounds by +1 to have the correct image size
    bounds[0] = floor(bounds[0]+0.5);
    bounds[1] = floor(bounds[1]+0.5)+1;
    bounds[2] = floor(bounds[2]+0.5);
    bounds[3] = floor(bounds[3]+0.5)+1;
    bounds[4] = floor(bounds[4]+0.5);
    bounds[5] = floor(bounds[5]+0.5)+1;

    unsigned int numberOfLayers = GetNumberOfLayers();
    UpdateBounds(bounds, layer, matrix, imageSize, imageSpacing, imageOrigin);

    this->SetCurrentLayer(layer);
    this->Slice = this->GetSliceForWorldCoordinates (this->CurrentPoint);
    this->UpdateDisplayExtent();
    this->UpdateSlicePlane();
    this->InvokeEvent (vtkImageView2D::SliceChangedEvent);

    if ((numberOfLayers == 0) && matrix)
        this->SetOrientationMatrix(matrix);
}

void vtkImageView2D::RemoveLayerActor(vtkActor *actor, int layer)
{
    vtkRenderer *renderer = this->GetRendererForLayer(layer);

    if (!renderer)
        return;

    renderer->RemoveActor(actor);

    this->SetCurrentLayer(layer);
    this->Slice = this->GetSliceForWorldCoordinates (this->CurrentPoint);
    this->UpdateDisplayExtent();
    this->UpdateSlicePlane();
    this->InvokeEvent (vtkImageView2D::SliceChangedEvent);
}

//----------------------------------------------------------------------------
/**
Return the vtkImageActor's instance.
*/
vtkImageActor *vtkImageView2D::GetImageActor(int layer) const
{
    int tempLayer = layer;
    if(tempLayer == 0)
        tempLayer = GetFirstLayer();

    if (!this->HasLayer(tempLayer))
        return nullptr;

    return this->GetImage2DDisplayForLayer(tempLayer)->GetImageActor();
}

//----------------------------------------------------------------------------
medVtkImageInfo* vtkImageView2D::GetMedVtkImageInfo(int layer) const
{
    medVtkImageInfo *imageInfo = nullptr;

    if (layer == 0)
    {
        layer = GetFirstLayer();
    }
    if (this->HasLayer(layer))
    {
        imageInfo = this->GetImage2DDisplayForLayer(layer)->GetMedVtkImageInfo();
    }

    return imageInfo;
}

//----------------------------------------------------------------------------
/**
Set/Get the ruler widget visibility.
*/
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
/**
Set/Get the ruler widget visibility.
*/
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
/**
Set/Get the ruler widget visibility.
*/
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
/**
Set/Get the axes2D widget visibility.
*/
void vtkImageView2D::SetShowImageAxis (int val)
{
  this->ShowImageAxis = val;
  if (this->Axes2DWidget->GetImageView() && this->RenderWindow && this->GetRenderer())
  {
    this->Axes2DWidget->SetEnabled(val);
    this->Modified();
  }
}

//----------------------------------------------------------------------------
/**
Set/Get whether the cursor should follow the mouse or not.
*/
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
/**
 adding a PointSet to the view.
 The method will "cut" the dataset, therefore reducing the dimension by 1.
 e.g. volumes become surfaces, surfaces lines, and lines points.
*/
vtkActor* vtkImageView2D::AddDataSet(vtkPointSet* arg, vtkProperty* prop)
{
  if (this->FindDataSetWidget (arg) != this->DataSetWidgets.end())
    return nullptr;

  vtkDataSet2DWidget* widget = vtkDataSet2DWidget::New();
  widget->GetActor()->SetProperty (prop);
  widget->SetSource (arg);
  widget->SetImageView (this);

  this->DataSetCollection->AddItem (arg);
  this->DataSetActorCollection->AddItem ( widget->GetActor());

  if ( this->GetIsInteractorInstalled () )
  {
    widget->On();
    this->Modified();
  }
  this->DataSetWidgets.push_back( widget );

  // If this is the first widget to be added, reset camera
  if ( (!this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized) && (this->DataSetWidgets.size() == 1))
  {
      this->ResetCamera(arg);
  }

  return widget->GetActor();
}

void vtkImageView2D::UpdateBounds (const double bounds[6], int layer, vtkMatrix4x4 *matrix, const int imageSize[3], const double imageSpacing[3], const double imageOrigin[3])
{
    bool isImageOutBounded = false;
    double imageBounds[6];

    if( GetNumberOfLayers() == 0 )
    {
        if (imageSize)
        {
            for (int i=0; i<6; i+=2)
                imageBounds[i]=0;
            for (int i=1; i<6; i+=2)
                imageBounds[i]=imageSize[(i-1)/2];
        }
        else
        {
            for (int i=0; i<6; i++)
            {
                imageBounds[i]=bounds[i];
            }
        }
        isImageOutBounded = true;
    }

    if(isImageOutBounded)
    {
        m_vtkImageFromBoundsSourceGenerator = vtkImageFromBoundsSource::New();
        unsigned int imSize [3];
        if( imageSize != nullptr )
        {
            imSize[0] = imageSize[0];
            imSize[1] = imageSize[1];
            imSize[2] = imageSize[2];
        }
        else
        {
            imSize[0]=100;
            imSize[1]=100;
            imSize[2]=100;
        }

        m_vtkImageFromBoundsSourceGenerator->SetOutputImageSize(imSize);
        if (imageSpacing)
            m_vtkImageFromBoundsSourceGenerator->SetOutputImageSpacing(imageSpacing);
        if (imageOrigin)
            m_vtkImageFromBoundsSourceGenerator->SetOutputImageOrigin(imageOrigin);

        m_vtkImageFromBoundsSourceGenerator->SetOutputImageBounds(imageBounds);

        m_vtkImageFromBoundsSourceGenerator->UpdateInformation();
        m_vtkImageFromBoundsSourceGenerator->Update();

        SetInput(m_vtkImageFromBoundsSourceGenerator->GetOutputPort(), matrix, layer);
        vtkImageActor *actor = GetImageActor(layer);
        actor->SetOpacity(0.0);
        isImageOutBounded=false;
        ResetCamera();
    }
}

//----------------------------------------------------------------------------
void vtkImageView2D::RemoveDataSet (vtkPointSet *arg)
{
  this->Superclass::RemoveDataSet (arg);

  for (std::list<vtkDataSet2DWidget*>::iterator it = this->DataSetWidgets.begin();
      it != this->DataSetWidgets.end(); ++it)
  {
    if ((*it)->GetSource()==arg)
    {
      (*it)->Off();
      (*it)->SetImageView (nullptr);
      (*it)->Delete();
      this->DataSetWidgets.erase (it);
      this->Modified();
      break;
    }
  }
}

//----------------------------------------------------------------------------
std::list<vtkDataSet2DWidget*>::iterator vtkImageView2D::FindDataSetWidget(vtkPointSet* arg)
{

    for (  std::list<vtkDataSet2DWidget*>::iterator it = this->DataSetWidgets.begin();
            it != this->DataSetWidgets.end(); ++it)
  {
    vtkDataSet2DWidget* widget = (*it);
    if (widget->GetSource() == arg)
      return it;
  }

  return this->DataSetWidgets.end();
}

//----------------------------------------------------------------------------
void vtkImageView2D::AddLayer(int layer)
{
  if (this->HasLayer(layer))
    return;

  if ( layer >= (int)this->LayerInfoVec.size() ) {
      this->LayerInfoVec.resize(layer + 1);
  }

  // needs to instantiate objects for layers being created
  for ( size_t i(0); i<this->LayerInfoVec.size(); ++i )
  {
      if (!this->LayerInfoVec[i].ImageDisplay)
      {
          this->LayerInfoVec[i].Renderer = vtkSmartPointer<vtkRenderer>::New();
          this->LayerInfoVec[i].ImageDisplay = vtkSmartPointer<vtkImage2DDisplay>::New();
          this->SetTransferFunctions(nullptr,nullptr, static_cast<int>(i));
      }
  }

  vtkRenderer *renderer = this->GetRendererForLayer(layer);
  renderer->SetLayer(layer);
  if (this->GetRenderWindow())
  {
    // Additional layer for the overlay
    int numLayers = this->GetNumberOfLayers();
    if ( this->OverlayRenderer )
    {
        this->OverlayRenderer->SetLayer(numLayers);
        ++numLayers;
    }
    this->GetRenderWindow()->SetNumberOfLayers (numLayers+1);
    this->GetRenderWindow()->AddRenderer(renderer);
  }

  if (this->GetRenderer())
    renderer->SetActiveCamera (this->GetRenderer()->GetActiveCamera());
}

//----------------------------------------------------------------------------
void vtkImageView2D::RemoveLayer(int layer)
{  
    if (this->HasLayer(layer))
    {
        // ////////////////////////////////////////////////////////////////////////
        // Save image informations of layer 0
        double  bounds[6];
        vtkMatrix4x4 *matrix  = nullptr;
        int    *imageSize     = nullptr;
        double *imageSpacing  = nullptr;
        double *imageOrigin   = nullptr;    
    
        medVtkImageInfo   sImgInfo;
        medVtkImageInfo* psImgInfo  = GetMedVtkImageInfo();
        GetInputBounds(bounds);
  
        if (psImgInfo)
        {
            sImgInfo = *psImgInfo;

            matrix = GetOrientationMatrix();
            imageSize = sImgInfo.dimensions;
            imageSpacing = sImgInfo.spacing;
            imageOrigin = sImgInfo.origin;
        }


        // ////////////////////////////////////////////////////////////////////////
        // Remove layer
        vtkRenderer *renderer = this->GetRendererForLayer(layer);
        vtkImageView::RemoveLayer(layer);
        if (renderer && this->GetRenderWindow())
        { 
            renderer->RemoveAllViewProps();
            renderer->Render();
            for (auto poWidget : this->DataSetWidgets)
            {
                poWidget->SetImageView(nullptr);
                poWidget->Off();
            }
            this->GetRenderWindow()->RemoveRenderer(renderer);
            this->Modified();
        }

        // Delete is handled by SmartPointers.
        this->LayerInfoVec.erase(this->LayerInfoVec.begin() + layer);

  
        // ////////////////////////////////////////////////////////////////////////
        // Rebuild a layer if necessary
        if (this->LayerInfoVec.size() == 0 )
        {
            AddLayer(0);
            for (auto poWidget : this->DataSetWidgets)
            {
                poWidget->SetImageView(this);
                poWidget->On();
                this->UpdateBounds(poWidget->GetSource()->GetBounds(), 0, matrix, imageSize, imageSpacing, imageOrigin);
                this->Modified();
                // If this is the first widget to be added, reset camera
                if ((!this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized) && (this->DataSetWidgets.size() == 1))
                {
                    this->ResetCamera(poWidget->GetSource());
                }
            }
        }

        // Make contiguous
        for ( size_t i(0); i<this->LayerInfoVec.size(); ++i )
        {
            if( this->LayerInfoVec[i].Renderer )
                this->LayerInfoVec[i].Renderer->SetLayer(static_cast<int>(i));
            this->SetCurrentLayer(static_cast<int>(i));
        }
    }
}

//----------------------------------------------------------------------------
void vtkImageView2D::RemoveAllLayers()
{
  while ( this->LayerInfoVec.size()>1 )
  {
    this->RemoveLayer (static_cast<int>(this->LayerInfoVec.size() - 1));
  }
}

//----------------------------------------------------------------------------
int vtkImageView2D::GetNumberOfLayers() const
{
    // I don't really know why, but LayerInfoVec size is set to 1 at initialisation time,
    // so we need one more check to know the real number of layer
    if( this->LayerInfoVec.size() == 1)
    {
        if( !this->LayerInfoVec.at(0).ImageDisplay->isInputSet())
            return 0;
        else return 1;
    }
    else return static_cast<int>(this->LayerInfoVec.size());
}

vtkImage2DDisplay * vtkImageView2D::GetImage2DDisplayForLayer( int layer ) const
{
    if ( layer > -1 && (unsigned int)layer < this->LayerInfoVec.size())
    {
        return this->LayerInfoVec.at(layer).ImageDisplay;
    }
    else return nullptr;
}

vtkRenderer * vtkImageView2D::GetRendererForLayer( int layer ) const
{
    if ( layer > -1 && (unsigned int)layer < this->LayerInfoVec.size())
    {
        return this->LayerInfoVec.at(layer).Renderer;
    }
    else return nullptr;
}

vtkImageAlgorithm * vtkImageView2D::GetImageAlgorithmForLayer(int layer) const
{
    if (layer > -1 && (unsigned int)layer < this->LayerInfoVec.size())
    {
        return this->LayerInfoVec.at(layer).ImageAlgo;
    }
    else return nullptr;
}

vtkRenderer * vtkImageView2D::GetRenderer() const
{
    return this->LayerInfoVec.at(this->CurrentLayer).Renderer;
}

//! Get the WindowLevel for given layer. Overrides Superclass.
vtkImageMapToColors * vtkImageView2D::GetWindowLevel( int layer/*=0*/ ) const
{
    vtkImage2DDisplay * imageDisplay = this->GetImage2DDisplayForLayer(layer);
    if (imageDisplay)
        return imageDisplay->GetWindowLevel();
    else return nullptr;
}

vtkAlgorithm* vtkImageView2D::Get2DDisplayMapperInputAlgorithm (int layer) const
{
    return this->GetWindowLevel(layer);
}

vtkImageAlgorithm* vtkImageView2D::GetInputAlgorithm(int layer) const
{
    vtkImageAlgorithm *poRes = nullptr;
    
    vtkImage2DDisplay * imageDisplay = this->GetImage2DDisplayForLayer(layer);
    if (imageDisplay)
        poRes = imageDisplay->GetInputProducer();

    return poRes;
}

////----------------------------------------------------------------------------


void vtkImageView2D::SetTransferFunctionRangeFromWindowSettings(int layer)
{
  this->Superclass::SetTransferFunctionRangeFromWindowSettings(layer);
  vtkImage2DDisplay * imageDisplay = this->GetImage2DDisplayForLayer(layer);
  imageDisplay->GetWindowLevel()->Modified();

}
double vtkImageView2D::GetColorLevel(int layer)const
{
  vtkImage2DDisplay * imageDisplay = this->GetImage2DDisplayForLayer(layer);
  if (imageDisplay)
  {
    return imageDisplay->GetColorLevel();
  }
  else return 0.0;
}

double vtkImageView2D::GetColorWindow(int layer)const
{
  vtkImage2DDisplay * imageDisplay = this->GetImage2DDisplayForLayer(layer);
  if (imageDisplay)
  {
    return imageDisplay->GetColorWindow();
  }
  else return 0.0;
}

void vtkImageView2D::StoreColorWindow(double s,int layer)
{
  vtkImage2DDisplay * imageDisplay = this->GetImage2DDisplayForLayer(layer);
  if (!imageDisplay)
    return;
  imageDisplay->SetColorWindow( s );
}

void vtkImageView2D::StoreColorLevel(double s,int layer)
{
    vtkImage2DDisplay * imageDisplay = this->GetImage2DDisplayForLayer(layer);
    if (!imageDisplay)
        return;
    imageDisplay->SetColorLevel(s);
}

vtkColorTransferFunction * vtkImageView2D::GetColorTransferFunction(int layer) const
{
  vtkImage2DDisplay * imageDisplay = this->GetImage2DDisplayForLayer(layer);
  if (!imageDisplay)
      return nullptr;
  return imageDisplay->GetColorTransferFunction();
}

vtkPiecewiseFunction * vtkImageView2D::GetOpacityTransferFunction(int layer) const
{
  vtkImage2DDisplay * imageDisplay = this->GetImage2DDisplayForLayer(layer);
  if (!imageDisplay)
      return nullptr;
  return imageDisplay->GetOpacityTransferFunction();
}

void vtkImageView2D::StoreColorTransferFunction(vtkColorTransferFunction *ctf, int layer)
{
  vtkImage2DDisplay * imageDisplay = this->GetImage2DDisplayForLayer(layer);
  if (!imageDisplay)
      return;
  imageDisplay->SetColorTransferFunction(ctf);
}

void vtkImageView2D::StoreOpacityTransferFunction(vtkPiecewiseFunction *otf, int layer)
{
  vtkImage2DDisplay * imageDisplay = this->GetImage2DDisplayForLayer(layer);
  if (!imageDisplay)
      return;
  imageDisplay->SetOpacityTransferFunction(otf);
}

vtkLookupTable * vtkImageView2D::GetLookupTable(int layer) const
{
  vtkImage2DDisplay * imageDisplay = this->GetImage2DDisplayForLayer(layer);
  if (!imageDisplay)
      return nullptr;
  return imageDisplay->GetLookupTable();
}

void vtkImageView2D::StoreLookupTable(vtkLookupTable* lookupTable, int layer)
{
  vtkImage2DDisplay * imageDisplay = this->GetImage2DDisplayForLayer(layer);
  if (!imageDisplay)
      return;
  imageDisplay->GetWindowLevel()->SetLookupTable(lookupTable);
}

bool vtkImageView2D::GetUseLookupTable(int layer) const
{
  vtkImage2DDisplay * imageDisplay = this->GetImage2DDisplayForLayer(layer);
  if (!imageDisplay)
      return false;
  return imageDisplay->GetUseLookupTable();
}

void vtkImageView2D::SetUseLookupTable(bool use, int layer)
{
  vtkImage2DDisplay * imageDisplay = this->GetImage2DDisplayForLayer(layer);
  if (!imageDisplay)
      return;
  imageDisplay->SetUseLookupTable(use);
}
