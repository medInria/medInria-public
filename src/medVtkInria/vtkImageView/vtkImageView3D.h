/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkInriaExport.h>

#include <vtkImageView.h>
#include <vtkOrientedBoxWidget.h>

#include <vtkPlaneWidget.h>
#include <vtkVolume.h>
#include <vtkImageView3DCroppingBoxCallback.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkVolumeProperty.h>
#include <vtkSmartPointer.h>

class vtkVolume;
class vtkPiecewiseFunction;
class vtkColorTransferFunction;
class vtkVolume;
class vtkImageActor;
class vtkAxes;
class vtkTubeFilter;
class vtkAnnotatedCubeActor;
class vtkAxesActor;
class vtkImageBlend;
class vtkViewImage2D;
class vtkScalarsToColors;
class vtkColorTransferFunction;
class vtkSmartVolumeMapper;
class vtkImage3DDisplay;
class vtkProp3DCollection;

/**
   \class vtkImageView3D vtkImageView3D.h "vtkImageView3D.h"
   \brief 3D image 3D rendering viewer
   \author Pierre Fillard & Marc Traina & Nicolas Toussaint

   This class allows to view 3D images. Images have to be
   vtkImageData.
   volume rendering and mulptiplane reconstructions are provided
   remote plan can also be used, so can be an orientation cube, ...
*/

class MEDVTKINRIA_EXPORT vtkImageView3D : public vtkImageView
{
public:

  static vtkImageView3D* New();
  vtkTypeRevisionMacro(vtkImageView3D, vtkImageView);

  // Override vtkObject - return the maximum mtime of this and any objects owned by this.
  unsigned long GetMTime();

  // Description:
  // Rendeing Modes available.
  // PLANAR_RENDERING will render every vtkImageActor instance added with Add2DPhantom()
  // whereas VOLUME_RENDERING will render the volume added with SetInput().
  //BTX
  enum RenderingModeIds
  {
    VOLUME_RENDERING = 0,
    PLANAR_RENDERING
  };
  //ETX

  vtkGetObjectMacro (VolumeActor, vtkVolume);
  // vtkGetObjectMacro (OpacityFunction, vtkPiecewiseFunction);
  // vtkGetObjectMacro (ColorFunction, vtkColorTransferFunction);
  vtkGetObjectMacro (VolumeProperty, vtkVolumeProperty);
  vtkGetObjectMacro (PlaneWidget, vtkPlaneWidget);
  vtkGetObjectMacro (BoxWidget, vtkOrientedBoxWidget);
  vtkGetObjectMacro (ActorX, vtkImageActor);
  vtkGetObjectMacro (ActorY, vtkImageActor);
  vtkGetObjectMacro (ActorZ, vtkImageActor);
  vtkGetObjectMacro (ExtraPlaneCollection, vtkProp3DCollection);

  virtual vtkImageData* GetInput(int layer = 0) const;

  virtual void SetVolumeMapperTo3DTexture();
  virtual void SetVolumeMapperToRayCast();
  virtual void SetVolumeMapperToRayCastAndTexture();
  virtual void SetVolumeMapperToGPU();
  virtual void SetVolumeMapperToDefault();

  virtual void SetVolumeRayCastFunctionToComposite();
  virtual void SetVolumeRayCastFunctionToMaximumIntensityProjection();
  virtual void SetVolumeRayCastFunctionToMinimumIntensityProjection();
  // virtual void SetVolumeRayCastFunctionToAdditive();

  virtual void SetInterpolationToNearestNeighbor();
  virtual void SetInterpolationToLinear();

  /** Set the box widget visibility */
  void SetShowBoxWidget (int a)
  {
    if (this->Interactor)
      this->BoxWidget->SetEnabled (a);
  }
  bool GetShowBoxWidget()
  {
    return this->BoxWidget->GetEnabled();
  }
  vtkBooleanMacro (ShowBoxWidget, int);

  /** Set the plane widget on */
  void SetShowPlaneWidget (int a)
  {
    if (this->Interactor)
      this->PlaneWidget->SetEnabled (a);
  }
  bool GetShowPlaneWidget()
  {
    return this->PlaneWidget->GetEnabled();
  }
  vtkBooleanMacro (ShowPlaneWidget, int);

  /** Set the cube widget on */
  void SetShowCube (int a)
  {
    if (this->Interactor)
      this->Marker->SetEnabled (a);
  }
  bool GetShowCube()
  {
    return this->Marker->GetEnabled();
  }
  vtkBooleanMacro (ShowCube, int);

  void SetShade (int a)
  {
    this->VolumeProperty->SetShade (a);
  }
  bool GetShade()
  {
    return this->VolumeProperty->GetShade();
  }
  vtkBooleanMacro (Shade, int);

  /** Set the rendering mode to volume rendering (VR). */
  virtual void SetRenderingModeToVR()
  {this->SetRenderingMode (VOLUME_RENDERING); }
  /** Set the rendering mode to planar views. */
  virtual void SetRenderingModeToPlanar()
  { this->SetRenderingMode (PLANAR_RENDERING); }
  /** Set the rendering mode. */
  virtual void SetRenderingMode (int mode);
  /** Get the current rendering mode. */
  vtkGetMacro (RenderingMode, int);

  // Description:
  // Cropping Modes available.
  // CROPPING_INSIDE will crop inside the image
  // whereas CROPPING_OUTSIDE will crop outside.
  enum CroppingModeIds
  {
    CROPPING_OFF     = 0,
    CROPPING_INSIDE  = 1,
    CROPPING_OUTSIDE = 2
  };

  /** Set the cropping mode */
  virtual void SetCroppingModeToOff();
  virtual void SetCroppingModeToInside();
  virtual void SetCroppingModeToOutside();
  virtual void SetCroppingMode(unsigned int);
  // vtkGetMacro (CroppingMode, int);
  virtual unsigned int GetCroppingMode ();

  virtual void SetInput (vtkImageData* input, vtkMatrix4x4 *matrix = 0, int layer = 0);
  virtual void AddInput (vtkImageData* input, vtkMatrix4x4 *matrix = 0);
  virtual void SetOrientationMatrix (vtkMatrix4x4* matrix);

  using vtkImageView::SetColorWindow;
  virtual void SetColorWindow(double s,int layer);
  using vtkImageView::SetColorLevel;
  virtual void SetColorLevel(double s,int layer);
  /** Set a user-defined lookup table */
  virtual void SetLookupTable (vtkLookupTable* lookuptable, int layer);
  /**
   * Transfer functions define the mapping of the intensity or color
   * values in the image to colors and opacity displayed on the screen.
   */
  virtual void SetTransferFunctions(vtkColorTransferFunction * color,
                                    vtkPiecewiseFunction * opacity,
                                    int layer);

  virtual void SetOpacity(double opacity, int layer);
  virtual double GetOpacity(int layer) const;

  virtual void SetVisibility(int visibility, int layer);
  virtual int  GetVisibility(int layer) const;

  // /** toggle use of VRQuality */
  // void SetUseVRQuality (bool on);
  // bool GetUseVRQuality ();
  // vtkBooleanMacro (UseVRQuality, bool);

  // /**
  //    Presets for VR quality.
  //  */
  // virtual void SetVRQualityToLow()
  // { this->SetVRQuality (0.0); };
  // virtual void SetVRQualityToMed()
  // { this->SetVRQuality (0.5); };
  // virtual void SetVRQualityToHigh()
  // { this->SetVRQuality (1.0); };
  // virtual void SetVRQuality (float);
  // virtual float GetVRQuality ();

  virtual void SetShowActorX (unsigned int);
  vtkGetMacro (ShowActorX, unsigned int);
  vtkBooleanMacro (ShowActorX, unsigned int);
  virtual void SetShowActorY (unsigned int);
  vtkGetMacro (ShowActorY, unsigned int);
  vtkBooleanMacro (ShowActorY, unsigned int);
  virtual void SetShowActorZ (unsigned int);
  vtkGetMacro (ShowActorZ, unsigned int);
  vtkBooleanMacro (ShowActorZ, unsigned int);

  void SetOrientationMarker(vtkActor *actor);
  void SetOrientationMarkerViewport(double, double, double, double);

  /**
     The wolrd is not always what we think it is ...

     Use this method to move the viewer slice such that the position
     (in world coordinates) given by the arguments is contained by
     the slice plane. If the given position is outside the bounds
     of the image, then the slice will be as close as possible.
  */
  virtual void SetCurrentPoint (double pos[3]);

  virtual void UpdateDisplayExtent();


  virtual void InstallInteractor();
  virtual void UnInstallInteractor();


  virtual vtkActor* AddDataSet (vtkPointSet* arg, vtkProperty* prop = NULL);
  virtual void RemoveDataSet (vtkPointSet* arg);

  /**
     Add an extra plane to the 3D view. the argument is an image actor
     that supposingly follows a vtkImageView2D instance. The actor will
     be displayed in the 3D scene and will be fully synchronized with
     the actor it came from.
  */
  virtual void AddExtraPlane (vtkImageActor* input);
  virtual void RemoveExtraPlane (vtkImageActor* input);


  virtual void AddLayer (int layer);
  virtual int GetNumberOfLayers() const;
  virtual void RemoveLayer (int layer);
  virtual void RemoveAllLayers();

  //pure virtual methods from base class:
  virtual vtkColorTransferFunction * GetColorTransferFunction(int layer) const;
  virtual vtkPiecewiseFunction* GetOpacityTransferFunction (int layer) const;
  virtual void StoreColorTransferFunction (vtkColorTransferFunction *ctf,
                                           int layer);
  virtual void StoreOpacityTransferFunction (vtkPiecewiseFunction *otf,
                                             int layer);
  virtual vtkLookupTable * GetLookupTable(int layer) const;
  virtual bool GetUseLookupTable(int layer) const;
  virtual void SetUseLookupTable (bool use, int layer);
  using vtkImageView::GetColorWindow;
  virtual double GetColorWindow(int layer) const;
  virtual void StoreColorWindow(double s,int layer);
  using vtkImageView::GetColorLevel;
  virtual double GetColorLevel(int layer) const;
  virtual void StoreColorLevel(double s,int layer);
  virtual void StoreLookupTable (vtkLookupTable *lookuptable, int layer);

protected:

  vtkImageView3D();
  ~vtkImageView3D();

  // Description:
  virtual void InstallPipeline();
  virtual void UnInstallPipeline();


  virtual void SetupVolumeRendering();
  virtual void SetupWidgets();
  virtual void UpdateVolumeFunctions(int layer);
  virtual void ApplyColorTransferFunction(vtkScalarsToColors * colors,
                                                  int layer);

  virtual void InternalUpdate();

  //! Get layer specific info
  vtkImage3DDisplay * GetImage3DDisplayForLayer(int layer) const;
  
  //! Cast layers to layer 0 's type if necessary
  void castLayers();

  // plane actors
  vtkImageActor* ActorX;
  vtkImageActor* ActorY;
  vtkImageActor* ActorZ;

  // volume property
  vtkVolumeProperty* VolumeProperty;
  // volume actor
  vtkVolume* VolumeActor;

  // smart volume mapper
  vtkSmartVolumeMapper* VolumeMapper;

//  // blender
//  vtkImageBlend* Blender;
  // image 3D cropping box callback
  vtkImageView3DCroppingBoxCallback* Callback;
  // box widget
  vtkOrientedBoxWidget* BoxWidget;
  // vtkPlane widget
  vtkPlaneWidget* PlaneWidget;
  // annotated cube actor
  vtkAnnotatedCubeActor* Cube;
  vtkOrientationMarkerWidget* Marker;


  /**
     The ExtraPlaneCollection is a collection gathering the ImageActor
     instances that are currently displayed in addition to common ones
     (ActorX, ActorY, ActorZ).
     ExtraPlaneInputCollection is "read-only" collection to be able to
     know which inputs the ExtraPlaneCollection instances are actually
     following.
  */
  vtkProp3DCollection* ExtraPlaneCollection;
  /**
     The ExtraPlaneCollection is a collection gathering the ImageActor
     instances that are currently displayed in addition to common ones
     (ActorX, ActorY, ActorZ).
     ExtraPlaneInputCollection is "read-only" collection to be able to
     know which inputs the ExtraPlaneCollection instances are actually
     following.
  */
  vtkProp3DCollection* ExtraPlaneInputCollection;


  unsigned int RenderingMode;
  unsigned int ShowActorX;
  unsigned int ShowActorY;
  unsigned int ShowActorZ;

  int          LastNodeIndex;

  int          CroppingMode;

  double Opacity;
  int Visibility;

  struct LayerInfo {
      vtkSmartPointer<vtkImage3DDisplay> ImageDisplay;
      bool NeedCast;
  };

  vtkSmartPointer<vtkImageMapToColors>        PlanarWindowLevel;
  // Layer indices start from 0, as they do for the 2d case
  //(API change)
  typedef std::vector<LayerInfo > LayerInfoVecType;
  LayerInfoVecType LayerInfoVec;
private:
  vtkImageView3D(const vtkImageView3D&);  // Not implemented.
  void operator=(const vtkImageView3D&);    // Not implemented.

};



