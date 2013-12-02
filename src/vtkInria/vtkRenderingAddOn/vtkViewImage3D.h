/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkRenderingAddOnExport.h"

#include <vector>
#include <vtkViewImage.h>
#include <vtkObjectFactory.h>

class vtkImageData;
class vtkVolumeTextureMapper3D;
class vtkVolumeTextureMapper2D;
class vtkGPUVolumeRayCastMapper;
class vtkVolumeMapper;
class vtkPiecewiseFunction;
class vtkColorTransferFunction;
class vtkVolume;
class vtkImageActor;
class vtkAxes;
class vtkDataSet3DCroppingPlaneCallback;
class vtkTubeFilter;
class vtkAnnotatedCubeActor;
class vtkOrientationMarkerWidget;
class vtkAxesActor;
class vtkImageBlend;
class vtkViewImage2D;
class vtkScalarsToColors;
class vtkColorTransferFunction;
class vtkDataSet;
class vtkProperty;
class vtkOrientedBoxWidget;
class vtkPlaneWidget;
class vtkVolumeProperty;
class vtkImageMapToColors;
class vtkImageMapToWindowLevelColors;
class vtkImage3DCroppingBoxCallback;

/**
   \class vtkViewImage3D vtkViewImage3D.h "vtkViewImage3D.h"
   \brief 3D image 3D rendering viewer
   \author Pierre Fillard & Marc Traina & Nicolas Toussaint
   
   This class allows to view 3D images. Images have to be
   vtkImageData. It also handles an overlapping image.
   volume rendering and mulptiplane reconstructions are provided
   remote plan can also be used, so can be an orientation cube, ...
*/



class VTK_RENDERINGADDON_EXPORT vtkViewImage3D : public vtkViewImage
{
  
public:

  static vtkViewImage3D* New();
  vtkTypeRevisionMacro(vtkViewImage3D, vtkViewImage);

  
  // 2 rendering modes are possible
  //BTX
  enum RenderingModeIds
  {
    VOLUME_RENDERING,
    PLANAR_RENDERING
  };
  //ETX
  
  // 2 possible mappers for volume rendering
  enum VRMapperTypeIds
  {
    TextureMapper3D,
    GPUMapper3D
  };


  /**
     In some cases, we would like to call some cleaning
     functions before going into the unregister vtk stuff.
  */
  virtual void PrepareForDelete();
  

  virtual void PrintSelf(ostream& os, vtkIndent indent);


  /**
     Set the current image.
  */
  virtual void SetImage(vtkImageData* image);
  
  
  /**
     Set/Get window/level for mapping pixels to colors.
  */
  virtual void SetWindow (double w);

  virtual void StartWindowing();
  virtual void EndWindowing();
  
  /**
     Set/Get window/level for mapping pixels to colors.
  */
  virtual void SetLevel (double l);


  /**
     Set the rendering mode to volume rendering (VR).
  */
  virtual void SetRenderingModeToVR()
  {
    this->SetRenderingMode (VOLUME_RENDERING);
    this->Modified();
  };


  /**
     Set the rendering mode to planar views.
  */
  virtual void SetRenderingModeToPlanar()
  {
    this->SetRenderingMode (PLANAR_RENDERING);
    this->Modified();
  };


  /**
     Set/Get the rendering mode.
  */
  virtual void SetRenderingMode (int mode);
  vtkGetMacro (RenderingMode, int);


  /**
   * Set/Get rendering mapper type to texture
   */
  virtual void SetVRMapperTypeToTexture()
  {
      this->SetVRMapperType(TextureMapper3D);
      this->Modified();
  };

  
  /**
   * Set/Get rendering mapper type to GPU
   */
  virtual void SetVRMapperTypeToGPU()
  {
      this->SetVRMapperType(GPUMapper3D);
      this->Modified();
  };

  
  /**
   * Set/Get rendering mapper type
   */
  virtual void SetVRMapperType(VRMapperTypeIds type);
  vtkGetMacro (VRMapperType, VRMapperTypeIds);


  /**
     Center the view and reset the camera clipping range.
  */
  VTK_LEGACY (virtual void Center());
  
  
  /**
     Turn on/off the visibility of the image.
  */
  virtual void SetVisibility (int);

  
  /**
     Set the box widget visibility.
  */
  virtual void SetBoxWidgetVisibility (int a);
  vtkBooleanMacro (BoxWidgetVisibility, int);
  vtkGetMacro (BoxWidgetVisibility, bool);


  /**
     Activate / Deactivate the cropping box.
   */
  virtual void SetCropping (int value);
  vtkBooleanMacro (Cropping, int);
  vtkGetMacro (Cropping, int);

  
  /**
     Set the plane widget On/Off.
  */
  virtual void SetPlaneWidgetVisibility (int a);
  vtkBooleanMacro (PlaneWidgetVisibility, int);
  vtkGetMacro (PlaneWidgetVisibility, int);

  
  /**
     Get the dataset callback.
  */
  vtkGetObjectMacro (DataSetCallback, vtkDataSet3DCroppingPlaneCallback);

  
  /**
     Set the visibility of the 3D cursor.
  */
  virtual void SetAxisVisibility (int a);
  vtkBooleanMacro (AxisVisibility, int);
  vtkGetMacro (AxisVisibility, int);


  /**
     Set the orientation cube visibility.
  */  
  virtual void SetCubeVisibility (int a);
  vtkBooleanMacro (CubeVisibility, int);
  vtkGetMacro (CubeVisibility, int);


  /**
     Change the Ray Cast function to composite (default).
   */  
  virtual void SetVolumeRayCastFunctionToComposite();
  

  /**
     Change the Ray Cast function to MIP.
   */
  virtual void SetVolumeRayCastFunctionToMIP();
  

  /**
     Swith On/Off shading when VR is On.
   */
  virtual void SetShade (int);
  vtkBooleanMacro (Shade, int);
  vtkGetMacro (Shade, int);


  /**
     Presets for VR quality.
   */  
  virtual void SetVRQualityToLow();
  virtual void SetVRQualityToMed();
  virtual void SetVRQualityToHigh();
  virtual void SetVRQuality (int);
  vtkGetMacro (VRQuality, int);
  
  /**
     Set an overlapping image
  */
  virtual void SetOverlappingImage (vtkImageData*);
  

  /** Remove the overlapping image */
  virtual void RemoveOverlappingImage();

  
  /**
     Set a user-defined lookup table.
  */
  virtual void SetLookupTable (vtkScalarsToColors* lut);


  /**
     Returns the Color transfer function object.
   */
  vtkScalarsToColors* GetColorFunction() const;

  
  /**
     Get the volume actor object.
  */
  vtkGetObjectMacro (VolumeActor, vtkVolume);

  
  /**
     Get each plane's actor object.
  */
  vtkGetObjectMacro (ActorAxial, vtkImageActor);
  vtkGetObjectMacro (ActorSagittal, vtkImageActor);
  vtkGetObjectMacro (ActorCoronal, vtkImageActor);


  /**
     Get the plane widget object.
   */
  vtkGetObjectMacro (PlaneWidget, vtkPlaneWidget);


  /**
     Get the volume property object.
   */
  vtkGetObjectMacro (VolumeProperty, vtkVolumeProperty);


  /**
     Get the box widget object.
   */
  vtkGetObjectMacro (BoxWidget, vtkOrientedBoxWidget);

  
  virtual void UpdatePosition();
  

  /**
     Set/Get each plane visibility flag.
  */
  void SetShowAxial (int val);
  vtkGetMacro (ShowAxial, int);
  vtkBooleanMacro (ShowAxial, int);

  /**
     Set/Get each plane visibility flag
  */
  void SetShowCoronal (int val);
  vtkGetMacro (ShowCoronal, int);
  vtkBooleanMacro (ShowCoronal, int);

  
  /**
     Set/Get each plane visibility flag
  */
  void SetShowSagittal (int val);
  vtkGetMacro (ShowSagittal, int);
  vtkBooleanMacro (ShowSagittal, int);

  
  /**
     Add a dataset to the view (polydata or grid)
     the dataset will be display as its intersection with the displayed image slice
     i.e. tetrahedron will be displayed as triangles, triangles as lines, lines as points
     you can associate a property to be plugged into the dataset actor
  */
  virtual vtkActor* AddDataSet (vtkDataSet* dataset, vtkProperty* property = NULL);

  
  /**
     This method colorizes a given dataset (previously added with AddDataSet()) by one of its arrays
     It doesn't handle a specific component colorization yet. If the array contains multi component scalars,
     It takes the norm of the vector.
  */
  virtual void ColorDataSetByArray(vtkDataSet* dataset, const char* arrayname, vtkColorTransferFunction* transfer);  


  virtual void SetDirectionMatrix (vtkMatrix4x4*);

  virtual void Initialize();
  virtual void Uninitialize();

  virtual void InitializeInteractor();
  virtual void UninitializeInteractor();
  
protected:

  vtkViewImage3D();
  ~vtkViewImage3D();

  virtual void UpdateDataSetPipeline (vtkDataSet* dataset);

  
private:

  // texture mapper in 3D
  vtkVolumeMapper*                VolumeMapper3D;

  // Default mapper in 3D (slow)
  vtkVolumeTextureMapper3D*       VolumeTextureMapper3D;
    
  vtkVolumeTextureMapper2D*       VolumeTextureMapper2D;

  // GPU mapper in 3D (fast)
  vtkGPUVolumeRayCastMapper*      VolumeGPUMapper3D;

  // volume property
  vtkVolumeProperty*              VolumeProperty;

  // volume actor
  vtkVolume*                      VolumeActor;

  // opacity transfer function
  vtkPiecewiseFunction*           OpacityFunction;

  // color transfer function
  vtkColorTransferFunction*       ColorFunction;

  // axes
  vtkAxes*                        Axes;
  int                             AxisVisibility;
  
  
  // turn axes into tubes
  vtkTubeFilter*                  TubeFilter;
  
  // axes actor
  vtkActor*                       AxesActor;

  // box widget
  vtkOrientedBoxWidget*           BoxWidget;
  int                             BoxWidgetVisibility;
  int                             PlaneWidgetVisibility;
  int                             CubeVisibility;

  // vtkImagePlane widget
  vtkPlaneWidget*                 PlaneWidget;
  
  // image 3D cropping box callback
  vtkImage3DCroppingBoxCallback*     Callback;
  vtkDataSet3DCroppingPlaneCallback* DataSetCallback;
  
  //vtkImage3DImagePlaneCallback*   PlaneCallback;

  // 4 image blenders
  vtkImageBlend*                  AxialBlender;
  vtkImageBlend*                  SagittalBlender;
  vtkImageBlend*                  CoronalBlender;
  vtkImageBlend*                  Blender;
  
  // Image color mapper
  vtkImageMapToColors* AxialColorMapper;
  vtkImageMapToColors* SagittalColorMapper;
  vtkImageMapToColors* CoronalColorMapper;
  
  // actors
  vtkImageActor*                  ActorSagittal;
  vtkImageActor*                  ActorCoronal;
  vtkImageActor*                  ActorAxial;

  
  // rendering mode
  RenderingModeIds                RenderingMode;

  // mapper type
  VRMapperTypeIds                   VRMapperType;

  
  // annotated cube actor
  vtkAnnotatedCubeActor*          Cube;
  vtkOrientationMarkerWidget*     Marker;


  // Flags for showing the planes
  int                            ShowAxial;
  int                            ShowCoronal;
  int                            ShowSagittal;

  int                            Cropping;
  int                            Shade;
  int                            VRQuality;

/*   //BTX */
/*   std::vector<vtkDataSet*>      DataSetList; */
/*   std::vector<vtkActor*>        DataSetActorList; */
/*   //ETX */

  
};
  


