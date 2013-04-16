/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#pragma once

#include "vtkRenderingAddOnExport.h"

#include <vector>
#include <string>

#include "vtkObject.h"


class vtkProp;
class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;
class vtkTextProperty;
class vtkCornerAnnotation;
class vtkOrientationAnnotation;
class vtkImageData;
class vtkLookupTable;
//class vtkInteractorStyle;


/**
   \class vtkSynchronizedView vtkSynchronizedView.h "vtkSynchronizedView.h"
   \brief Base class for vtkViewImage
   \author Pierre Fillard & Marc Traina & Nicolas Toussaint
   
   This class is a vtkObject that includes a renderer, a renderwindow and
   a renderwindowinteractor for visualization purposes. You can can manually add actors
   or use vtkViewImage class for more convenience.

   It uses a tree-like structure to propagate functions call in order to synchronize
   several views together, like changing the window/level, the current position, etc.
   Each view has one parent and several children. When a children is added, its parent
   is automatically set. Once a view is removed from the tree using function Detach(),
   its children are automatically appended to its parent's children, so that the function
   propagation mechanism is not broken.

   The tree structure should cycled, i.e. the last added children should point to
   the first view. In that case, and in order to prevent infinite loops when propagating
   functions, the functions Lock() and UnLock() must be used. When Lock() is called, a view
   does not transmit anything. UnLock() permits to go back to normal function propagation.
   
*/


class VTK_RENDERINGADDON_EXPORT vtkSynchronizedView : public vtkObject
{
  
 public:
  
  static vtkSynchronizedView* New();
  vtkTypeRevisionMacro(vtkSynchronizedView, vtkObject);
  

  /**
     Set/Get the RenderWindow.
  */
  virtual void SetRenderWindow(vtkRenderWindow *arg);
  vtkGetObjectMacro (RenderWindow, vtkRenderWindow);
  

  /**
     Set/Get the Renderer.
  **/
  virtual void SetRenderer(vtkRenderer *arg);
  vtkGetObjectMacro (Renderer, vtkRenderer);

  
  /**
     Attach an interactor to the internal RenderWindow.
  **/
  VTK_LEGACY (virtual void SetInteractor(vtkRenderWindowInteractor*));
  virtual void SetRenderWindowInteractor(vtkRenderWindowInteractor*);
  vtkGetObjectMacro (RenderWindowInteractor, vtkRenderWindowInteractor);

  
  /**
     Add the actor to the first renderer of the render window if exist. 
      Do nothing otherwise.
  */
  virtual void AddActor(vtkProp* actor);

  
  /**
     Remove the actor from the first renderer of the render window if exist. 
     Do nothing otherwise.
  */
  virtual void RemoveActor(vtkProp* actor);
  

  /**
     Enable or Disable interaction on the view. The Interaction mode is store
     internaly and set at each time the widget is showed. The interaction
     mode cannot be set before the vtkRenderWindowInteractor is initialized.
  */
  virtual void SetInteraction(int);
  vtkBooleanMacro (Interaction, int);
  vtkGetMacro (Interaction, int);
  

  /**
     Set the background color. Format is RGB, 0 <= R,G,B <=1
     Example: SetBackgroundColor(0.9,0.9,0.9) for grey-white.
  */
  virtual void SetBackgroundColor(double r, double g, double b);
  
  
  /**
     Set/Get annotations methods.
  */
  void         SetUpLeftAnnotation (const char *p_annotation);  
  const char * GetUpLeftAnnotation () const;  
  void         SetUpRightAnnotation (const char *p_annotation);  
  const char * GetUpRightAnnotation () const;
  void         SetDownLeftAnnotation (const char *p_annotation);  
  const char * GetDownLeftAnnotation () const;
  void         SetDownRightAnnotation (const char *p_annotation);  
  const char * GetDownRightAnnotation () const;
  void         SetNorthAnnotation (const char *p_annotation);  
  const char * GetNorthAnnotation () const;
  void         SetSouthAnnotation (const char *p_annotation);  
  const char * GetSouthAnnotation () const;
  void         SetEastAnnotation  (const char *p_annotation);
  const char * GetEastAnnotation  () const;
  void         SetWestAnnotation (const char *p_annotation);  
  const char * GetWestAnnotation () const;


  /**
     Set/Get about data string. Data string is automatically set in the
     down-left text area. Overload this method to change the default behaviour.
  */
  virtual void SetAboutData (const char* str);
  virtual const char* GetAboutData() const;


  /**
     Set/Get the about data text visibility. If you default the about data text in a
     place different than the down-left text area, you must overload SetAboutDataVisibility
     and place the text in the correct spot.
   */
  virtual void SetAboutDataVisibility (int val);
  vtkBooleanMacro (AboutDataVisibility, int);
  vtkGetMacro (AboutDataVisibility, int);

  /**
     Show/Hide the annotations, i.e., the up-left, up-right, down-left and down-right text areas.
  */
  //vtkSetMacro (ShowAnnotations, int);
  virtual void SetShowAnnotations (int val);
  vtkBooleanMacro (ShowAnnotations, int);
  vtkGetMacro (ShowAnnotations, int);


  /**
     Show/Hide the directions, i.e., the up, down, left and right text areas.
  */
  virtual void SetShowDirections (int val);
  vtkBooleanMacro (ShowDirections, int);
  vtkGetMacro (ShowDirections, int);

  
  /**
     Get the corner annotation object.
  */
  vtkGetObjectMacro (CornerAnnotation, vtkCornerAnnotation);

  
  /**
     Get the orientation annotation object.
  */
  vtkGetObjectMacro (OrientationAnnotation, vtkOrientationAnnotation);
  
  
  /**
     Update the annotations. Make sure they are up to date.
  */
  VTK_LEGACY (virtual void UpdateAnnotations());
  
  
  virtual void PrintSelf(ostream& os, vtkIndent indent);
  

  /**
     Call the RenderWindow's Render() method.
  */
  virtual void Render();
  virtual void SyncRender();
  

  /**
     Synonym of Render(), except that the rendering is acutally made
     only if needed, i.e., if view is not up to date.
  */
  virtual void Update();
  virtual void SyncUpdate();


  /**
     Set the camera position, focal and view up.
   */
  virtual void SetCameraPosition(double position[3]);
  virtual void SetCameraFocalPoint(double position[3]);
  virtual void SetCameraViewUp(double position[3]);


  /**
     Manually set/get the camera focal and position. Used to set the translation.
  */
  virtual void SetCameraFocalAndPosition (double focal[3], double pos[3]);
  virtual void GetCameraFocalAndPosition (double focal[3], double pos[3]);

  
  /**
     Synchronized version of SetCameraFocalAndPosition.
  */
  virtual void SyncSetCameraFocalAndPosition (double focal[3], double pos[3]);

  
  /**
     Reset the camera focal and position.
  */
  virtual void ResetCamera();


  /**
     In the tree-structure of the view, returns the View's parent.
   */
  vtkSynchronizedView* GetParent() const;

  
  /**
     Add a child to the list of children. Check if the child is already
     in the list firt.
   */
  void AddChild (vtkSynchronizedView* p_view);
  

  //BTX
  void AddChildren (std::vector<vtkSynchronizedView*> p_viewlist);
  //ETX

  int GetNumberOfChildren() const;

  /**
     Returns true if the view has this child in its list.
  */
  bool HasChild (vtkSynchronizedView*) const;
  
  
  /**
     Remove a child form the list of children.
  */
  void RemoveChild (vtkSynchronizedView* view);

  
  /**
     Remove all children of the view.
   */
  void RemoveAllChildren();

  
  /**
     Detach the view, i.e. add its own children (if any) to its parent's children (if any).
  */
  void Detach();
  

  //BTX
  std::vector < vtkSynchronizedView* > GetChildren() const
  { return this->Children;}
  VTK_LEGACY(std::vector < vtkSynchronizedView* > GetViewToObserve() const);
  //ETX
  

  /**
     Set the color of the annotations.
   */
  void SetTextColor (double color[3]);
  void SetTextColor (double r, double g, double b);
  
  
  /**
     Part of the function propagation mechanism, when the function Lock() is
     called, the view does not transmit the function to its children (and does
     not do anything in fact).
   */
  void Lock();


  /**
     Returns whether the view is Locked or not.
   */
  int IsLocked()
  { return this->GetIsProcessed(); }

    
  /**
     A call to UnLock() permits to transmit function calls to the view's children.
   */
  void UnLock();


  /**
     Set/Get the camera focal and position link ON or OFF.
  */
  vtkSetMacro (LinkCameraFocalAndPosition, int);
  vtkGetMacro (LinkCameraFocalAndPosition, int);
  

  /**
     Set the render link ON or OFF.
  */
  vtkSetMacro (LinkRender, int);
  vtkGetMacro (LinkRender, int);
  

  /**
     This function is called right after setting both Renderer and RenderWindow.
     It allows a class to add actors for instance without knowing when the Renderer
     and RenderWindow are set. For example, vtkSynchronizedView will add the corner annotations
     during the call to the Initialize function.
  */
  virtual void Initialize();
  virtual void Uninitialize();


  virtual void InitializeInteractor(){};
  virtual void UninitializeInteractor(){};

  
 protected:
  vtkSynchronizedView();
  ~vtkSynchronizedView();
  

  //  vtkSetMacro (IsProcessed, int);
  vtkGetMacro (IsProcessed, int);
 
  
  //BTX
  vtkSynchronizedView*                 Parent;
  std::vector < vtkSynchronizedView* > Children;
  //ETX
  
  /**
     Set the parent for this view. Internal use only.
   */
  void SetParent (vtkSynchronizedView* view);
    
 private:

  /// VTK Renderer:
  vtkRenderWindow*                RenderWindow;
  vtkRenderer*                    Renderer;
  vtkRenderWindowInteractor*      RenderWindowInteractor;
  
  //vtkInteractorStyle*             InteractorStyle;
  vtkCornerAnnotation*            CornerAnnotation;
  vtkOrientationAnnotation*       OrientationAnnotation;

  vtkTextProperty*                TextProperty;
  
  //BTX
  /*
  std::string             UpLeftAnnotation;
  std::string             UpRightAnnotation;
  std::string             DownLeftAnnotation;
  std::string             DownRightAnnotation;
  std::string             NorthAnnotation;
  std::string             SouthAnnotation;
  std::string             EastAnnotation;
  std::string             WestAnnotation;
  */
  std::string             AboutData;
  //ETX
  
  int                    ShowAnnotations;
  int                    ShowDirections;
  int                    AboutDataVisibility;
  
  int                    IsProcessed;
  int                    Interaction;
  int                    LinkRender;
  int                    LinkCameraFocalAndPosition;
  
  unsigned long          InternalMTime;
  
};


