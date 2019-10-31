/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
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
#include <vtkImageMapper3D.h>

#include <vtkAlgorithmOutput.h>
#include <vtkImageAlgorithm.h>
#include "vtkImage3DDisplay.h"


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

  this->PlanarWindowLevelX = vtkSmartPointer<vtkImageMapToColors>::New();
  this->PlanarWindowLevelY = vtkSmartPointer<vtkImageMapToColors>::New();
  this->PlanarWindowLevelZ = vtkSmartPointer<vtkImageMapToColors>::New();
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
/** Override vtkObject - return the maximum mtime of this and any objects owned by this. */
vtkMTimeType vtkImageView3D::GetMTime()
{
  vtkMTimeType mTime = Superclass::GetMTime();

  vtkObject * objectsToInclude[] = {
    this->VolumeMapper,
    this->VolumeProperty,
    this->VolumeActor,
    this->BoxWidget,
    this->Cube,
    this->Marker,
    this->PlaneWidget,
    this->PlanarWindowLevelX,
    this->PlanarWindowLevelY,
    this->PlanarWindowLevelZ,
    this->ActorX,
    this->ActorY,
    this->ActorZ};

  const int numObjects = sizeof(objectsToInclude) / sizeof(vtkObject *);

  for ( int i(0); i<numObjects; ++i ) {
    vtkObject * object = objectsToInclude[i];
    if (object) {
      const vtkMTimeType testMtime = object->GetMTime();
      if ( testMtime > mTime )
    mTime = testMtime;
    }
  }

  this->ExtraPlaneCollection->InitTraversal();
  vtkObject* item = this->ExtraPlaneCollection->GetNextProp3D();
  while(item)
  {
    const vtkMTimeType testMtime = item->GetMTime();
    if ( testMtime > mTime )
      mTime = testMtime;
    item = this->ExtraPlaneCollection->GetNextProp3D();
  }

  return mTime;
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeMapperToRayCast()
{
  this->VolumeMapper->SetRequestedRenderMode(vtkSmartVolumeMapper::RayCastRenderMode );
}

#ifdef MED_USE_OSPRAY_4_VR_BY_CPU
//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeMapperToOSPRayRenderMode()
{
  this->VolumeMapper->SetRequestedRenderMode(vtkSmartVolumeMapper::OSPRayRenderMode);
}
#endif // MED_USE_OSPRAY_4_VR_BY_CPU

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeMapperToGPU()
{
  this->VolumeMapper->SetRequestedRenderMode(vtkSmartVolumeMapper::GPURenderMode );
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeMapperToDefault()
{
  this->VolumeMapper->SetRequestedRenderMode(vtkSmartVolumeMapper::DefaultRenderMode );
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeRayCastFunctionToComposite()
{
  this->VolumeMapper->SetBlendModeToComposite();
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeRayCastFunctionToMaximumIntensityProjection()
{
  this->VolumeMapper->SetBlendModeToMaximumIntensity();
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeRayCastFunctionToMinimumIntensityProjection()
{
  this->VolumeMapper->SetBlendModeToMinimumIntensity();
}

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

/** Set the box widget visibility */
void vtkImageView3D::SetShowBoxWidget (int a)
{
    if (this->Interactor)
        this->BoxWidget->SetEnabled (a);
}

bool vtkImageView3D::GetShowBoxWidget()
{
    return this->BoxWidget->GetEnabled();
}

/** Set the plane widget on */
void vtkImageView3D::SetShowPlaneWidget (int a)
{
    if (this->Interactor)
        this->PlaneWidget->SetEnabled (a);
}

bool vtkImageView3D::GetShowPlaneWidget()
{
    return this->PlaneWidget->GetEnabled();
}

/** Set the cube widget on */
void vtkImageView3D::SetShowCube (int a)
{
    if (this->Interactor)
        this->Marker->SetEnabled (a);
}

bool vtkImageView3D::GetShowCube()
{
    return this->Marker->GetEnabled();
}

void vtkImageView3D::SetShade (int a)
{
    this->VolumeProperty->SetShade (a);
}
bool vtkImageView3D::GetShade()
{
    return this->VolumeProperty->GetShade();
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
       this->VolumeProperty->GetIndependentComponents())
  {
    this->VolumeProperty->SetScalarOpacity(0, this->GetOpacityTransferFunction(0) );
    this->VolumeProperty->SetColor(0, this->GetColorTransferFunction(0));
    this->PlanarWindowLevelX->SetLookupTable(this->GetColorTransferFunction(0));
    this->PlanarWindowLevelY->SetLookupTable(this->GetColorTransferFunction(0));
    this->PlanarWindowLevelZ->SetLookupTable(this->GetColorTransferFunction(0));
  }
  else if (!this->VolumeProperty->GetIndependentComponents())
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

  this->Cube->SetTextEdgesVisibility (1);
  this->Cube->SetCubeVisibility(1);
  this->Cube->SetFaceTextVisibility(1);

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
    //this->Renderer->RemoveViewProp (this->VolumeActor);
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
    this->BoxWidget->SetInteractor (nullptr);
    this->PlaneWidget->SetInteractor (nullptr);
    this->Marker->SetInteractor (nullptr);

    if (this->Interactor)
    {
        auto poRenderer = this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
        while (poRenderer)
        {
            this->RenderWindow->RemoveRenderer(poRenderer);
            poRenderer = this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
        }
        this->Interactor->SetRenderWindow (nullptr);
        this->Interactor->SetInteractorStyle (nullptr);
    }
    this->IsInteractorInstalled = 0;
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetInput(vtkAlgorithmOutput* pi_poVtkAlgoOutput, vtkMatrix4x4 *matrix /*= 0*/, int layer /*= 0*/)
{
    vtkImageData *vtkImgTmp = ((vtkImageAlgorithm*)pi_poVtkAlgoOutput->GetProducer())->GetOutput();
  if(layer==0)
  {
    if (this->GetMedVtkImageInfo() && this->GetMedVtkImageInfo()->initialized)
    {
      this->RemoveAllLayers();
    }

    if(pi_poVtkAlgoOutput)
    {
        this->Superclass::SetInput (pi_poVtkAlgoOutput, matrix, layer);
        this->GetImage3DDisplayForLayer(0)->SetInputConnection(pi_poVtkAlgoOutput);

        double *range = vtkImgTmp->GetScalarRange();
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

  if( !pi_poVtkAlgoOutput)
  {
    return;
  }

  // Get whole extent : More reliable than dimensions if not up-to-date.
  int * w_extent = vtkImgTmp->GetExtent();

  int size [3] = { w_extent [1] - w_extent[0], w_extent [3] - w_extent[2], w_extent [5] - w_extent[4] };

  if ( (size[0] < 2) ||(size[1] < 2) || (size[2] < 2) )
  {
    vtkWarningMacro ( <<"Cannot do volume rendering for a single slice, skipping"<<endl);
    this->ActorX->GetMapper()->SetInputConnection (nullptr);
    this->ActorY->GetMapper()->SetInputConnection (nullptr);
    this->ActorZ->GetMapper()->SetInputConnection (nullptr);

    this->VolumeMapper->SetInputConnection(nullptr);
    this->BoxWidget->SetInputConnection (nullptr);
    this->PlaneWidget->SetInputConnection(nullptr);

    return;
  }

  // bool multiLayers = false;

  if (layer>0 && layer<4)
  {
    if (!this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized)
    {
      vtkErrorMacro (<< "Set input prior to adding layers");
      return;
    }

    // reslice input image if needed
    vtkAlgorithmOutput *poReslicerOutput = this->ResliceImageToInput(pi_poVtkAlgoOutput, matrix);
    if (!poReslicerOutput)
    {
      vtkErrorMacro (<< "Could not reslice image to input");
      return;
    }

    vtkAlgorithmOutput *poVtkAlgoOutputTmp = poReslicerOutput;
    // cast it if needed
    if (static_cast<vtkImageAlgorithm*>(poReslicerOutput->GetProducer())->GetOutput()->GetScalarType()!=this->GetMedVtkImageInfo()->scalarType)
    {
      vtkImageCast *cast = vtkImageCast::New();
      cast->SetInputConnection(poReslicerOutput);
      cast->SetOutputScalarType (this->GetMedVtkImageInfo()->scalarType);
      cast->Update();

      poVtkAlgoOutputTmp = cast->GetOutputPort();
    }

    this->AddLayer (layer);

    this->GetImage3DDisplayForLayer(layer)->SetInputConnection (poVtkAlgoOutputTmp);

    // set default display properties
    this->VolumeProperty->SetShade(layer, 1);
    this->VolumeProperty->SetComponentWeight(layer, 1.0);

    vtkColorTransferFunction *rgb   = this->GetDefaultColorTransferFunction();
    vtkPiecewiseFunction     *alpha = this->GetDefaultOpacityTransferFunction();

    this->SetTransferFunctions (rgb, alpha, layer);

    rgb->Delete();
    alpha->Delete();
  }
  else if (layer >=4)
  {
    vtkErrorMacro( <<"Only 4 layers are supported in 3D fusion" );
    return;
  }

  this->InternalUpdate();
}

//----------------------------------------------------------------------------
void vtkImageView3D::InternalUpdate()
{
    bool multiLayers = false;
    bool multichannelInput = (this->m_poInternalImageFromInput->GetScalarType() == VTK_UNSIGNED_CHAR &&
                              (this->m_poInternalImageFromInput->GetNumberOfScalarComponents() == 3 ||
                               this->m_poInternalImageFromInput->GetNumberOfScalarComponents() == 4 ));
    if(this->GetMedVtkImageInfo() == nullptr || !this->GetMedVtkImageInfo()->initialized)
    {
        this->Renderer->RemoveAllViewProps();
        //TODO apparently RemoveAllViewProps() is not enough, though it should be
        this->ActorX->GetMapper()->SetInputConnection(nullptr);
        this->ActorY->GetMapper()->SetInputConnection(nullptr);
        this->ActorZ->GetMapper()->SetInputConnection(nullptr);
        this->Render();
        return;
    }
    vtkImageAppendComponents *appender = vtkImageAppendComponents::New();
    if (this->LayerInfoVec.size()>0 && !multichannelInput)
    {
        // append all scalar buffer into the same image
        //vtkImageAppendComponents *appender = vtkImageAppendComponents::New();
        for( LayerInfoVecType::const_iterator it = this->LayerInfoVec.begin();
             it!=this->LayerInfoVec.end(); ++it)
        {
            if (!it->ImageDisplay->GetVtkImageInfo() || !it->ImageDisplay->GetVtkImageInfo()->initialized)
                continue;
            appender->AddInputConnection(it->ImageDisplay->GetOutputPort());
        }
        if (this->LayerInfoVec.size()>1)
        {
            multiLayers = true;
        }
    }
    appender->Update();
    appender->GetOutput();
    // hack: modify the input MTime such that it is higher
    // than the VolumeMapper's one to force it to refresh
    // (see vtkSmartVolumeMapper::ConnectMapperInput(vtkVolumeMapper *m))
    /*
    if (this->VolumeMapper->GetInput())
    {
        unsigned long mtime = this->VolumeMapper->GetInput()->GetMTime();
        while (input->GetMTime()<=mtime)
            input->Modified();
    }
    */
    this->VolumeMapper->SetInputConnection( appender->GetOutputPort());
    this->VolumeMapper->Update();
    this->VolumeMapper->Modified();
    //appender->Delete();
    // If an image is already of type unsigned char, there is no need to
    // map it through a lookup table
    if ( !multiLayers && multichannelInput )
    {
        this->VolumeProperty->IndependentComponentsOff();
        //shading and more than one dependent component (rgb) don't work well...
        //as vtk stands now in debug mode an assert makes this crash.
        this->VolumeProperty->ShadeOff();
        this->ActorX->GetMapper()->SetInputConnection(appender->GetOutputPort());
        this->ActorY->GetMapper()->SetInputConnection(appender->GetOutputPort());
        this->ActorZ->GetMapper()->SetInputConnection(appender->GetOutputPort());
    }
    else
    {
        this->VolumeProperty->IndependentComponentsOn();
        this->VolumeProperty->ShadeOn();
        this->PlanarWindowLevelX->SetInputConnection(this->m_poInputVtkAlgoOutput);
        //this->PlanarWindowLevelX->SetInputData(this->m_poInternalImageFromInput);
        this->PlanarWindowLevelX->SetOutputFormatToRGB();
        this->PlanarWindowLevelX->UpdateInformation();
        this->PlanarWindowLevelX->Update();
        this->PlanarWindowLevelY->SetInputConnection(this->m_poInputVtkAlgoOutput);
        //this->PlanarWindowLevelY->SetInputData(this->m_poInternalImageFromInput);
        this->PlanarWindowLevelY->SetOutputFormatToRGB();
        this->PlanarWindowLevelY->UpdateInformation();
        this->PlanarWindowLevelY->Update();
        this->PlanarWindowLevelZ->SetInputConnection(this->m_poInputVtkAlgoOutput);
        //this->PlanarWindowLevelZ->SetInputData(this->m_poInternalImageFromInput);
        this->PlanarWindowLevelZ->SetOutputFormatToRGB();
        this->PlanarWindowLevelZ->UpdateInformation();
        this->PlanarWindowLevelZ->Update();
        vtkScalarsToColors* lut = this->VolumeProperty->GetRGBTransferFunction(0);
        if (lut)
        {
            this->PlanarWindowLevelX->SetLookupTable(lut);
            this->PlanarWindowLevelY->SetLookupTable(lut);
            this->PlanarWindowLevelZ->SetLookupTable(lut);
            this->ActorX->GetMapper()->SetInputConnection(this->PlanarWindowLevelX->GetOutputPort());
            this->ActorY->GetMapper()->SetInputConnection(this->PlanarWindowLevelY->GetOutputPort());
            this->ActorZ->GetMapper()->SetInputConnection(this->PlanarWindowLevelZ->GetOutputPort());
        }
    }
    // Read bounds and use these to place widget, rather than force whole dataset to be read.
    double bounds [6];
    this->GetInputBounds (bounds);
    this->BoxWidget->SetInputConnection(appender->GetOutputPort());
    this->BoxWidget->PlaceWidget (bounds);
    this->Callback->Execute (this->BoxWidget, 0, bounds);
    this->PlaneWidget->SetInputConnection(appender->GetOutputPort());
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
/** Set a user-defined lookup table */
void vtkImageView3D::SetLookupTable (vtkLookupTable* lookuptable,int layer)
{
  this->Superclass::SetLookupTable (lookuptable,layer);
  this->UpdateVolumeFunctions(layer);
}

//----------------------------------------------------------------------------
/**
* Transfer functions define the mapping of the intensity or color
* values in the image to colors and opacity displayed on the screen.
*/
void vtkImageView3D::SetTransferFunctions (vtkColorTransferFunction * color,
                                           vtkPiecewiseFunction * opacity,
                                           int layer)
{
  if (this->HasLayer (layer))
  {
    if (!this->GetImage3DDisplayForLayer(layer)->GetVtkImageInfo())
      return;
    double *range = this->GetImage3DDisplayForLayer(layer)->GetVtkImageInfo()->scalarRange;
    this->SetTransferFunctionRangeFromWindowSettings(color, opacity, range[0], range[1]);
    this->VolumeProperty->SetColor(layer, color );

    this->VolumeProperty->SetScalarOpacity(layer, opacity );
    if (layer == 0)
    {
       //update planar window level only if we change layer 0
       this->PlanarWindowLevelX->SetLookupTable(color);
       this->PlanarWindowLevelY->SetLookupTable(color);
       this->PlanarWindowLevelZ->SetLookupTable(color);
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
  if ( !this->GetUseLookupTable(layer) || lookuptable == nullptr )
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
/** Set the rendering mode. */
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
/**
 The world is not always what we think it is ...

 Use this method to move the viewer slice such that the position
 (in world coordinates) given by the arguments is contained by
 the slice plane. If the given position is outside the bounds
 of the image, then the slice will be as close as possible.
*/
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
  if (!this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized)
  {
    return;
  }

  int *w_ext = this->GetMedVtkImageInfo()->extent;

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
  geometryextractor->SetInputData (arg);
  normalextractor->SetInputConnection (geometryextractor->GetOutputPort());
  mapper->SetInputConnection (normalextractor->GetOutputPort());
  actor->SetMapper (mapper);
  if (prop)
    actor->SetProperty (prop);

  this->Renderer->AddViewProp(actor);

  mapper->Delete();
  normalextractor->Delete();
  geometryextractor->Delete();
  actor->Delete();

  // If this is the first widget to be added, reset camera
  if ( ! this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized)
  {
      this->ResetCamera(arg);
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
  if ( static_cast<size_t>(layer) >= this->LayerInfoVec.size() )
  {
      this->LayerInfoVec.resize(layer + 1);
  }

  // needs to instantiate objects for layers being created
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
    // I don't really know why, but LayerInfoVec size is set to 1 at initialization time,
    // so we need one more check to know the real number of layer
    if( this->LayerInfoVec.size() == 1)
    {
        if( this->LayerInfoVec.at(0).ImageDisplay->GetVtkImageInfo() == nullptr)
            return 0;
        else return 1;
    }
    else return static_cast<int>(this->LayerInfoVec.size());
}

//----------------------------------------------------------------------------
void vtkImageView3D::RemoveLayer (int layer)
{
    if (this->HasLayer(layer))
    {
        // ////////////////////////////////////////////////////////////////////////
        // Remove layer
        vtkRenderer *renderer = this->Renderer;
        vtkImageView::RemoveLayer(layer);
        std::vector<vtkActor*> vActorsBackup;
        if (renderer)
        {
            renderer->RemoveAllViewProps();
            for(int i=0; i< this->DataSetActorCollection->GetNumberOfItems(); ++i)
            {
                auto poWidget = vtkActor::SafeDownCast(this->DataSetActorCollection->GetItemAsObject(i));
                poWidget->VisibilityOff();
                vActorsBackup.push_back(poWidget);
            }
            if (this->GetRenderWindow())
            {
                this->GetRenderWindow()->RemoveRenderer(renderer);
            }
            this->Modified();
        }

        // Delete is handled by SmartPointers.
        this->LayerInfoVec.erase(this->LayerInfoVec.begin() + layer);


        // ////////////////////////////////////////////////////////////////////////
        // Rebuild a layer if necessary
        if (this->LayerInfoVec.size() == 0)
        {
            AddLayer(0);
            bool bRestCam = true;
            for (auto pActor : vActorsBackup)
            {
                this->Renderer->AddViewProp(pActor);
                pActor->VisibilityOn();
                this->Modified();
            }
        }
    }    
}

//----------------------------------------------------------------------------
void vtkImageView3D::RemoveAllLayers()
{
  while (this->LayerInfoVec.size() > 1)
  {
    this->RemoveLayer (static_cast<int>(this->LayerInfoVec.size() -1));
  }
}


//! Get layer specific info
vtkImage3DDisplay * vtkImageView3D::GetImage3DDisplayForLayer( int layer ) const
{
  if ( layer > -1 && (unsigned int)layer < this->LayerInfoVec.size())
  {
      return this->LayerInfoVec.at(layer).ImageDisplay;
  }
  else return nullptr;
}

void vtkImageView3D::ApplyColorTransferFunction(vtkScalarsToColors * colors, int layer)
{
  return;
}


vtkColorTransferFunction * vtkImageView3D::GetColorTransferFunction(int layer) const
{

  if (this->HasLayer(layer))
    //warning if it does not exist, a default one is created.
    return this->VolumeProperty->GetRGBTransferFunction(layer);
  else return nullptr;
}

vtkPiecewiseFunction * vtkImageView3D::GetOpacityTransferFunction(int layer) const
{
  if (this->HasLayer(layer))
    //warning if it does not exist, a default one is created.
    return this->VolumeProperty->GetScalarOpacity(layer);
  else return nullptr;
}

void vtkImageView3D::StoreColorTransferFunction(vtkColorTransferFunction *ctf, int layer)
{
  if(this->HasLayer(layer) &&  this-VolumeProperty->GetIndependentComponents())
  {
    this->VolumeProperty->SetColor(layer,ctf);
    if (layer ==0)
    {
       //only update multiplanar windowlevel if layer 0 changes
       this->PlanarWindowLevelX->SetLookupTable(this->VolumeProperty->GetRGBTransferFunction());
       this->PlanarWindowLevelY->SetLookupTable(this->VolumeProperty->GetRGBTransferFunction());
       this->PlanarWindowLevelZ->SetLookupTable(this->VolumeProperty->GetRGBTransferFunction());
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
    return nullptr;
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

medVtkImageInfo* vtkImageView3D::GetMedVtkImageInfo(int layer /*= 0*/) const
{
  vtkImage3DDisplay * imageDisplay = this->GetImage3DDisplayForLayer(layer);
  if (!imageDisplay)
    return nullptr;
  return imageDisplay->GetVtkImageInfo();
}
