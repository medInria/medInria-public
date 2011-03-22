/*=========================================================================
 
 Program:   vtkINRIA3D
 Module:    $Id: vtkImageView3D.cxx 1324 2009-11-17 13:19:09Z filus $
 Language:  C++
 Author:    $Author: filus $
 Date:      $Date: 2009-11-17 14:19:09 +0100 (mar, 17 nov 2009) $
 Version:   $Revision: 1324 $
 
 Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
 See Copyright.txt for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/
#include "vtkImageView3D.h"

#ifndef VTK_MAJOR_VERSION
#  include "vtkVersion.h"
#endif

#include <vtkBoundingBox.h>
#include <vtkInteractorStyleTrackball.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRendererCollection.h>
#include <vtkImageData.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkFiniteDifferenceGradientEstimator.h>
#include <vtkProperty.h>
#include <vtkVolume.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkImageActor.h>
#include <vtkAxes.h>
#include <vtkMatrix4x4.h>
#include <vtkTubeFilter.h>
#include <vtkLookupTable.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkPropAssembly.h>
#include <vtkAxesActor.h>
#include <vtkTextProperty.h>
#include <vtkCaptionActor2D.h>
#include <vtkPointData.h>
#include <vtkImageBlend.h>
#include <vtkImageReslice.h>
#include "vtkRenderWindow.h"
#include "vtkScalarsToColors.h"
//#include <vtkDataSet3DCroppingPlaneCallback.h>
#include <vtkCamera.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkPlane.h>
#include <vtkPlaneCollection.h>
#include <vtkGeometryFilter.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkCellData.h>
#include <vtkMath.h>
#include <vtkOrientedBoxWidget.h>
#include <vtkPlaneWidget.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkVolumeProperty.h>
#include <vtkImageMapToColors.h>
#include "vtkScalarBarActor.h"
#include "vtkSmartVolumeMapper.h"
#include <vtkImageAppendComponents.h>
#include <vtkImageAppend.h>
#include <vtkImageCast.h>
#include <vtkSmartPointer.h>

class vtkImage3DDisplay : public vtkObject
{
public:
  static vtkImage3DDisplay *New();
  vtkTypeRevisionMacro (vtkImage3DDisplay, vtkObject);
  
  vtkSetObjectMacro(Input, vtkImageData);
  vtkGetObjectMacro(Input, vtkImageData);
  
  vtkSetMacro(Opacity, double);
  vtkGetMacro(Opacity, double);
  
  vtkSetMacro(Visibility, int);
  vtkGetMacro(Visibility, int);
  
protected:
  vtkImage3DDisplay();
  ~vtkImage3DDisplay();
  
private:
  vtkImageData         *Input;
  double Opacity;
  int    Visibility;
  
  vtkImage3DDisplay(const vtkImage3DDisplay&);
  void operator=(const vtkImage3DDisplay&);
};

vtkCxxRevisionMacro(vtkImage3DDisplay, "$Revision: $");
vtkStandardNewMacro(vtkImage3DDisplay);

vtkImage3DDisplay::vtkImage3DDisplay()
{
  this->Input      = 0;
  this->Opacity    = 1.0;
  this->Visibility = 1;
}

vtkImage3DDisplay::~vtkImage3DDisplay()
{
  if (this->Input)
  {
    this->Input->Delete();
  }
}



vtkCxxRevisionMacro(vtkImageView3D, "$Revision: 1324 $");
vtkStandardNewMacro(vtkImageView3D);

//----------------------------------------------------------------------------
vtkImageView3D::vtkImageView3D()
{
  this->VolumeProperty  = vtkVolumeProperty::New();
  this->VolumeActor     = vtkVolume::New();
  
  this->VolumeMapper = vtkSmartVolumeMapper::New();
  
  this->Callback    = vtkImageView3DCroppingBoxCallback::New();
  this->BoxWidget   = vtkOrientedBoxWidget::New();
  this->PlaneWidget = vtkPlaneWidget::New();
  this->Marker      = vtkOrientationMarkerWidget::New();
  this->Cube        = vtkAnnotatedCubeActor::New();
  
  this->ActorX = vtkImageActor::New();
  this->ActorY = vtkImageActor::New();
  this->ActorZ = vtkImageActor::New();

  this->VolumeActor->SetVisibility (0);
  
  this->SetupVolumeRendering();
  this->SetupWidgets();  
  
  this->ShowAnnotationsOn();
  this->TextProperty->SetColor (0,0,0);
  double white[3] = {0.9, 0.9, 0.9};
  this->SetBackground (white);
  
  this->RenderingMode = PLANAR_RENDERING;
  this->ShowActorX = 1;
  this->ShowActorY = 1;
  this->ShowActorZ = 1;
    
  this->Opacity    = 1.0;
  this->Visibility = 1;
  
  this->CroppingMode = CROPPING_OFF;
  
  vtkInteractorStyleSwitch* styleswitch = vtkInteractorStyleSwitch::New();
  styleswitch->SetCurrentStyleToTrackballCamera();
  this->SetInteractorStyle ( styleswitch );
  styleswitch->Delete();
}

//----------------------------------------------------------------------------
vtkImageView3D::~vtkImageView3D()
{
  // delete all vtk objects
  std::map<int, vtkImage3DDisplay*>::iterator it = this->ImageDisplayMap.begin();
  while (it!=this->ImageDisplayMap.end())
  {
    if (it->second)
    {
      it->second->Delete();
    }
    ++it;
  }
  this->ImageDisplayMap.clear();
  
  this->VolumeMapper->Delete();
  this->VolumeProperty->Delete();
  this->VolumeActor->Delete();
  this->BoxWidget->Delete();
  this->Callback->Delete();
  this->Cube->Delete();
  this->Marker->Delete();
  this->PlaneWidget->Delete();
  this->ActorX->Delete();
  this->ActorY->Delete();
  this->ActorZ->Delete();
}

//----------------------------------------------------------------------------
unsigned long vtkImageView3D::GetMTime()
{
  typedef unsigned long MTimeType;
  
  MTimeType mTime = Superclass::GetMTime();
  
  vtkObject * objectsToInclude[] = {
    this->VolumeMapper,
    this->VolumeProperty,
    this->VolumeActor,
    this->BoxWidget,
    this->Cube,
    this->Marker,
    this->PlaneWidget,
    this->ActorX,
    this->ActorY,
    this->ActorZ};
  
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
void vtkImageView3D::SetVolumeMapperTo3DTexture (void)
{
  this->VolumeMapper->SetRequestedRenderMode(
                                             vtkSmartVolumeMapper::TextureRenderMode );
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeMapperToRayCast (void)
{
  this->VolumeMapper->SetRequestedRenderMode(
                                             vtkSmartVolumeMapper::RayCastRenderMode );
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeMapperToRayCastAndTexture (void)
{
  this->VolumeMapper->SetRequestedRenderMode(
                                             vtkSmartVolumeMapper::RayCastAndTextureRenderMode );
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeMapperToGPU (void)
{
  this->VolumeMapper->SetRequestedRenderMode(
                                             vtkSmartVolumeMapper::GPURenderMode );
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeMapperToDefault (void)
{
  this->VolumeMapper->SetRequestedRenderMode(
                                             vtkSmartVolumeMapper::DefaultRenderMode );
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeRayCastFunctionToComposite (void)
{
  this->VolumeMapper->SetBlendModeToComposite();
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeRayCastFunctionToMaximumIntensityProjection (void)
{
  this->VolumeMapper->SetBlendModeToMaximumIntensity();
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeRayCastFunctionToMinimumIntensityProjection (void)
{
  this->VolumeMapper->SetBlendModeToMinimumIntensity();
}

//----------------------------------------------------------------------------
// void vtkImageView3D::SetVolumeRayCastFunctionToAdditive (void)
// {
//   this->VolumeMapper->SetBlendModeToAdditive();
// }

//----------------------------------------------------------------------------
void vtkImageView3D::SetInterpolationToNearestNeighbor (void)
{
  this->VolumeProperty->SetInterpolationTypeToNearest();
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetInterpolationToLinear (void)
{
  this->VolumeProperty->SetInterpolationTypeToLinear();
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetupVolumeRendering()
{
#ifdef __APPLE__
  this->VolumeMapper->SetRequestedRenderMode( vtkSmartVolumeMapper::RayCastRenderMode );
#else
  this->VolumeMapper->SetRequestedRenderMode( vtkSmartVolumeMapper::DefaultRenderMode );
#endif
  this->SetCroppingModeToInside();
  
  this->VolumeProperty->IndependentComponentsOn();
  this->VolumeProperty->SetInterpolationTypeToLinear();
  this->VolumeProperty->ShadeOff();
  this->VolumeProperty->SetDiffuse (0.9);
  this->VolumeProperty->SetAmbient (0.15);
  this->VolumeProperty->SetSpecular (0.3);
  this->VolumeProperty->SetSpecularPower (15.0);
  if ( !this->UseLookupTable )
  {
    this->VolumeProperty->SetScalarOpacity(0, this->OpacityTransferFunction );
    this->VolumeProperty->SetColor(0, this->ColorTransferFunction );
  }
  
  this->VolumeActor->SetProperty ( this->VolumeProperty );
  this->VolumeActor->SetMapper ( this->VolumeMapper );
  this->VolumeActor->PickableOff();
  this->VolumeActor->DragableOff();  
  
  this->Callback->SetVolumeMapper ( this->VolumeMapper );
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetupWidgets()
{
  // Create an annotated cube actor (directions)
  this->Cube->SetXPlusFaceText ("L");
  this->Cube->SetXMinusFaceText ("R");
  this->Cube->SetYPlusFaceText ("P");
  this->Cube->SetYMinusFaceText ("A");
  this->Cube->SetZPlusFaceText ("S");
  this->Cube->SetZMinusFaceText ("I");
  this->Cube->SetZFaceTextRotation (90);
  this->Cube->SetFaceTextScale (0.65);
  this->Cube->GetCubeProperty()->SetColor (0.5, 1, 1);
  this->Cube->GetTextEdgesProperty()->SetLineWidth (1);
  this->Cube->GetTextEdgesProperty()->SetDiffuse (0);
  this->Cube->GetTextEdgesProperty()->SetAmbient (1);
  this->Cube->GetTextEdgesProperty()->SetColor (0.18, 0.28, 0.23);
  
#if VTK_MAJOR_VERSION==5 && VTK_MINOR_VERSION>=1
  this->Cube->SetTextEdgesVisibility (1);  
  this->Cube->SetCubeVisibility(1);  
  this->Cube->SetFaceTextVisibility(1);
#else
  this->Cube->TextEdgesOn ();
  this->Cube->FaceTextOn();
  this->Cube->CubeOn();
#endif
  
  this->Cube->GetXPlusFaceProperty()->SetColor (1, 0, 0);
  this->Cube->GetXPlusFaceProperty()->SetInterpolationToFlat();
  this->Cube->GetXMinusFaceProperty()->SetColor (1, 0, 0);
  this->Cube->GetXMinusFaceProperty()->SetInterpolationToFlat();
  this->Cube->GetYPlusFaceProperty()->SetColor (0, 1, 0);
  this->Cube->GetYPlusFaceProperty()->SetInterpolationToFlat();
  this->Cube->GetYMinusFaceProperty()->SetColor (0, 1, 0);
  this->Cube->GetYMinusFaceProperty()->SetInterpolationToFlat();
  this->Cube->GetZPlusFaceProperty()->SetColor (0, 0, 1);
  this->Cube->GetZPlusFaceProperty()->SetInterpolationToFlat();
  this->Cube->GetZMinusFaceProperty()->SetColor (0, 0, 1);
  this->Cube->GetZMinusFaceProperty()->SetInterpolationToFlat();
  
  this->Marker->SetOutlineColor (0.93, 0.57, 0.13);
  this->Marker->SetOrientationMarker (this->Cube);
  this->Marker->SetViewport (0.0, 0.05, 0.15, 0.15);
  
  this->BoxWidget->RotationEnabledOff();
  this->BoxWidget->SetPlaceFactor (0.5);
  this->BoxWidget->SetKeyPressActivationValue ('b');
  this->BoxWidget->AddObserver (vtkCommand::InteractionEvent, this->Callback);  
  
  this->PlaneWidget->SetKeyPressActivationValue ('p');
  //   this->PlaneWidget->NormalToZAxisOn();
}

//----------------------------------------------------------------------------
void vtkImageView3D::InstallPipeline()
{
  this->Superclass::InstallPipeline();
  
  if (this->Renderer)
  {
    this->Renderer->AddViewProp (this->VolumeActor);
    this->Renderer->AddViewProp (this->ActorX);
    this->Renderer->AddViewProp (this->ActorY);
    this->Renderer->AddViewProp (this->ActorZ);
  }
}

//----------------------------------------------------------------------------
void vtkImageView3D::UnInstallPipeline()
{  
  if (this->Renderer)
  {
    this->Renderer->RemoveViewProp (this->VolumeActor);
    this->Renderer->RemoveViewProp (this->ActorX);
    this->Renderer->RemoveViewProp (this->ActorY);
    this->Renderer->RemoveViewProp (this->ActorZ);
  }
  
  this->Superclass::UnInstallPipeline();
	
  this->IsInteractorInstalled = 0;
}

//----------------------------------------------------------------------------
void vtkImageView3D::InstallInteractor()
{
  if (this->Interactor && this->InteractorStyle)
  {
    this->Interactor->SetInteractorStyle (this->InteractorStyle);
  }
  
  if (this->Interactor && this->RenderWindow)
  {
    this->Interactor->SetRenderWindow(this->RenderWindow);
    
    this->BoxWidget->SetInteractor ( this->Interactor );
    this->PlaneWidget->SetInteractor ( this->Interactor );
    this->Marker->SetInteractor ( this->Interactor );
    
    this->Marker->On();
    this->Marker->InteractiveOff ();
  }
	
  this->IsInteractorInstalled = 1;
}

//----------------------------------------------------------------------------
void vtkImageView3D::UnInstallInteractor()
{
  this->BoxWidget->SetInteractor (NULL);
  this->PlaneWidget->SetInteractor (NULL);
  this->Marker->SetInteractor (NULL);
  
  if (this->Interactor)
  {
    this->Interactor->SetRenderWindow (NULL);
    this->Interactor->SetInteractorStyle (NULL);
  }
  this->IsInteractorInstalled = 0;    
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetInput(vtkImageData* image, vtkMatrix4x4 *matrix, int layer)
{
  if(layer==0)
  {
    if (this->GetInput())
    {
      this->RemoveAllLayers();
    }
    this->Superclass::SetInput (image, matrix, layer);
  }
  
  if( !image )
  {
    return;
  }
  
  image->UpdateInformation ();
  
  // Get whole extent : More reliable than dimensions if not up-to-date.
  int * w_extent = image->GetWholeExtent ();
  
  int size [3] = { w_extent [1] - w_extent[0], w_extent [3] - w_extent[2], w_extent [5] - w_extent[4] };
  
  //  int* size = image->GetDimensions();
  if ( (size[0] < 2) ||
      (size[1] < 2) ||
      (size[2] < 2) )
  {
    vtkWarningMacro ( <<"Cannot do volume rendering for a single slice, skipping"<<endl);
    this->ActorX->SetInput ( (vtkImageData*)0 );
    this->ActorY->SetInput ( (vtkImageData*)0 );
    this->ActorZ->SetInput ( (vtkImageData*)0 );
    
    this->VolumeMapper->SetInput( static_cast< vtkImageData * >( NULL ) );
    this->BoxWidget->SetInput ( (vtkImageData*)0 );
    this->PlaneWidget->SetInput ( (vtkImageData*)0 );
    
    return;
  }
  
  bool multiLayers = false;

  if (layer>0 && layer<4)
  {
    if (!this->GetInput())
    {
      vtkErrorMacro (<< "Set input prior to adding layers");
      return;
    }
   
    // reslice input image if needed
    vtkImageData *reslicedImage = this->ResliceImageToInput(image, matrix); //vtkImageData::New();
    if (!reslicedImage)
    {
      vtkErrorMacro (<< "Could not reslice image to input");
      return;
    }

    // cast it if needed    
    if (reslicedImage->GetScalarType()!=this->GetInput()->GetScalarType())
    {
      vtkImageCast *cast = vtkImageCast::New();
      cast->SetInput  (reslicedImage);
      cast->SetOutputScalarType (this->GetInput()->GetScalarType());
      cast->Update();

      reslicedImage->ShallowCopy (cast->GetOutput());
      
      cast->Delete();
    }
    
    this->AddLayer (layer);
    
    this->ImageDisplayMap.at(layer)->SetInput (reslicedImage);

    // set default display properties
    this->VolumeProperty->SetShade(layer, 1);
    this->VolumeProperty->SetComponentWeight(layer, 1.0);

    vtkColorTransferFunction *rgb   = this->GetDefaultColorTransferFunction();
    vtkPiecewiseFunction     *alpha = this->GetDefaultOpacityTransferFunction();
    
    this->SetTransferFunctions (rgb, alpha, layer);

    multiLayers = true;
    
    rgb->Delete();
    alpha->Delete();
    reslicedImage->Delete();
  }
  else if (layer >=4)
  {
    vtkErrorMacro( <<"Only 4 layers are supported in 3D fusion" );
    return;
  }

  this->InternalUpdate();
}

//----------------------------------------------------------------------------
void vtkImageView3D::InternalUpdate (void)
{
  vtkSmartPointer<vtkImageData> input = this->GetInput();

  bool multiLayers = false;

  if (this->ImageDisplayMap.size()>0)
  {
    multiLayers = true;
    
    // append all scalar buffer into the same image
    vtkImageAppendComponents *appender = vtkImageAppendComponents::New();
    appender->SetInput (0, this->GetInput());
    
    std::map<int, vtkImage3DDisplay*>::const_iterator it = this->ImageDisplayMap.begin();
    while (it!=this->ImageDisplayMap.end())
    {
      appender->AddInput (it->second->GetInput());
      ++it;
    }
    
    appender->Update();

    input = appender->GetOutput();
    
    appender->Delete();
  }

  // hack: modify the input MTime such that it is higher
  // than the VolumeMapper's one to force it to refresh
  // (see vtkSmartVolumeMapper::ConnectMapperInput(vtkVolumeMapper *m))
  if (this->VolumeMapper->GetInput())
  {
    unsigned long mtime = this->VolumeMapper->GetInput()->GetMTime();
    
    while (input->GetMTime()<=mtime)
      input->Modified();
  }
  
  this->VolumeMapper->SetInput (input);
  this->VolumeMapper->Modified();
  
  // If an image is already of type unsigned char, there is no need to
  // map it through a lookup table
  if ( !multiLayers &&  (input->GetScalarType() == VTK_UNSIGNED_CHAR &&
			(input->GetNumberOfScalarComponents() == 3 ||
			 input->GetNumberOfScalarComponents() == 4 )) )
  {
    this->VolumeProperty->IndependentComponentsOff();
    
    this->ActorX->SetInput ( input );
    this->ActorY->SetInput ( input );
    this->ActorZ->SetInput ( input );
  }
  else
  {
    this->VolumeProperty->IndependentComponentsOn();

    this->ActorX->SetInput ( this->WindowLevel->GetOutput() );
    this->ActorY->SetInput ( this->WindowLevel->GetOutput() );
    this->ActorZ->SetInput ( this->WindowLevel->GetOutput() );
  }

  // Read bounds and use these to place widget, rather than force whole dataset to be read.
  double bounds [6];
  this->GetInputBounds (bounds);
  
  this->BoxWidget->SetInput (input);
  this->BoxWidget->PlaceWidget (bounds);
  this->Callback->Execute (this->BoxWidget, 0, bounds);
  
  this->PlaneWidget->SetInput (input);
  this->PlaneWidget->PlaceWidget(bounds);
  
  this->UpdateDisplayExtent();
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetOrientationMatrix (vtkMatrix4x4* matrix)
{
  this->Superclass::SetOrientationMatrix (matrix);
  this->VolumeActor->SetUserMatrix (this->OrientationMatrix);
  this->BoxWidget->SetOrientationMatrix (this->OrientationMatrix);
  this->ActorX->SetUserMatrix (this->OrientationMatrix);
  this->ActorY->SetUserMatrix (this->OrientationMatrix);
  this->ActorZ->SetUserMatrix (this->OrientationMatrix);
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetLookupTable (vtkLookupTable* lookuptable)
{
  this->Superclass::SetLookupTable (lookuptable);  
  this->UpdateVolumeFunctions();
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetTransferFunctions (vtkColorTransferFunction * color,
					   vtkPiecewiseFunction * opacity, int layer)
{
  if(layer==0)
  {
    this->Superclass::SetTransferFunctions( color, opacity );
    // superclass generates a default function if one of the arguments is NULL
    this->VolumeProperty->SetColor(0, this->ColorTransferFunction );
    this->VolumeProperty->SetScalarOpacity(0, this->OpacityTransferFunction );
  }
  else if (this->HasLayer (layer))
  {
    double *range = this->ImageDisplayMap.at(layer)->GetInput()->GetScalarRange();
    this->SetTransferFunctionRangeFromWindowSettings(color, opacity, range[0], range[1]);
    this->VolumeProperty->SetColor(layer, color );
    this->VolumeProperty->SetScalarOpacity(layer, opacity );
  }
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetOpacity (double opacity, int layer)
{
  if (layer==0) 
  {
    this->Opacity = opacity;
    this->VolumeProperty->SetComponentWeight(0, opacity);
  }
  else if (layer <= (int)this->ImageDisplayMap.size()) 
  {
    this->VolumeProperty->SetComponentWeight(layer, opacity);
    this->ImageDisplayMap[layer]->SetOpacity(opacity);
  }
}

//----------------------------------------------------------------------------
double vtkImageView3D::GetOpacity(int layer) const
{
  if (layer==0)
    return this->Opacity;
  else if (layer <= (int)this->ImageDisplayMap.size())
    return this->ImageDisplayMap.at(layer)->GetOpacity();
  
  return 0.0;
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVisibility (int visibility, int layer)
{
  if (layer==0) 
  {
    if (visibility)
    {
      this->VolumeProperty->SetComponentWeight(0, this->GetOpacity(0));
    }
    else 
    {
      this->VolumeProperty->SetComponentWeight(layer, 0);
    }
    this->Visibility = visibility;
    
  }
  else if (layer <= (int)this->ImageDisplayMap.size()) 
  {
    if (visibility)
    {
      this->VolumeProperty->SetComponentWeight(layer, this->GetOpacity(layer));
    }
    else 
    {
      this->VolumeProperty->SetComponentWeight(layer, 0);
    }
    this->ImageDisplayMap[layer]->SetVisibility(visibility);
  }
}

//----------------------------------------------------------------------------
int vtkImageView3D::GetVisibility (int layer) const
{
  if (layer==0)
    return this->Visibility;
  
  else if (layer <= (int)this->ImageDisplayMap.size())
    return this->ImageDisplayMap.at(layer)->GetVisibility();
  
  return 0;
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetColorWindow (double s)
{
  this->Superclass::SetColorWindow (s);
  this->UpdateVolumeFunctions();
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetColorLevel (double s)
{
  this->Superclass::SetColorLevel (s);
  this->UpdateVolumeFunctions();
}

//----------------------------------------------------------------------------
void vtkImageView3D::UpdateVolumeFunctions()
{
  if ( !this->UseLookupTable || this->GetLookupTable() == NULL )
    return;
  
  vtkColorTransferFunction * color   =
  this->VolumeProperty->GetRGBTransferFunction(0);
  vtkPiecewiseFunction     * opacity =
  this->VolumeProperty->GetScalarOpacity(0);
  
  double colorValue[6]   = { 0.0, 0.0, 0.0, 0.0, 0.5, 0.0 };
  double opacityValue[4] = { 0.0, 0.0,           0.5, 0.0 };
  
  const double * range = this->LookupTable->GetRange();
  double width = range[1] - range[0];
  
  int numColors = this->GetLookupTable()->GetNumberOfTableValues();
  double factor = 1.0 / static_cast< double >( numColors - 1 );
  if ( color->GetSize() == numColors && opacity->GetSize() == numColors )
  {
    for( int i = 0; i < numColors; ++i )
    {
      double x = range[0] + factor * static_cast< double >( i ) * width;
      
      double * val = this->GetLookupTable()->GetTableValue( i );
      colorValue[0] = x;
      colorValue[1] = val[0];
      colorValue[2] = val[1];
      colorValue[3] = val[2];
      color->SetNodeValue( i, colorValue );
      
      opacityValue[0] = x;
      opacityValue[1] = val[3];
      opacity->SetNodeValue( i, opacityValue);
    }
  }
  else
  {
    color->RemoveAllPoints();
    opacity->RemoveAllPoints();
    
    // this->OpacityFunction->AddPoint (0.0,  0.0);
    for ( int i = 0; i < numColors; ++i )
    {
      double x = range[0] + factor * static_cast< double >( i ) * width;
      
      double * val = this->GetLookupTable()->GetTableValue( i );
      color->AddRGBPoint( x, val[0], val[1], val[2]);
      opacity->AddPoint( x, val[3] );
    }
  }
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetShowActorX(unsigned int arg)
{
  this->ShowActorX = arg;
  this->ActorX->SetVisibility (arg);
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetShowActorY(unsigned int arg)
{
  this->ShowActorY = arg;
  this->ActorY->SetVisibility (arg);
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetShowActorZ(unsigned int arg)
{
  this->ShowActorZ = arg;
  this->ActorZ->SetVisibility (arg);
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetRenderingMode(int arg)
{
  this->RenderingMode = arg;
  this->VolumeActor->SetVisibility (arg == vtkImageView3D::VOLUME_RENDERING);
  this->ActorX->SetVisibility ((arg == vtkImageView3D::PLANAR_RENDERING) && this->ShowActorX);
  this->ActorY->SetVisibility ((arg == vtkImageView3D::PLANAR_RENDERING) && this->ShowActorY);
  this->ActorZ->SetVisibility ((arg == vtkImageView3D::PLANAR_RENDERING) && this->ShowActorZ);
}

// //---------------------------------------------------------------------------
// void vtkImageView3D::SetUseVRQuality (bool on)
// {
//   this->VolumeMapper->SetUseVRQuality( on );
// }

// //---------------------------------------------------------------------------
// bool vtkImageView3D::GetUseVRQuality ()
// {
//   return this->VolumeMapper->GetUseVRQuality();
// }

// //---------------------------------------------------------------------------
// void vtkImageView3D::SetVRQuality (float vrq)
// {
//   this->VolumeMapper->SetVRQuality( vrq );
// }

// //---------------------------------------------------------------------------
// float vtkImageView3D::GetVRQuality ()
// {
//   return this->VolumeMapper->GetVRQuality();
// }

//---------------------------------------------------------------------------
void vtkImageView3D::SetOrientationMarker(vtkActor *actor)
{
  this->Marker->SetOrientationMarker( actor );
}

//---------------------------------------------------------------------------
void vtkImageView3D::SetOrientationMarkerViewport(double x1, double x2, double x3, double x4)
{
  this->Marker->SetViewport(x1, x2, x3, x4);
}

//---------------------------------------------------------------------------
void vtkImageView3D::SetCroppingModeToOff( void )
{
  this->SetCroppingMode( CROPPING_OFF );
}

//---------------------------------------------------------------------------
void vtkImageView3D::SetCroppingModeToInside( void )
{
  this->SetCroppingMode( CROPPING_INSIDE );
}

//---------------------------------------------------------------------------
void vtkImageView3D::SetCroppingModeToOutside( void )
{
  this->SetCroppingMode( CROPPING_OUTSIDE );
}

//---------------------------------------------------------------------------
void vtkImageView3D::SetCroppingMode( unsigned int mode )
{
  this->CroppingMode = mode;
  
  switch ( mode )
  {
    case CROPPING_OFF:
      this->VolumeMapper->CroppingOff();
      
      break;
    case CROPPING_OUTSIDE:
      this->VolumeMapper->CroppingOn();
      this->VolumeMapper->SetCroppingRegionFlagsToSubVolume();
      
      break;
    case CROPPING_INSIDE:         // fall through to default
    default:                      // default is CROPPING_INSIDE
      this->VolumeMapper->CroppingOn();
      this->VolumeMapper->SetCroppingRegionFlags( 0x7ffdfff );
  }
}

//---------------------------------------------------------------------------
unsigned int vtkImageView3D::GetCroppingMode()
{
  return this->CroppingMode;
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetCurrentPoint (double pos[3])
{
  this->Superclass::SetCurrentPoint (pos);
  //if (this->GetRenderingMode() == vtkImageView3D::PLANAR_RENDERING) // No! Because
  // if suddenly you switch from VR to MPR, you are screw
  this->UpdateDisplayExtent();
}

//----------------------------------------------------------------------------
void vtkImageView3D::UpdateDisplayExtent (void)
{
  vtkImageData *input = this->GetInput();
  if (!input)
  {
    return;
  }
  
  input->UpdateInformation();
  int *w_ext = input->GetWholeExtent();
    
  int slices[3];
  this->GetImageCoordinatesFromWorldCoordinates (this->CurrentPoint, slices);
  
  // Is the slice in range ? If not, fix it
  for (unsigned int i=0; i<3; i++)
  {
    slices[i] = std::max (w_ext[2*i], slices[i]);
    slices[i] = std::min (w_ext[2*i + 1], slices[i]);
  }	 
  
  // Set the image actors
  this->ActorX->SetDisplayExtent(
                                 slices[0], slices[0],
                                 w_ext[2],  w_ext[3],
                                 w_ext[4],  w_ext[5] );
  
  this->ActorY->SetDisplayExtent(
                                 w_ext[0],  w_ext[1],
                                 slices[1], slices[1],
                                 w_ext[4],  w_ext[5] );
  
  this->ActorZ->SetDisplayExtent(
                                 w_ext[0],  w_ext[1],
                                 w_ext[2],  w_ext[3],
                                 slices[2], slices[2]);
  
  // this->Modified(); // not in UpdateXXX() methods
}

//----------------------------------------------------------------------------
vtkActor* vtkImageView3D::AddDataSet (vtkPointSet* arg, vtkProperty* prop)
{
  vtkDataSetSurfaceFilter* geometryextractor = vtkDataSetSurfaceFilter::New();
  vtkPolyDataNormals* normalextractor = vtkPolyDataNormals::New();
  vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
  vtkActor* actor = vtkActor::New();
  
  normalextractor->SetFeatureAngle (90);
  ///\todo try to skip the normal extraction filter in order to
  // enhance the visualization speed when the data is time sequence.
  geometryextractor->SetInput (arg);
  normalextractor->SetInput (geometryextractor->GetOutput());
  mapper->SetInput (normalextractor->GetOutput());
  actor->SetMapper (mapper);
  if (prop)
    actor->SetProperty (prop);
  
  this->Renderer->AddViewProp(actor);
  
  mapper->Delete();
  normalextractor->Delete();
  geometryextractor->Delete();
  actor->Delete();

  // If this is the first widget to be added, reset camera
  if ( ! this->GetInput() ) {

      vtkBoundingBox box;
      box.AddBounds( arg->GetBounds() );

    double center[3];
    box.GetCenter(center);
    this->SetCurrentPoint(center);
    double bounds[6];
    box.GetBounds(bounds);
    this->Renderer->ResetCamera(bounds);

  }
  
  // the actor is actually not deleted as it has
  // been referenced in the renderer, so we can
  // safely return it. well hopefully.
  return actor;
}

//----------------------------------------------------------------------------
void vtkImageView3D::AddLayer (int layer)
{
  if (this->HasLayer(layer))
  {
    return;
  }
  this->ImageDisplayMap.insert (std::pair<int, vtkImage3DDisplay*>(layer, vtkImage3DDisplay::New()));
  this->VolumeProperty->SetShade(layer, 1);
  this->VolumeProperty->SetComponentWeight(layer, 1.0);

}

//----------------------------------------------------------------------------
bool vtkImageView3D::HasLayer (int layer) const
{
  std::map<int, vtkImage3DDisplay*>::const_iterator it = this->ImageDisplayMap.begin();
  while (it!=this->ImageDisplayMap.end())
  {
    if ((*it).first==layer)
      return true;
    ++it;
  }
  return false;
}

//----------------------------------------------------------------------------
void vtkImageView3D::RemoveLayer (int layer)
{
  if (!this->HasLayer(layer))
  {
    return;
  }

  this->ImageDisplayMap.at (layer)->Delete();
  this->ImageDisplayMap.erase (layer);

  this->InternalUpdate();
}

//----------------------------------------------------------------------------
void vtkImageView3D::RemoveAllLayers (void)
{
  std::map<int, vtkImage3DDisplay*> displayMap = this->ImageDisplayMap;
  std::map<int, vtkImage3DDisplay*>::iterator it = displayMap.begin();
  while (it!=displayMap.end())
  {
    this->RemoveLayer (it->first);
    ++it;
  }
}
