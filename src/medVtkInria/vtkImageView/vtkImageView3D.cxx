/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

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
#include <vtkDataSetCollection.h>
#include <vtkProp3DCollection.h>



class vtkImage3DDisplay : public vtkObject
{
public:
  static vtkImage3DDisplay *New();
  vtkTypeRevisionMacro (vtkImage3DDisplay, vtkObject);

  vtkSetObjectMacro(Input, vtkImageData);
  virtual vtkImageData* GetInput() { return this->Input;}

  vtkSetMacro(Opacity, double);
  vtkGetMacro(Opacity, double);

  vtkSetMacro(Visibility, int);
  vtkGetMacro(Visibility, int);

  vtkSetMacro(UseLookupTable, bool);
  vtkGetMacro(UseLookupTable, bool);

  vtkSetObjectMacro(LookupTable, vtkLookupTable);
  virtual vtkLookupTable* GetLookupTable() { return this->LookupTable; }
  vtkSetMacro(ColorWindow,double);
  vtkGetMacro(ColorWindow,double);

  vtkSetMacro(ColorLevel,double);
  vtkGetMacro(ColorLevel,double);

protected:
  vtkImage3DDisplay();
  ~vtkImage3DDisplay();

private:
  vtkSmartPointer<vtkImageData>               Input;
  double                                      Opacity;
  int                                         Visibility;
  double                                      ColorWindow;
  double                                      ColorLevel;
  bool                                        UseLookupTable;
  vtkSmartPointer<vtkLookupTable>             LookupTable;

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
  this->ColorWindow = 1e-3 * VTK_DOUBLE_MAX;
  this->ColorLevel  = 0;
  this->UseLookupTable = false;
  this->LookupTable = NULL;
}

vtkImage3DDisplay::~vtkImage3DDisplay()
{
  if (this->Input) {
    this->Input->Delete();
  }
  if (this->LookupTable) {
    this->LookupTable->Delete();
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

  this->PlanarWindowLevel = vtkSmartPointer<vtkImageMapToColors>::New();
  this->ActorX = vtkImageActor::New();
  this->ActorY = vtkImageActor::New();
  this->ActorZ = vtkImageActor::New();

  this->ExtraPlaneCollection = vtkProp3DCollection::New();
  this->ExtraPlaneInputCollection = vtkProp3DCollection::New();

  this->LayerInfoVec.resize(1);
  this->LayerInfoVec[0].ImageDisplay = vtkSmartPointer<vtkImage3DDisplay>::New();

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

  this->Renderer= vtkRenderer::New();
}

//----------------------------------------------------------------------------
vtkImageView3D::~vtkImageView3D()
{
  // delete all vtk objects
  this->LayerInfoVec.clear();  // Delete handled by smartpointer

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
  this->ExtraPlaneCollection->Delete();
  this->ExtraPlaneInputCollection->Delete();
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
    this->PlanarWindowLevel,
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

  this->ExtraPlaneCollection->InitTraversal();
  vtkObject* item = this->ExtraPlaneCollection->GetNextProp3D();
  while(item)
  {
    const MTimeType testMtime = item->GetMTime();
    if ( testMtime > mTime )
      mTime = testMtime;
    item = this->ExtraPlaneCollection->GetNextProp3D();
  }

  return mTime;
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeMapperTo3DTexture()
{
  this->VolumeMapper->SetRequestedRenderMode(
                                             vtkSmartVolumeMapper::TextureRenderMode );
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeMapperToRayCast()
{
  this->VolumeMapper->SetRequestedRenderMode(
                                             vtkSmartVolumeMapper::RayCastRenderMode );
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeMapperToRayCastAndTexture()
{
  this->VolumeMapper->SetRequestedRenderMode(
                                             vtkSmartVolumeMapper::RayCastAndTextureRenderMode );
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeMapperToGPU()
{
  this->VolumeMapper->SetRequestedRenderMode(
                                             vtkSmartVolumeMapper::GPURenderMode );
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeMapperToDefault()
{
  this->VolumeMapper->SetRequestedRenderMode(
                                             vtkSmartVolumeMapper::DefaultRenderMode );
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeRayCastFunctionToComposite()
{
  this->castLayers();
  this->VolumeMapper->SetBlendModeToComposite();
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeRayCastFunctionToMaximumIntensityProjection()
{
  this->castLayers();
  this->VolumeMapper->SetBlendModeToMaximumIntensity();
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeRayCastFunctionToMinimumIntensityProjection()
{
  this->castLayers();
  this->VolumeMapper->SetBlendModeToMinimumIntensity();
}

//----------------------------------------------------------------------------
// void vtkImageView3D::SetVolumeRayCastFunctionToAdditive()
// {
//   this->VolumeMapper->SetBlendModeToAdditive();
// }

//----------------------------------------------------------------------------
void vtkImageView3D::SetInterpolationToNearestNeighbor()
{
  this->VolumeProperty->SetInterpolationTypeToNearest();
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetInterpolationToLinear()
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
  //Warning circular reasonning here
  if ( !this->GetUseLookupTable(0)  &&
       this-VolumeProperty->GetIndependentComponents())
  {
    this->VolumeProperty->SetScalarOpacity(0, this->GetOpacityTransferFunction(0) );
    this->VolumeProperty->SetColor(0, this->GetColorTransferFunction(0) );
    this->PlanarWindowLevel->SetLookupTable(this->GetColorTransferFunction(0));
  }
  else if (!this-VolumeProperty->GetIndependentComponents())
  {
    this->VolumeProperty->SetScalarOpacity(
          this->GetOpacityTransferFunction(0));
    this->VolumeProperty->SetColor(this->GetColorTransferFunction(0));
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

    if(image)
    {
        this->Superclass::SetInput (image, matrix, layer);
        this->GetImage3DDisplayForLayer(0)->SetInput(image);
        //this->GetWindowLevel(0)->SetInput(image);
        //in order to get the right range, update the image first.
        image->Update();
        double *range = image->GetScalarRange();
        this->SetColorRange(range,0);
        this->VolumeProperty->SetShade(0, 1);
        this->VolumeProperty->SetComponentWeight(0, 1.0);
        vtkColorTransferFunction *rgb   = this->GetDefaultColorTransferFunction();
        vtkPiecewiseFunction     *alpha = this->GetDefaultOpacityTransferFunction();

        this->SetTransferFunctions (rgb, alpha, 0);
        rgb->Delete();
        alpha->Delete();
    }
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

  // bool multiLayers = false;

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

    this->AddLayer (layer);

    if (reslicedImage->GetScalarType()!=this->GetInput()->GetScalarType())
        LayerInfoVec[layer].NeedCast = true;
        
    this->GetImage3DDisplayForLayer(layer)->SetInput (reslicedImage);

    // set default display properties
    this->VolumeProperty->SetShade(layer, 1);
    this->VolumeProperty->SetComponentWeight(layer, 1.0);

    vtkColorTransferFunction *rgb   = this->GetDefaultColorTransferFunction();
    vtkPiecewiseFunction     *alpha = this->GetDefaultOpacityTransferFunction();

    this->SetTransferFunctions (rgb, alpha, layer);

    // multiLayers = true;

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

void vtkImageView3D::AddInput (vtkImageData* input, vtkMatrix4x4 *matrix)
{
    int layer = GetNumberOfLayers();
    SetInput (input, matrix, layer);
}

//----------------------------------------------------------------------------
void vtkImageView3D::InternalUpdate()
{
  vtkSmartPointer<vtkImageData> input = this->GetInput();
  bool multiLayers = false;

  bool multichannelInput = (this->Input->GetScalarType() == VTK_UNSIGNED_CHAR &&
                            (this->Input->GetNumberOfScalarComponents() == 3 ||
                             this->Input->GetNumberOfScalarComponents() == 4 ));

  if(input == NULL)
  {
      this->Renderer->RemoveAllViewProps();

      //TODO apparently RemoveAllViewProps() is not enough, though it should be
      this->ActorX->SetInput ( (vtkImageData*)0 );
      this->ActorY->SetInput ( (vtkImageData*)0 );
      this->ActorZ->SetInput ( (vtkImageData*)0 );

      this->Render();
      return;
  }

  if (this->LayerInfoVec.size()>0 &&  !multichannelInput)
  {
    // append all scalar buffer into the same image
    vtkImageAppendComponents *appender = vtkImageAppendComponents::New();
    
    if (this->RenderingMode == VOLUME_RENDERING) // if we are in VR mode, we need the cast.
      castLayers();

    int cpt=0;
    for(LayerInfoVecType::const_iterator it = this->LayerInfoVec.begin();
      it!=this->LayerInfoVec.end(); ++it)
    {
      if (!it->ImageDisplay->GetInput())
        continue;
      
      if (it->ImageDisplay->GetVisibility())
      {
        cpt++;
        appender->AddInput(it->ImageDisplay->GetInput());
      }
    }
    if (cpt==0)
      this->VolumeActor->SetVisibility(0);
    
    input = appender->GetOutput();
    appender->Delete();
    
    if (cpt>1)
      multiLayers = true;
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

  if ( !multiLayers &&  multichannelInput )
  {
    this->VolumeProperty->IndependentComponentsOff();
    //shading and more than one dependent component (rgb) don't work well...
    //as vtk stands now in debug mode an assert makes this crash.
    this->VolumeProperty->ShadeOff();
    this->ActorX->SetInput ( input );
    this->ActorY->SetInput ( input );
    this->ActorZ->SetInput ( input );
  }
  else
  {
    this->VolumeProperty->IndependentComponentsOn();
    this->VolumeProperty->ShadeOn();
    this->PlanarWindowLevel->SetInput(this->Input);
    this->PlanarWindowLevel->SetOutputFormatToRGB();


    vtkScalarsToColors* lut = this->VolumeProperty->GetRGBTransferFunction(0);
    if (lut)
    {
      this->PlanarWindowLevel->SetLookupTable(lut);
      this->ActorX->SetInput ( this->PlanarWindowLevel->GetOutput() );
      this->ActorY->SetInput ( this->PlanarWindowLevel->GetOutput() );
      this->ActorZ->SetInput ( this->PlanarWindowLevel->GetOutput() );
    }
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
void vtkImageView3D::SetLookupTable (vtkLookupTable* lookuptable,int layer)
{
  this->Superclass::SetLookupTable (lookuptable,layer);
  this->UpdateVolumeFunctions(layer);
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetTransferFunctions (vtkColorTransferFunction * color,
                                           vtkPiecewiseFunction * opacity,
                                           int layer)
{
  if (this->HasLayer (layer))
  {
    if (!this->GetImage3DDisplayForLayer(layer)->GetInput())
      return;
    double *range = this->GetImage3DDisplayForLayer(layer)->GetInput()->GetScalarRange();
    this->SetTransferFunctionRangeFromWindowSettings(color, opacity, range[0], range[1]);
    this->VolumeProperty->SetColor(layer, color );

    this->VolumeProperty->SetScalarOpacity(layer, opacity );
    if (layer == 0)
    {
      //update planar window level only if we change layer 0
      this->PlanarWindowLevel->SetLookupTable(color);
    }
  }

}

//----------------------------------------------------------------------------
void vtkImageView3D::SetOpacity (double opacity, int layer)
{
  if (this->HasLayer(layer))
  {
    this->VolumeProperty->SetComponentWeight(layer, opacity);
    this->GetImage3DDisplayForLayer(layer)->SetOpacity(opacity);
  }
}

//----------------------------------------------------------------------------
double vtkImageView3D::GetOpacity(int layer) const
{
    if (this->HasLayer(layer))
    return this->GetImage3DDisplayForLayer(layer)->GetOpacity();

  return 0.0;
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVisibility (int visibility, int layer)
{
  if (layer == 0 && this->RenderingMode == vtkImageView3D::PLANAR_RENDERING)
  {
    this->ActorX->SetVisibility(visibility);
    this->ActorY->SetVisibility(visibility);
    this->ActorZ->SetVisibility(visibility);
  }
  if (this->HasLayer(layer))
  {
    if (visibility)
    {
      if (this->RenderingMode != vtkImageView3D::PLANAR_RENDERING)
      {
        //ensure the volumeActor is visible
        //(if there is one layer, or used to have only one layer)
        this->VolumeActor->SetVisibility(1);
      }
      this->VolumeProperty->SetComponentWeight(layer, this->GetOpacity(layer));
    }
    else
    {
      if (this->GetNumberOfLayers() ==1 )
      {
        //setting just the weight for a single component does nothing.
        this->VolumeActor->SetVisibility(0);
      }
      //still set the component weight to 0 even if only 1 layer
      //if we add new layers later the first one will stay hidden
      this->VolumeProperty->SetComponentWeight(layer, 0);
    }
    this->GetImage3DDisplayForLayer(layer)->SetVisibility(visibility);
  }
  this->InternalUpdate();
}

//----------------------------------------------------------------------------
int vtkImageView3D::GetVisibility (int layer) const
{
    if (this->HasLayer(layer))
    return this->GetImage3DDisplayForLayer(layer)->GetVisibility();

  return 0;
}

void vtkImageView3D::SetColorWindow (double s,int layer)
{
  if (this->VolumeProperty->GetIndependentComponents())
  {
    this->Superclass::SetColorWindow (s,layer);
    this->UpdateVolumeFunctions(layer);
  }
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetColorLevel (double s,int layer)
{
  if (this->VolumeProperty->GetIndependentComponents())
  {
    this->Superclass::SetColorLevel (s,layer);
    this->UpdateVolumeFunctions(layer);
  }
}


//----------------------------------------------------------------------------
void vtkImageView3D::UpdateVolumeFunctions(int layer)
{
  vtkLookupTable* lookuptable = this->GetLookupTable(layer);
  if ( !this->GetUseLookupTable(layer) || lookuptable == NULL )
    return;

  vtkColorTransferFunction * color   =
  this->VolumeProperty->GetRGBTransferFunction(layer);
  vtkPiecewiseFunction     * opacity =
  this->VolumeProperty->GetScalarOpacity(layer);

  const double * range = lookuptable->GetRange();
  double width = range[1] - range[0];

  int numColors = lookuptable->GetNumberOfTableValues();
  double factor = 1.0 / static_cast< double >( numColors - 1 );
  color->RemoveAllPoints();
  opacity->RemoveAllPoints();

  // this->OpacityFunction->AddPoint (0.0,  0.0);
  for ( int i = 0; i < numColors; ++i )
  {
    double x = range[0] + factor * static_cast< double >( i ) * width;

    double * val = lookuptable->GetTableValue( i );
    color->AddRGBPoint( x, val[0], val[1], val[2]);
    opacity->AddPoint( x, val[3] );
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

  this->ExtraPlaneCollection->InitTraversal();
  vtkProp3D* item = this->ExtraPlaneCollection->GetNextProp3D();
  while(item)
  {
    item->SetVisibility(arg == vtkImageView3D::PLANAR_RENDERING);
    item = this->ExtraPlaneCollection->GetNextProp3D();
  }
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
void vtkImageView3D::SetCroppingModeToOff()
{
  this->SetCroppingMode( CROPPING_OFF );
}

//---------------------------------------------------------------------------
void vtkImageView3D::SetCroppingModeToInside()
{
  this->SetCroppingMode( CROPPING_INSIDE );
}

//---------------------------------------------------------------------------
void vtkImageView3D::SetCroppingModeToOutside()
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
  // if suddenly you switch from VR to MPR, you are screwed
  this->UpdateDisplayExtent();
}

//----------------------------------------------------------------------------
void vtkImageView3D::UpdateDisplayExtent()
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

  this->DataSetCollection->AddItem (arg);
  this->DataSetActorCollection->AddItem ( actor);

  // the actor is actually not deleted as it has
  // been referenced in the renderer, so we can
  // safely return it. well hopefully.
  return actor;
}

//----------------------------------------------------------------------------
void vtkImageView3D::RemoveDataSet(vtkPointSet* arg)
{
  vtkProp3D* actor = this->FindDataSetActor (arg);
  if (actor)
    this->Renderer->RemoveViewProp (actor);

  this->Superclass::RemoveDataSet (arg);
}

//----------------------------------------------------------------------------
void vtkImageView3D::AddLayer (int layer)
{
  if (this->HasLayer(layer) || (layer < 0) )
  {
    return;
  }
  if ( static_cast<size_t>(layer) >= this->LayerInfoVec.size() ) {
      this->LayerInfoVec.resize(layer + 1);
  }

  // needs to instanciate objects for layers being created
  for ( size_t i(0); i<this->LayerInfoVec.size(); ++i )
  {
      if (!this->LayerInfoVec[i].ImageDisplay)
      {
          this->LayerInfoVec[i].ImageDisplay = vtkSmartPointer<vtkImage3DDisplay>::New();
      }
  }
  this->VolumeProperty->SetShade(layer, 1);
  this->VolumeProperty->SetComponentWeight(layer, 1.0);
}


//----------------------------------------------------------------------------
int vtkImageView3D::GetNumberOfLayers() const
{
    // I don't really know why, but LayerInfoVec size is set to 1 at initialisation time,
    // so we need one more check to know the real number of layer
    if( this->LayerInfoVec.size() == 1)
    {
        if( this->LayerInfoVec.at(0).ImageDisplay->GetInput() == NULL)
            return 0;
        else return 1;
    }
    else return this->LayerInfoVec.size();
}

//----------------------------------------------------------------------------
void vtkImageView3D::RemoveLayer (int layer)
{
  if (!this->HasLayer(layer))
  {
    return;
  }

  this->LayerInfoVec.erase (this->LayerInfoVec.begin() + layer );

  if(this->LayerInfoVec.size() == 0)
    AddLayer(0);
    
  this->InternalUpdate();
}

//----------------------------------------------------------------------------
void vtkImageView3D::RemoveAllLayers()
{
  while (this->LayerInfoVec.size() > 1)
  {
    this->RemoveLayer (this->LayerInfoVec.size() -1);
  }
}

//----------------------------------------------------------------------------
class ImageActorCallback : public vtkCommand
{
public:
  static ImageActorCallback *New()
  { return new ImageActorCallback; }

  void Execute(vtkObject *caller,
               unsigned long event,
               void *vtkNotUsed(callData))
  {
    if (this->Actor != NULL)
    {
      vtkImageActor* imagecaller = vtkImageActor::SafeDownCast (caller);
      if (imagecaller && (event == vtkCommand::ModifiedEvent))
      {
        this->Actor->SetDisplayExtent (imagecaller->GetDisplayExtent());
        this->Actor->SetVisibility(imagecaller->GetVisibility());
        this->Actor->SetInput(imagecaller->GetInput());
        this->Actor->SetInterpolate(imagecaller->GetInterpolate());
        this->Actor->SetOpacity(imagecaller->GetOpacity());
        this->Actor->SetUserMatrix (imagecaller->GetUserMatrix());
        this->Actor->Modified();
      }
      vtkImageData* image = vtkImageData::SafeDownCast (caller);
      if (image && (event == vtkCommand::ModifiedEvent))
      {
        this->Actor->Modified();
      }
    }
  }

  void SetActor (vtkImageActor* arg)
  {
    if (this->Actor == arg)
      return;
    if (this->Actor)
      this->Actor->UnRegister (this);
    this->Actor = arg;
    if (this->Actor)
      this->Actor->Register(this);
  }

  vtkImageActor* GetActor()
  {
    return this->Actor;
  }

protected:
  ImageActorCallback()
  {
    this->Actor = NULL;
  }
  ~ImageActorCallback()
  {
    if (this->Actor)
      this->Actor->Delete ();
  }

  vtkImageActor* Actor;

};


//----------------------------------------------------------------------------
void vtkImageView3D::AddExtraPlane (vtkImageActor* input)
{

  if (!this->GetRenderer())
    return;

  ImageActorCallback* cbk = ImageActorCallback::New();
  vtkImageActor* actor = vtkImageActor::New();
  cbk->SetActor (actor);
  actor->SetInput (input->GetInput());
  actor->SetDisplayExtent (input->GetDisplayExtent());
  actor->SetUserMatrix (input->GetUserMatrix());
  actor->SetInterpolate(input->GetInterpolate());
  actor->SetOpacity(input->GetOpacity());
  actor->SetVisibility (input->GetVisibility());

  input->AddObserver (vtkCommand::ModifiedEvent, cbk);
  if (input->GetInput())
    input->GetInput()->AddObserver (vtkCommand::ModifiedEvent, cbk);

  this->GetRenderer()->AddViewProp (actor);

  this->ExtraPlaneCollection->AddItem (actor);
  this->ExtraPlaneInputCollection->AddItem (input);

  actor->Delete();
  cbk->Delete();

  /**
     IMPORTANT NOTE

     Adding a 2D actor in the 3D scene should be as simple as the next line

     instead of the code above...

     Unfortunately it does not seem to work properly. But this is something
     we should investigate in because it would be much simpler
  */
//  this->GetRenderer()->AddActor (input);

}

//----------------------------------------------------------------------------
void vtkImageView3D::RemoveExtraPlane (vtkImageActor* input)
{
  if (!this->GetRenderer())
    return;
  this->ExtraPlaneCollection->InitTraversal();
  this->ExtraPlaneInputCollection->InitTraversal();
  vtkProp3D* item = this->ExtraPlaneCollection->GetNextProp3D();
  vtkProp3D* iteminput = this->ExtraPlaneInputCollection->GetNextProp3D();
  while(item && iteminput)
  {
    if ( iteminput == input)
    {
      this->GetRenderer()->RemoveViewProp (item);
      break;
    }
    item = this->ExtraPlaneCollection->GetNextProp3D();
    iteminput = this->ExtraPlaneCollection->GetNextProp3D();
  }
}

vtkImage3DDisplay * vtkImageView3D::GetImage3DDisplayForLayer( int layer ) const
{
  if ( layer > -1 && (unsigned int)layer < this->LayerInfoVec.size())
  {
      return this->LayerInfoVec.at(layer).ImageDisplay;
  }
  else return NULL;
}

void vtkImageView3D::ApplyColorTransferFunction(vtkScalarsToColors * colors,
                                                int layer)
{
  return;
}


vtkColorTransferFunction * vtkImageView3D::GetColorTransferFunction(int layer) const
{

  if (this->HasLayer(layer))
    //warning if it does not exist, a default one is created.
    return this->VolumeProperty->GetRGBTransferFunction(layer);
  else return NULL;
}

vtkPiecewiseFunction * vtkImageView3D::GetOpacityTransferFunction(int layer) const
{
  if (this->HasLayer(layer))
    //warning if it does not exist, a default one is created.
    return this->VolumeProperty->GetScalarOpacity(layer);
  else return NULL;
}

void vtkImageView3D::StoreColorTransferFunction(vtkColorTransferFunction *ctf, int layer)
{
  if(this->HasLayer(layer) &&  this-VolumeProperty->GetIndependentComponents())
  {
    this->VolumeProperty->SetColor(layer,ctf);
    if (layer ==0)
    {
      //only update multiplanar windowlevel if layer 0 changes
      this->PlanarWindowLevel->SetLookupTable(this->VolumeProperty->GetRGBTransferFunction());
    }
  }

}

void vtkImageView3D::StoreOpacityTransferFunction(vtkPiecewiseFunction *otf, int layer)
{
  if(this->HasLayer(layer))
  {
    this->VolumeProperty->SetScalarOpacity(layer,otf);
  }
}

vtkLookupTable * vtkImageView3D::GetLookupTable(int layer) const
{
  vtkImage3DDisplay * imageDisplay = this->GetImage3DDisplayForLayer(layer);
  if (!imageDisplay)
    return NULL;
  return imageDisplay->GetLookupTable();
}

bool vtkImageView3D::GetUseLookupTable(int layer) const
{
  vtkImage3DDisplay * imageDisplay = this->GetImage3DDisplayForLayer(layer);
  if (!imageDisplay)
    return false;
  return imageDisplay->GetUseLookupTable();
}

void vtkImageView3D::SetUseLookupTable(bool use, int layer)
{
  vtkImage3DDisplay * imageDisplay = this->GetImage3DDisplayForLayer(layer);
  if (!imageDisplay)
    return;
  imageDisplay->SetUseLookupTable( use );
}

double vtkImageView3D::GetColorWindow(int layer)const
{
  vtkImage3DDisplay * imageDisplay = this->GetImage3DDisplayForLayer(layer);
  if (!imageDisplay)
    return 0;
  return imageDisplay->GetColorWindow();
}

void vtkImageView3D::StoreColorWindow(double s, int layer)
{
  vtkImage3DDisplay * imageDisplay = this->GetImage3DDisplayForLayer(layer);
  if (!imageDisplay)
    return;
  imageDisplay->SetColorWindow( s );
}

double vtkImageView3D::GetColorLevel(int layer)const
{
  vtkImage3DDisplay * imageDisplay = this->GetImage3DDisplayForLayer(layer);
  if (!imageDisplay)
    return 0;
  return imageDisplay->GetColorLevel();
}

void vtkImageView3D::StoreColorLevel(double s, int layer)
{
  vtkImage3DDisplay * imageDisplay = this->GetImage3DDisplayForLayer(layer);
  if (!imageDisplay)
      return;
  imageDisplay->SetColorLevel(s);
}

void vtkImageView3D::StoreLookupTable(vtkLookupTable *lookuptable, int layer)
{
  vtkImage3DDisplay * imageDisplay = this->GetImage3DDisplayForLayer(layer);
  if (!imageDisplay)
    return;
  return imageDisplay->SetLookupTable(lookuptable);
}

vtkImageData * vtkImageView3D::GetInput(int layer) const
{
  vtkImage3DDisplay * imageDisplay = this->GetImage3DDisplayForLayer(layer);
  if (!imageDisplay)
    return NULL;
  return imageDisplay->GetInput();
}

void vtkImageView3D::castLayers()
{
  for(int i=0;i<LayerInfoVec.size();i++)
  { 
    if (LayerInfoVec[i].NeedCast)
    {
      vtkImage3DDisplay * imageDisplay = this->GetImage3DDisplayForLayer(i);
      if (!imageDisplay)
        continue;
      vtkImageData * dataToCast = imageDisplay->GetInput();
      if (imageDisplay->GetVisibility() && dataToCast && dataToCast->GetScalarType()!=this->GetInput()->GetScalarType())
      {
        vtkImageCast *cast = vtkImageCast::New();
        vtkImageData * dataToCast = this->GetImage3DDisplayForLayer(i)->GetInput();
        cast->SetInput(dataToCast);
        cast->SetOutputScalarType (this->GetInput()->GetScalarType());
        cast->Update();
        dataToCast->ShallowCopy (cast->GetOutput());
        this->GetImage3DDisplayForLayer(i)->SetInput (dataToCast);
        cast->Delete();
        vtkColorTransferFunction *rgb   = this->GetDefaultColorTransferFunction();
        vtkPiecewiseFunction     *alpha = this->GetDefaultOpacityTransferFunction();
        this->SetTransferFunctions (rgb, alpha, i);
      }
    }
  }
}