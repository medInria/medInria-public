/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkImageView3D.h 1323 2009-11-16 18:14:33Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-11-16 19:14:33 +0100 (lun, 16 nov 2009) $
Version:   $Revision: 1323 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// version vtkRenderingAddOn
#ifndef _vtkImageView3D_h_
#define _vtkImageView3D_h_

#include "vtkINRIA3DConfigure.h"

#include "vtkImageView.h"

#include <vtkPlaneWidget.h>
#include <vtkOrientedBoxWidget.h>
#include <vtkVolume.h>
#include <vtkImageView3DCroppingBoxCallback.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkVolumeProperty.h>

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

/**
   \class vtkImageView3D vtkImageView3D.h "vtkImageView3D.h"
   \brief 3D image 3D rendering viewer
   \author Pierre Fillard & Marc Traina & Nicolas Toussaint
   
   This class allows to view 3D images. Images have to be
   vtkImageData.
   volume rendering and mulptiplane reconstructions are provided
   remote plan can also be used, so can be an orientation cube, ...
*/

class VTK_IMAGEVIEW_EXPORT vtkImageView3D : public vtkImageView
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
  
  virtual void SetVolumeMapperTo3DTexture (void);
  virtual void SetVolumeMapperToRayCast (void);
  virtual void SetVolumeMapperToRayCastAndTexture (void);
  virtual void SetVolumeMapperToGPU (void);
  virtual void SetVolumeMapperToDefault (void);

  virtual void SetVolumeRayCastFunctionToComposite (void);
  virtual void SetVolumeRayCastFunctionToMaximumIntensityProjection (void);
  virtual void SetVolumeRayCastFunctionToMinimumIntensityProjection (void);
  // virtual void SetVolumeRayCastFunctionToAdditive (void);
   
  virtual void SetInterpolationToNearestNeighbor (void);
  virtual void SetInterpolationToLinear (void);

  /** Set the box widget visibility */
  void SetShowBoxWidget (int a)
  {
    if (this->Interactor)
      this->BoxWidget->SetEnabled (a);
  }
  bool GetShowBoxWidget (void)
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
  bool GetShowPlaneWidget (void)
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
  bool GetShowCube (void)
  {
    return this->Marker->GetEnabled();
  }
  vtkBooleanMacro (ShowCube, int);
  
  void SetShade (int a)
  {
    this->VolumeProperty->SetShade (a);
  }
  bool GetShade (void)
  {
    return this->VolumeProperty->GetShade();
  }
  vtkBooleanMacro (Shade, int);

  /** Set the rendering mode to volume rendering (VR). */
  virtual void SetRenderingModeToVR (void)
  {this->SetRenderingMode (VOLUME_RENDERING); }
  /** Set the rendering mode to planar views. */
  virtual void SetRenderingModeToPlanar (void)
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
  virtual void SetCroppingModeToOff(void);
  virtual void SetCroppingModeToInside(void);
  virtual void SetCroppingModeToOutside(void);
  virtual void SetCroppingMode(unsigned int);
  // vtkGetMacro (CroppingMode, int);
  virtual unsigned int GetCroppingMode ();
  
  virtual void SetInput (vtkImageData* input, vtkMatrix4x4 *matrix = 0, int layer = 0);
  virtual void SetOrientationMatrix (vtkMatrix4x4* matrix);
  // Description:
  // Set window and level for mapping pixels to colors.
  virtual void SetColorWindow(double s);
  virtual void SetColorLevel(double s);
  /** Set a user-defined lookup table */
  virtual void SetLookupTable (vtkLookupTable* lookuptable);
  /**
   * Transfer functions define the mapping of the intensity or color
   * values in the image to colors and opacity displayed on the screen.
   */
  virtual void SetTransferFunctions(vtkColorTransferFunction * color,
                                    vtkPiecewiseFunction * opacity, int layer);

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
  // virtual void SetVRQualityToLow (void)
  // { this->SetVRQuality (0.0); };
  // virtual void SetVRQualityToMed (void)
  // { this->SetVRQuality (0.5); };
  // virtual void SetVRQualityToHigh (void)
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

  virtual void UpdateDisplayExtent (void);


  virtual void InstallInteractor();
  virtual void UnInstallInteractor();


  virtual vtkActor* AddDataSet (vtkPointSet* arg, vtkProperty* prop = NULL);

  virtual void AddLayer (int layer);
  virtual bool HasLayer (int layer) const;
  virtual void RemoveLayer (int layer);
  virtual void RemoveAllLayers (void);
    
protected: 

  vtkImageView3D();
  ~vtkImageView3D();

  // Description:
  virtual void InstallPipeline();
  virtual void UnInstallPipeline();

  
  virtual void SetupVolumeRendering();
  virtual void SetupWidgets();
  virtual void UpdateVolumeFunctions();

  virtual void InternalUpdate (void);

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
  
  // blender
  vtkImageBlend* Blender;
  // image 3D cropping box callback
  vtkImageView3DCroppingBoxCallback* Callback; 
  // box widget
  vtkOrientedBoxWidget* BoxWidget;
  // vtkPlane widget
  vtkPlaneWidget* PlaneWidget;
  // annotated cube actor
  vtkAnnotatedCubeActor* Cube;
  vtkOrientationMarkerWidget* Marker;

  
  unsigned int RenderingMode;
  unsigned int ShowActorX;
  unsigned int ShowActorY;
  unsigned int ShowActorZ;
  
  int          LastNodeIndex;

  int          CroppingMode;
    
  double Opacity;
  int Visibility;
    
  std::map<int, vtkImage3DDisplay*> ImageDisplayMap;
  
private:
  vtkImageView3D(const vtkImageView3D&);  // Not implemented.
  void operator=(const vtkImageView3D&);    // Not implemented.
  
};
  

#endif
