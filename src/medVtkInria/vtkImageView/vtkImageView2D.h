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
#include <vtkInteractorStyleImageView2D.h>

#include <vtkSmartPointer.h>

#include <vector>

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
 the slice orientation enum has changed to match VTK : XY / XZ / YZ
 The axis enum is therefore of no need.

 B) The ImageToColor instance has been moved to ImageView (see vtkImageView.h)

 C) Orientation and Convention systems have been put in place,
 thus replacing the DirectionMatrix system

 D) The Zoom / Pan differentiation : is it needed ?

 E) again here Visibility --> Show

 F) The command / style and events.
 Well, I put A LOT of time and efforts in optimizing the ImageView2DCommand so that
 it fully match VTK requirements, simplifying a lot the readability and clearness
 of the overall code. So I put it back.
 One thing remains though. Pierre differentiated the Zoom event from the Pan event,
 thus authorizing sync. of one and not the other. In the system I propose there is
 no differentiation, they are gathered in the CameraMove event. to be discussed.

 G) All mouse interactions have thus been put in place.

 H) The UpdateOrientation() and PostUpdateOrientation() have been re-installed,
 as well as the SetAnnotationsFromOrientation(), adding the cool ImageViewCornerAnnotation
 instance from Pierre.

 I) SetSlice() now handles the CurrentPoint change, so that this instance is always
 up to date.

 J) still some work to do on the annotations
 */

class MEDVTKINRIA_EXPORT vtkImageView2D : public vtkImageView
{
public:
  static vtkImageView2D *New();
  vtkTypeRevisionMacro (vtkImageView2D, vtkImageView);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Override vtkObject - return the maximum mtime of this and any objects owned by this.
  unsigned long GetMTime();

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
  virtual void SetInput (vtkImageData *image,
                         vtkMatrix4x4 *matrix = 0,
                         int layer=0);
  virtual void SetInput (vtkActor *actor, int layer = 0, vtkMatrix4x4 *matrix = 0,
                         const int imageSize[3] = 0, const double imageSpacing[] = 0, const double imageOrigin[] = 0);

  virtual void SetInputConnection (vtkAlgorithmOutput* arg,
                                   vtkMatrix4x4 *matrix = 0,
                                   int layer=0);

    void RemoveLayerActor(vtkActor *actor, int layer = 0);
    
  int AddInput (vtkImageData *image, vtkMatrix4x4 *matrix);

  vtkImageData *GetImageInput(int layer) const;
  vtkImageData *GetInput(int layer = 0) const;

  // Description:
  // Start/Stop the interactor relation with the view.
  // it basically plug or unplug the interactor.
  virtual void InstallInteractor();
  virtual void UnInstallInteractor();

//  /**
//   * This function is equivalent to setTransferFunctions(color, opacity, 0)
//   */
//  virtual void SetTransferFunctions (vtkColorTransferFunction* color,
//                                     vtkPiecewiseFunction *opacity);

//  virtual void SetTransferFunctions (vtkColorTransferFunction* color,
//                                     vtkPiecewiseFunction *opacity,
//                                     int layer);
//  virtual void SetLookupTable (vtkLookupTable* lookuptable, int layer);
  virtual void StoreLookupTable (vtkLookupTable *lookuptable, int layer);

  /**
   Description:
   The orientation of the view is a abstract representation of the object
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
  /**
   Description:
   **/
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

  /**
   Return the vtkImageActor's instance.
   */
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

  // Description:
  // Set/Get the current slice to display (depending on the orientation
  // this can be in X, Y or Z).
  virtual int  GetSlice() const;
  virtual void SetSlice(int s);
  /**
   The wolrd is not always what we think it is ...

   Use this method to move the viewer slice such that the position
   (in world coordinates) given by the arguments is contained by
   the slice plane. If the given position is outside the bounds
   of the image, then the slice will be as close as possible.
   */
  virtual void SetCurrentPoint (double pos[3]);

  // Description:
  // Update the display extent manually so that the proper slice for the
  // given orientation is displayed. It will also try to set a
  // reasonable camera clipping range.
  // This method is called automatically when the Input is changed, but
  // most of the time the input of this class is likely to remain the same,
  // i.e. connected to the output of a filter, or an image reader. When the
  // input of this filter or reader itself is changed, an error message might
  // be displayed since the current display extent is probably outside
  // the new whole extent. Calling this method will ensure that the display
  // extent is reset properly.
  virtual void UpdateDisplayExtent();

  // Description:
  // Return the minimum and maximum slice values (depending on the orientation
  // this can be in X, Y or Z).
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
  vtkGetMacro (ViewConvention, int);
  virtual void SetViewConvention (int convention);
  /**
   Convert an indices coordinate point (image coordinates) into a world coordinate point
   */
  virtual void GetWorldCoordinatesForSlice (int slice, double* position);
  /**
   Convert a world coordinate point into an image indices coordinate point
   */
  virtual int GetSliceForWorldCoordinates(double pos[3]) const;
  /**
   Reset position - zoom - window/level to default
   */
  virtual void Reset();
  /**
   Get/Set the zoom factor of the view
   */
  //virtual void SetZoom(double arg); // already in vtkImageView
  //virtual double GetZoom(); // already in vtkImageView
  /**
   Get/Set the pan factor of the view
   */
  virtual void SetPan(double* arg);
  virtual double* GetPan();
  virtual void GetPan(double pan[2]);

  using vtkImageView::ResetCamera;
  /**
   Reset the camera in a nice way for the 2D view
   */
  virtual void ResetCamera() override;
  /**
   Useful method that transform a display position into a world corrdinate point
   */
  virtual void GetWorldCoordinatesFromDisplayPosition (int xy[2], double* position);

  //! Get the WindowLevel for given layer. Overrides Superclass.
  virtual vtkImageMapToColors *GetWindowLevel(int layer=0) const;

  //BTX
  /**
   Access to the command of the viewer.
   This instance is in charge of observing the interactorstyle (GetInteractorStyle())
   and update things accordingly in the view (i.e. the slice number when moving slice).
   */
  vtkGetObjectMacro (Command, vtkImageView2DCommand);
  //ETX
  /**
     Get/Set weither or not the interpolation between pixels should be activated.
     It is On by default
  */
  virtual void SetInterpolate (int val, int layer=0);
  virtual int  GetInterpolate (int layer = 0) const;
  vtkBooleanMacro (Interpolate, int);

  /**
     Show/Hide the annotations (CornerAnnotation AND OrientationAnnotation)
  */
  virtual void SetShowAnnotations (int);

  /**
   Access to the position of the center of the view.
   CAUTION: for the moment this feature is de-activated because updating it
   slows down the visualization process.
   */
  vtkGetVector3Macro (ViewCenter, double);

  /**
   Change the interaction triggered by the mouse buttons.
   Choices are listed in vtkInteractorStyleImageView2D class:
   InteractionTypeSlice : changes the slice number.
   InteractionTypeWindowLevel : changes the window-level values.
   InteractionTypeZoom : changes the zoom level.
   InteractionTypePan : translate the view in-plane.
   */
  void SetLeftButtonInteractionStyle (int arg)
  {
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
      t->SetLeftButtonInteraction (arg);
  }
  int GetLeftButtonInteractionStyle()
  {
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
      return t->GetLeftButtonInteraction ();
    return -1;
  }

  /**
   Change the interaction triggered by the keyboard.
   Choices are listed in vtkInteractorStyleImageView2D class:
   InteractionTypeSlice : changes the slice number.
   InteractionTypeWindowLevel : changes the window-level values.
   InteractionTypeZoom : changes the zoom level.
   InteractionTypePan : translate the view in-plane.
  */
  void SetKeyboardInteractionStyle (int arg)
  {
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
      t->SetKeyboardInteraction (arg);
  }
  int GetKeyboardInteractionStyle()
  {
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
      return t->GetKeyboardInteraction ();
    return -1;
  }

  /**
   Change the interaction triggered by the mouse buttons.
   Choices are listed in vtkInteractorStyleImageView2D class:
   InteractionTypeSlice : changes the slice number.
   InteractionTypeWindowLevel : changes the window-level values.
   InteractionTypeZoom : changes the zoom level.
   InteractionTypePan : translate the view in-plane.
   */
  void SetRightButtonInteractionStyle (int arg)
  {
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
      t->SetRightButtonInteraction (arg);
  }
  int GetRightButtonInteractionStyle()
  {
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
      return t->GetRightButtonInteraction ();
    return -1;
  }
  /**
   Change the interaction triggered by the mouse buttons.
   Choices are listed in vtkInteractorStyleImageView2D class:
   InteractionTypeSlice : changes the slice number.
   InteractionTypeWindowLevel : changes the window-level values.
   InteractionTypeZoom : changes the zoom level.
   InteractionTypePan : translate the view in-plane.
   */
  void SetMiddleButtonInteractionStyle (int arg)
  {
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
      t->SetMiddleButtonInteraction (arg);
  }
  int GetMiddleButtonInteractionStyle()
  {
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
      return t->GetMiddleButtonInteraction ();
    return -1;
  }
  /**
   Change the interaction triggered by the mouse buttons.
   Choices are listed in vtkInteractorStyleImageView2D class:
   InteractionTypeSlice : changes the slice number.
   InteractionTypeWindowLevel : changes the window-level values.
   InteractionTypeZoom : changes the zoom level.
   InteractionTypePan : translate the view in-plane.
   */
  void SetWheelInteractionStyle (int arg)
  {
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
      t->SetWheelButtonInteraction (arg);
  }
  int GetWheelInteractionStyle()
  {
    vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
      return t->GetWheelButtonInteraction ();
    return -1;
  }

  /**
   Change the interaction triggered by the mouse buttons.
   Choices are listed in vtkInteractorStyleImageView2D class:
   InteractionTypeSlice : changes the slice number.
   InteractionTypeWindowLevel : changes the window-level values.
   InteractionTypeZoom : changes the zoom level.
   InteractionTypePan : translate the view in-plane.
   */
  void SetInteractionStyle (int arg)
  {
    this->SetLeftButtonInteractionStyle (arg);
    this->SetRightButtonInteractionStyle (arg);
    this->SetMiddleButtonInteractionStyle (arg);
    this->SetWheelInteractionStyle (arg);
  }
  /**
   Change the interaction triggered by the mouse buttons.
   Choices are listed in vtkInteractorStyleImageView2D class:
   InteractionTypeSlice : changes the slice number.
   InteractionTypeWindowLevel : changes the window-level values.
   InteractionTypeZoom : changes the zoom level.
   InteractionTypePan : translate the view in-plane.
   */
  int GetInteractionStyle()
  { vtkInteractorStyleImageView2D* t = vtkInteractorStyleImageView2D::SafeDownCast (this->InteractorStyle);
    if (t)
      return t->GetLeftButtonInteraction();
    else
      return 0;
  }

  /**
   Set/Get the ruler widget visibility.
   */
  virtual void SetShowRulerWidget (int);
  vtkGetMacro (ShowRulerWidget, int);
  vtkBooleanMacro (ShowRulerWidget, int);

  /**
   Set/Get the axes2D widget visibility.
   */
  virtual void SetShowImageAxis (int);
  vtkGetMacro (ShowImageAxis, int);
  vtkBooleanMacro (ShowImageAxis, int);

  /**
   Set/Get the ruler widget visibility.
   */
  virtual void SetShowDistanceWidget (int);
  vtkGetMacro (ShowDistanceWidget, int);
  vtkBooleanMacro (ShowDistanceWidget, int);

  /**
   Set/Get the ruler widget visibility.
   */
  virtual void SetShowAngleWidget (int);
  vtkGetMacro (ShowAngleWidget, int);
  vtkBooleanMacro (ShowAngleWidget, int);

  /**
   Set/Get whether the cursor should follow the mouse or not.
   */
  virtual void SetCursorFollowMouse (int val);
  vtkBooleanMacro (CursorFollowMouse, int);
  vtkGetMacro (CursorFollowMouse, int);
  /**
     Switch between annotation style.
     AnnotationStyle1 is there by default, it shows the X-Y position (in-plane),
     and specify the orientation on lower-right corner
     AnnotationStyle1 shows the global physical x-y-z position (scanner coordinates),
     and leaves lower-right corner empty for user to fill.
  */
  void SetAnnotationStyle (unsigned int arg);
  vtkGetMacro (AnnotationStyle, unsigned int);
  /**
     adding a PointSet to the view.
     The method will "cut" the dataset, therefore reducing the dimension by 1.
     e.g. volumes become surfaces, surfaces lines, and lines points.
  */
  virtual vtkActor* AddDataSet (vtkPointSet* arg, vtkProperty* prop = NULL);
  virtual void RemoveDataSet (vtkPointSet *arg);

  virtual void AddLayer(int);
  virtual void RemoveLayer(int layer);
  virtual void RemoveAllLayers();
  virtual int GetNumberOfLayers() const;

  using vtkImageView::GetColorLevel;
  virtual double GetColorLevel(int layer) const;
  using vtkImageView::GetColorWindow;
  virtual double GetColorWindow(int layer)const;
//  virtual void SetTransferFunctionRangeFromWindowSettings();
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

  virtual void UpdateBounds (const double bounds[6], int layer = 0, const int imageSize[3] = 0, const double imageSpacing[] = 0,
                             const double imageOrigin[] = 0);

  virtual vtkRenderer * GetRenderer() const;

protected:
  vtkImageView2D();
  ~vtkImageView2D();

  virtual void ApplyColorTransferFunction(vtkScalarsToColors * colors,
                                                  int layer);

  /**
   The SlicePlane instance (GetSlicePlane()) is the polygonal
   square corresponding to the slice plane,
   it is color-coded according to conventions.

   UpdateSlicePlane() is thus called each time we change slice
   or change orientation.
   */
  virtual void UpdateSlicePlane();
  /**
   The ViewCenter instance follows the center of the view
   in world coordinates. It is updated UpdateCenter() each
   time the slice or the orientation changes.

   CAUTION: for the moment it is de-activated to speed up the
   visualization. (The ViewCenter is not used anywhere else).
   */
  virtual void UpdateCenter();
  /**
   After the orientation has changed, it is crucial to adapt
   a couple of things according to new orientation.
   In UpdateOrientation() the SlicePlane, the Camera settings,
   the CornerAnnotation are modified.
   */
  virtual void UpdateOrientation();
  /**
   After the orientation has changed, it is crucial to adapt
   a couple of things according to new orientation.
   In UpdateOrientation() the SlicePlane, the Camera settings,
   the CornerAnnotation are modified.
   */
  virtual void SetSlicePlaneFromOrientation();
  /**
   Returns the estimated View-Orientation corresponding
   to a given Slice-Orientation, and outputs the camera's parameters
   This is crucial when choosing to impose the View-Orientation
   instead of imposing the Slice-Orientation.
   */
  virtual int GetViewOrientationFromSliceOrientation(int sliceorientation, double* cam_pos = 0, double* cam_focus = 0);
  /**
   After the orientation has changed, it is crucial to adapt
   a couple of things according to new orientation.
   In UpdateOrientation() the SlicePlane, the Camera settings,
   the CornerAnnotation are modified.
   */
  virtual int  SetCameraFromOrientation();
  /**
   After the orientation has changed, it is crucial to adapt
   a couple of things according to new orientation.
   In UpdateOrientation() the SlicePlane, the Camera settings,
   the CornerAnnotation are modified.
   */
  virtual void SetAnnotationsFromOrientation();
  /**
   This method is called each time the orientation changes (SetViewOrientation())
   and sets the appropriate color to the slice plane.

   Red: R-L direction --> sagittal orientation

   Green: A-P direction --> coronal orientation

   Blue: I-S direction --> axial orientation
   */
  virtual void InitializeSlicePlane();
  /**
   Overwrite of the Superclass InstallPipeline() method in order to set up the
   home made InteractorStyle, and make it observe all events we need
   */
  virtual void InstallPipeline();
  /**
   Overwrite of the Superclass UnInstallPipeline() method in order to set up the
   home made InteractorStyle, and make it observe all events we need
   */
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

  void SetFirstLayer (vtkImageData *image, vtkMatrix4x4 *matrix, int layer);
  bool IsFirstLayer(int layer) const;
  int GetFirstLayer() const;

  /**
   local instances
   */
  int ViewOrientation;
  int ViewConvention;
  int    SliceOrientation;
  double InitialParallelScale;
  int ShowRulerWidget;
  int ShowDistanceWidget;
  int ShowAngleWidget;
  int ShowImageAxis;

  unsigned int AnnotationStyle;

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
      vtkSmartPointer<vtkRenderer> Renderer;
  };
  typedef std::vector<LayerInfo > LayerInfoVecType;
  LayerInfoVecType LayerInfoVec;

private:
  vtkImageView2D(const vtkImageView2D&);  // Not implemented.
  void operator=(const vtkImageView2D&);    // Not implemented.

};



