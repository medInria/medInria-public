#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkInriaExport.h>

#include <vtkImageView.h>
#include <vtkImageView3DCroppingBoxCallback.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkOrientedBoxWidget.h>
#include <vtkPlaneWidget.h>
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
class vtkProp3DCollection;

/**
   \class vtkImageView3D vtkImageView3D.h "vtkImageView3D.h"
   \brief 3D image 3D rendering viewer
   \author Pierre Fillard & Marc Traina & Nicolas Toussaint

   This class allows to view 3D images. Images have to be
   vtkImageData.
   volume rendering and multiplane reconstructions are provided
   remote plan can also be used, so can be an orientation cube, ...
*/

class MEDVTKINRIA_EXPORT vtkImageView3D : public vtkImageView
{
public:

    static vtkImageView3D* New();
    vtkTypeMacro(vtkImageView3D, vtkImageView);

    vtkMTimeType GetMTime() override;

    // Rendering Modes available.
    // PLANAR_RENDERING will render every vtkImageActor instance added with Add2DPhantom()
    // whereas VOLUME_RENDERING will render the volume added with SetInputData().
    //BTX
    enum RenderingModeIds
    {
        VOLUME_RENDERING = 0,
        PLANAR_RENDERING
    };
    //ETX

    vtkGetObjectMacro (VolumeActor, vtkVolume);
    vtkGetObjectMacro (VolumeProperty, vtkVolumeProperty);
    vtkGetObjectMacro (PlaneWidget, vtkPlaneWidget);
    vtkGetObjectMacro (BoxWidget, vtkOrientedBoxWidget);
    vtkGetObjectMacro (ActorX, vtkImageActor);
    vtkGetObjectMacro (ActorY, vtkImageActor);
    vtkGetObjectMacro (ActorZ, vtkImageActor);
    vtkGetObjectMacro (ExtraPlaneCollection, vtkProp3DCollection);

    medVtkImageInfo* GetMedVtkImageInfo(int layer = 0) const override;

    virtual void SetVolumeMapperToRayCast();
#ifdef MED_USE_OSPRAY_4_VR_BY_CPU
    virtual void SetVolumeMapperToOSPRayRenderMode();
#endif //MED_USE_OSPRAY_4_VR_BY_CPU
    virtual void SetVolumeMapperToGPU();
    virtual void SetVolumeMapperToDefault();

    virtual void SetVolumeRayCastFunctionToComposite();
    virtual void SetVolumeRayCastFunctionToMaximumIntensityProjection();
    virtual void SetVolumeRayCastFunctionToMinimumIntensityProjection();


    virtual void SetInterpolationToNearestNeighbor();
    virtual void SetInterpolationToLinear();

    void SetShowBoxWidget (int a);
    bool GetShowBoxWidget();
    vtkBooleanMacro (ShowBoxWidget, int);

    void SetShowPlaneWidget (int a);
    bool GetShowPlaneWidget();
    vtkBooleanMacro (ShowPlaneWidget, int);

    void SetShowCube (int a);
    bool GetShowCube();
    vtkBooleanMacro (ShowCube, int);

    void SetShade (int a);
    bool GetShade();
    vtkBooleanMacro (Shade, int);

    /** Set the rendering mode to volume rendering (VR). */
    virtual void SetRenderingModeToVR()
    {this->SetRenderingMode (VOLUME_RENDERING); }

    /** Set the rendering mode to planar views. */
    virtual void SetRenderingModeToPlanar()
    { this->SetRenderingMode (PLANAR_RENDERING); }


    virtual void SetRenderingMode (int mode);
    /** Get the current rendering mode. */
    vtkGetMacro (RenderingMode, int);

    // Cropping Modes available.
    // CROPPING_INSIDE will crop inside the image
    // whereas CROPPING_OUTSIDE will crop outside.
    enum CroppingModeIds
    {
        CROPPING_OFF     = 0,
        CROPPING_INSIDE  = 1,
        CROPPING_OUTSIDE = 2
    };

    virtual void SetCroppingModeToOff();
    virtual void SetCroppingModeToInside();
    virtual void SetCroppingModeToOutside();
    virtual void SetCroppingMode(unsigned int);
    virtual unsigned int GetCroppingMode ();

    void SetInput      (vtkAlgorithmOutput* pi_poVtkAlgoOutput, vtkMatrix4x4 *matrix = nullptr, int layer = 0) override;
    virtual bool data2DTreatment();
    static vtkSmartPointer<vtkActor> DataSetToActor(vtkPointSet* arg, vtkProperty* prop = nullptr);
    virtual void SetInputLayer (vtkAlgorithmOutput* pi_poVtkAlgoOutput, vtkMatrix4x4 *matrix = nullptr, int layer = 0);
    void SetFirstLayer(vtkAlgorithmOutput *pi_poInputAlgoImg, vtkMatrix4x4 *matrix= nullptr, int layer = 0);

    void SetOrientationMatrix (vtkMatrix4x4* matrix) override;

    using vtkImageView::SetColorWindow;
    void SetColorWindow(double s,int layer) override;

    using vtkImageView::SetColorLevel;
    void SetColorLevel(double s,int layer) override;


    void SetLookupTable (vtkLookupTable* lookuptable, int layer) override;

    void SetTransferFunctions(vtkColorTransferFunction * color,
                                      vtkPiecewiseFunction * opacity,
                                      int layer) override;

    virtual void SetOpacity(double opacity, int layer);
    virtual double GetOpacity(int layer) const;

    virtual void SetVisibility(int visibility, int layer);
    virtual int  GetVisibility(int layer) const;

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


    void SetCurrentPoint (double pos[3]) override;

    virtual void UpdateDisplayExtent();

    void InstallInteractor() override;
    void UnInstallInteractor() override;

    vtkActor* AddDataSet (vtkPointSet* arg, vtkProperty* prop = nullptr) override;
    void RemoveDataSet (vtkPointSet* arg) override;

    void AddLayer (int layer) override;
    int GetNumberOfLayers() const override;
    void RemoveLayer (int layer) override;
    void RemoveAllLayers() override;

    //pure virtual methods from base class:
    vtkColorTransferFunction * GetColorTransferFunction(int layer) const override;
    vtkPiecewiseFunction* GetOpacityTransferFunction (int layer) const override;
    void StoreColorTransferFunction (vtkColorTransferFunction *ctf,
                                             int layer) override;
    void StoreOpacityTransferFunction (vtkPiecewiseFunction *otf,
                                               int layer) override;
    vtkLookupTable * GetLookupTable(int layer) const override;
    bool GetUseLookupTable(int layer) const override;
    void SetUseLookupTable (bool use, int layer) override;
    using vtkImageView::GetColorWindow;
    double GetColorWindow(int layer) const override;
    void StoreColorWindow(double s,int layer) override;
    using vtkImageView::GetColorLevel;
    double GetColorLevel(int layer) const override;
    void StoreColorLevel(double s,int layer) override;
    void StoreLookupTable (vtkLookupTable *lookuptable, int layer) override;

protected:

    vtkImageView3D();
    ~vtkImageView3D();

    virtual void InstallPipeline();
    virtual void UnInstallPipeline();

    virtual void SetupVolumeRendering();
    virtual void SetupWidgets();
    virtual void UpdateVolumeFunctions(int layer);
    void ApplyColorTransferFunction(vtkScalarsToColors *, int) override;
    virtual void InternalUpdate();

    vtkImage3DDisplay * GetImage3DDisplayForLayer(int layer) const;

    // plane actors
    vtkImageActor* ActorX;
    vtkImageActor* ActorY;
    vtkImageActor* ActorZ;

    vtkVolumeProperty* VolumeProperty;
    vtkVolume* VolumeActor;

    vtkSmartVolumeMapper* VolumeMapper;

    vtkImageView3DCroppingBoxCallback* Callback;

    vtkOrientedBoxWidget* BoxWidget;
    vtkPlaneWidget* PlaneWidget;

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

    int LastNodeIndex;

    int  CroppingMode;

    double Opacity;
    int Visibility;

    struct LayerInfo {
        vtkSmartPointer<vtkImage3DDisplay> ImageDisplay;
    };

    vtkSmartPointer<vtkImageMapToColors>        PlanarWindowLevelX;
    vtkSmartPointer<vtkImageMapToColors>        PlanarWindowLevelY;
    vtkSmartPointer<vtkImageMapToColors>        PlanarWindowLevelZ;
    // Layer indices start from 0, as they do for the 2d case
    //(API change)
    typedef std::vector<LayerInfo > LayerInfoVecType;
    LayerInfoVecType LayerInfoVec;

private:
    vtkImageView3D(const vtkImageView3D&);  // Not implemented.
    void operator=(const vtkImageView3D&);    // Not implemented.
	
    void  initializeTransferFunctions(int pi_iLayer);
};

