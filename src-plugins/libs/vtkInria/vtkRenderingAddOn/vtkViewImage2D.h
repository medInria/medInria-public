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

#include <iostream>
#include <vtkRenderingAddOn/vtkViewImage.h>

class vtkActor;
class vtkAlgorithmOutput;
class vtkBox;
class vtkBoxCutPolyLines;
class vtkColorTransferFunction;
class vtkColorTransferFunction;
class vtkCutter;
class vtkCutter;
class vtkDataSet;
class vtkDataSetMapper;
class vtkImageActor;
class vtkImageBlend;
class vtkImageBlendWithMask;
class vtkImageMapToColors;
class vtkImageMapToWindowLevelColors;
class vtkImageReslice;
class vtkInteractorStyleImage2D;
class vtkLineSource;
class vtkLookupTable;
class vtkMatrix4x4;
class vtkPlane;
class vtkPolyDataMapper;
class vtkProp;
class vtkProperty;
class vtkScalarsToColors;
class vtkTextMapper;
class vtkTransform;
class vtkRulerWidget;
class vtkDistanceWidget;

/**
   \class vtkViewImage2D vtkViewImage2D.h "vtkViewImage2D.h"
   \brief 2D synchronized viewer of a 3D image.
   \author Pierre Fillard & Marc Traina & Nicolas Toussaint.

   This class allows to view 2D orthogonal slices (namely axial, sagittal and coronal) of a 3D image.
   This class implements lots of features like: various possible interactions (positioning, zooming,
   windowing), coloring  of the slice w.r.t. a lookup table, overlapping of a second image,
   overlapping of a mask image (image of labels). Display conventions can be chosen between radiologic
   (the left of the patient appears on the right of the screen) and neurologic (left is left and right
   is right).

   It uses a synchronization mechanisms between other views. Views are linked together using a cycle
   graph structure (implemented in the base class \class vtkSynchronizedView). All interactions are
   passed to the other Views, which in turn pass them to their Children, and so on. Thus, zooming,
   positioning, etc. are synchronized among different views (views can display other orientation of
   the image).

   See Examples/SynchronizedViews/SynchronizedViews for a working example of how to use this class.

*/


class VTK_RENDERINGADDON_EXPORT vtkViewImage2D : public vtkViewImage
{
  
 public:

  static vtkViewImage2D* New();
  vtkTypeRevisionMacro(vtkViewImage2D, vtkViewImage);


  //BTX
  enum InteractionStyleIds {
    NO_INTERACTION,
    SELECT_INTERACTION,
    WINDOW_LEVEL_INTERACTION,
    FULL_PAGE_INTERACTION,
    MEASURE_INTERACTION,
    ZOOM_INTERACTION
  };
  //ETX

  //BTX
  enum ConventionIds{
    RADIOLOGIC,
    NEUROLOGIC
  };
  //ETX  

  
  void PrintSelf(ostream& os, vtkIndent indent);

  
  /**
     Show/hide the indication label about the displayed slice.
   */
  virtual void SetShowSliceNumber(int p_showSliceNumber);
  vtkBooleanMacro (ShowSliceNumber, int);
  vtkGetMacro (ShowSliceNumber, int);

  
  /**
     Show/hide the 2D axes.
   */
  virtual void SetShow2DAxis(int);
  vtkBooleanMacro (Show2DAxis, int);
  vtkGetMacro (Show2DAxis, int);
  

  /**
     Set the image to display.
  */
  virtual void SetImage(vtkImageData* image);


  /**
     Set a transform to the internal image reslicer.
  */
  virtual void SetTransform(vtkTransform* p_transform);

  
  /**
     Apply a user matrix to the image actor. The boolean centered tells to
     apply the matrix w.r.t. the image center (useful for rotations).
  */
  virtual void SetMatrix(vtkMatrix4x4* p_matrix, bool centered = false);

  
  /**
     Set/Get the orientation of the image.
     @param orientation 0 is XY (axial), 1 is YZ (sagittal), 2 is XZ (coronal).
  */
  virtual void SetOrientation(unsigned int orientation);
  vtkGetMacro (Orientation, unsigned int);


  /**
     Get the id of the first slice available relative to the current orientation.
     Returns 0 no matter what.
  */
  virtual int GetWholeZMin();

  
  /**
     Get the id of the last slice available relative to the current orientation.
  */
  virtual int GetWholeZMax();

  
   /**
      Get the current slice. Z is relative to the view orientation.
   */
  virtual int GetZSlice();

  
  /**
     Set the current slice. Z is relative to the view orientation.
  */
  virtual void SetZSlice(int p_slice);

  
  /**
     Set/Get window/level for mapping pixels to colors.
  */
  virtual double GetColorWindow();

  
  /**
     Set/Get window/level for mapping pixels to colors.
  */
  virtual double GetColorLevel();

  
  /**
     Set/Get window/level for mapping pixels to colors.
  */
  virtual void SetWindow (double);

  
  /**
     Set/Get window/level for mapping pixels to colors.
  */
  virtual void SetLevel (double);

  
  
  
  /**
     Set/Get the interpolation method (1: linear, 0: nearest neighbor).
  */
  virtual void SetInterpolationMode(int i);

  
  /**
     Set/Get the interpolation method (1: linear, 0: nearest neighbor).
  */
  virtual int GetInterpolationMode();

  
  /**
     Set/Get the type of user interaction type for the left button of the mouse.
  */
  vtkSetMacro (LeftButtonInteractionStyle, unsigned int);
  vtkGetMacro (LeftButtonInteractionStyle, unsigned int);

  
  /**
     Set/Get the type of user interaction type for the right button of the mouse.
  */
  vtkSetMacro (RightButtonInteractionStyle, unsigned int);
  vtkGetMacro (RightButtonInteractionStyle, unsigned int);

  
  /**
     Set/Get the type of user interaction type for the middle button of the mouse.
  */
  vtkSetMacro (MiddleButtonInteractionStyle, unsigned int);
  vtkGetMacro (MiddleButtonInteractionStyle, unsigned int);

  /**
     Set/Get the type of user interaction type for the wheel of the mouse.
  */
  vtkSetMacro (WheelInteractionStyle, unsigned int);
  vtkGetMacro (WheelInteractionStyle, unsigned int);

  
  /**
     Set/Get the type of user interaction to all mouse buttons at once.
  */
  virtual void SetInteractionStyle (const unsigned int& style)
  {
    if (this->GetLeftButtonInteractionStyle() == this->InteractionStyle)
      this->SetLeftButtonInteractionStyle   (style);
    if (this->GetMiddleButtonInteractionStyle() == this->InteractionStyle)
      this->SetMiddleButtonInteractionStyle (style);
    if (this->GetRightButtonInteractionStyle() == this->InteractionStyle)
      this->SetRightButtonInteractionStyle  (style);
    if (this->GetWheelInteractionStyle() == this->InteractionStyle)
      this->SetWheelInteractionStyle        (style);
    this->InteractionStyle = style;
  }
  vtkGetMacro (InteractionStyle, unsigned int);


  /**
     Set/Get whether the cursor should follow the mouse or not.
   */
  vtkSetMacro (CursorFollowMouse, int);
  vtkBooleanMacro (CursorFollowMouse, int);
  vtkGetMacro (CursorFollowMouse, int);
  

  /**
     Set/Get the ruler widget visibility.
   */
  virtual void SetRulerWidgetVisibility (int);
  vtkBooleanMacro (RulerWidgetVisibility, int);
  vtkGetMacro (RulerWidgetVisibility, int);


  /**
     Get the distance widget instance.
   */
  vtkGetObjectMacro (DistanceWidget, vtkDistanceWidget);


  /**
     Set/Get the distance widget visibility.
   */
  virtual void SetDistanceWidgetVisibility (int);
  vtkBooleanMacro (DistanceWidgetVisibility, int);
  vtkGetMacro (DistanceWidgetVisibility, int);

  
  /**
     Get the ruler widget instance.
  */
  vtkGetObjectMacro (RulerWidget, vtkRulerWidget);

  
  /**
     Set an overlapping mask image with its corresponding LookupTable. A label image MUST
     be of type unsigned char. Labels are mapped through colors using the vtkLookupTable.
     For instance, label 1 (i.e. pixels whose value is 1) will be mapped throught index 1
     (table value 1) of the vtkLookupTable.
  */
  virtual void SetMaskImage (vtkImageData* mask, vtkLookupTable* lut);

  
  /**
     Remove the mask image (if any).
  */
  virtual void RemoveMaskImage();

  
  /**
     Set an overlapping second image, which will appear by transparency on top of the
      initial image.
  */
  virtual void SetOverlappingImage (vtkImageData* image);


  /**
     Set/Get the overlapping image opacity
   */
  virtual void SetOverlappingImageOpacity (double val);
  virtual double GetOverlappingImageOpacity();
  
  
  /**
     Remove the overlapping image (if any).
  */
  virtual void RemoveOverlappingImage();

  
  /**
     Set/Get a user-defined lookup table to color the image with.
  */
  virtual void SetLookupTable (vtkScalarsToColors* lut);


  /**
     Set/Get a user-defined lookup table to color the background image with.
  */
  virtual void SetBGLookupTable (vtkScalarsToColors* lut);
  virtual vtkScalarsToColors* GetBGLookupTable() const;

  
  /**
    Update the position of the image. Convenient method to make sure that
    the displayed point it up to date.
  */
  virtual void UpdatePosition();

  
  /**
     Show/hide the image.
  */
  virtual void SetVisibility (int state);

  
  /**
     Set/Get the opacity of the principale ImageActor
  */
  virtual void SetOpacity(double opacity);
  virtual double GetOpacity() const;
  
  
  /**
     Add a dataset to the view.
     The dataset will be display as an intersection of it with the displayed image slice,
     i.e., tetrahedron will be displayed as triangles, triangles as lines, lines as points.
     A vtkProperty can be specified to specify the freshly generated actor properties.
  */
  virtual vtkActor* AddDataSet (vtkDataSet* dataset, vtkProperty* property = NULL);

  
  /**
     This method is similar to AddDataSet(), except it only works with vtkPolyData.
     It cuts the polydata using 2 planes defined by the current image slice being
     visualized and a user-provided thickness (generally the spacing between 2 consecutive
     slices). For instance, it works nicely with lines: AddDataSet() turns lines into points
     while AddPolyData() displays lines as short segments.
  */
  virtual vtkActor* AddPolyData (vtkPolyData* polydata,  vtkProperty* property = NULL, double thickness = 0.2);
  virtual vtkActor* SyncAddPolyData (vtkPolyData* polydata,  vtkProperty* property = NULL, double thickness = 0.2);

  
  /**
     Set the actual zoom factor of the view.
  */
  virtual void SetZoom (double factor);

  
  /**
     Reset the zoom factor of the view to its initial value and center the camera.
  */
  virtual void ResetZoom();

  
  /**
     Call ResetZoom() and restablish the zoom factor and the camera position to the current state.
     The call to ResetZoom() is sometimes usefull to force VTK to update the bounding box of all
     objects in the scene so that everything becomes visible.
   */
  virtual void ResetAndRestablishZoomAndCamera();

  
  /**
     Switch between radiological (left is right and right is left) and
     neurological (left is left and right is right) conventions.
  */
  virtual void SetConventionsToRadiological();

  
  /**
     Switch between radiological (left is right and right is left) and
     neurological (left is left and right is right) conventions.
  */
  virtual void SetConventionsToNeurological();

  
  /**
     Switch between radiological (left is right and right is left) and
     neurological (left is left and right is right) conventions.
  */
  virtual void SetConventions (int);
  vtkGetMacro (Conventions, unsigned int);


  /**
     Global functions to Set/Get the display conventions. Useful to change the display conventions of all view
     by calling only once vtkViewImage2D::SetViewImage2DDisplayConventions(0/1).
  */
  static void SetViewImage2DDisplayConventions(int val);
  static void SetViewImage2DDisplayConventionsToRadiologic() {vtkViewImage2D::SetViewImage2DDisplayConventions(0);};
  static void SetViewImage2DDisplayConventionsToNeurologic() {vtkViewImage2D::SetViewImage2DDisplayConventions(1);};
  static int  GetViewImage2DDisplayConventions();

  /**
     Add an imagebackground image. You can apply to the image actor a transformation transform, and choose a slice if it is a volume.
  */
  virtual void SetBGImage (vtkImageData* image, int slice = 0, vtkTransform* transform = NULL);
  vtkGetObjectMacro (BGImage, vtkImageData);

  
  /**
     Remove the background image (if any).
  */
  virtual void RemoveBGImage();

  
  /**
     Set the opacity of the background Actor
  */
  virtual void SetBGImageOpacity(double opacity);
  double GetBGImageOpacity() const;


  /**
     A complete reset: a classical reset and also cancel all the transformations.
  */
  virtual void Clear();

  
  /**
     Transform screen coordinates (as returned by any vtk picker listening to the mouse
     press event) into real world coordinates. Makes use of the OrientationMatrix and
     the ScreenToRealWorldMatrix.
  */
  virtual void ScreenCoordinatesToPosition (double sc[2], double pos[3]);



  
  // access methods to internal classes
  

  /**
     Return the vtkImageActor of the view.
  */
  vtkGetObjectMacro (ImageActor, vtkImageActor);

  
  /**
     Return the vtkImageActor of the background image.
  */
  vtkGetObjectMacro (BGActor, vtkImageActor);

  
  /**
     Return a pointer to the instance of vtkImageMapToColors of the View.
  */
  vtkGetObjectMacro (WindowLevel, vtkImageMapToColors);

  
  /**
     Return a pointer to the instance of vtkImageReslice of the View.
  */
  vtkGetObjectMacro (ImageReslice, vtkImageReslice);


  /**
     Reslice matrices are default matrices defining the axes for reslice depending
     on the chosen orientation:
     - Axial: X -Y
     - Sagittal: Y Z
     - Coronal: X Z
     You can feed any vtkImageReslice with those matrices to obtain an Axial, Sagittal
     or Coronal slice.

     Internally, those matrices are used with the DirectionMatrix: this user-defined
     matrix tells how the image is stored (axis permutations and flips may exist, we
     must take them into account). The final reslice matrix will be given by the
     product: this->DirectionMatrix * this->Axial/Sagittal/CoronalResliceMatrix.
   */
  vtkGetObjectMacro (AxialResliceMatrix, vtkMatrix4x4);
  vtkGetObjectMacro (SagittalResliceMatrix, vtkMatrix4x4);
  vtkGetObjectMacro (CoronalResliceMatrix, vtkMatrix4x4);


  /**
     Overridden from vtkViewImage::SetDirectionMatrix: tell the view to update the
     reslice axes when DirectionMatrix has changed.
   */
  virtual void SetDirectionMatrix (vtkMatrix4x4*);

  
  /**
     Mainly for internal use, SliceNavigation is used to infer the way we go through
     slices. For instance, if there is a flip in the Z-axis, navigation through Axial
     slices will be inverted.
   */
  vtkGetMacro (SliceNavigation, int);

	
  void SyncSetCameraFocalAndPosition (double focal[3], double pos[3]);

  void SyncSetCameraFocalAndPosition (double focal[3], double pos[3], unsigned int orientation);

	
  /**
     This function is called right after setting both Renderer and RenderWindow.
     It allows a class to add actors for instance without knowing when the Renderer
     and RenderWindow are set. For example, vtkSynchronizedView will add the corner annotations
     during the call to the Initialize function.
  */
  virtual void Initialize();

  
  /**
     Remove any actor set by the class into the Renderer. Remove any added dataset.
   */
  virtual void Uninitialize();


  virtual void InitializeInteractor();
  virtual void UninitializeInteractor();

  
 protected:
  vtkViewImage2D();
  ~vtkViewImage2D();


  /**
     Initialize rendering parameters only once. m_FirstRender controls if the
     initialization has been done or not.
  */
  virtual void InitializeImagePositionAndSize();

  
  /**
     Draw the annotations. For internal use only.
  */
  //virtual void SetupAnnotations();

  /**
     Get the ScreenToRealWorldMatrix. For internal use only. It does not correspond to
     the projection matrix of the view.
   */
  vtkGetObjectMacro (ScreenToRealWorldMatrix, vtkMatrix4x4);

  
  /**
     Get the OrientationMatrix. For internal use only.
   */
  vtkGetObjectMacro (OrientationMatrix, vtkMatrix4x4);


  /**
     Update the internal pipeline (slice extraction, coloring, etc.) and reset the position (place the cursor on the center pixel),
     zooming (zoom factor of 1.0), and windowing. It also re-computes the clipping range and other camera related parameters.
     It is mainly for internal used should in general not be used. See Reset() if you only want to reset position, zoom and
     window/level.
  */
  virtual void InternalUpdate();


  virtual void SetupAnnotations();

  vtkSetMacro (SliceNavigation, int);

 private:

  // Update data information and set the update and display extents.
  // Reset the camera position.
  void           UpdateImageActor();


  /// internal use: image to input to the mask filter or image blender
  vtkImageData*                   AuxInput;

  /// internal use: image to input to the reslicer
  vtkImageData*                   ResliceInput;

  /// to map the image to the current window level
  vtkImageMapToWindowLevelColors* WindowLevelForCorner;
  vtkImageMapToColors*            WindowLevel;

  /// vtkImageReslice is the swiss-army-knife of image geometry filters: It can
  /// permute, rotate, flip, scale, resample, deform, and pad image data in any
  /// combination with reasonably high efficiency.
  vtkImageReslice*                ImageReslice;

  // mask filter
  vtkImageBlendWithMask*          MaskFilter;

  // image blender
  vtkImageBlend*                  Blender;

  /// actors and mappers:
  vtkImageActor*                  ImageActor;
  vtkActor*                       HorizontalLineActor;
  vtkLineSource*                  HorizontalLineSource;
  vtkActor*                       VerticalLineActor;
  vtkLineSource*                  VerticalLineSource;

  vtkScalarsToColors*             BGLookupTable;

  int                            FirstRender;
  int                            ShowSliceNumber;
  int                            Show2DAxis;
  int                            RulerWidgetVisibility;
  int                            DistanceWidgetVisibility;
  int                            CursorFollowMouse;
  
  /// Orientation: 0 is XY, 1 is ZY, 2 is XZ
  unsigned int                    Orientation;
  unsigned int                    ViewOrientation;
  unsigned int                    InteractionStyle;


  vtkPlane*                     DataSetCutPlane;
  vtkBox*                       DataSetCutBox;
  double                        BoxThickness;

  unsigned int LeftButtonInteractionStyle;
  unsigned int RightButtonInteractionStyle;
  unsigned int MiddleButtonInteractionStyle;
  unsigned int WheelInteractionStyle;


  double       InitialParallelScale;

  unsigned int Conventions;

  /// background image
  vtkImageData*                  BGImage;
  vtkImageActor *                BGActor;
  vtkImageMapToColors*           BGWindowLevel;


  vtkMatrix4x4 *OrientationMatrix;
  vtkMatrix4x4 *ScreenToRealWorldMatrix;

  vtkMatrix4x4 *AxialResliceMatrix;
  vtkMatrix4x4 *SagittalResliceMatrix;
  vtkMatrix4x4 *CoronalResliceMatrix;

  vtkRulerWidget    *RulerWidget;
  vtkDistanceWidget *DistanceWidget;

  int                SliceNavigation;

};


