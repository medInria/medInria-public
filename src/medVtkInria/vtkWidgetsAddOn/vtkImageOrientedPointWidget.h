/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medVtkInriaExport.h"

#include "vtk3DWidget.h"

class vtkAbstractPropPicker;
class vtkActor;
class vtkCellArray;
class vtkCellPicker;
class vtkFloatArray;
class vtkGlyphSource2D;
class vtkPoints;
class vtkPolyData;
class vtkProp;
class vtkProperty;
class vtkPropPicker;
class vtkTransform;
class vtkTransformPolyDataFilter;

#define VTK_ITW_PROJECTION_YZ 0
#define VTK_ITW_PROJECTION_XZ 1
#define VTK_ITW_PROJECTION_XY 2
#define VTK_ITW_SNAP_CELLS    0
#define VTK_ITW_SNAP_POINTS   1



/**
   \class vtkImageOrientedPointWidget vtkImageOrientedPointWidget.h "vtkImageOrientedPointWidget.h"
   \brief 3D widget for registering purposes
   \author Nicolas Toussaint
   This class can be used in order to recover user set point positions (and directions)
   in a 2d view
*/

class MEDVTKINRIA_EXPORT vtkImageOrientedPointWidget : public vtk3DWidget
{
public:
  // Description:
  // Instantiate the object.
  static vtkImageOrientedPointWidget *New();
  vtkTypeRevisionMacro(vtkImageOrientedPointWidget,vtk3DWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Methods that satisfy the superclass' API.
  virtual void SetEnabled(int);
  virtual void PlaceWidget(double bounds[6]);
  void PlaceWidget()
    {this->Superclass::PlaceWidget();}
  void PlaceWidget(double xmin, double xmax, double ymin, double ymax,
                   double zmin, double zmax)
    {this->Superclass::PlaceWidget(xmin,xmax,ymin,ymax,zmin,zmax);}

  // Description:
  // Set/Get the handle properties (the 2D glyphs are the handles). The
  // properties of the handles when selected and normal can be manipulated.
  virtual void SetHandleProperty1(vtkProperty*);
  vtkGetObjectMacro(HandleProperty1, vtkProperty);
  virtual void SetHandleProperty2(vtkProperty*);
  vtkGetObjectMacro(HandleProperty2, vtkProperty);
  virtual void SetSelectedHandleProperty(vtkProperty*);
  vtkGetObjectMacro(SelectedHandleProperty, vtkProperty);

  // Description:
  // Set/Get the line properties. The properties of the line when selected
  // and unselected can be manipulated.
  virtual void SetLineProperty(vtkProperty*);
  vtkGetObjectMacro(LineProperty, vtkProperty);
  virtual void SetSelectedLineProperty(vtkProperty*);
  vtkGetObjectMacro(SelectedLineProperty, vtkProperty);

  // Description:
  // Set the prop, usually a vtkImageActor, to trace over.
  void SetViewProp(vtkProp* prop);

  // Description:
  // Force handles to be on a specific ortho plane.
  vtkSetMacro(ProjectToPlane,int);
  vtkGetMacro(ProjectToPlane,int);
  vtkBooleanMacro(ProjectToPlane,int);

  // Description:
  // Set the projection normal.  The normal in SetProjectionNormal is 0,1,2
  // for YZ,XZ,XY planes respectively.  Since the handles are 2D glyphs, it is
  // necessary to specify a plane on which to generate them, even though
  // ProjectToPlane may be turned off.
  vtkSetClampMacro(ProjectionNormal,int,VTK_ITW_PROJECTION_YZ,VTK_ITW_PROJECTION_XY);
  vtkGetMacro(ProjectionNormal,int);
  void SetProjectionNormalToXAxes()
    { this->SetProjectionNormal(0); }
  void SetProjectionNormalToYAxes()
    { this->SetProjectionNormal(1); }
  void SetProjectionNormalToZAxes()
    { this->SetProjectionNormal(2); }

  // Description:
  // Set the position of the widgets' handles in terms of a plane's position.
  // e.g., if ProjectionNormal is 0, all of the x-coordinate values of the
  // handles are set to ProjectionPosition.  No attempt is made to ensure that
  // the position is within the bounds of either the underlying image data or
  // the prop on which tracing is performed.
  void SetProjectionPosition(double position);
  vtkGetMacro(ProjectionPosition,double);

   // Description:
  // Force snapping to image data while tracing.
  void SetSnapToImage(int snap);
  vtkGetMacro(SnapToImage,int);
  vtkBooleanMacro(SnapToImage,int);

  // Description:
  // In concert with a CaptureRadius value, automatically
  // form a closed path by connecting first to last path points.
  vtkSetMacro(AutoClose,int);
  vtkGetMacro(AutoClose,int);
  vtkBooleanMacro(AutoClose,int);

  // Description:
  // Set/Get the capture radius for automatic path closing.  For image
  // data, capture radius should be half the distance between voxel/pixel
  // centers.
  vtkSetMacro(CaptureRadius,double);
  vtkGetMacro(CaptureRadius,double);

  // Description:
  // Grab the points and lines that define the traced path. These point values
  // are guaranteed to be up-to-date when either the InteractionEvent or
  // EndInteraction events are invoked. The user provides the vtkPolyData and
  // the points and cells representing the line are added to it.
  void GetPath(vtkPolyData *pd);

  // Description:
  // Get the handles' geometric representation via vtkGlyphSource2D.
  vtkGlyphSource2D* GetGlyphSource() { return this->HandleGenerator1; }

  // Description:
  // Set/Get the type of snapping to image data: center of a pixel/voxel or
  // nearest point defining a pixel/voxel.
  vtkSetClampMacro(ImageSnapType,int,VTK_ITW_SNAP_CELLS,VTK_ITW_SNAP_POINTS);
  vtkGetMacro(ImageSnapType,int);

  // Description:
  // Set/Get the handle position in terms of a zero-based array of handles.
  void SetHandlePosition(int handle, double xyz[3]);
  void SetHandlePosition(int handle, double x, double y, double z);
  void GetHandlePosition(int handle, double xyz[3]);
  double* GetHandlePosition(int handle);

  // Description:
  // Get the number of handles.
  vtkGetMacro(NumberOfHandles,int);

  // Description:
  // Enable/disable mouse interaction when the widget is visible.
  void SetInteraction(int interact);
  vtkGetMacro(Interaction,int);
  vtkBooleanMacro(Interaction,int);

  // Description:
  // Initialize the widget with a set of points and generate
  // lines between them.  If AutoClose is on it will handle the
  // case wherein the first and last points are congruent.
  void InitializeHandles(vtkPoints*);

  // Description:
  // Is the path closed or open?
  int IsClosed();

#ifdef VTK_WORKAROUND_WINDOWS_MANGLE
# define SetPropA SetProp
# define SetPropW SetProp
#endif

  // Description:
  // @deprecated Replaced by vtkImageOrientedPointWidget::SetViewProp() as of VTK 5.0.
  VTK_LEGACY(void SetProp(vtkProp* prop));

#ifdef VTK_WORKAROUND_WINDOWS_MANGLE
# undef SetPropA
# undef SetPropW
  //BTX
  VTK_LEGACY(void SetPropA(vtkProp*));
  VTK_LEGACY(void SetPropW(vtkProp*));
  //ETX
#endif

protected:
  vtkImageOrientedPointWidget();
  ~vtkImageOrientedPointWidget();

//BTX - manage the state of the widget
  int State;
  enum WidgetState
  {
    Start=0,
    //    Tracing,
    Snapping,
    Erasing,
    //    Inserting,
    Moving,
    Translating,
    Outside
  };
//ETX

  //handles the events
  static void ProcessEvents(vtkObject* object,
                            unsigned long event,
                            void* clientdata,
                            void* calldata);

  // ProcessEvents() dispatches to these methods.
  void OnLeftButtonDown();
  void OnLeftButtonUp();
  void OnMiddleButtonDown();
  void OnMiddleButtonUp();
  void OnRightButtonDown();
  void OnRightButtonUp();
  void OnMouseMove();

  void AddObservers();

  // Controlling ivars
  int    Interaction;
  int    ProjectionNormal;
  double ProjectionPosition;
  int    ProjectToPlane;
  int    ImageSnapType;
  int    SnapToImage;
  double CaptureRadius; // tolerance for auto path close
  int    AutoClose;
  int    IsSnapping;
  int    LastX;
  int    LastY;

  void  Trace(int , int );
  void  Snap(double* );
  void  MovePoint(const double* , const double* );
  void  Translate(const double* , const double* );
  void  ClosePath();

  // 2D glyphs representing hot spots (e.g., handles)
  vtkActor          **Handle;
  vtkPolyData       **HandleGeometry;
  vtkGlyphSource2D   *HandleGenerator1;
  vtkGlyphSource2D   *HandleGenerator2;

  // Transforms required as 2D glyphs are generated in the x-y plane
  vtkTransformPolyDataFilter *TransformFilter1;
  vtkTransformPolyDataFilter *TransformFilter2;
  vtkTransform               *Transform;
  vtkFloatArray              *TemporaryHandlePoints;

  void AppendHandles(double*);
  void ResetHandles();
  void AllocateHandles(const int& );
  void AdjustHandlePosition(const int& , double*);
  int  HighlightHandle(vtkProp* ); // returns handle index or -1 on fail
  void EraseHandle(const int& );
  virtual void SizeHandles();
  void InsertHandleOnLine(double* );

  int NumberOfHandles;
  vtkActor *CurrentHandle;
  int CurrentHandleIndex;

  vtkProp       *ViewProp;    // the prop we want to pick on
  vtkPropPicker *PropPicker;  // the prop's picker

  // Representation of the line
  vtkPoints         *LinePoints;
  vtkCellArray      *LineCells;
  vtkActor          *LineActor;
  vtkPolyData       *LineData;
  vtkIdType          CurrentPoints[2];

  void HighlightLine(const int& );
  void BuildLinesFromHandles();
  void ResetLine(double* );
  void AppendLine(double* );
  int  PickCount;

  // Do the picking of the handles and the lines
  vtkCellPicker *HandlePicker;
  vtkCellPicker *LinePicker;
  vtkAbstractPropPicker* CurrentPicker;

  // Properties used to control the appearance of selected objects and
  // the manipulator in general.
  vtkProperty *HandleProperty1;
  vtkProperty *HandleProperty2;
  vtkProperty *SelectedHandleProperty;
  vtkProperty *LineProperty;
  vtkProperty *SelectedLineProperty;
  void CreateDefaultProperties();

private:
  vtkImageOrientedPointWidget(const vtkImageOrientedPointWidget&);  //Not implemented
  void operator=(const vtkImageOrientedPointWidget&);  //Not implemented
};


