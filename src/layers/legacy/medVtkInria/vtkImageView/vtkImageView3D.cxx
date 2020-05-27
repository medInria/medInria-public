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
#include <vtkDataSetSurfaceFilter.h>
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
    if (this->Interactor)
    {
        if (this->InteractorStyle)
        {
            this->Interactor->SetInteractorStyle (this->InteractorStyle);
        }

        if (this->RenderWindow)
        {
            this->Interactor->SetRenderWindow(this->RenderWindow);
            this->BoxWidget->SetInteractor ( this->Interactor );
            this->PlaneWidget->SetInteractor ( this->Interactor );

            this->Marker->SetInteractor ( this->Interactor );
            this->Marker->On();
            this->Marker->InteractiveOff ();
        }
    }
    this->IsInteractorInstalled = 1;
}

//----------------------------------------------------------------------------
void vtkImageView3D::UnInstallInteractor()
{
    this->BoxWidget->SetInteractor (nullptr);
    this->PlaneWidget->SetInteractor (nullptr);
    this->Marker->SetInteractor (nullptr);

    // Happening for instance switching from 2D->3D->2D
    if (this->Interactor)
    {
        if (this->Interactor->GetRenderWindow())
        {
            auto poRenderer = this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
            while (poRenderer)
            {
                this->RenderWindow->RemoveRenderer(poRenderer);
                poRenderer = this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
            }
            this->Interactor->SetRenderWindow (nullptr);
        }

        this->Interactor->SetInteractorStyle (nullptr);

        this->IsInteractorInstalled = 0;
    }
}

//----------------------------------------------------------------------------
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
                SetOtherLayer(pi_poVtkAlgoOutput, matrix, layer);
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

void vtkImageView3D::SetInput(vtkActor * actor, int layer, vtkMatrix4x4 * matrix, const int imageSize[3], const double imageSpacing[], const double imageOrigin[])
{
    unsigned int numberOfLayers = GetNumberOfLayers();
    if ((numberOfLayers == 0) && matrix)
    {
        this->SetOrientationMatrix(matrix);
    }

    vtkDataSet *arg = actor->GetMapper()->GetInput();
    // If this is the first widget to be added, reset camera
    if (!this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized)
    {
        this->ResetCamera(arg);
    }

    this->DataSetCollection->AddItem(arg);
    this->DataSetActorCollection->AddItem(actor);
    this->Renderer->AddViewProp(actor); //same as this->Renderer->AddActor(actor);
}

bool vtkImageView3D::data2DTreatment()
{
    bool isTheData2D = false;

    int * w_extent = this->GetMedVtkImageInfo()->extent;
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

        isTheData2D = true;
    }

    return isTheData2D;
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetOtherLayer(vtkAlgorithmOutput* pi_poVtkAlgoOutput, vtkMatrix4x4 *matrix /*= 0*/, int layer /*= 0*/)
{
    vtkAlgorithmOutput *poVtkAlgoOutputTmp = this->ResliceImageToInput(pi_poVtkAlgoOutput, matrix);
    // cast it if needed
    int inputImgScalarType = static_cast<vtkImageAlgorithm*>(poVtkAlgoOutputTmp->GetProducer())->GetOutput()->GetScalarType();
    if (inputImgScalarType != this->GetMedVtkImageInfo()->scalarType)
    {
        vtkImageCast *cast = vtkImageCast::New();
        cast->SetInputConnection(poVtkAlgoOutputTmp);
        cast->SetOutputScalarType (this->GetMedVtkImageInfo()->scalarType);
        cast->Update();

        poVtkAlgoOutputTmp = cast->GetOutputPort();
    }

    this->AddLayer(layer);
    this->GetImage3DDisplayForLayer(layer)->SetInputProducer(poVtkAlgoOutputTmp);
    auto image = static_cast<vtkImageAlgorithm*>(poVtkAlgoOutputTmp->GetProducer())->GetOutput();
    double *range = image->GetScalarRange();
    SetColorRange(range, layer);
    this->GetImage3DDisplayForLayer(layer)->SetInputData(image);
}

void vtkImageView3D::SetFirstLayer(vtkAlgorithmOutput *pi_poInputAlgoImg, vtkMatrix4x4 *matrix, int layer)
{
    vtkImage3DDisplay *imageDisplay = this->GetImage3DDisplayForLayer(0);
    if (imageDisplay)
    {
        imageDisplay->SetInputProducer(pi_poInputAlgoImg);
        this->Superclass::SetInput(pi_poInputAlgoImg, matrix, layer);
        imageDisplay->SetInputData(m_poInternalImageFromInput);
        
        //The code below is useful for Volume rendering and more precisely for LUT 
        double *range = m_poInternalImageFromInput->GetScalarRange(); 
        this->SetColorRange(range, 0);
    }
}

//----------------------------------------------------------------------------
void vtkImageView3D::InternalUpdate()
{
    bool multiLayers = false;
    bool multichannelInput = (this->m_poInternalImageFromInput->GetScalarType() == VTK_UNSIGNED_CHAR &&
                              (this->m_poInternalImageFromInput->GetNumberOfScalarComponents() == 3 ||
                               this->m_poInternalImageFromInput->GetNumberOfScalarComponents() == 4 ));

    vtkImageAppendComponents *appender = vtkImageAppendComponents::New();
    if (this->LayerInfoVec.size()>0 && !multichannelInput)
    {
        // append all scalar buffer into the same image
        for (auto &it : this->LayerInfoVec)
        {
            if (it.ImageDisplay->GetInputProducer())
            {
                appender->AddInputConnection(it.ImageDisplay->GetInputProducer()->GetOutputPort());
            }
        }
        if (this->LayerInfoVec.size()>1)
        {
            multiLayers = true;
        }
    }
    appender->Update();
    appender->GetOutput();

    this->VolumeMapper->SetInputConnection( appender->GetOutputPort());
    this->VolumeMapper->Update();
    this->VolumeMapper->Modified();

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
    auto image = appender->GetOutput();
    double * bounds = image->GetBounds();
    this->BoxWidget->SetInputConnection(appender->GetOutputPort());
    this->BoxWidget->PlaceWidget (bounds);
    this->Callback->Execute (this->BoxWidget, 0, bounds);
    this->PlaneWidget->SetInputConnection(appender->GetOutputPort());
    this->PlaneWidget->PlaceWidget(bounds);
    this->UpdateDisplayExtent();

    appender->Delete();
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
        vtkImage3DDisplay *imageDisplay = this->GetImage3DDisplayForLayer(layer);
        if (imageDisplay)
        {
            if (imageDisplay->GetMedVtkImageInfo())
            {
                double *range = imageDisplay->GetMedVtkImageInfo()->scalarRange;
                this->SetTransferFunctionRangeFromWindowSettings(color, opacity, range[0], range[1]);

                this->VolumeProperty->SetColor(layer, color);
                this->VolumeProperty->SetScalarOpacity(layer, opacity);

                if (layer == 0)
                {
                    //update planar window level only if we change layer 0
                    this->PlanarWindowLevelX->SetLookupTable(color);
                    this->PlanarWindowLevelY->SetLookupTable(color);
                    this->PlanarWindowLevelZ->SetLookupTable(color);
                }
            }
        }
    }
}

//----------------------------------------------------------------------------
void vtkImageView3D::SetOpacity(double opacity, int layer)
{
    if (this->HasLayer(layer))
    {
        this->VolumeProperty->SetComponentWeight(layer, opacity);
        vtkImage3DDisplay *imageDisplay = this->GetImage3DDisplayForLayer(layer);
        if (imageDisplay)
        {
            imageDisplay->SetOpacity(opacity);
        }
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

    vtkImage3DDisplay *imageDisplay = this->GetImage3DDisplayForLayer(layer);
    if (imageDisplay)
    {
        imageDisplay->SetVisibility(visibility);
    }
  }
}

//----------------------------------------------------------------------------
int vtkImageView3D::GetVisibility (int layer) const
{
    int iRes = 0;

    if (this->HasLayer(layer))
    {
        vtkImage3DDisplay *imageDisplay = this->GetImage3DDisplayForLayer(layer);
        if (imageDisplay)
        {
            iRes = imageDisplay->GetVisibility();
        }
    }

    return iRes;
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
vtkActor* vtkImageView3D::DataSetToActor(vtkPointSet* arg, vtkProperty* prop)
{
    vtkDataSetSurfaceFilter* geometryextractor = vtkDataSetSurfaceFilter::New();
    vtkPolyDataNormals* normalextractor = vtkPolyDataNormals::New();
    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    vtkActor* actor = vtkActor::New();

    normalextractor->SetFeatureAngle(90);
    ///\todo try to skip the normal extraction filter in order to
    // enhance the visualization speed when the data is time sequence.
    geometryextractor->SetInputData(arg);
    normalextractor->SetInputConnection(geometryextractor->GetOutputPort());
    mapper->SetInputConnection(normalextractor->GetOutputPort());
    actor->SetMapper(mapper);
    if (prop)
    {
        actor->SetProperty(prop);
    }

    mapper->Delete();
    normalextractor->Delete();
    geometryextractor->Delete();

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
        if( this->LayerInfoVec.at(0).ImageDisplay->GetMedVtkImageInfo() == nullptr)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return static_cast<int>(this->LayerInfoVec.size());
    }
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
                this->SetRenderWindow(GetRenderWindow());
            }
            this->Modified();
        }

        // Delete is handled by SmartPointers.
        this->LayerInfoVec.erase(this->LayerInfoVec.begin() + layer);

        if(this->LayerInfoVec.size()>0 && layer == 0)
        {
            auto imDisplay = this->GetImage3DDisplayForLayer(0);
            if(imDisplay && imDisplay->GetInputProducer())
            {
                this->Superclass::SetInput(imDisplay->GetInputProducer()->GetOutputPort(), VolumeActor->GetUserMatrix(), layer);
            }
            double *range = m_poInternalImageFromInput->GetScalarRange();
            this->SetColorRange(range, 0);
            this->initializeTransferFunctions(0);
        }

        InternalUpdate();

        // ////////////////////////////////////////////////////////////////////////
        // Rebuild a layer if necessary
        if (this->LayerInfoVec.size() == 0)
        {
            AddLayer(0);

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
    medVtkImageInfo *imageInfo = nullptr;

    if (imageDisplay)
    {
        imageInfo = imageDisplay->GetMedVtkImageInfo();
    }

    return imageInfo;
}

//The code below is useful for Volume rendering and more precisely for LUT 
void  vtkImageView3D::initializeTransferFunctions(int pi_iLayer)
{
    this->VolumeProperty->SetShade(pi_iLayer, 1);
    this->VolumeProperty->SetComponentWeight(pi_iLayer, 1.0);
    vtkColorTransferFunction *rgb = this->GetDefaultColorTransferFunction();
    vtkPiecewiseFunction     *alpha = this->GetDefaultOpacityTransferFunction();

    this->SetTransferFunctions(rgb, alpha, pi_iLayer);
    this->UpdateVolumeFunctions(pi_iLayer);
    rgb->Delete();
    alpha->Delete();
}

