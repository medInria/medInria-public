/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkRenderingAddOn/vtkViewImage3D.h>

#ifndef VTK_MAJOR_VERSION
#  include "vtkVersion.h"
#endif

#include <vtkSmartPointer.h>
#include <vtkInteractorStyleTrackball.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRendererCollection.h>
#include <vtkImageData.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkVolumeTextureMapper2D.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeMapper.h>
#include <vtkFiniteDifferenceGradientEstimator.h>
#include <vtkVolumeTextureMapper2D.h>
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
#include <vtkRenderingAddOn/vtkDataSet3DCroppingPlaneCallback.h>
#include "vtkColorTransferFunction.h"
#include <vtkCamera.h>
#include <vtkImageShiftScale.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkPlane.h>
#include <vtkPlaneCollection.h>
#include <vtkPolyDataMapper.h>
#include <vtkGeometryFilter.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkCellData.h>
#include <vtkMath.h>
#include <vtkOrientedBoxWidget.h>
#include <vtkPlaneWidget.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkVolumeProperty.h>
#include <vtkImageMapToColors.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkRenderingAddOn/vtkImage3DCroppingBoxCallback.h>


vtkCxxRevisionMacro(vtkViewImage3D, "$Revision: 1469 $");
vtkStandardNewMacro(vtkViewImage3D);


vtkViewImage3D::vtkViewImage3D ()
{

  // Volume Rendering stuff
  this->VolumeProperty   = vtkVolumeProperty::New(); 
  this->VolumeActor      = vtkVolume::New();
  this->OpacityFunction  = vtkPiecewiseFunction::New();
  this->ColorFunction    = vtkColorTransferFunction::New(); 
  
				
  // Multiplanar reconstruction stuff
  this->AxialColorMapper       = vtkImageMapToColors::New();
  this->SagittalColorMapper    = vtkImageMapToColors::New();
  this->CoronalColorMapper     = vtkImageMapToColors::New();
  this->ActorSagittal          = vtkImageActor::New();
  this->ActorCoronal           = vtkImageActor::New();
  this->ActorAxial             = vtkImageActor::New();
  this->Axes                   = vtkAxes::New();
  this->TubeFilter             = vtkTubeFilter::New();
  this->AxesActor              = vtkActor::New();
  this->BoxWidget              = vtkOrientedBoxWidget::New();
  this->Callback               = vtkImage3DCroppingBoxCallback::New();
  this->DataSetCallback        = vtkDataSet3DCroppingPlaneCallback::New();
  this->AxialBlender           = vtkImageBlend::New();
  this->SagittalBlender        = vtkImageBlend::New();
  this->CoronalBlender         = vtkImageBlend::New();
  this->Blender                = vtkImageBlend::New();
  this->PlaneWidget            = vtkPlaneWidget::New();

  
  this->AxialColorMapper->SetOutputFormatToRGB();
  this->SagittalColorMapper->SetOutputFormatToRGB();
  this->CoronalColorMapper->SetOutputFormatToRGB();

  
  this->RenderingMode = PLANAR_RENDERING;
  this->BoxWidgetVisibility   = 1;
  this->AxisVisibility        = 0;
  this->PlaneWidgetVisibility = 0;
  this->ShowAxial             = 1;
  this->ShowCoronal           = 1;
  this->ShowSagittal          = 1;
  this->Cropping              = 1;
  this->CubeVisibility        = 1;
  this->Shade                 = 0;
  this->VRQuality             = 2;

  
  // set up 3D axes
  this->Axes->SetOrigin (0,0,0);
  this->Axes->SymmetricOn();
  this->Axes->SetScaleFactor (50.0);
  this->TubeFilter->SetInputConnection( this->Axes->GetOutputPort() );
  this->TubeFilter->SetRadius(1.0);
  this->TubeFilter->SetNumberOfSides(6);  
  vtkPolyDataMapper* axesMapper = vtkPolyDataMapper::New();
  axesMapper->SetInputConnection ( this->TubeFilter->GetOutputPort() );
  this->Axes->Update();
  this->Axes->GetOutput()->GetPointData()->GetScalars()->SetTuple1 (0, 0.0);
  this->Axes->GetOutput()->GetPointData()->GetScalars()->SetTuple1 (1, 0.0);
  this->Axes->GetOutput()->GetPointData()->GetScalars()->SetTuple1 (2, 0.5);
  this->Axes->GetOutput()->GetPointData()->GetScalars()->SetTuple1 (3, 0.5);
  this->Axes->GetOutput()->GetPointData()->GetScalars()->SetTuple1 (4, 1.0);
  this->Axes->GetOutput()->GetPointData()->GetScalars()->SetTuple1 (5, 1.0);    
  this->AxesActor->SetMapper (axesMapper);
  axesMapper->Delete();
  

  // set up the filters (properties, etc.)
  this->AxialBlender->SetBlendModeToNormal();
  this->AxialBlender->SetOpacity (0, 0.25);
  this->AxialBlender->SetOpacity (1, 0.75);


  this->SagittalBlender->SetBlendModeToNormal();
  this->SagittalBlender->SetOpacity (0, 0.25);
  this->SagittalBlender->SetOpacity (1, 0.75);


  this->CoronalBlender->SetBlendModeToNormal();
  this->CoronalBlender->SetOpacity (0, 0.25);
  this->CoronalBlender->SetOpacity (1, 0.75);


  this->Blender->SetBlendModeToNormal();
  this->Blender->SetOpacity (0, 0.25);
  this->Blender->SetOpacity (1, 0.75);

  this->VolumeTextureMapper3D = vtkVolumeTextureMapper3D::New();
  this->VolumeTextureMapper3D->SetSampleDistance(1.0);
  this->VolumeTextureMapper3D->SetPreferredMethodToNVidia();
  this->VolumeTextureMapper3D->CroppingOn();
  this->VolumeTextureMapper3D->SetCroppingRegionFlags (0x7ffdfff);

  this->VolumeGPUMapper3D = vtkGPUVolumeRayCastMapper::New();
  this->VolumeGPUMapper3D->CroppingOn();
  this->VolumeGPUMapper3D->SetCroppingRegionFlags(0x7ffdfff);
  this->VolumeGPUMapper3D->AutoAdjustSampleDistancesOn();

  this->VolumeTextureMapper2D = vtkVolumeTextureMapper2D::New();
  this->VolumeTextureMapper2D->CroppingOn();
  this->VolumeTextureMapper2D->SetCroppingRegionFlags (0x7ffdfff);
    
#ifdef __APPLE__
  this->VolumeMapper3D = this->VolumeTextureMapper3D;
  this->VRMapperType = TextureMapper3D;
#else    
  this->VolumeMapper3D = this->VolumeGPUMapper3D;
  this->VRMapperType = GPUMapper3D;
#endif    

  
  this->OpacityFunction->AddPoint (0, 0.0);
  this->OpacityFunction->AddPoint (255, 1.0);
  this->ColorFunction->AddRGBPoint (0, 0.0, 0.0, 0.0);
  this->ColorFunction->AddRGBPoint (255, 1.0, 1.0, 1.0);


  this->VolumeProperty->SetColor (this->ColorFunction);
  this->ColorFunction->Delete();
  this->VolumeProperty->SetScalarOpacity(this->OpacityFunction);
  this->VolumeProperty->SetInterpolationTypeToLinear();
  this->VolumeProperty->ShadeOff();
  this->VolumeProperty->SetDiffuse (0.9);
  this->VolumeProperty->SetAmbient (0.15);
  this->VolumeProperty->SetSpecular (0.3);
  this->VolumeProperty->SetSpecularPower (15.0);
  
  
  // set up the vtk pipeline: volume rendering
  this->VolumeActor->SetProperty ( this->VolumeProperty );
  this->VolumeActor->SetMapper ( this->VolumeMapper3D );
  this->VolumeActor->PickableOff();
  
  
  // set up the boxwidget/ callback
  this->Callback->SetVolumeMapper ( this->VolumeMapper3D );
  this->DataSetCallback->SetPlaneWidget ( this->PlaneWidget );
  
  this->BoxWidget->RotationEnabledOff();
  this->BoxWidget->SetPlaceFactor (0.5);
  this->BoxWidget->SetKeyPressActivationValue ('b');
  this->BoxWidget->AddObserver (vtkCommand::InteractionEvent, this->Callback);

  
  //this->PlaneWidget->AddObserver (vtkCommand::InteractionEvent, this->DataSetCallback);
  this->PlaneWidget->SetKeyPressActivationValue ('p');
  
  
  // Create an annotated cube actor (directions)
  this->Cube = vtkAnnotatedCubeActor::New();
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

#if VTK_MAJOR_VERSION> 5 || (VTK_MAJOR_VERSION==5 && VTK_MINOR_VERSION>=1)
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

  /*
    this->CubeAxes = vtkAxesActor::New(); 
    this->CubeAxes->SetShaftTypeToCylinder();
    this->CubeAxes->SetXAxisLabelText ("x");
    this->CubeAxes->SetYAxisLabelText ("y");
    this->CubeAxes->SetZAxisLabelText ("z");
    this->CubeAxes->SetTotalLength (1.5, 1.5, 1.5);
    vtkTextProperty* tprop = vtkTextProperty::New();
    tprop->ItalicOn();
    tprop->ShadowOn();
    tprop->SetFontFamilyToTimes();
    this->CubeAxes->GetXAxisCaptionActor2D()->SetCaptionTextProperty (tprop);
    this->CubeAxes->GetYAxisCaptionActor2D()->SetCaptionTextProperty (tprop);
    this->CubeAxes->GetZAxisCaptionActor2D()->SetCaptionTextProperty (tprop);
  */

  /*
    vtkPropAssembly *assembly = vtkPropAssembly::New();
    assembly->AddPart (this->CubeAxes);
    assembly->AddPart (this->Cube);
  */
  
  this->Marker = vtkOrientationMarkerWidget::New();
  this->Marker->SetOutlineColor (0.93, 0.57, 0.13);
  //this->Marker->SetOrientationMarker (assembly);
  this->Marker->SetOrientationMarker (this->Cube);
  this->Marker->SetViewport (0.0, 0.05, 0.15, 0.15);
  
  //assembly->Delete();
  //tprop->Delete();
  

  this->VolumeActor->SetVisibility (0);
  this->AxesActor->SetVisibility (0);

    
  this->ActorSagittal->SetVisibility ( this->GetVisibility() );
  this->ActorCoronal->SetVisibility ( this->GetVisibility() );
  this->ActorAxial->SetVisibility ( this->GetVisibility() );

}


vtkViewImage3D::~vtkViewImage3D()
{
  //this->Marker->SetEnabled (0);
  //this->BoxWidget->SetEnabled (0);
  //this->PlaneWidget->SetEnabled (0);

  this->BoxWidget->RemoveObserver (this->Callback);

  // delete all vtk objetcts:
  this->VolumeActor->Delete();
  this->VolumeTextureMapper3D->Delete();
  this->VolumeGPUMapper3D->Delete();
  this->VolumeTextureMapper2D->Delete();
  this->VolumeProperty->Delete();
  this->OpacityFunction->Delete();
  //this->ColorFunction->Delete();
  this->AxialColorMapper->Delete();
  this->SagittalColorMapper->Delete();
  this->CoronalColorMapper->Delete();
  this->ActorSagittal->Delete();
  this->ActorCoronal->Delete();
  this->ActorAxial->Delete();
  this->Axes->Delete();
  this->TubeFilter->Delete();
  this->AxesActor->Delete();
  this->BoxWidget->Delete();
  this->Callback->Delete();
  this->DataSetCallback->Delete();
  this->Cube->Delete();
  //this->CubeAxes->Delete();
  this->Marker->Delete();
  this->AxialBlender->Delete();
  this->CoronalBlender->Delete();
  this->SagittalBlender->Delete();
  this->Blender->Delete();
  this->PlaneWidget->Delete();
}


void vtkViewImage3D::InitializeInteractor()
{
  if( this->GetRenderWindowInteractor() )
  {
    this->BoxWidget->SetInteractor ( this->GetRenderWindowInteractor() );
    this->PlaneWidget->SetInteractor ( this->GetRenderWindowInteractor() );
    this->Marker->SetInteractor ( this->GetRenderWindowInteractor() );

    vtkInteractorStyleSwitch* interactorStyle = vtkInteractorStyleSwitch::New();
    interactorStyle->SetCurrentStyleToTrackballCamera();
    this->GetRenderWindowInteractor()->SetInteractorStyle (interactorStyle);
    interactorStyle->Delete();

    if( !this->GetFirstImage() )
    {
      if( this->BoxWidgetVisibility )
      {
	this->BoxWidget->On();
      }
      if( this->PlaneWidgetVisibility )
      {
	this->PlaneWidget->On();
      }
      if( this->CubeVisibility )
      {
	this->Marker->SetEnabled (1);
	this->Marker->InteractiveOff();
      }
    }
    
  }
}


void vtkViewImage3D::Initialize()
{
  vtkViewImage::Initialize();
  
  this->InitializeInteractor();
}


void vtkViewImage3D::UninitializeInteractor()
{
  this->BoxWidget->SetInteractor (NULL);
  this->Marker->SetInteractor ( NULL );
  this->PlaneWidget->SetInteractor ( NULL );
  
  if (this->GetRenderWindowInteractor())
  {
    this->GetRenderWindowInteractor()->SetInteractorStyle( NULL );
  }
}


void vtkViewImage3D::Uninitialize()
{
  this->RemoveActor (this->ActorSagittal);
  this->RemoveActor (this->ActorCoronal);
  this->RemoveActor (this->ActorAxial);
  
  this->RemoveActor (this->AxesActor);
  this->RemoveActor (this->VolumeActor);
  for (unsigned int i=0; i<this->DataSetActorList.size(); i++)
  {
    this->RemoveActor( this->DataSetActorList[i] );
  }

  this->UninitializeInteractor();
  vtkViewImage::Uninitialize();
}


void vtkViewImage3D::PrepareForDelete()
{
  this->SetRenderingModeToPlanar();
  this->PlaneWidget->Off();
}


void vtkViewImage3D::SetVolumeRayCastFunctionToComposite ()
{
  this->VolumeMapper3D->SetBlendModeToComposite();
  this->Modified();
}


void vtkViewImage3D::SetVolumeRayCastFunctionToMIP ()
{
  this->VolumeMapper3D->SetBlendModeToMaximumIntensity();
  this->Modified();
}


void vtkViewImage3D::SetImage ( vtkImageData* image )
{
  if( !image )
  {
    return;
  }

  int* extent = image->GetExtent();
  if( extent[1]<extent[0] || extent[3]<extent[2] || extent[5]<extent[4] )
  {
    vtkErrorMacro ( << "Image extent is not valid: " << extent[0] << " "
		    << extent[1] << " "
		    << extent[2] << " "
		    << extent[3] << " "
		    << extent[4] << " "
		    << extent[5]);
    return;
  }
  

  if (!this->GetRenderWindow() || !this->GetRenderWindowInteractor())
  {
    return;
  }
  
  //this->RegisterImage (image);
  vtkViewImage::SetImage (image);

  this->VolumeMapper3D->SetInput(this->GetImage());

  /*
  double* spacing = this->GetImage()->GetSpacing();
  double  sampleDistance = 0.33*0.1*(spacing[0]+spacing[1]+spacing[2]);

  this->VolumeTextureMapper3D->SetSampleDistance (sampleDistance);
  this->VolumeGPUMapper3D->SetSampleDistance (sampleDistance);
  */
  
  vtkVolumeTextureMapper3D* mapper3D = vtkVolumeTextureMapper3D::SafeDownCast ( this->VolumeMapper3D );
  if( mapper3D && !this->GetRenderWindow()->GetNeverRendered() )
  {
    if( !mapper3D->IsRenderSupported ( this->VolumeProperty
#if (VTK_MAJOR_VERSION == 5 && ((VTK_MINOR_VERSION >= 6 && VTK_BUILD_VERSION >=1) || VTK_MINOR_VERSION >= 7)) || VTK_MAJOR_VERSION > 5
				       , this->GetRenderer()
#endif
	  ) )
    {
      //try the ATI fragment program implementation
      mapper3D->SetPreferredMethodToFragmentProgram();
      if( !mapper3D->IsRenderSupported ( this->VolumeProperty
#if (VTK_MAJOR_VERSION == 5 && ((VTK_MINOR_VERSION >= 6 && VTK_BUILD_VERSION >=1) || VTK_MINOR_VERSION >= 7)) || VTK_MAJOR_VERSION > 5
					 , this->GetRenderer()
#endif
	    ) )
      {
         vtkWarningMacro (<<"Warning: 3D texture volume rendering is not supported by your hardware, switching to 2D texture rendering."<<endl);
         
         double* range = this->GetImage()->GetScalarRange();
         double shift = 0 - range[0];
         double scale = 65535.0/(range[1] - range[0]);
         
         vtkImageShiftScale* scaler = vtkImageShiftScale::New();
         scaler->SetInput (image);
         scaler->SetShift (shift);
         scaler->SetScale (scale);
         scaler->SetOutputScalarTypeToUnsignedShort();
         
         scaler->Update();
         
         this->SetShift(scaler->GetShift());
         this->SetScale(scaler->GetScale());
         
         this->VolumeTextureMapper2D->SetInput ( scaler->GetOutput() );
         
         scaler->Delete();
         
         this->VolumeMapper3D = this->VolumeTextureMapper2D;
         this->VolumeActor->SetMapper ( this->VolumeMapper3D );
         this->Callback->SetVolumeMapper( this->VolumeMapper3D );
      }    
    }    
  }
  
  
  // Before all: remove the overlapping image (if any)
  this->RemoveOverlappingImage();
  
  // If an image is already of type unsigned char, there is no
  // need to map it through a lookup table
  if( this->GetImage()->GetScalarType() == VTK_UNSIGNED_CHAR  && (this->GetImage()->GetNumberOfScalarComponents()==3 || this->GetImage()->GetNumberOfScalarComponents()==4) )
  {
    this->VolumeProperty->IndependentComponentsOff();
    
    this->ActorSagittal->SetInput ( this->GetImage() );
    this->ActorCoronal->SetInput  ( this->GetImage() );
    this->ActorAxial->SetInput    ( this->GetImage() );
  }
  else
  {
    this->VolumeProperty->IndependentComponentsOn();
    
    this->AxialColorMapper->SetInput    (this->GetImage());
    this->SagittalColorMapper->SetInput (this->GetImage());
    this->CoronalColorMapper->SetInput  (this->GetImage());
    
    this->ActorSagittal->SetInput (this->SagittalColorMapper->GetOutput());
    this->ActorCoronal->SetInput (this->CoronalColorMapper->GetOutput());
    this->ActorAxial->SetInput (this->AxialColorMapper->GetOutput());
  }


  this->VolumeActor->SetProperty (this->VolumeProperty);
  

  this->BoxWidget->SetInput (this->GetImage());
  this->BoxWidget->PlaceWidget();
  this->Callback->Execute (this->BoxWidget, 0, NULL);

  
  if( this->RenderingMode==VOLUME_RENDERING && this->GetVisibility() && this->BoxWidgetVisibility)
  {
    this->BoxWidget->On();
  }
  else
  {
    this->BoxWidget->Off();
  }


  this->PlaneWidget->SetInput (this->GetImage());
  this->PlaneWidget->NormalToZAxisOn();
  this->PlaneWidget->PlaceWidget();


  
  /**
     The first time ResetWindowLevel is called, optimal window/level values are
     stored internally for efficiency purposes when Reset is called a second time.
     When a new image is set, we have to force the calculation of the new optimal
     window/level values.
   */
  this->SetFirstResetWindowLevel ( 1 );
  
  

  /**
     Restablish the direction matrix, and compute the correct translation
     if needed.
   */
  this->SetDirectionMatrix ( this->GetDirectionMatrix() );

  
  double pt[3];
  this->GetCurrentPoint (pt);
  this->SetCurrentPoint (pt);
     
  // add actors to the renderwindow
  this->AddActor (this->ActorSagittal);
  this->AddActor (this->ActorCoronal);
  this->AddActor (this->ActorAxial);
  
  this->AddActor (this->AxesActor);
  this->AddActor (this->VolumeActor);

  if( this->GetFirstImage() )
  {
    this->ResetCamera();
    this->SetFirstImage (0);
  }
}


void vtkViewImage3D::StartWindowing()
{
  this->GetRenderWindow()->SetDesiredUpdateRate( this->GetRenderWindowInteractor()->GetDesiredUpdateRate() );
  this->Modified();
}


void vtkViewImage3D::EndWindowing()
{
  this->GetRenderWindow()->SetDesiredUpdateRate( this->GetRenderWindowInteractor()->GetStillUpdateRate() );
  this->Modified();
}


void vtkViewImage3D::SetWindow (double w)
{

  if( w<0.0 )
  {
    w = 0.0;
  }
    
  // scale the value w.r.t Scale/Shift parameters
  vtkViewImage::SetWindow ( this->GetShift() + w*this->GetScale() );

  
  double v_min = this->GetLevel() - 0.5*this->GetWindow();
  double v_max = this->GetLevel() + 0.5*this->GetWindow();

  double scaleOpacity = 1.4;
  
  if( this->GetLookupTable() )
  {      
    this->GetLookupTable()->SetRange ( (v_min-0.5*this->GetShift())/this->GetScale(),
                                       (v_max-1.5*this->GetShift())/this->GetScale());
    
    this->AxialColorMapper->GetLookupTable()->SetRange (v_min, v_max);
    this->SagittalColorMapper->GetLookupTable()->SetRange (v_min, v_max);
    this->CoronalColorMapper->GetLookupTable()->SetRange (v_min, v_max);
      
    vtkLookupTable* lut = vtkLookupTable::SafeDownCast ( this->GetLookupTable() );
    
    if( lut )
    {
      this->ColorFunction->RemoveAllPoints();
      this->OpacityFunction->RemoveAllPoints();
      
      int numColors = lut->GetNumberOfTableValues();

      this->OpacityFunction->AddPoint(v_min, 0.0);
      for( int i=1; i<=numColors; i++)
      {
        double* color = lut->GetTableValue (i-1);
        this->ColorFunction->AddRGBPoint (v_min + (double)(i-1)*(v_max-v_min)/(numColors-1), color[0], color[1], color[2]);
	this->OpacityFunction->AddPoint( v_min + (double)(i)*(v_max-v_min)/(numColors), color[3]/scaleOpacity);
      }
      this->OpacityFunction->AddPoint(v_max, 1.0/scaleOpacity);
    }      
  }
}



void vtkViewImage3D::SetLevel (double l)
{

  vtkViewImage::SetLevel( this->GetShift() + l*this->GetScale() );

  double v_min = this->GetLevel() - 0.5*this->GetWindow();
  double v_max = this->GetLevel() + 0.5*this->GetWindow();

  double scaleOpacity = 1.4;
  
  if( this->GetLookupTable() )
  {
    this->GetLookupTable()->SetRange ( (v_min-0.5*this->GetShift())/this->GetScale(),
                                       (v_max-1.5*this->GetShift())/this->GetScale() );
    this->AxialColorMapper->GetLookupTable()->SetRange (v_min, v_max);
    this->SagittalColorMapper->GetLookupTable()->SetRange (v_min, v_max);
    this->CoronalColorMapper->GetLookupTable()->SetRange (v_min, v_max);
    
    
    vtkLookupTable* lut = vtkLookupTable::SafeDownCast ( this->GetLookupTable() );
    
    if( lut )
    {
      this->ColorFunction->RemoveAllPoints();
      this->OpacityFunction->RemoveAllPoints();

      int numColors = lut->GetNumberOfTableValues();

      this->OpacityFunction->AddPoint(v_min, 0.0);
      for( int i=1; i<=numColors; i++)
      {
        double* color = lut->GetTableValue (i-1);
        this->ColorFunction->AddRGBPoint (v_min + (double)(i-1)*(v_max-v_min)/(numColors-1), color[0], color[1], color[2]);
	this->OpacityFunction->AddPoint(v_min + (double)(i)*(v_max-v_min)/(numColors), color[3]/scaleOpacity);
      }
      this->OpacityFunction->AddPoint(v_max, 1.0/scaleOpacity);
    }
  }
  
}


void vtkViewImage3D::UpdatePosition ()
{

  if( this->GetImage() )
  {
    
    double*       spacing = this->GetImage()->GetSpacing();
    double*       origin  = this->GetImage()->GetOrigin();
    int*          dims    = this->GetImage()->GetWholeExtent();
    //double*      imBounds = this->GetImage()->GetBounds();

    const double* currentPoint = this->GetCurrentPoint();
    
    int imCoor[3];
    imCoor[0] = (int)(vtkMath::Round ( ( currentPoint[0]-origin[0] )/spacing[0] ));
    imCoor[1] = (int)(vtkMath::Round ( ( currentPoint[1]-origin[1] )/spacing[1] ));
    imCoor[2] = (int)(vtkMath::Round ( ( currentPoint[2]-origin[2] )/spacing[2] ));

    
    if( imCoor[0]<dims[0] ) imCoor[0]=dims[0];
    if( imCoor[0]>dims[1] ) imCoor[0]=dims[1];
    if( imCoor[1]<dims[2] ) imCoor[1]=dims[2];
    if( imCoor[1]>dims[3] ) imCoor[1]=dims[3];
    if( imCoor[2]<dims[4] ) imCoor[2]=dims[4];
    if( imCoor[2]>dims[5] ) imCoor[2]=dims[5];

   
    double ppoint[3];
    for( int i=0; i<3; i++)
    {
      ppoint[i] = currentPoint[i];
    }
    
    
    this->ActorCoronal->SetDisplayExtent  (dims[0],   dims[1],   imCoor[1], imCoor[1], dims[4],   dims[5]);
    this->ActorSagittal->SetDisplayExtent (imCoor[0], imCoor[0], dims[2],   dims[3],   dims[4],   dims[5]);
    this->ActorAxial->SetDisplayExtent    (dims[0],   dims[1],   dims[2],   dims[3],   imCoor[2], imCoor[2]);
    
    
    this->Axes->SetOrigin(ppoint[0], ppoint[1], ppoint[2]);
    
  }

  if( !this->GetRenderingMode()==VOLUME_RENDERING || this->GetAxisVisibility() )
    this->Modified();
  
}


void vtkViewImage3D::SetRenderingMode (int mode)
{
  if( this->RenderingMode==mode )
    return;
  
  switch(mode)
  {
      case VOLUME_RENDERING :
        
        this->RenderingMode = VOLUME_RENDERING;
	
        this->ActorSagittal->SetVisibility (0);
        this->ActorCoronal->SetVisibility (0);
        this->ActorAxial->SetVisibility (0);
	
        this->VolumeActor->SetVisibility ( this->GetVisibility() );
        this->AxesActor->SetVisibility ( this->GetVisibility() && this->AxisVisibility);

	if( this->GetRenderWindow() && !this->GetRenderWindow()->GetNeverRendered() )
	{
	  if( this->GetVisibility() )
	  {
	    this->SetBoxWidgetVisibility (this->BoxWidgetVisibility);
	  }
	  else
	  {
	    this->BoxWidget->Off ();
	    }
	}
	
	break;
	  
      case PLANAR_RENDERING :

        this->RenderingMode = PLANAR_RENDERING;

        this->VolumeActor->SetVisibility (0);
        this->AxesActor->SetVisibility (this->GetVisibility() && this->AxisVisibility);
        this->BoxWidget->Off();

        this->ActorSagittal->SetVisibility (this->GetVisibility() && this->ShowSagittal);
        this->ActorCoronal->SetVisibility (this->GetVisibility() && this->ShowCoronal);
        this->ActorAxial->SetVisibility (this->GetVisibility() && this->ShowAxial);

        break;

      default :
        vtkErrorMacro ( << "Unknown rendering mode!");
  };
    
  this->Modified();
}

void vtkViewImage3D::SetVRMapperType(VRMapperTypeIds type)
{
    if( this->VRMapperType==type )
      return;
    
    this->VRMapperType = type;

    vtkVolumeMapper *oldMapper = this->VolumeMapper3D;

    switch(type)
    {
        case TextureMapper3D:
           this->VolumeMapper3D = this->VolumeTextureMapper3D;
           break;
           
        case GPUMapper3D:
            this->VolumeMapper3D = this->VolumeGPUMapper3D;
            break;
           
        default :
            vtkErrorMacro ( << "Unknown VR Mapper type!");
    };
    
    // copy settings from oldMapper
    this->VolumeMapper3D->SetInput     ( oldMapper->GetInput() );
    this->VolumeMapper3D->SetCropping  ( oldMapper->GetCropping() );
    this->VolumeMapper3D->SetBlendMode ( oldMapper->GetBlendMode() );
    
    this->Callback->SetVolumeMapper ( this->VolumeMapper3D );
    this->VolumeActor->SetMapper    ( this->VolumeMapper3D );

    this->Callback->Execute (this->BoxWidget, 0, NULL);
}

void vtkViewImage3D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Rendering mode: " << this->RenderingMode << "\n";
}

#ifndef VTK_LEGACY_REMOVE
void vtkViewImage3D::Center()
{
  VTK_LEGACY_REPLACED_BODY(vtkViewImage3D::Center, "vtkINRIA3D 0.1",
			   vtkViewImage3D::ResetCamera);
  this->ResetCamera ();
}
#endif


void vtkViewImage3D::SetVisibility (int isVisible)
{
  vtkViewImage::SetVisibility ( isVisible );

  if( this->GetRenderingMode() == PLANAR_RENDERING )
  {
    this->VolumeActor->SetVisibility( 0 );
    this->AxesActor->SetVisibility( 0 );

    this->BoxWidget->Off();

    this->ActorSagittal->SetVisibility( isVisible );
    this->ActorCoronal->SetVisibility( isVisible );
    this->ActorAxial->SetVisibility( isVisible );
  }


  if( this->GetRenderingMode() == VOLUME_RENDERING )
  {
    this->ActorSagittal->SetVisibility( 0 );
    this->ActorCoronal->SetVisibility( 0 );
    this->ActorAxial->SetVisibility( 0 );

    this->VolumeActor->SetVisibility( isVisible );
    this->AxesActor->SetVisibility( isVisible && this->AxisVisibility);

    if( isVisible)
    {
      this->SetBoxWidgetVisibility (this->BoxWidgetVisibility);
    }
    else
    { 
      this->BoxWidget->Off();
    }
  }


}


void vtkViewImage3D::SetBoxWidgetVisibility (int a)
{
  this->BoxWidgetVisibility = a;

  if (this->GetRenderWindowInteractor())
  {
    if( this->BoxWidgetVisibility )
    {
      if ( this->GetVisibility() && this->RenderingMode == VOLUME_RENDERING )
      {
        this->BoxWidget->On();
      }
    }
    else
    {
      this->BoxWidget->Off();
    }
    this->Modified();
  }
}


void vtkViewImage3D::SetPlaneWidgetVisibility (int a)
{
  this->PlaneWidgetVisibility = a;
  if (this->GetRenderWindowInteractor())
  {
    if(a)
	{
	  this->PlaneWidget->On();
	}
	else
	{
	  this->PlaneWidget->Off();
    }
    this->Modified();
  }
}


void vtkViewImage3D::SetAxisVisibility (int a)
{
  this->AxisVisibility = a;
  this->AxesActor->SetVisibility (a);
  this->Modified();
}


void vtkViewImage3D::SetCubeVisibility (int a)
{
  this->CubeVisibility = a;
  this->Marker->SetEnabled (a);
  if( a )
  {
    this->Marker->InteractiveOff();
  }
  this->Modified();
}


void vtkViewImage3D::SetShade (int shade)
{
  this->Shade = shade;
  this->VolumeProperty->SetShade (shade);
  this->Modified();
}


void vtkViewImage3D::SetLookupTable(vtkScalarsToColors* lut)
{
  
  if( !lut )
  {
    return;
  }
  
  vtkViewImage::SetLookupTable (lut);


  double v_min = this->GetLevel() - 0.5*this->GetWindow();
  double v_max = this->GetLevel() + 0.5*this->GetWindow();

  /**
     In the case of a shift/scale, one must set the lut range to values
     without this shift/scale, because the object can be shared by different
     views.
   */
  lut->SetRange ( (v_min-0.5*this->GetShift())/this->GetScale(),
                  (v_max-1.5*this->GetShift())/this->GetScale());


  vtkLookupTable* realLut = vtkLookupTable::SafeDownCast (lut);

  if( !realLut )
  {
    vtkErrorMacro ( << "Error: Cannot cast vtkScalarsToColors to vtkLookupTable.");
    return;
  }

  /**
     Due to the same problem as above (shift/scale), one must copy the lut
     so that it does not change values of the shared object.
  */
  vtkLookupTable* newLut = vtkLookupTable::New();
  newLut->DeepCopy (realLut);
  newLut->SetRange (v_min, v_max);
  this->AxialColorMapper->SetLookupTable (newLut);
  this->SagittalColorMapper->SetLookupTable (newLut);
  this->CoronalColorMapper->SetLookupTable (newLut);
  
  newLut->Delete();


  this->ColorFunction->RemoveAllPoints();
  this->OpacityFunction->RemoveAllPoints();

  int numColors = newLut->GetNumberOfTableValues();
  for( int i=0; i<numColors; i++)
  {
    double* color = newLut->GetTableValue (i);
    this->ColorFunction->AddRGBPoint (v_min + (double)(i)*(v_max-v_min)/(double(numColors)-1.0), color[0], color[1], color[2]);
    this->OpacityFunction->AddPoint  (v_min + (double)(i)*(v_max-v_min)/(double(numColors)-1.0), color[3]);
  }
}


vtkScalarsToColors* vtkViewImage3D::GetColorFunction() const
{
  return this->ColorFunction;
}


void vtkViewImage3D::SetOverlappingImage (vtkImageData* image)
{
  vtkViewImage::SetOverlappingImage ( image );

  if( !this->GetImage() || !image )
  {
    return;
  }
  
  this->AxialBlender->RemoveAllInputs();
  this->SagittalBlender->RemoveAllInputs();
  this->CoronalBlender->RemoveAllInputs();
  this->Blender->RemoveAllInputs();
  
  if( this->GetImage()->GetScalarType() == VTK_UNSIGNED_CHAR )
  {
    this->AxialBlender->AddInput    ( this->GetImage() );
    this->SagittalBlender->AddInput ( this->GetImage() );
    this->CoronalBlender->AddInput  ( this->GetImage() );
    this->Blender->AddInput         ( this->GetImage() );
  }
  else
  {
    this->AxialBlender->AddInputConnection (0, this->AxialColorMapper->GetOutputPort() );
    this->SagittalBlender->AddInputConnection (0, this->SagittalColorMapper->GetOutputPort() );
    this->CoronalBlender->AddInputConnection (0, this->CoronalColorMapper->GetOutputPort() );
    
    vtkImageMapToWindowLevelColors* colorMapper = vtkImageMapToWindowLevelColors::New();
    colorMapper->SetInput ( this->GetImage() );
    //colorMapper->SetWindow ( this->AxialColorMapper->GetWindow() );
    colorMapper->SetWindow ( this->GetWindow() );
    //colorMapper->SetLevel ( this->AxialColorMapper->GetLevel() );
    colorMapper->SetLevel ( this->GetLevel() );
    colorMapper->Update();
    
    vtkImageData* outColor = colorMapper->GetOutput();
    int* dims = outColor->GetDimensions();
    int  tot  = dims[0]*dims[1]*dims[2];
    
    // Set the last component to be the grey-value of the original image.
    // Thus, when windowing the image in volume rendering mode, the last
    // component will be used to compute opacity.
    
	  
    unsigned char* outBuffer = (unsigned char*)(outColor->GetScalarPointer());
    for(int i=0; i<tot; i++)
    {
      int val = (int)(*outBuffer);
      outBuffer += 3;
      (*outBuffer++)=(unsigned char)(val);
    }
    
    
    this->Blender->AddInput ( outColor );
    colorMapper->Delete();
  }
  
  this->AxialBlender->AddInput ( image );
  this->SagittalBlender->AddInput ( image );
  this->CoronalBlender->AddInput ( image );
  this->Blender->AddInput ( image );
  
  this->ActorSagittal->SetInput ( this->AxialBlender->GetOutput() );
  this->ActorCoronal->SetInput ( this->SagittalBlender->GetOutput() );
  this->ActorAxial->SetInput ( this->CoronalBlender->GetOutput() );
  
  this->VolumeMapper3D->SetInput ( this->Blender->GetOutput() );
  
}


void vtkViewImage3D::RemoveOverlappingImage()
{
  if( !this->GetImage() || !this->GetOverlappingImage() )
  {
    return;
  }
  

  this->VolumeMapper3D->SetInput ( this->GetImage() );
  
  if( this->GetImage()->GetScalarType() == VTK_UNSIGNED_CHAR )
  {
    this->ActorSagittal->SetInput ( this->GetImage() );
    this->ActorCoronal->SetInput  ( this->GetImage() );
    this->ActorAxial->SetInput    ( this->GetImage() );
  }
  else
  {
    this->ActorSagittal->SetInput (this->SagittalColorMapper->GetOutput());
    this->ActorCoronal->SetInput (this->CoronalColorMapper->GetOutput());
    this->ActorAxial->SetInput (this->AxialColorMapper->GetOutput());
  }
  
  vtkViewImage::SetOverlappingImage ( 0 );
}


void vtkViewImage3D::SetShowAxial (int val)
{
  this->ShowAxial = val;
  this->ActorAxial->SetVisibility (this->GetVisibility() && this->ShowAxial && (this->RenderingMode==PLANAR_RENDERING) );
  this->Modified();
}


void vtkViewImage3D::SetShowCoronal (int val)
{
  this->ShowCoronal = val;
  this->ActorCoronal->SetVisibility (this->GetVisibility() && this->ShowCoronal && (this->RenderingMode==PLANAR_RENDERING) );
  this->Modified();
}


void vtkViewImage3D::SetShowSagittal (int val)
{
  this->ShowSagittal = val;
  this->ActorSagittal->SetVisibility (this->GetVisibility() && this->ShowSagittal && (this->RenderingMode==PLANAR_RENDERING) );
  this->Modified();
}


vtkActor* vtkViewImage3D::AddDataSet (vtkDataSet* dataset, vtkProperty* property)
{
  
  bool doit = true;
    
  if (!dataset)
    doit = false;
  
  if( this->HasDataSet (dataset) )
    doit = false;
  
  vtkImageData* imagedata = vtkImageData::SafeDownCast(dataset);
  
  if (imagedata)
  {
    this->SetImage(imagedata);
  }
  else
  {
      
    if (doit)
    {
      vtkDataSetSurfaceFilter* geometryextractor = vtkDataSetSurfaceFilter::New();
      vtkPolyDataNormals*      normalextractor = vtkPolyDataNormals::New();
      vtkPolyDataMapper*       mapper = vtkPolyDataMapper::New();
      vtkActor*                actor = vtkActor::New();
	
      normalextractor->SetFeatureAngle (90);
      ///\todo try to skip the normal extraction filter in order to enhance the visualization speed when the data is time sequence.
      geometryextractor->SetInput (dataset);
      normalextractor->SetInputConnection (geometryextractor->GetOutputPort());
      mapper->SetInput (normalextractor->GetOutput());
      actor->SetMapper (mapper);
      if (property)
	actor->SetProperty (property);
      
      this->AddActor (actor);
      this->DataSetList.push_back (dataset);
      this->DataSetActorList.push_back (actor);	  
	
      mapper->Delete();
      normalextractor->Delete();
      geometryextractor->Delete();
      actor->Delete();
    }
  }
    
  return this->GetDataSetActor(dataset);
    
}


void vtkViewImage3D::ColorDataSetByArray(vtkDataSet* dataset, const char* arrayname, vtkColorTransferFunction* transfer)
{

  
  bool doit = true;
    
    
    if (!dataset)
      doit = false;

    vtkDataArray* array = NULL;
    vtkMapper* mapper = NULL;
    
    if (doit)
    {  
      for (unsigned int i=0; i<this->DataSetList.size(); i++)
      {
	if (dataset == this->DataSetList[i])
	{
	  mapper = this->DataSetActorList[i]->GetMapper();
	  break;
	}
      }
      if (!mapper)
      {
	doit = false;
      }
    }

    if (doit)
    {
      mapper->Modified();
      
      if (dataset->GetCellData())
      {
	array = dataset->GetCellData()->GetArray (arrayname);
	if (array)
	{
	  if (!mapper->GetInput()->GetCellData()->HasArray (arrayname))
	  {
	    this->UpdateDataSetPipeline (dataset);
	    for (unsigned int i=0; i<this->DataSetList.size(); i++)
	    {
	      if (dataset == this->DataSetList[i])
	      {
		mapper = this->DataSetActorList[i]->GetMapper();
		break;
	      }
	    }
	  }
	  
	  mapper->SetScalarModeToUseCellFieldData();
	  
	}
	
      }
      
      if (!array && dataset->GetPointData())
      {
	array = dataset->GetPointData()->GetArray (arrayname);
	if (array)
	{
	  if (!mapper->GetInput()->GetPointData()->HasArray (arrayname))
	  {
	    this->UpdateDataSetPipeline (dataset);
	    for (unsigned int i=0; i<this->DataSetList.size(); i++)
	    {
	      if (dataset == this->DataSetList[i])
	      {
		mapper = this->DataSetActorList[i]->GetMapper();
		break;
	      }
	    }
	  }
	  
	  mapper->SetScalarModeToUsePointFieldData();
	}
      }
      
      if (!array)
      {
	mapper->SetScalarModeToDefault();
	mapper->SetInterpolateScalarsBeforeMapping(0);
	doit = false;
      }
    }

    if (doit)
    {
      mapper->SetLookupTable (transfer);
      mapper->SetScalarRange (array->GetRange()[0], array->GetRange()[1]);
      mapper->SetInterpolateScalarsBeforeMapping(1);
      mapper->SelectColorArray (array->GetName());
    }
}


void vtkViewImage3D::SetDirectionMatrix (vtkMatrix4x4 *mat)
{
    //vtkViewImage::SetDirectionMatrix (mat);
	this->GetDirectionMatrix()->DeepCopy( mat );

  /**
     If image has an origin, it is already taken into account in the image actors (position==origin).
     When giving a DirectionMatrix with permutations/flips, the origin will be warped as well, which
     is not what is expected. To correct for this, we change the translation of the DirectionMatrix
     such that DirectionMatrix*Origin = Origin.
   */

  if (this->GetImage())
  {
    // clear the former translation in case there is any
    for (int i=0; i<3; i++)
    {
      this->GetDirectionMatrix()->SetElement (i, 3, 0.0);
    }
    double origin[4];
    this->GetImage()->GetOrigin (origin);
    origin[3] = 1.0;
    double w_origin[4];
    this->GetDirectionMatrix()->MultiplyPoint (origin, w_origin);
    for (int i=0; i<3; i++)
    {
      this->GetDirectionMatrix()->SetElement (i, 3, origin[i]-w_origin[i]);
    }
  }

  this->ActorSagittal->SetUserMatrix ( this->GetDirectionMatrix() );
  this->ActorCoronal->SetUserMatrix ( this->GetDirectionMatrix() );
  this->ActorAxial->SetUserMatrix ( this->GetDirectionMatrix() );
  this->VolumeActor->SetUserMatrix ( this->GetDirectionMatrix() );

  this->AxesActor->SetUserMatrix ( this->GetDirectionMatrix() );
  
  this->BoxWidget->SetOrientationMatrix (this->GetDirectionMatrix() );

  this->Modified();
}


void vtkViewImage3D::UpdateDataSetPipeline (vtkDataSet* dataset)
{

  vtkProperty* prop = NULL;
  vtkActor* actor = this->GetDataSetActor (dataset);
  if (actor)
  {
    prop = actor->GetProperty();
    prop->Register(this);
  }  
  
  this->RemoveDataSet (dataset);
  this->AddDataSet (dataset, prop);

  prop->UnRegister (this);
}

  
void vtkViewImage3D::SetCropping ( int value )
{
  this->Cropping = value;
  if( value )
  {
    this->VolumeMapper3D->CroppingOn();
  }
  else
  {
    this->VolumeMapper3D->CroppingOff();
  }
  this->Modified();
}


void vtkViewImage3D::SetVRQualityToLow()
{
  if ( vtkVolumeTextureMapper3D* mapper2 = vtkVolumeTextureMapper3D::SafeDownCast ( this->VolumeMapper3D ) )
  {
    mapper2->SetSampleDistance (5.0);
  }
  else if ( vtkVolumeTextureMapper2D* mapper3 = vtkVolumeTextureMapper2D::SafeDownCast ( this->VolumeMapper3D ) )
  {
    mapper3->SetMaximumNumberOfPlanes (64);
  }
  else
  {
    return;
  }
  this->Modified();
}


void vtkViewImage3D::SetVRQualityToMed()
{
  if ( vtkVolumeTextureMapper3D* mapper2 = vtkVolumeTextureMapper3D::SafeDownCast ( this->VolumeMapper3D ) )
  {
    mapper2->SetSampleDistance (2.5);
  }
  else if ( vtkVolumeTextureMapper2D* mapper3 = vtkVolumeTextureMapper2D::SafeDownCast ( this->VolumeMapper3D ) )
  {
    mapper3->SetMaximumNumberOfPlanes (128);
  }
  else
  {
    return;
  }
  this->Modified();
}


void vtkViewImage3D::SetVRQualityToHigh()
{
  if ( vtkVolumeTextureMapper3D* mapper2 = vtkVolumeTextureMapper3D::SafeDownCast ( this->VolumeMapper3D ) )
  {
    mapper2->SetSampleDistance (1.0);
  }
  else if ( vtkVolumeTextureMapper2D* mapper3 = vtkVolumeTextureMapper2D::SafeDownCast ( this->VolumeMapper3D ) )
  {
    mapper3->SetMaximumNumberOfPlanes (256);
  }
  else
  {
    return;
  }
  this->Modified();
}


void vtkViewImage3D::SetVRQuality (int value)
{
  this->VRQuality = value;
  switch( value )
  {
      case 0: // low
	this->SetVRQualityToLow();
	break;

      case 1: // med
	this->SetVRQualityToMed();
	break;

      case 2: // high
	this->SetVRQualityToHigh();
	break;

      default:
	break;
  }
}
