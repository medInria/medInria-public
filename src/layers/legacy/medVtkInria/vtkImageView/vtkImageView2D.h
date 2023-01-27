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
#include <medVtkImageInfo.h>

#include <vtkImageView.h>
#include <vtkInteractorStyleImageView2D.h>
#include <vtkSmartPointer.h>

#include <vector>

#include <QObject>

class vtkImageActor;
class vtkAxes2DWidget;
class vtkRulerWidget;
class vtkAngleWidget;
class vtkDistanceWidget;
class vtkDataSet2DWidget;
class vtkOrientationAnnotation;
class vtkPointHandleRepresentation2D;
class vtkCursor2D;
class vtkPlane;
class vtkImageView2DCommand;
class vtkTransform;
class vtkPolyData;
class vtkImage2DDisplay;
class vtkImageAlgorithm;

/**

 \class vtkViewImage2D
 \brief This class is a top-level convenience class for displaying a scalar - or RGB
 image in a 3D scene, as well as some other objects.


 \ingroup AdvancedRendering
 \see vtkViewImage vtkViewImageCollection vtkViewImage3D
 \author Pierre Fillard and Nicolas Toussaint, INRIA
 */

/**
 Notes on Nicolas Toussaint changes


 A) SLICE_ORIENTATION enum
 the slice orientation enum has changed to match VTK: XY / XZ / YZ
 The axis enum is therefore of no need.

 B) The ImageToColor instance has been moved to ImageView (see vtkImageView.h)

 C) Orientation and Convention systems have been put in place,
 thus replacing the DirectionMatrix system

 D) The Zoom / Pan differentiation: is it needed ?

 E) again here Visibility --> Show

 F) The command / style and events.
 Well, I put A LOT of time and efforts in optimizing the ImageView2DCommand so that
 it fully match VTK requirements, simplifying a lot the readability and clearness
 of the overall code. So I put it back.
 One thing remains though. Pierre differentiated the Zoom event from the Pan event,
 thus authorizing sync. of one and not the other. In the system I propose there is
 no differentiation, they are gathered in the CameraMove event. To be discussed.

 G) All mouse interactions have thus been put in place.

 H) The UpdateOrientation() and PostUpdateOrientation() have been re-installed,
 as well as the SetAnnotationsFromOrientation(), adding the cool ImageViewCornerAnnotation
 instance from Pierre.

 I) SetSlice() now handles the CurrentPoint change, so that this instance is always
 up to date.

 J) still some work to do on the annotations
 */
class MEDVTKINRIA_EXPORT vtkImageView2DQtSignals : public QObject
{
    Q_OBJECT
public:
    vtkImageView2DQtSignals() {}
    ~vtkImageView2DQtSignals() {}

    void emitInterpolate(bool pi_bInterpolation, int pi_iLayer){emit interpolate(pi_bInterpolation, pi_iLayer);}
    void emitSliceChanged(int slice, int sliceOrientation){emit sliceChanged(slice, sliceOrientation);}

signals:
    void interpolate(bool, int);
    void sliceChanged(int, int);

private:
   
};

class MEDVTKINRIA_EXPORT vtkImageView2D : public vtkImageView
{
public:
    static vtkImageView2D *New();
    vtkTypeMacro (vtkImageView2D, vtkImageView)
    void PrintSelf(ostream& os, vtkIndent indent);

    vtkMTimeType GetMTime();

    //BTX
    enum EventIds
    {
        OrientationChangedEvent = vtkCommand::UserEvent+600,
        SliceChangedEvent
    };
    //ETX

    virtual void SetVisibility(int visible, int layer);
    virtual int  GetVisibility(int layer) const;

    virtual void   SetOpacity(double opacity, int layer);
    virtual double GetOpacity(int layer) const;

    // Description:
    // Set/Get the input image to the viewer.
    virtual void SetInput      (vtkAlgorithmOutput* pi_poVtkAlgoOutput, vtkMatrix4x4 *matrix = nullptr, int layer = 0);
    virtual void SetInputLayer (vtkAlgorithmOutput* pi_poVtkAlgoOutput, vtkMatrix4x4 *matrix = nullptr, int layer = 0);
    virtual void SetInputCommon(vtkAlgorithmOutput* pi_poVtkAlgoOutput, int layer = 0);
    virtual void SetInput (vtkActor *actor, int layer = 0, vtkMatrix4x4 *matrix = nullptr,
                           const int imageSize[3] = nullptr, const double imageSpacing[] = nullptr, const double imageOrigin[] = nullptr);

    void RemoveLayerActor(vtkActor *actor, int layer = 0);

    virtual vtkActor* AddDataSet (vtkPointSet* arg, vtkProperty* prop = nullptr);
    virtual void RemoveDataSet (vtkPointSet *arg);

    medVtkImageInfo* GetMedVtkImageInfo(int layer = 0) const;

    virtual void InstallInteractor();
    virtual void UnInstallInteractor();

    virtual void StoreLookupTable (vtkLookupTable *lookuptable, int layer);

    /**
   Description:
   The orientation of the view is an abstract representation of the object
   we are looking at. It results from the acquisition plane. Setting the View
   Orientation by calling SetViewOrientation() will imply the view to set its
   inner "slice" orientation. (slice orientation == 2 means plane of acquisition.)

   IMPORTANT NOTE:

   The view orientations defined here are orthogonal to the normal basis
   in the scanner. A very interesting improvement would be to define "oblique"
   view orientations for cardiac imaging, something like:

   VIEW_ORIENTATION_SHORT_AXIS, VIEW_ORIENTATION_LONG_AXIS, and VIEW_ORIENTATION_FOUR_CHAMBER
   could define the different views that are usually used in cardiac imaging.

   From this user-input information, the idea would be to evaluate which slice
   orientation does correspond to the requested view. This can be done by evaluating the
   dot product between the axis of acquisition and a pre-defined oblique axis that
   "should" correspond to the requested orientation...
   **/
    //BTX
    enum
    {
        VIEW_ORIENTATION_SAGITTAL = 0,
        VIEW_ORIENTATION_CORONAL = 1,
        VIEW_ORIENTATION_AXIAL = 2
    };
    //ETX

    //BTX
    enum
    {
        VIEW_CONVENTION_RADIOLOGICAL = 0,
        VIEW_CONVENTION_NEUROLOGICAL = 1
    };

    // Description:
    // Set/get the slice orientation
    //BTX
    enum
    {
        SLICE_ORIENTATION_YZ = 0,
        SLICE_ORIENTATION_XZ = 1,
        SLICE_ORIENTATION_XY = 2
    };
    //ETX
    //BTX
    enum AnnotationStyleIds
    {
        AnnotationStyle1 = 0,
        AnnotationStyle2
    };
    //ETX


    vtkImageActor *GetImageActor(int layer=0) const;
    /**
     Return the SlicePlane vtkPolyData instance. This polydata consists on 4 points linked
     as a square, placed at the corners of the ImageActor. It can be used to follow the position
     of the slice in space.
    */
    vtkGetObjectMacro (SlicePlane, vtkPolyData);

    vtkGetMacro(SliceOrientation, int);
    virtual void SetSliceOrientation(int orientation);
    virtual void SetSliceOrientationToXY()
    { this->SetSliceOrientation(vtkImageView2D::SLICE_ORIENTATION_XY); };
    virtual void SetSliceOrientationToYZ()
    { this->SetSliceOrientation(vtkImageView2D::SLICE_ORIENTATION_YZ); };
    virtual void SetSliceOrientationToXZ()
    { this->SetSliceOrientation(vtkImageView2D::SLICE_ORIENTATION_XZ); };


    virtual int  GetSlice() const;
    virtual void SetSlice(int s);

    virtual void SetCurrentPoint (double pos[3]);

    virtual void UpdateDisplayExtent();


    virtual int GetSliceMin() const;
    virtual int GetSliceMax() const;
    virtual void GetSliceRange(int range[2]) const
    { this->GetSliceRange(range[0], range[1]); }
    virtual void GetSliceRange(int &min, int &max) const;
    virtual int* GetSliceRange() const;

    /**
   Instead of setting the slice orientation to an axis (YZ - XZ - XY),
   you can force the view to be axial (foot-head), coronal (front-back),
   or sagittal (left-right). It will just use the OrientationMatrix
   (GetOrientationMatrix()) to check which slice orientation to pick.
   */
    vtkGetMacro (ViewOrientation, int);
    virtual void SetViewOrientation (int orientation);
    virtual void SetOrientationMatrix (vtkMatrix4x4* matrix);

    /**
   The ViewConvention instance explains where to place the camera around
   the patient. Default behaviour is Radiological convention, meaning
   we respectively look at the patient from his feet, his face and his left ear.

   For Neurological convention, we respectively look from the top of his head,
   the the back of his head, and his left ear.
   */
    ///\todo Why not adding cardiologic conventions where we look at the patient in oblique angles ?
    vtkGetMacro (ViewConvention, int)
    virtual void SetViewConvention (int convention);

    virtual void GetWorldCoordinatesForSlice (int slice, double* position);
    virtual int GetSliceForWorldCoordinates(double pos[3]) const;


    virtual void SetPan(double* arg);
    virtual double* GetPan();
    virtual void GetPan(double pan[2]);

    using vtkImageView::ResetCamera;

    virtual void Reset();
    virtual void ResetCamera() override;

    virtual void GetWorldCoordinatesFromDisplayPosition (int xy[2], double* position);

    virtual vtkImageMapToColors *GetWindowLevel(int layer=0) const;

    using vtkImageView::Get2DDisplayMapperInputAlgorithm;
    virtual vtkAlgorithm* Get2DDisplayMapperInputAlgorithm(int layer) const;
    virtual vtkImageAlgorithm* GetInputAlgorithm(int layer) const;

    //BTX
    /**
   Access to the command of the viewer.
   This instance is in charge of observing the interactorstyle (GetInteractorStyle())
   and update things accordingly in the view (i.e. the slice number when moving slice).
   */
    vtkGetObjectMacro (Command, vtkImageView2DCommand)
    //ETX

    virtual void SetInterpolate (int val, int layer=0);
    virtual int  GetInterpolate (int layer = 0) const;
    vtkBooleanMacro (Interpolate, int)


    virtual void SetShowAnnotations (int);

    /**
   Access to the position of the center of the view.
   CAUTION: for the moment this feature is de-activated because updating it
   slows down the visualization process.
   */
    vtkGetVector3Macro (ViewCenter, double);


    void SetLeftButtonInteractionStyle (int arg);
    int GetLeftButtonInteractionStyle();

    void SetKeyboardInteractionStyle (int arg);
    int GetKeyboardInteractionStyle();

    void SetRightButtonInteractionStyle (int arg);
    int GetRightButtonInteractionStyle();

    void SetMiddleButtonInteractionStyle (int arg);
    int GetMiddleButtonInteractionStyle();

    void SetWheelInteractionStyle (int arg);
    int GetWheelInteractionStyle();

    void SetInteractionStyle (int arg);
    int GetInteractionStyle();

    virtual void SetShowRulerWidget (int);
    vtkGetMacro (ShowRulerWidget, int);
    vtkBooleanMacro (ShowRulerWidget, int);


    virtual void SetShowImageAxis (int);
    vtkGetMacro (ShowImageAxis, int);
    vtkBooleanMacro (ShowImageAxis, int);


    virtual void SetShowDistanceWidget (int);
    vtkGetMacro (ShowDistanceWidget, int);
    vtkBooleanMacro (ShowDistanceWidget, int);


    virtual void SetShowAngleWidget (int);
    vtkGetMacro (ShowAngleWidget, int);
    vtkBooleanMacro (ShowAngleWidget, int);


    virtual void SetCursorFollowMouse (int val);
    vtkBooleanMacro (CursorFollowMouse, int);
    vtkGetMacro (CursorFollowMouse, int);

    void SetAnnotationStyle (unsigned int arg);
    vtkGetMacro (AnnotationStyle, unsigned int)
    unsigned int GetAnnotationStyle2D();

    virtual void AddLayer(int);
    virtual void RemoveLayer(int layer);
    virtual void RemoveAllLayers();
    virtual int GetNumberOfLayers() const;

    using vtkImageView::GetColorLevel;
    virtual double GetColorLevel(int layer) const;
    using vtkImageView::GetColorWindow;
    virtual double GetColorWindow(int layer)const;
    virtual void SetTransferFunctionRangeFromWindowSettings(int layer);

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
    virtual void StoreColorWindow(double s,int layer);
    virtual void StoreColorLevel(double s,int layer);

    virtual void UpdateBounds (const double bounds[6], int layer = 0, vtkMatrix4x4 *matrix = 0, const int imageSize[3] = 0, const double imageSpacing[] = 0,
                             const double imageOrigin[] = 0);

    virtual vtkRenderer * GetRenderer() const;
    vtkImageAlgorithm * GetImageAlgorithmForLayer(int layer) const;

protected:
    vtkImageView2D();
    ~vtkImageView2D();

    void ApplyColorTransferFunction(vtkScalarsToColors * colors, int layer) override;

    virtual void UpdateSlicePlane();
    virtual void UpdateCenter();
    virtual void UpdateOrientation();

    virtual void SetSlicePlaneFromOrientation();
    virtual int GetViewOrientationFromSliceOrientation(int sliceorientation, double* cam_pos = 0, double* cam_focus = 0);

    virtual int  SetCameraFromOrientation();
    virtual void SetAnnotationsFromOrientation();

    virtual void InitializeSlicePlane();

    virtual void InstallPipeline();
    virtual void UnInstallPipeline();

    /**
   The ViewConvention instance explains where to place the camera around
   the patient. Default behaviour is Radiological convention, meaning
   we respectively look at the patient from his feet, his face and his left ear.

   For Neurological convention, we respectively look from the top of his head,
   the the back of his head, and his left ear.

   The ConventionMatrix is here to materialize this concept with a simple
   4x4 matrix where

   (1) from the 3x3 matrix, each column vector is the conventional viewup to use
   for the camera for each slice orientation.

   (2) The 4th column vector represent the conventional position of the camera.
   */
    vtkMatrix4x4* ConventionMatrix;
    /**
   This polydata instance is a square colored (see InitializeSlicePlane()) according to the
   orientation of the view. It follows the image actor and is used by other view to display
   intersections between views.
   */
    vtkPolyData* SlicePlane;
    /**
   Get the orientation annotation. This annotation describes the orientation
   of the slice plane, according to the rule
   Right(R)-Left(L) Anterior(A)-Posterior(P) Inferior(I)-Superior(S)
   */
    vtkOrientationAnnotation* OrientationAnnotation;
    /**
   Access to the command of the viewer.
   This instance is in charge of observing the interactorstyle (GetInteractorStyle())
   and update things accordingly in the view (i.e. the slice number when moving slice).
   */
    vtkImageView2DCommand* Command;

    // Get layer specific renderer.
    vtkImage2DDisplay * GetImage2DDisplayForLayer(int layer) const;
    vtkRenderer * GetRendererForLayer(int layer) const;

    //BTX
    std::list<vtkDataSet2DWidget*>::iterator FindDataSetWidget(vtkPointSet* arg);
    //ETX

    void SetFirstLayer(vtkAlgorithmOutput *pi_poInputAlgoImg, vtkMatrix4x4 *matrix, int layer);
    bool IsFirstLayer(int layer) const;
    int GetFirstLayer() const;

    int ViewOrientation;
    int ViewConvention;
    int    SliceOrientation;
    double InitialParallelScale;
    int ShowRulerWidget;
    int ShowDistanceWidget;
    int ShowAngleWidget;
    int ShowImageAxis;

    unsigned int AnnotationStyle;

    std::string matrixOrientation[3][2];

    int CursorFollowMouse;
    int Slice;
    double Pan[2];
    double ViewCenter[3];
    int CurrentLayer;

    vtkRulerWidget     *RulerWidget;
    vtkAxes2DWidget    *Axes2DWidget;
    vtkDistanceWidget  *DistanceWidget;
    vtkAngleWidget     *AngleWidget;

    std::list<vtkDataSet2DWidget*> DataSetWidgets;

    struct LayerInfo {
        vtkSmartPointer<vtkImage2DDisplay> ImageDisplay;
        vtkSmartPointer<vtkImageAlgorithm> ImageAlgo;
        vtkSmartPointer<vtkRenderer> Renderer;
    };
    typedef std::vector<LayerInfo > LayerInfoVecType;
    LayerInfoVecType LayerInfoVec;

private:
    vtkImageView2D(const vtkImageView2D&);  // Not implemented.
    void operator=(const vtkImageView2D&);    // Not implemented.

public:
    vtkImageView2DQtSignals *qtSignalHandler;
};
