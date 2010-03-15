/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkLandmarkManager.h 1289 2009-10-14 16:58:30Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-10-14 18:58:30 +0200 (Wed, 14 Oct 2009) $
Version:   $Revision: 1289 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtk_LandmarkManager_h_
#define _vtk_LandmarkManager_h_

#include "vtkINRIA3DConfigure.h"

#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkCollection.h>
#include <string>
#include <vector>
#include <map>


class vtkViewImage;
class vtkPolyData;
class vtkLandmarkManagerCallback;
class vtkActor;
class vtkActorCollection;
class vtkCollection;
class vtkSphereWidget;
class vtkSphereSource;
class vtkFollower;
class vtkVectorText;


/**
   \class vtkLandmark vtkLandmarkManager.h "vtkLandmarkManager.h"
   \brief Concrete class representing a point in space, its representation, and some additional parameters
   \author Nicolas Toussaint
   
  This class implements a concrete representation of a pont in space. Besides
  its position - SetPosition() - the vtkLandmark contains a Polygonal representation
  of this point. You can choose the style of this representation by calling SetLandmarkType().

  This representation has some visualization properties that can be set. SetScale()
  will scale the polygonal representation. The position of its center corresponds to
  the vtkLandmark global position : SetPosition().
  
  The landmark has a selection flag, for the user conveniance, A call of Select() will make each actor
  switch between the two different properties provided by the user : SetProperty() and SetSelectedProperty().
  

  The landmark contains a list of actors that represent the landmark in the 3D space.
  Adding and removing actors can be done by AddActor() and RemoveActor().
  
  Some additional parameters can be held by the vtkLandmark, such as a name (with SetName()), but
  also any kind of scalar value. Simply call SetFlag(). Any previous value of the same key
  will be overridden by the new one. Access to the list of flag keys by GetFlagKeys().

  WARNING : Don't use this class alone. Please use the vtkLandmarkManager class instead.
  
   \see
   vtkLandmarkManager vtkPolyData vtkProperty vtkActor
   
*/

class VTK_VISUMANAGEMENT_EXPORT vtkLandmark : public vtkPolyData
{
 public:

  static vtkLandmark* New();
  vtkTypeRevisionMacro(vtkLandmark, vtkPolyData);
  void PrintSelf(ostream& os, vtkIndent indent);

  //BTX
  /**
     Type Ids of the landmark representation.
  */
  enum LandmarkTypeIds
  {
    LANDMARK_CROSS,
    LANDMARK_SPHERE
  };
  //ETX
  /** Get/Set the name of the landmark */
  virtual void SetName (const char* name) 
  { this->Name = name; }
  /** Get/Set the name of the landmark */
  virtual const char* GetName (void)
  { return this->Name.c_str(); }
  /**
     Set/Access to the voxel-coordinates of the landmark. This information
     has to be provided by the user. However, if the landmark manager contains
     some vtkViewImage, then everytime the landmark is manually moved, the voxel
     coordinates are filled with those found in the vtkViewImage.
  */
  virtual int* GetVoxelCoord()
  {
    return this->VoxelCoord;
  }
  /**
     Set/Access to the voxel-coordinates of the landmark. This information
     has to be provided by the user. However, if the landmark manager contains
     some vtkViewImage, then everytime the landmark is manually moved, the voxel
     coordinates are filled with those found in the vtkViewImage.
  */
  virtual void SetVoxelCoord (const int val[3])
  {
    this->VoxelCoord[0] = val[0];
    this->VoxelCoord[1] = val[1];
    this->VoxelCoord[2] = val[2];
  }
  /**
     The main information contained by the landmark is obviously its
     position in space. These methods set or return the postion vector.
  */
  virtual double* GetPosition();
  /**
     The main information contained by the landmark is obviously its
     position in space. These methods set or return the postion vector.
     @arg val : the 3D position.
  */
  virtual void SetPosition (const double val[3]);
  virtual void SetPosition (double x, double y, double z)
  {
    double xyz[3] = {x, y, z};
    this->SetPosition (xyz);
  }
  virtual double* GetCoord (void)
  { return this->GetPosition(); }
  virtual void SetCoord (const double val[3])
  { this->SetPosition (val); }
  virtual void SetCoord (double x, double y, double z)
  { this->SetPosition (x,y,z); }
  
  /**
     Access to the visualization property of the vtkLandmark.
     A call of SetColor() or SetScale() will change this vtkProperty with
     respect to the argument. 
  */
  vtkGetObjectMacro(Property, vtkProperty);
  /**
     Access to the visualization selected-property of the vtkLandmark. That is, the property
     that is used when the landmark is \"selected\" (Select()). 
     A call of SetSelectedColor() will change this vtkProperty with
     respect to the argument. 
  */
  vtkGetObjectMacro(SelectedProperty, vtkProperty);
  /**
     Access to the visualization property of the vtkLandmark.
     A call of SetColor() or SetScale() will change this vtkProperty with
     respect to the argument. 
  */
  vtkGetObjectMacro(Property2D, vtkProperty);
  /**
     Access to the visualization selected-property of the vtkLandmark. That is, the property
     that is used when the landmark is \"selected\" (Select()). 
     A call of SetSelectedColor() will change this vtkProperty with
     respect to the argument. 
  */
  vtkGetObjectMacro(SelectedProperty2D, vtkProperty);
/*   void SetProperty (vtkProperty* prop); */
/*   void SetSelectedProperty (vtkProperty* prop); */

  /**
     The vtkLandmark class contains an arbitrary amount of flags that can
     be used by the user. These flags are stored as double values. Access to
     a specific flag value can be done by its \"key\" with GetFlag().

     Note that if the provided key already exists with a previously set value,
     another call of SetFlag() will override the value.
  */
  virtual void SetFlag (const char* key, double val);
  /**
     The vtkLandmark class contains an arbitrary amount of flags that can
     be used by the user. These flags are stored as double values. Access to
     a specific flag value can be done by its \"key\" with GetFlag().

     Note that if the provided key already exists with a previously set value,
     another call of SetFlag() will override the value.
  */
  virtual double GetFlag (const char* key);
  /** Test if a given key exists. */
  virtual bool HasFlag (const char* key);
  //BTX
  /** access to the list of keys that are present in flags */
  virtual std::vector<std::string> GetFlagKeys (void);
  //ETX
  /**
     Set the color of the landmark representation. This methods
     affects the vtkProperty given by GetProperty(), and all actors that share
     this property (AddActor()) will be updated with the given color.
  */
  virtual void SetColor(double r, double g, double b);
  /**
     Set the color of the landmark representation when and only when the select
     flag (Select() and UnSelect()). This methods affects the vtkProperty given by GetSelectedProperty(),
     and all actors that share this property (AddActor()) will be updated with
     the given color if the selected flag is set to ON.
  */
  virtual void SetSelectedColor(double r, double g, double b);
  /** access to the color of the landmark (given by the landmark's property : GetProperty()). */ 
  virtual double* GetColor (void)
  { return this->Property->GetColor();}
  /**
     access to the selected color of the landmark, the one that is used when landmark is selected (Select(),
     given by the landmark's selected property : GetSelectedProperty().
  */ 
  virtual double* GetSelectedColor (void)
  { return this->SelectedProperty->GetColor(); }
  /**
     Register an actor to be associated to the landmark. This means that the visualization
     properties will be applied to this actor.
  */
  virtual void AddActor (vtkActor* actor);
  /**
     Register an actor to be associated to the landmark. This means that the visualization
     properties will be applied to this actor.
  */
  virtual void AddActor2D (vtkActor* actor);
  /** Remove a specific actor associated to the landmark. */
  virtual void RemoveActor (vtkActor* actor);
  /** Remove all actorsassociated to the landmark. */
  virtual void RemoveAllActors (void);
  /**
     Set/Get the scaling factor of the landmark. The argument has to be provided in mm.
     This factor is used to compute the size of the graphic representation of the landmark.
     It corresponds to the radius of the sphere, or the size of the cross, depending on the
     visualization type : SetLandmarkType().
  */
  virtual void SetScale (double scale);
  /**
     Set/Get the scaling factor of the landmark. The argument has to be provided in mm.
     This factor is used to compute the size of the graphic representation of the landmark.
     It corresponds to the radius of the sphere, or the size of the cross, depending on the
     visualization type : SetLandmarkType().
  */
  virtual double GetScale (void);
  /**
     Turn Off/On the selected flag of the landmark. Visualization properties will then be
     changed in consequences. Use SetSelectedColor() or SetColor() to control these properties.
     To access to more specific properties, use the GetProperty() and GetSelectedProperty() methods.
  */
  virtual void Select(bool changecolor = true);
  /**
     Turn Off/On the selected flag of the landmark. Visualization properties will then be
     changed in consequences. Use SetSelectedColor() or SetColor() to control these properties.
     To access to more specific properties, use the GetProperty() and GetSelectedProperty() methods.
  */
  virtual void UnSelect();
  /** Check the current selected flag of the landmark */ 
  bool IsSelected (void)
  { return this->SelectFlag; }
  /**
     Set/Get the current representation type of the landmark. Two representations are
     available : LANDMARK_CROSS or LANDMARK_SPHERE
  */
  vtkGetMacro (LandmarkType, unsigned int);
  /**
     Set/Get the current representation type of the landmark. Two representations are
     available : LANDMARK_CROSS or LANDMARK_SPHERE
  */
  vtkSetClampMacro (LandmarkType, unsigned int, LANDMARK_CROSS, LANDMARK_SPHERE);
  /**
     Get/Set method used by the manager to set a specific widget that is used by
     a 3D interactor to move manually the landmark when landmark type
     is LANDMARK_SPHERE
  */
  vtkGetObjectMacro (SphereWidget, vtkSphereWidget);
  /**
     Get/Set the comment associated to this landmark. Use this method in combination
     with SetCommentVisibility() to be able to display a comment in front of the
     landmark sphere in the 3D scene

     NOTE : this feature is only possible when the landmark type is LANDMARK_SPHERE
     and after the SphereWidget has been set with SetSphereWidget(). This is automatically
     the case when one of the vtkViewImage is a vtkViewImage3D. 
  */
  virtual const char* GetComment(void)
  { return this->Comment.c_str(); }
  /**
     Get/Set the comment associated to this landmark. Use this method in combination
     with SetCommentVisibility() to be able to display a comment in front of the
     landmark sphere in the 3D scene

     NOTE : this feature is only possible when the landmark type is LANDMARK_SPHERE
     and after the SphereWidget has been set with SetSphereWidget(). This is automatically
     the case when one of the vtkViewImage is a vtkViewImage3D. 
  */
  virtual void SetComment (const char* comment)
  {
    this->Comment = comment;
    this->ConfigureComment();
  }
  /**
     Get/Set the comment visibility of this landmark. Use this method in combination
     with SetComment() to be able to display a comment in front of the
     landmark sphere in the 3D scene

     NOTE : this feature is only possible when the landmark type is LANDMARK_SPHERE
     and after the SphereWidget has been set with SetSphereWidget(). This is automatically
     the case when one of the vtkViewImage is a vtkViewImage3D. 
  */
  vtkGetMacro (UseComment, int);
  void SetUseComment (int);
  vtkBooleanMacro (UseComment, int);
  vtkGetObjectMacro (TextActor, vtkFollower);

  
  virtual void Initialize(void);
  
  
 protected:
  
  vtkLandmark();
  ~vtkLandmark();

  virtual void UpdateCrossPosition (void);
  virtual void UpdateSpherePosition (void);
  virtual void UpdateCommentPosition (void);
  virtual void ConfigureComment (void);
  
  
 private:

  vtkProperty* Property;
  vtkProperty* Property2D;
  vtkProperty* SelectedProperty;
  vtkProperty* SelectedProperty2D;

  vtkSphereWidget* SphereWidget;
  vtkSphereSource* Source;
  vtkFollower* TextActor;
  vtkVectorText* TextMaker;
  
  //BTX
  std::string Name;
  std::string Comment;
  std::map<std::string, double> FlagMap;
  //ETX

  vtkActorCollection* ActorList;
  vtkActorCollection* Actor2DList;
  int VoxelCoord[3];
  double Position[3];
  
  double Scale;
  bool SelectFlag;
  unsigned int LandmarkType;
  int UseComment;
  
  
};



/**
   \class vtkLandmarkManager vtkLandmarkManager.h "vtkLandmarkManager.h"
   \brief Concrete class for handling a set of vtkLandmark.
   \author Nicolas Toussaint
   
  The aim of this class is to provide a concrete object to manage a set of specific points represented
  in view scenes. These points, vtkLandmark, are located in space, and are displayed in one or several
  vtkViewImage objects.

  In order to display the landmarks, please provide the vtkLandmarkManager with a set of views to be used, with
  the method AddView().

  To add landmarks, several methods are possible :

  1. CreateAndAddLandmark() will create a landmark from scratch and add it to the manager. its position will
   \see
   vtkLandmark vtkPolyData vtkProperty vtkActor vtkViewImage vtkCollection
   
*/

class VTK_VISUMANAGEMENT_EXPORT vtkLandmarkManager : public vtkObject
{
 public:
  
  static vtkLandmarkManager* New();
  vtkTypeRevisionMacro(vtkLandmarkManager, vtkObject);

  //BTX
  enum ColorModeIds
  {
    LANDMARK_COLOR_RANDOM = 0,
    LANDMARK_COLOR_LUT,
    LANDMARK_COLOR_NONE
  };
  //ETX
  
  void ReadFile (const char* filename);
  void WriteFile (const char* filename);
  void ReadFileOld (const char* filename);
  
  vtkLandmark* GetLandmark (unsigned int i);
  vtkLandmark* FindLandmark (const char* name);
  void AddLandmark(vtkLandmark* landmark);
  void RemoveLandmark(vtkLandmark* landmark);
  void RemoveAllLandmarks(void);
  vtkLandmark* GetLastLandmark (void);
  
  vtkLandmark* CreateAndAddLandmark (void);

  /** Set the render window interactor */
  void AddView (vtkViewImage* view);
  void RemoveView (vtkViewImage* view);
  void InteractionOn(void);
  void InteractionOff(void);
  bool HasView(vtkViewImage* view);
  void RemoveAllViews(void);

  vtkViewImage* GetFirstView (void);
  vtkSetMacro (ViewIdToTrust, unsigned int);
  vtkGetMacro (ViewIdToTrust, unsigned int);
  
  void Update();
  void Reset (void);
  void ShowAll(void);
  void HideAll(void);
  void SelectLandmark (vtkLandmark* landmark);

  vtkLandmark* GetFirstSelectedLandmark (void);
  
  unsigned int GetNumberOfLandmarks (void) const
  {
    return this->LandmarkList->GetNumberOfItems();
  }
  
  vtkGetObjectMacro(Property, vtkProperty);
  vtkGetObjectMacro(SelectedProperty, vtkProperty);

  vtkGetObjectMacro(ViewList, vtkCollection);
  
  vtkGetMacro (ChangeColorOnSelected, int);
  vtkSetClampMacro(ChangeColorOnSelected, int, 0 ,1);
  vtkBooleanMacro(ChangeColorOnSelected, int);

  vtkGetMacro (AutoSelect, int);
  vtkSetClampMacro(AutoSelect, int, 0 ,1);
  vtkBooleanMacro(AutoSelect, int);

  vtkGetMacro (ColorMode, unsigned int);
  vtkSetClampMacro(ColorMode, unsigned int, LANDMARK_COLOR_RANDOM, LANDMARK_COLOR_NONE);
  //vtkBooleanMacro(ColorMode, unsigned int);
  
  // Change the color of the selected item
  void SetSelectedColor(double r, double g, double b);
  void SetSelectedColor(double c[3]);
  double* GetSelectedColor (void)
  { return this->SelectedProperty->GetColor(); }
  
  void ScaleAll (double scale);
  vtkGetMacro (Scale, double);
  
  
  void AddLandmarkToLinker (vtkLandmark* landmark);

  
  void LinkerOn(void);
  void LinkerOff(void);

  void SetLinkerVisibility (bool val)
  {
    if (val)
      this->LinkerOn();
    else
      this->LinkerOff();
  }
  
  
  void UpdateLinker(void);

  vtkGetObjectMacro (LandmarkBeingRemoved, vtkLandmark);

  void SetLastPickedLandmark (vtkLandmark* land)
  { this->LastPickedLandmark = land; }

  vtkGetObjectMacro (LastPickedLandmark, vtkLandmark);
  vtkGetMacro (LandmarkType, unsigned int);
  vtkSetClampMacro (LandmarkType, unsigned int, vtkLandmark::LANDMARK_CROSS, vtkLandmark::LANDMARK_SPHERE );

 protected:
  vtkLandmarkManager();
  ~vtkLandmarkManager();

  void AddSphereWidgetToLandmark (vtkLandmark* landmark);  
  
 private:


  vtkCollection* ViewList;
  vtkCollection* LandmarkList;  

  vtkLandmarkManagerCallback* Callback;
  vtkProperty* Property;
  vtkProperty* SelectedProperty;
  
  int AutoSelect;
  int ChangeColorOnSelected;
  unsigned int ColorMode;

  vtkPolyData* Linker;
  int LinkerVisibility;

  double Scale;

  vtkLandmark* LandmarkBeingRemoved;
  vtkLandmark* LastPickedLandmark;

  unsigned int ViewIdToTrust;
  unsigned int LandmarkType;
  
  
  
  
};


#endif
