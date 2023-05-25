/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "vtkImage3DDisplay.h"
#include "vtkImageView3D.h"

#ifndef VTK_MAJOR_VERSION
#  include "vtkVersion.h"
#endif

#include <vtkActor.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataSetCollection.h>
#include <vtkGeometryFilter.h>
#include <vtkIdFilter.h>
#include <vtkImageActor.h>
#include <vtkImageAppendComponents.h>
#include <vtkImageCast.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMapToColors.h>
#include <vtkInteractorStyleTrackball.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPointSet.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProp3DCollection.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRendererCollection.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkTextProperty.h>

vtkStandardNewMacro(vtkImageView3D)

//----------------------------------------------------------------------------
vtkImageView3D::vtkImageView3D()
{
  VolumeProperty  = vtkVolumeProperty::New();
  VolumeActor     = vtkVolume::New();

  VolumeMapper = vtkSmartVolumeMapper::New();

  Callback    = vtkImageView3DCroppingBoxCallback::New();
  BoxWidget   = vtkOrientedBoxWidget::New();
  PlaneWidget = vtkPlaneWidget::New();
  Marker      = vtkOrientationMarkerWidget::New();
  Cube        = vtkAnnotatedCubeActor::New();

  PlanarWindowLevelX = vtkSmartPointer<vtkImageMapToColors>::New();
  PlanarWindowLevelY = vtkSmartPointer<vtkImageMapToColors>::New();
  PlanarWindowLevelZ = vtkSmartPointer<vtkImageMapToColors>::New();
  ActorX = vtkImageActor::New();
  ActorY = vtkImageActor::New();
  ActorZ = vtkImageActor::New();

  ExtraPlaneCollection = vtkProp3DCollection::New();
  ExtraPlaneInputCollection = vtkProp3DCollection::New();

  LayerInfoVec.resize(1);
  LayerInfoVec[0].ImageDisplay = vtkSmartPointer<vtkImage3DDisplay>::New();

  VolumeActor->SetVisibility (0);

  SetupVolumeRendering();
  SetupWidgets();

  ShowAnnotationsOn();
  TextProperty->SetColor (0,0,0);
  double white[3] = {0.9, 0.9, 0.9};
  SetBackground (white);

  RenderingMode = PLANAR_RENDERING;
  ShowActorX = ShowActorY = ShowActorZ =1;

  Visibility = Opacity = 1.0;

  CroppingMode = CROPPING_OFF;

  vtkInteractorStyleSwitch* styleswitch = vtkInteractorStyleSwitch::New();
  styleswitch->SetCurrentStyleToTrackballCamera();
  SetInteractorStyle ( styleswitch );
  styleswitch->Delete();

  Renderer= vtkRenderer::New();
}

//----------------------------------------------------------------------------
vtkImageView3D::~vtkImageView3D()
{
  // delete all vtk objects
  LayerInfoVec.clear();  // Delete handled by smartpointer

  VolumeMapper->Delete();
  VolumeProperty->Delete();
  VolumeActor->Delete();
  BoxWidget->Delete();
  Callback->Delete();
  Cube->Delete();
  Marker->Delete();
  PlaneWidget->Delete();
  ActorX->Delete();
  ActorY->Delete();
  ActorZ->Delete();
  ExtraPlaneCollection->Delete();
  ExtraPlaneInputCollection->Delete();

}

//----------------------------------------------------------------------------
/** Override vtkObject - return the maximum mtime of this and any objects owned by this. */
vtkMTimeType vtkImageView3D::GetMTime()
{
  vtkMTimeType mTime = Superclass::GetMTime();

  vtkObject * objectsToInclude[] = {
    VolumeMapper,
    VolumeProperty,
    VolumeActor,
    BoxWidget,
    Cube,
    Marker,
    PlaneWidget,
    PlanarWindowLevelX,
    PlanarWindowLevelY,
    PlanarWindowLevelZ,
    ActorX,
    ActorY,
    ActorZ};

  const int numObjects = sizeof(objectsToInclude) / sizeof(vtkObject *);

  for ( int i(0); i<numObjects; ++i )
  {
      vtkObject * object = objectsToInclude[i];
      if (object)
      {
          const vtkMTimeType testMtime = object->GetMTime();
          if ( testMtime > mTime )
          {
              mTime = testMtime;
          }
      }
  }

  ExtraPlaneCollection->InitTraversal();
  vtkObject* item = ExtraPlaneCollection->GetNextProp3D();
  while(item)
  {
    const vtkMTimeType testMtime = item->GetMTime();
    if ( testMtime > mTime )
      mTime = testMtime;
    item = ExtraPlaneCollection->GetNextProp3D();
  }

  return mTime;
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeMapperToRayCast()
{
  VolumeMapper->SetRequestedRenderMode(vtkSmartVolumeMapper::RayCastRenderMode );
}

#ifdef MED_USE_OSPRAY_4_VR_BY_CPU
//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeMapperToOSPRayRenderMode()
{
  VolumeMapper->SetRequestedRenderMode(vtkSmartVolumeMapper::OSPRayRenderMode);
}
#endif // MED_USE_OSPRAY_4_VR_BY_CPU

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeMapperToGPU()
{
  VolumeMapper->SetRequestedRenderMode(vtkSmartVolumeMapper::GPURenderMode );
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeMapperToDefault()
{
  VolumeMapper->SetRequestedRenderMode(vtkSmartVolumeMapper::DefaultRenderMode );
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeRayCastFunctionToComposite()
{
  VolumeMapper->SetBlendModeToComposite();
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeRayCastFunctionToMaximumIntensityProjection()
{
  VolumeMapper->SetBlendModeToMaximumIntensity();
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVolumeRayCastFunctionToMinimumIntensityProjection()
{
  VolumeMapper->SetBlendModeToMinimumIntensity();
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetInterpolationToNearestNeighbor()
{
  VolumeProperty->SetInterpolationTypeToNearest();
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetInterpolationToLinear()
{
  VolumeProperty->SetInterpolationTypeToLinear();
}

/** Set the box widget visibility */
void vtkImageView3D::SetShowBoxWidget (int a)
{
    if (Interactor)
        BoxWidget->SetEnabled (a);
}

bool vtkImageView3D::GetShowBoxWidget()
{
    return BoxWidget->GetEnabled();
}

/** Set the plane widget on */
void vtkImageView3D::SetShowPlaneWidget (int a)
{
    if (Interactor)
        PlaneWidget->SetEnabled (a);
}

bool vtkImageView3D::GetShowPlaneWidget()
{
    return PlaneWidget->GetEnabled();
}

/** Set the cube widget on */
void vtkImageView3D::SetShowCube (int a)
{
    if (Interactor)
        Marker->SetEnabled (a);
}

bool vtkImageView3D::GetShowCube()
{
    return Marker->GetEnabled();
}

void vtkImageView3D::SetShade (int a)
{
    VolumeProperty->SetShade (a);
}
bool vtkImageView3D::GetShade()
{
    return VolumeProperty->GetShade();
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetupVolumeRendering()
{
#ifdef __APPLE__
  VolumeMapper->SetRequestedRenderMode( vtkSmartVolumeMapper::RayCastRenderMode );
#else
  VolumeMapper->SetRequestedRenderMode( vtkSmartVolumeMapper::DefaultRenderMode );
#endif

  this->SetCroppingModeToInside();

  VolumeProperty->IndependentComponentsOn();
  VolumeProperty->SetInterpolationTypeToLinear();
  VolumeProperty->ShadeOff();
  VolumeProperty->SetDiffuse (0.9);
  VolumeProperty->SetAmbient (0.15);
  VolumeProperty->SetSpecular (0.3);
  VolumeProperty->SetSpecularPower (15.0);
  //Warning circular reasonning here
  if ( !GetUseLookupTable(0)  &&
       VolumeProperty->GetIndependentComponents())
  {
    VolumeProperty->SetScalarOpacity(0, GetOpacityTransferFunction(0) );
    auto colortransferfunction = GetColorTransferFunction(0);
    VolumeProperty->SetColor(0, colortransferfunction);
    PlanarWindowLevelX->SetLookupTable(colortransferfunction);
    PlanarWindowLevelY->SetLookupTable(colortransferfunction);
    PlanarWindowLevelZ->SetLookupTable(colortransferfunction);
  }
  else if (!VolumeProperty->GetIndependentComponents())
  {
    VolumeProperty->SetScalarOpacity(GetOpacityTransferFunction(0));
    VolumeProperty->SetColor(GetColorTransferFunction(0));
  }

  VolumeActor->SetProperty ( VolumeProperty );
  VolumeActor->SetMapper ( VolumeMapper );
  VolumeActor->PickableOff();
  VolumeActor->DragableOff();

  Callback->SetVolumeMapper ( VolumeMapper );
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetupWidgets()
{
  // Create an annotated cube actor (directions)
  Cube->SetXPlusFaceText ("L");
  Cube->SetXMinusFaceText ("R");
  Cube->SetYPlusFaceText ("P");
  Cube->SetYMinusFaceText ("A");
  Cube->SetZPlusFaceText ("S");
  Cube->SetZMinusFaceText ("I");
  Cube->SetZFaceTextRotation (90);
  Cube->SetFaceTextScale (0.65);
  Cube->GetCubeProperty()->SetColor (0.5, 1, 1);
  Cube->GetTextEdgesProperty()->SetLineWidth (1);
  Cube->GetTextEdgesProperty()->SetDiffuse (0);
  Cube->GetTextEdgesProperty()->SetAmbient (1);
  Cube->GetTextEdgesProperty()->SetColor (0.18, 0.28, 0.23);

  Cube->SetTextEdgesVisibility (1);
  Cube->SetCubeVisibility(1);
  Cube->SetFaceTextVisibility(1);

  Cube->GetXPlusFaceProperty()->SetColor (1, 0, 0);
  Cube->GetXPlusFaceProperty()->SetInterpolationToFlat();
  Cube->GetXMinusFaceProperty()->SetColor (1, 0, 0);
  Cube->GetXMinusFaceProperty()->SetInterpolationToFlat();
  Cube->GetYPlusFaceProperty()->SetColor (0, 1, 0);
  Cube->GetYPlusFaceProperty()->SetInterpolationToFlat();
  Cube->GetYMinusFaceProperty()->SetColor (0, 1, 0);
  Cube->GetYMinusFaceProperty()->SetInterpolationToFlat();
  Cube->GetZPlusFaceProperty()->SetColor (0, 0, 1);
  Cube->GetZPlusFaceProperty()->SetInterpolationToFlat();
  Cube->GetZMinusFaceProperty()->SetColor (0, 0, 1);
  Cube->GetZMinusFaceProperty()->SetInterpolationToFlat();

  Marker->SetOutlineColor (0.93, 0.57, 0.13);
  Marker->SetOrientationMarker (Cube);
  Marker->SetViewport (0.0, 0.05, 0.15, 0.15);

  BoxWidget->RotationEnabledOff();
  BoxWidget->SetPlaceFactor (0.5);
  BoxWidget->SetKeyPressActivationValue ('b');
  BoxWidget->AddObserver (vtkCommand::InteractionEvent, Callback);

  PlaneWidget->SetKeyPressActivationValue ('p');
}

//----------------------------------------------------------------------------
void vtkImageView3D::InstallPipeline()
{
  this->Superclass::InstallPipeline();

  if (Renderer)
  {
    Renderer->AddViewProp (VolumeActor);
    Renderer->AddViewProp (ActorX);
    Renderer->AddViewProp (ActorY);
    Renderer->AddViewProp (ActorZ);
  }
}

//----------------------------------------------------------------------------
void vtkImageView3D::UnInstallPipeline()
{
  if (Renderer)
  {
    Renderer->RemoveViewProp (ActorX);
    Renderer->RemoveViewProp (ActorY);
    Renderer->RemoveViewProp (ActorZ);
  }
  this->Superclass::UnInstallPipeline();
  IsInteractorInstalled = 0;
}

//----------------------------------------------------------------------------
void vtkImageView3D::InstallInteractor()
{
    if (Interactor)
    {
        if (InteractorStyle)
        {
            Interactor->SetInteractorStyle (InteractorStyle);
        }
        if (RenderWindow)
        {
            Interactor->SetRenderWindow(RenderWindow);
            BoxWidget->SetInteractor ( Interactor );
            PlaneWidget->SetInteractor ( Interactor );
            Marker->SetInteractor ( Interactor );
            Marker->On();
            Marker->InteractiveOff ();
        }
    }
    IsInteractorInstalled = 1;
}

//----------------------------------------------------------------------------
void vtkImageView3D::UnInstallInteractor()
{
    BoxWidget->SetInteractor (nullptr);
    PlaneWidget->SetInteractor (nullptr);
    Marker->SetInteractor (nullptr);
    // Happening for instance switching from 2D->3D->2D
    if (Interactor)
    {
        if (Interactor->GetRenderWindow())
        {
            auto poRenderer = Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
            while (poRenderer)
            {
                RenderWindow->RemoveRenderer(poRenderer);
                poRenderer = Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
            }
            Interactor->SetRenderWindow (nullptr);
        }
        Interactor->SetInteractorStyle (nullptr);
        IsInteractorInstalled = 0;
    }
}

void vtkImageView3D::SetInput(vtkAlgorithmOutput* pi_poVtkAlgoOutput, vtkMatrix4x4 *matrix /*= 0*/, int layer /*= 0*/)
{
    if(pi_poVtkAlgoOutput)
    {
        if(layer == 0)
        {
            SetFirstLayer(pi_poVtkAlgoOutput, matrix, layer);
            initializeTransferFunctions(layer);

            data2DTreatment();
            this->InternalUpdate();
        }
        else if (!data2DTreatment())
        {
            if (layer > 0 && layer < 4)
            {
                SetInputLayer(pi_poVtkAlgoOutput, matrix, layer);
                initializeTransferFunctions(layer);
                this->InternalUpdate();
            }
            else if (layer >= 4)
            {
                vtkErrorMacro( <<"Only 4 layers are supported in 3D fusion" );
            }
        }
    }
}

bool vtkImageView3D::data2DTreatment()
{
    bool isTheData2D = false;

    int * w_extent = GetMedVtkImageInfo()->extent;
    int size [3] = { w_extent [1] - w_extent[0], w_extent [3] - w_extent[2], w_extent [5] - w_extent[4] };

    if ( (size[0] < 2) ||(size[1] < 2) || (size[2] < 2) )
    {
        vtkWarningMacro ( <<"Cannot do volume rendering for a single slice, skipping"<<endl);
        ActorX->GetMapper()->SetInputConnection (nullptr);
        ActorY->GetMapper()->SetInputConnection (nullptr);
        ActorZ->GetMapper()->SetInputConnection (nullptr);

        VolumeMapper->SetInputConnection(nullptr);
        BoxWidget->SetInputConnection (nullptr);
        PlaneWidget->SetInputConnection(nullptr);

        isTheData2D = true;
    }
    return isTheData2D;
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetInputLayer(vtkAlgorithmOutput* pi_poVtkAlgoOutput, vtkMatrix4x4 *matrix /*= 0*/, int layer /*= 0*/)
{
    auto *poVtkAlgoOutputTmp = ResliceImageToInput(pi_poVtkAlgoOutput, matrix);
    // cast it if needed
    int inputImgScalarType = static_cast<vtkImageAlgorithm*>(poVtkAlgoOutputTmp->GetProducer())->GetOutput()->GetScalarType();
    if (inputImgScalarType != GetMedVtkImageInfo()->scalarType)
    {
        auto *cast = vtkImageCast::New();
        cast->SetInputConnection(poVtkAlgoOutputTmp);
        cast->SetOutputScalarType (GetMedVtkImageInfo()->scalarType);
        cast->Update();

        poVtkAlgoOutputTmp = cast->GetOutputPort();
    }

    AddLayer(layer);
    GetImage3DDisplayForLayer(layer)->SetInputProducer(poVtkAlgoOutputTmp);
    auto image = static_cast<vtkImageAlgorithm*>(poVtkAlgoOutputTmp->GetProducer())->GetOutput();
    double *range = image->GetScalarRange();
    SetColorRange(range, layer);
    GetImage3DDisplayForLayer(layer)->SetInputData(image);
}

void vtkImageView3D::SetFirstLayer(vtkAlgorithmOutput *pi_poInputAlgoImg, vtkMatrix4x4 *matrix, int layer)
{
    auto *imageDisplay = GetImage3DDisplayForLayer(0);
    if (imageDisplay)
    {
        imageDisplay->SetInputProducer(pi_poInputAlgoImg);
        this->Superclass::SetInput(pi_poInputAlgoImg, matrix, layer);
        imageDisplay->SetInputData(m_poInternalImageFromInput);
        
        //The code below is useful for Volume rendering and more precisely for LUT
        double *range = m_poInternalImageFromInput->GetScalarRange();
        SetColorRange(range, 0);
    }
}

//----------------------------------------------------------------------------
void vtkImageView3D::InternalUpdate()
{
    bool multiLayers = LayerInfoVec.size()>1;
    bool multichannelInput = (m_poInternalImageFromInput->GetScalarType() == VTK_UNSIGNED_CHAR &&
                              (m_poInternalImageFromInput->GetNumberOfScalarComponents() == 3 ||
                               m_poInternalImageFromInput->GetNumberOfScalarComponents() == 4 ));

    auto *appender = vtkImageAppendComponents::New();
    if (LayerInfoVec.size()>0 && !multichannelInput)
    {
        // append all scalar buffer into the same image
        for (auto &it : this->LayerInfoVec)
        {
            if (it.ImageDisplay->GetInputProducer())
            {
                appender->AddInputConnection(it.ImageDisplay->GetInputProducer()->GetOutputPort());
            }
        }
    }
    appender->Update();
    appender->GetOutput();
    VolumeMapper->SetInputConnection( appender->GetOutputPort());
    VolumeMapper->Update();
    VolumeMapper->Modified();

    // If an image is already of type unsigned char, there is no need to
    // map it through a lookup table
    if ( !multiLayers && multichannelInput )
    {
        VolumeProperty->IndependentComponentsOff();
        //shading and more than one dependent component (rgb) don't work well...
        //as vtk stands now in debug mode an assert makes this crash.
        VolumeProperty->ShadeOff();
        ActorX->GetMapper()->SetInputConnection(appender->GetOutputPort());
        ActorY->GetMapper()->SetInputConnection(appender->GetOutputPort());
        ActorZ->GetMapper()->SetInputConnection(appender->GetOutputPort());
    }
    else if(LayerInfoVec.size()>0)
    {
        VolumeProperty->IndependentComponentsOn();
        VolumeProperty->ShadeOn();
        auto imDisplay = GetImage3DDisplayForLayer(0);
        if(imDisplay && imDisplay->GetInputProducer())
        {
            PlanarWindowLevelX->SetInputConnection(imDisplay->GetInputProducer()->GetOutputPort());
            PlanarWindowLevelX->SetOutputFormatToRGB();
            PlanarWindowLevelX->UpdateInformation();
            PlanarWindowLevelX->Update();

            PlanarWindowLevelY->SetInputConnection(imDisplay->GetInputProducer()->GetOutputPort());
            PlanarWindowLevelY->SetOutputFormatToRGB();
            PlanarWindowLevelY->UpdateInformation();
            PlanarWindowLevelY->Update();

            PlanarWindowLevelZ->SetInputConnection(imDisplay->GetInputProducer()->GetOutputPort());
            PlanarWindowLevelZ->SetOutputFormatToRGB();
            PlanarWindowLevelZ->UpdateInformation();
            PlanarWindowLevelZ->Update();
        }

        auto * lut = VolumeProperty->GetRGBTransferFunction(0);
        if (lut)
        {
            PlanarWindowLevelX->SetLookupTable(lut);
            PlanarWindowLevelY->SetLookupTable(lut);
            PlanarWindowLevelZ->SetLookupTable(lut);
            ActorX->GetMapper()->SetInputConnection(PlanarWindowLevelX->GetOutputPort());
            ActorY->GetMapper()->SetInputConnection(PlanarWindowLevelY->GetOutputPort());
            ActorZ->GetMapper()->SetInputConnection(PlanarWindowLevelZ->GetOutputPort());
        }
    }
    // Read bounds and use these to place widget, rather than force whole dataset to be read.

    auto image = appender->GetOutput();
    double * bounds = image->GetBounds();

    BoxWidget->SetInputConnection(appender->GetOutputPort());
    BoxWidget->PlaceWidget (bounds);
    Callback->Execute (BoxWidget, 0, bounds);
    PlaneWidget->SetInputConnection(appender->GetOutputPort());
    PlaneWidget->PlaceWidget(bounds);

    appender->Delete();
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetOrientationMatrix (vtkMatrix4x4* matrix)
{
  this->Superclass::SetOrientationMatrix (matrix);
  VolumeActor->SetUserMatrix (OrientationMatrix);
  BoxWidget->SetOrientationMatrix (OrientationMatrix);
  ActorX->SetUserMatrix (OrientationMatrix);
  ActorY->SetUserMatrix (OrientationMatrix);
  ActorZ->SetUserMatrix (OrientationMatrix);
}

//----------------------------------------------------------------------------
/** Set a user-defined lookup table */
void vtkImageView3D::SetLookupTable (vtkLookupTable* lookuptable,int layer)
{
  this->Superclass::SetLookupTable (lookuptable,layer);
  UpdateVolumeFunctions(layer);
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
    if (HasLayer (layer))
    {
        if (GetImage3DDisplayForLayer(layer)->GetMedVtkImageInfo())
        {
            double *range = GetImage3DDisplayForLayer(layer)->GetMedVtkImageInfo()->scalarRange;
            SetTransferFunctionRangeFromWindowSettings(color, opacity, range[0], range[1]);

            VolumeProperty->SetColor(layer, color );
            VolumeProperty->SetScalarOpacity(layer, opacity );

            if (layer == 0)
            {
                PlanarWindowLevelX->SetLookupTable(color);
                PlanarWindowLevelY->SetLookupTable(color);
                PlanarWindowLevelZ->SetLookupTable(color);
            }
        }
    }
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetOpacity (double opacity, int layer)
{
  if (HasLayer(layer))
  {
    VolumeProperty->SetComponentWeight(layer, opacity);
    GetImage3DDisplayForLayer(layer)->SetOpacity(opacity);
  }
}

//----------------------------------------------------------------------------
double vtkImageView3D::GetOpacity(int layer) const
{
    double dfRes = 0.0;
    if (this->HasLayer(layer))
    {
        vtkImage3DDisplay *imageDisplay = GetImage3DDisplayForLayer(layer);
        if (imageDisplay)
        {
            dfRes = imageDisplay->GetOpacity();
        }
    }
    return dfRes;
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetVisibility (int visibility, int layer)
{
  if (layer == 0 && RenderingMode == vtkImageView3D::PLANAR_RENDERING)
  {
    ActorX->SetVisibility(visibility);
    ActorY->SetVisibility(visibility);
    ActorZ->SetVisibility(visibility);
  }
  if (HasLayer(layer))
  {
    if (visibility)
    {
      if (RenderingMode != vtkImageView3D::PLANAR_RENDERING)
      {
        //ensure the volumeActor is visible
        //(if there is one layer, or used to have only one layer)
        VolumeActor->SetVisibility(1);
      }
      VolumeProperty->SetComponentWeight(layer, this->GetOpacity(layer));
    }
    else
    {
      if (GetNumberOfLayers() == 1 )
      {
        //setting just the weight for a single component does nothing.
        VolumeActor->SetVisibility(0);
      }
      //still set the component weight to 0 even if only 1 layer
      //if we add new layers later the first one will stay hidden
      VolumeProperty->SetComponentWeight(layer, 0);
    }
    GetImage3DDisplayForLayer(layer)->SetVisibility(visibility);
  }
}

//----------------------------------------------------------------------------
int vtkImageView3D::GetVisibility (int layer) const
{
    return HasLayer(layer)? GetImage3DDisplayForLayer(layer)->GetVisibility() : 0;
}

void vtkImageView3D::SetColorWindow (double s,int layer)
{
  if (VolumeProperty->GetIndependentComponents())
  {
    this->Superclass::SetColorWindow (s,layer);
    UpdateVolumeFunctions(layer);
  }
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetColorLevel (double s,int layer)
{
  if (VolumeProperty->GetIndependentComponents())
  {
    this->Superclass::SetColorLevel (s,layer);
    UpdateVolumeFunctions(layer);
  }
}

//----------------------------------------------------------------------------
void vtkImageView3D::UpdateVolumeFunctions(int layer)
{
  auto* lookuptable = this->GetLookupTable(layer);
  if ( !GetUseLookupTable(layer) || lookuptable == nullptr )
    return;

  auto * color   = VolumeProperty->GetRGBTransferFunction(layer);
  auto * opacity = VolumeProperty->GetScalarOpacity(layer);

  const double * range = lookuptable->GetRange();
  double width = range[1] - range[0];

  int numColors = lookuptable->GetNumberOfTableValues();
  double factor = 1.0 / static_cast< double >( numColors - 1 );
  color->RemoveAllPoints();
  opacity->RemoveAllPoints();

  for ( int i = 0; i < numColors; ++i )
  {
    double x = range[0] + factor * i * width;
    double * val = lookuptable->GetTableValue( i );
    color->AddRGBPoint( x, val[0], val[1], val[2]);
    opacity->AddPoint( x, val[3] );
  }
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetShowActorX(unsigned int arg)
{
  ShowActorX = arg;
  ActorX->SetVisibility ( arg);
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetShowActorY(unsigned int arg)
{
  ShowActorY = arg;
  ActorY->SetVisibility ( arg);
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetShowActorZ(unsigned int arg)
{
  ShowActorZ = arg;
  ActorZ->SetVisibility ( arg);
}

//----------------------------------------------------------------------------
/** Set the rendering mode. */
void vtkImageView3D::SetRenderingMode(int arg)
{
  RenderingMode = arg;
  VolumeActor->SetVisibility (arg == vtkImageView3D::VOLUME_RENDERING);
  ActorX->SetVisibility ((arg == vtkImageView3D::PLANAR_RENDERING) && ShowActorX);
  ActorY->SetVisibility ((arg == vtkImageView3D::PLANAR_RENDERING) && ShowActorY);
  ActorZ->SetVisibility ((arg == vtkImageView3D::PLANAR_RENDERING) && ShowActorZ);

  ExtraPlaneCollection->InitTraversal();
  vtkProp3D* item = ExtraPlaneCollection->GetNextProp3D();
  while(item)
  {
    item->SetVisibility(arg == vtkImageView3D::PLANAR_RENDERING);
    item = ExtraPlaneCollection->GetNextProp3D();
  }
}

//---------------------------------------------------------------------------
void vtkImageView3D::SetOrientationMarker(vtkActor *actor)
{
  Marker->SetOrientationMarker( actor );
}

//---------------------------------------------------------------------------
void vtkImageView3D::SetOrientationMarkerViewport(double x1, double x2, double x3, double x4)
{
  Marker->SetViewport(x1, x2, x3, x4);
}

//---------------------------------------------------------------------------
void vtkImageView3D::SetCroppingModeToOff()
{
  SetCroppingMode( CROPPING_OFF );
}

//---------------------------------------------------------------------------
void vtkImageView3D::SetCroppingModeToInside()
{
  SetCroppingMode( CROPPING_INSIDE );
}

//---------------------------------------------------------------------------
void vtkImageView3D::SetCroppingModeToOutside()
{
  SetCroppingMode( CROPPING_OUTSIDE );
}

//---------------------------------------------------------------------------
void vtkImageView3D::SetCroppingMode( unsigned int mode )
{
  CroppingMode = mode;

  switch ( mode )
  {
    case CROPPING_OFF:
      VolumeMapper->CroppingOff();
      break;
    case CROPPING_OUTSIDE:
      VolumeMapper->CroppingOn();
      VolumeMapper->SetCroppingRegionFlagsToSubVolume();
      break;
    case CROPPING_INSIDE:         // fall through to default
    default:                      // default is CROPPING_INSIDE
      VolumeMapper->CroppingOn();
      VolumeMapper->SetCroppingRegionFlags( 0x7ffdfff );
  }
}

//---------------------------------------------------------------------------
unsigned int vtkImageView3D::GetCroppingMode()
{
  return CroppingMode;
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

  UpdateDisplayExtent();
}

//----------------------------------------------------------------------------
void vtkImageView3D::UpdateDisplayExtent()
{
  if (!GetMedVtkImageInfo() || !GetMedVtkImageInfo()->initialized)
  {
    return;
  }

  int *w_ext = GetMedVtkImageInfo()->extent;

  int slices[3];
  this->GetImageCoordinatesFromWorldCoordinates (CurrentPoint, slices);

  // Is the slice in range ? If not, fix it
  for (unsigned int i=0; i<3; i++)
  {
    slices[i] = std::max (w_ext[2*i], slices[i]);
    slices[i] = std::min (w_ext[2*i + 1], slices[i]);
  }

  // Set the image actors
  ActorX->SetDisplayExtent(slices[0], slices[0],
                            w_ext[2],  w_ext[3],
                            w_ext[4],  w_ext[5] );

  ActorY->SetDisplayExtent(w_ext[0],  w_ext[1],
                            slices[1], slices[1],
                            w_ext[4],  w_ext[5] );

  ActorZ->SetDisplayExtent(w_ext[0],  w_ext[1],
                            w_ext[2],  w_ext[3],
                            slices[2], slices[2]);

  // this->Modified(); // not in UpdateXXX() methods
}

//----------------------------------------------------------------------------
vtkActor* vtkImageView3D::AddDataSet(vtkPointSet* arg, vtkProperty* prop)
{
    vtkSmartPointer<vtkActor> actor = DataSetToActor(arg, prop);
    Renderer->AddViewProp(actor);

    // If this is the first widget to be added, reset camera
    if (!GetMedVtkImageInfo() || !GetMedVtkImageInfo()->initialized)
    {
        this->ResetCamera(arg);
    }

    this->DataSetCollection->AddItem(arg);
    this->DataSetActorCollection->AddItem(actor);

    return actor;
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkActor> vtkImageView3D::DataSetToActor(vtkPointSet* arg, vtkProperty* prop)
{
    auto idFilter = vtkSmartPointer<vtkIdFilter>::New();
    auto geometryextractor = vtkSmartPointer<vtkGeometryFilter>::New();
    auto normalextractor = vtkSmartPointer<vtkPolyDataNormals>::New();
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    auto actor = vtkSmartPointer<vtkActor>::New();

    idFilter->PointIdsOn();
    idFilter->CellIdsOn();
    idFilter->FieldDataOn();
    idFilter->SetCellIdsArrayName("vtkOriginalIds");
    idFilter->SetPointIdsArrayName("vtkOriginalIds");
    idFilter->SetInputData(arg);
    idFilter->Update();

    geometryextractor->SetInputConnection(idFilter->GetOutputPort());
    normalextractor->SetFeatureAngle(90);
    normalextractor->SetInputConnection(geometryextractor->GetOutputPort());
    mapper->SetInputConnection(normalextractor->GetOutputPort());
    actor->SetMapper(mapper);
    if (prop)
    {
        actor->SetProperty(prop);
    }

    return actor;
}

//----------------------------------------------------------------------------
void vtkImageView3D::RemoveDataSet(vtkPointSet* arg)
{
  vtkProp3D* actor = this->FindDataSetActor (arg);
  if (actor)
    Renderer->RemoveViewProp (actor);

  this->Superclass::RemoveDataSet (arg);
}

//----------------------------------------------------------------------------
void vtkImageView3D::AddLayer (int layer)
{
  if (HasLayer(layer) || (layer < 0) )
  {
    return;
  }
  if ( static_cast<size_t>(layer) >= LayerInfoVec.size() )
  {
      LayerInfoVec.resize(layer + 1);
  }

  // needs to instantiate objects for layers being created
  for ( size_t i(0); i<LayerInfoVec.size(); ++i )
  {
      if (!LayerInfoVec[i].ImageDisplay)
      {
          LayerInfoVec[i].ImageDisplay = vtkSmartPointer<vtkImage3DDisplay>::New();
      }
  }
  VolumeProperty->SetShade(layer, 1);
  VolumeProperty->SetComponentWeight(layer, 1.0);
}


//----------------------------------------------------------------------------
int vtkImageView3D::GetNumberOfLayers() const
{
    // I don't really know why, but LayerInfoVec size is set to 1 at initialization time,
    // so we need one more check to know the real number of layer
    int result = static_cast<int>(LayerInfoVec.size());
    if( LayerInfoVec.size() == 1)
    {
         result = (LayerInfoVec.at(0).ImageDisplay->GetMedVtkImageInfo())? 1 : 0;
    }
    return result;
}

//----------------------------------------------------------------------------
void vtkImageView3D::RemoveLayer (int layer)
{
    if (HasLayer(layer))
    {
        // ////////////////////////////////////////////////////////////////////////
        // Remove layer
        vtkRenderer *renderer = Renderer;
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
        LayerInfoVec.erase(LayerInfoVec.begin() + layer);

        if( LayerInfoVec.size()>0 && layer == 0)
        {
            auto imDisplay = GetImage3DDisplayForLayer(0);
            if(imDisplay && imDisplay->GetInputProducer())
            {
                this->Superclass::SetInput(imDisplay->GetInputProducer()->GetOutputPort(), VolumeActor->GetUserMatrix(), layer);
            }
            double *range = m_poInternalImageFromInput->GetScalarRange();
            SetColorRange(range, 0);
            initializeTransferFunctions(0);
        }
        if(LayerInfoVec.size()>0 && GetImage3DDisplayForLayer(0))
        {
           SetRenderWindow(GetRenderWindow());
        }

        InternalUpdate();
        // //////////////////////////////////////////////////////////////////////
        // Rebuild a layer if necessary
        if (LayerInfoVec.size() == 0)
        {
            AddLayer(0);
            for (auto pActor : vActorsBackup)
            {
                Renderer->AddViewProp(pActor);
                pActor->VisibilityOn();
                Modified();
            }
        }
    }
}

//----------------------------------------------------------------------------
void vtkImageView3D::RemoveAllLayers()
{
  while (LayerInfoVec.size() > 1)
  {
    RemoveLayer (static_cast<int>(LayerInfoVec.size() -1));
  }
}

vtkImage3DDisplay * vtkImageView3D::GetImage3DDisplayForLayer( int layer ) const
{
  if ( layer > -1 && (unsigned int)layer < this->LayerInfoVec.size())
  {
      return this->LayerInfoVec.at(layer).ImageDisplay;
  }
  else return nullptr;
}

void vtkImageView3D::ApplyColorTransferFunction(vtkScalarsToColors * /*colors*/, int /*layer*/)
{
    return;
}

vtkColorTransferFunction * vtkImageView3D::GetColorTransferFunction(int layer) const
{
  return  HasLayer(layer)? VolumeProperty->GetRGBTransferFunction(layer) : nullptr;
}

vtkPiecewiseFunction * vtkImageView3D::GetOpacityTransferFunction(int layer) const
{
   return  HasLayer(layer)? VolumeProperty->GetScalarOpacity(layer) : nullptr;
}

void vtkImageView3D::StoreColorTransferFunction(vtkColorTransferFunction *ctf, int layer)
{
  if(HasLayer(layer) &&  VolumeProperty->GetIndependentComponents())
  {
    VolumeProperty->SetColor(layer,ctf);
    if (layer ==0)
    {
       //only update multiplanar windowlevel if layer 0 changes
       PlanarWindowLevelX->SetLookupTable(VolumeProperty->GetRGBTransferFunction());
       PlanarWindowLevelY->SetLookupTable(VolumeProperty->GetRGBTransferFunction());
       PlanarWindowLevelZ->SetLookupTable(VolumeProperty->GetRGBTransferFunction());
    }
  }

}

void vtkImageView3D::StoreOpacityTransferFunction(vtkPiecewiseFunction *otf, int layer)
{
  if(HasLayer(layer))
  {
    VolumeProperty->SetScalarOpacity(layer,otf);
  }
}

vtkLookupTable * vtkImageView3D::GetLookupTable(int layer) const
{
  auto * imageDisplay = GetImage3DDisplayForLayer(layer);
  return imageDisplay? imageDisplay->GetLookupTable() : nullptr;
}

bool vtkImageView3D::GetUseLookupTable(int layer) const
{
  auto * imageDisplay = GetImage3DDisplayForLayer(layer);
  return imageDisplay? imageDisplay->GetUseLookupTable() : false;
}

void vtkImageView3D::SetUseLookupTable(bool use, int layer)
{
  auto * imageDisplay = GetImage3DDisplayForLayer(layer);
  if (!imageDisplay)
    return;
  imageDisplay->SetUseLookupTable( use );
}

double vtkImageView3D::GetColorWindow(int layer)const
{
  auto * imageDisplay = GetImage3DDisplayForLayer(layer);
  return imageDisplay? imageDisplay->GetColorWindow() : 0;
}

void vtkImageView3D::StoreColorWindow(double s, int layer)
{
  auto * imageDisplay = GetImage3DDisplayForLayer(layer);
  if (!imageDisplay)
    return;
  imageDisplay->SetColorWindow( s );
}

double vtkImageView3D::GetColorLevel(int layer)const
{
  auto * imageDisplay = GetImage3DDisplayForLayer(layer);
  if (!imageDisplay)
    return 0;
  return imageDisplay->GetColorLevel();
}

void vtkImageView3D::StoreColorLevel(double s, int layer)
{
  auto * imageDisplay = GetImage3DDisplayForLayer(layer);
  if (!imageDisplay)
      return;
  imageDisplay->SetColorLevel(s);
}

void vtkImageView3D::StoreLookupTable(vtkLookupTable *lookuptable, int layer)
{
  auto * imageDisplay = GetImage3DDisplayForLayer(layer);
  if (!imageDisplay)
    return;
  return imageDisplay->SetLookupTable(lookuptable);
}

medVtkImageInfo* vtkImageView3D::GetMedVtkImageInfo(int layer /*= 0*/) const
{
    auto * imageDisplay = GetImage3DDisplayForLayer(layer);

    return imageDisplay? imageDisplay->GetMedVtkImageInfo() : nullptr;

}

//The code below is useful for Volume rendering and more precisely for LUT
void  vtkImageView3D::initializeTransferFunctions(int pi_iLayer)
{
    VolumeProperty->SetShade(pi_iLayer, 1);
    VolumeProperty->SetComponentWeight(pi_iLayer, 1.0);
    auto *rgb = this->GetDefaultColorTransferFunction();
    auto *alpha = this->GetDefaultOpacityTransferFunction();
    SetTransferFunctions(rgb, alpha, pi_iLayer);
    UpdateVolumeFunctions(pi_iLayer);
    rgb->Delete();
    alpha->Delete();
}


