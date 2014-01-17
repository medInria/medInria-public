/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkImageViewExport.h"

#include <vtkCollection.h>
#include <vtkCommand.h>
#include <vtkLookupTable.h>

#include <vtkImageView2D.h>


/**
   This macro can be useful as we frequently set
   an instance this way, meaning unregistering previously set instance,
   registering given instance, and call a modified event.
   However this might be not the best place to define it...
*/
//BTX
#define vtkSyncSetMacro(name, type)					\
  inline virtual void SyncSet##name (type _arg, vtkImageView* caller=0, int checkForInteractor=0)	\
  {									\
    this->InitTraversal();						\
    vtkImageView* item = this->GetNextItem();				\
    while(item)								\
    {									\
      if( item!=caller && (!checkForInteractor || (checkForInteractor&&item->GetIsInteractorInstalled())) )							\
	item->Set##name (_arg);						\
      item = this->GetNextItem();					\
    }									\
  }
//ETX

//BTX
#define vtkSyncSet2DMacro(name, type)					\
  inline virtual void SyncSet##name (type _arg, vtkImageView *caller=0, int checkForInteractor=0)	\
  {									\
    this->InitTraversal();						\
    vtkImageView* item = this->GetNextItem();				\
    while(item)								\
    {									\
      if(item!=caller && (!checkForInteractor || (checkForInteractor&&item->GetIsInteractorInstalled())) ) {						\
	vtkImageView2D* item2D = vtkImageView2D::SafeDownCast (item);	\
	if (item2D)							\
	  item2D->Set##name (_arg);					\
      }									\
      item = this->GetNextItem();					\
    }									\
  }
//ETX

//BTX
#define vtkSyncSetObjectMacro(name, type)				\
  inline virtual void SyncSet##name (type* _arg, vtkImageView *caller=0, int checkForInteractor=0)	\
  {									\
    this->InitTraversal();						\
    vtkImageView* item = this->GetNextItem();				\
    while(item)								\
    {									\
      if(item!=caller && (!checkForInteractor || (checkForInteractor&&item->GetIsInteractorInstalled())) )							\
	item->Set##name (_arg);						\
      item = this->GetNextItem();					\
    }									\
  }
//ETX


class vtkScalarBarActor;
class vtkLookupTable;
class vtkColorTransferFunction;
class vtkPiecewiseFunction;
class vtkTextProperty;
class vtkCornerAnnotation;
class vtkOrientationAnnotation;
class vtkPlane;
class vtkActor;
class vtkDataSet;
class vtkPointSet;
class vtkPolyData;
class vtkProperty;
class vtkProp3DCollection;
class vtkDataSetCollection;
class vtkMatrixToLinearTransform;
class vtkImageViewCollectionCommand;



/**
   Notes on Nicolas Toussaint changes

   A) All linking systems have been put back in place here.

   B) we still have to discuss the best solution for axes VS cursor thing

   C)

   
*/
class VTK_IMAGEVIEW_EXPORT vtkImageViewCollection : public vtkCollection
{

 public:

  static vtkImageViewCollection* New();
  vtkTypeRevisionMacro(vtkImageViewCollection, vtkCollection);


  // Description:
  // Get the next vtkImageView in the list. Return NULL when at the end of the
  // list.
  vtkImageView *GetNextItem() {
    return static_cast<vtkImageView *>(this->GetNextItemAsObject());};
  // Description:
  // Get the next vtkImageView in the list. Return NULL when at the end of the
  // list.
  vtkImageView *GetItem(int i) {
    return static_cast<vtkImageView *>(this->GetItemAsObject(i));};
  // Description:
  // Add an object to the list. Does not prevent duplicate entries.
  void AddItem(vtkImageView*);

  // Description:
  // Replace the i'th item in the collection with a
  void ReplaceItem(int i, vtkImageView *);

  // Description:
  // Remove the i'th item in the list.
  // Be careful if using this function during traversal of the list using
  // GetNextItemAsObject (or GetNextItem in derived class).  The list WILL
  // be shortened if a valid index is given!  If this->Current is equal to the
  // element being removed, have it point to then next element in the list.
  void RemoveItem(int i);

  // Description:
  // Remove an object from the list. Removes the first object found, not
  // all occurrences. If no object found, list is unaffected.  See warning
  // in description of RemoveItem(int).
  void RemoveItem(vtkImageView *);

  // Description:
  // Remove all objects from the list.
  void RemoveAllItems();

  // Description:
  // Show the intersections between slices (SlicePlanes).
  vtkGetMacro     (ShowSlicePlanes, unsigned int);
  void SetShowSlicePlanes (unsigned int arg);
  vtkBooleanMacro (ShowSlicePlanes, unsigned int);
  
  //BTX
  vtkGetObjectMacro (Command, vtkImageViewCollectionCommand);
  //ETX

  // All methods below use the macro definition (top) to implement the
  // synchronization between the different view items present in this collection.

  /// Description: Synchronize show annotations between views
  vtkSyncSetMacro (ShowAnnotations, int);
  /// Description: Synchronize show scalar bar between views
  vtkSyncSetMacro (ShowScalarBar, int);
  /// Description: Synchronize use lookup table between views
  // vtkSyncSetMacro (UseLookupTable, int);
  
  /// Description: Synchronize color window between views
  vtkSyncSetMacro (ColorWindow, double);
  /// Description: Synchronize color level between views
  vtkSyncSetMacro (ColorLevel, double);
  /// Description: Synchronize color window and level given by range
  /// between views
  vtkSyncSetObjectMacro (ColorRange, double);

  /// Description: Synchronize orientation matrix between views
  vtkSyncSetObjectMacro (OrientationMatrix, vtkMatrix4x4);
  /// Description: Synchronize lookup table between views
  vtkSyncSetObjectMacro (LookupTable, vtkLookupTable);
  /// Description: Synchronize color transfer function between views
  vtkSyncSetObjectMacro (ColorTransferFunction, vtkColorTransferFunction);
  /// Description: Synchronize opacity transfer function between views
  vtkSyncSetObjectMacro (OpacityTransferFunction, vtkPiecewiseFunction);
  /// Description: Synchronize annotation text property between views
  vtkSyncSetObjectMacro (TextProperty, vtkTextProperty);
  /// Description: Synchronize image input between views
  vtkSyncSetObjectMacro (Input, vtkImageData);
  /// Description: Synchronize image input between views
  vtkSyncSetObjectMacro (InputConnection, vtkAlgorithmOutput);

  /// Description: Synchronize wondpw position between views
  vtkSyncSetObjectMacro (Position, int);
  /// Description: Synchronize wondpw position between views
  vtkSyncSetObjectMacro (Size, int);
  /// Description: Synchronize world coordinates between views
  vtkSyncSetObjectMacro (CurrentPoint, double);
  /// Description: Synchronize background color between views
  vtkSyncSetObjectMacro (Background, double);
  /// Description: Synchronize camera position between views
  vtkSyncSetObjectMacro (CameraPosition, double);
  /// Description: Synchronize camera focal point between views
  vtkSyncSetObjectMacro (CameraFocalPoint, double);
  /// Description: Synchronize camera view up between views
  vtkSyncSetObjectMacro (CameraViewUp, double);
  /// Description: Synchronize camera parallel scale between views
  vtkSyncSetMacro (CameraParallelScale, double);
  /// Description: Synchronize zoom between views
  vtkSyncSetMacro (Zoom, double);
  /// Description: Synchronize slice between views
  vtkSyncSet2DMacro (Slice, int);
  /// Description: Synchronize time between views
  vtkSyncSetMacro (TimeIndex, int);
  /// Description: Synchronize slice orientation between views (also see SyncSetViewOrientation())
  vtkSyncSet2DMacro (SliceOrientation, int);
  /// Description: Synchronize view orientation between views (also see SyncSetSliceOrientation())
  vtkSyncSet2DMacro (ViewOrientation, int);
  /// Description: Synchronize annotation style
  vtkSyncSet2DMacro (AnnotationStyle, int);
  /// Description: Synchronize view convention between views
  vtkSyncSet2DMacro (ViewConvention, int);
  /// Description: Synchronize interpolate between views
  vtkSyncSet2DMacro (Interpolate, int);
  /// Description: Synchronize mouse interaction between views
  vtkSyncSet2DMacro (LeftButtonInteractionStyle, int);
  /// Description: Synchronize mouse interaction between views
  vtkSyncSet2DMacro (RightButtonInteractionStyle, int);
  /// Description: Synchronize mouse interaction between views
  vtkSyncSet2DMacro (MiddleButtonInteractionStyle, int);
  /// Description: Synchronize mouse interaction between views
  vtkSyncSet2DMacro (WheelInteractionStyle, int);
  /// Description: Synchronize mouse interaction between views
  vtkSyncSet2DMacro (KeyboardInteractionStyle, int);
  /// Description: Synchronize mouse interaction between views
  vtkSyncSet2DMacro (InteractionStyle, int);

  vtkSyncSet2DMacro (ShowImageAxis, int);
  vtkSyncSet2DMacro (ShowRulerWidget, int);
  vtkSyncSet2DMacro (CursorFollowMouse, int);

  /// Description: paning sync.
  virtual void SyncSetPan (double* arg, vtkImageView* caller = NULL);
  /// Description: Synchronize reset window level between views
  virtual void SyncResetWindowLevel (vtkImageView* caller = NULL);
  /// Description: Synchronize reset camera between views
  virtual void SyncResetCamera (vtkImageView* caller = NULL);
  /// Description: Synchronize reset position between views
  virtual void SyncResetCurrentPoint (vtkImageView* caller = NULL);
  /// Description: Synchronize render between views
  virtual void SyncRender (vtkImageView* caller = NULL, int checkForInteractor=0);
  /// Description: Synchronize reset between views
  virtual void SyncReset (vtkImageView* caller = NULL);
  /// Description: Synchronize reset between views
  virtual void SyncStart();

  /// Description: Synchronize add dataset
  virtual void SyncAddDataSet (vtkPointSet* arg, vtkProperty* prop = NULL);
  virtual void SyncRemoveDataSet (vtkPointSet* arg);

  // Decide weither or not the collection will link interactions

  /// Description: link slice flag
  vtkGetMacro (LinkSliceMove, unsigned int);
  /// Description: link slice flag
  virtual void SetLinkSliceMove (unsigned int v);
  /// Description: link slice flag
  vtkBooleanMacro (LinkSliceMove, unsigned int);

  /// Description: link time flag
  vtkGetMacro (LinkTimeChange, unsigned int);
  /// Description: link slice flag
  virtual void SetLinkTimeChange (unsigned int v);
  /// Description: link slice flag
  vtkBooleanMacro (LinkTimeChange, unsigned int);	
	
  /// Description: link color window flag
  vtkGetMacro (LinkColorWindowLevel, unsigned int);
  /// Description: link color window flag
  virtual void SetLinkColorWindowLevel (unsigned int v);
  /// Description: link color window flag
  vtkBooleanMacro (LinkColorWindowLevel, unsigned int);

  /// Description: link reset color window flag
  vtkGetMacro (LinkResetWindowLevel, unsigned int);
  /// Description: link reset color window flag
  virtual void SetLinkResetWindowLevel (unsigned int v);
  /// Description: link reset color window flag
  vtkBooleanMacro (LinkResetWindowLevel, unsigned int);

  /// Description: link reset viewer flag
  vtkGetMacro (LinkResetViewer, unsigned int);
  /// Description: link reset viewer flag
  virtual void SetLinkResetViewer (unsigned int v);
  /// Description: link reset viewer flag
  vtkBooleanMacro (LinkResetViewer, unsigned int);

  /// Description: link requested position (double click) flag
  vtkGetMacro (LinkRequestedPosition, unsigned int);
  /// Description: link requested position (double click) flag
  virtual void SetLinkRequestedPosition (unsigned int v);
  /// Description: link requested position (double click) flag
  vtkBooleanMacro (LinkRequestedPosition, unsigned int);

  /// Description: link camera flag
  vtkGetMacro (LinkCamera, unsigned int);
  /// Description: link camera flag
  virtual void SetLinkCamera (unsigned int v);
  /// Description: link camera flag
  vtkBooleanMacro (LinkCamera, unsigned int);

  /// Description: link camera flag
  vtkGetMacro (LinkZoom, unsigned int);
  /// Description: link camera flag
  virtual void SetLinkZoom (unsigned int v);
  /// Description: link camera flag
  vtkBooleanMacro (LinkZoom, unsigned int);

  /// Description: link camera flag
  vtkGetMacro (LinkPan, unsigned int);
  /// Description: link camera flag
  virtual void SetLinkPan (unsigned int v);
  /// Description: link camera flag
  vtkBooleanMacro (LinkPan, unsigned int);

  /// Description: link position flag
  vtkGetMacro (LinkCurrentPoint, unsigned int);
  /// Description: link position flag
  virtual void SetLinkCurrentPoint (unsigned int v);
  /// Description: link position flag
  vtkBooleanMacro (LinkCurrentPoint, unsigned int);

 protected:

  vtkImageViewCollection();
  ~vtkImageViewCollection();

  vtkImageViewCollectionCommand* Command;

  unsigned int LinkSliceMove;
  unsigned int LinkTimeChange;
  unsigned int LinkColorWindowLevel;
  unsigned int LinkResetWindowLevel;
  unsigned int LinkResetViewer;
  unsigned int LinkRequestedPosition;
  unsigned int LinkCamera;
  unsigned int LinkZoom;
  unsigned int LinkPan;
  unsigned int LinkCurrentPoint;
  unsigned int ShowSlicePlanes;
};



//BTX
class VTK_IMAGEVIEW_EXPORT vtkImageViewCollectionCommand : public vtkCommand
{

 public:

  static vtkImageViewCollectionCommand *New()
    {return new vtkImageViewCollectionCommand;};

  vtkImageViewCollection* GetCollection()
  { return this->Collection;};
  void SetCollection (vtkImageViewCollection* p);

  // Description:
  // Satisfy the superclass API for callbacks. Recall that the caller is
  // the instance invoking the event; eid is the event id (see
  // vtkCommand.h); and calldata is information sent when the callback
  // was invoked (e.g., progress value in the vtkCommand::ProgressEvent).
  virtual void Execute(vtkObject *caller,unsigned long event, void *vtkNotUsed(callData));

 protected:

  vtkImageViewCollectionCommand();
  ~vtkImageViewCollectionCommand(){};

 private:
  vtkImageViewCollection* Collection;

};
//ETX




