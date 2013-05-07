/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkLandmarkManagementExport.h"

#include <vtkLandmark.h>

#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkCollection.h>
#include <string>
#include <vector>
#include <map>


class vtkImageView;
class vtkImageViewCollection;

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
   \class vtkLandmarkManager vtkLandmarkManager.h "vtkLandmarkManager.h"
   \brief Concrete class for handling a set of vtkLandmark.
   \author Nicolas Toussaint
   
  The aim of this class is to provide a concrete object to manage a set of specific points represented
  in view scenes. These points, vtkLandmark, are located in space, and are displayed in one or several
  vtkImageView objects.

  In order to display the landmarks, please provide the vtkLandmarkManager with a set of views to be used, with
  the method AddView().

  To add landmarks, several methods are possible :

  1. CreateAndAddLandmark() will create a landmark from scratch and add it to the manager. its position will
   \see
   vtkLandmark vtkPolyData vtkProperty vtkActor vtkImageView vtkCollection
   
*/

class VTK_LANDMARKMANAGEMENT_EXPORT vtkLandmarkManager : public vtkObject
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
  void RemoveAllLandmarks();
  vtkLandmark* GetLastLandmark();
  
  vtkLandmark* CreateAndAddLandmark();

  /** Set the render window interactor */
  void AddView (vtkImageView* view);
  void RemoveView (vtkImageView* view);
  void InteractionOn();
  void InteractionOff();
  bool HasView(vtkImageView* view);
  void RemoveAllViews();

  vtkImageView* GetFirstView();
  vtkSetMacro (ViewIdToTrust, unsigned int);
  vtkGetMacro (ViewIdToTrust, unsigned int);
  
  void Update();
  void Reset();
  void ShowAll();
  void HideAll();
  void SelectLandmark (vtkLandmark* landmark);

  vtkLandmark* GetFirstSelectedLandmark();
  
  unsigned int GetNumberOfLandmarks() const
  {
    return this->LandmarkList->GetNumberOfItems();
  }
  
  vtkGetObjectMacro(Property, vtkProperty);
  vtkGetObjectMacro(SelectedProperty, vtkProperty);

  vtkGetObjectMacro(ViewList, vtkImageViewCollection);
  
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
  double* GetSelectedColor()
  { return this->SelectedProperty->GetColor(); }
  
  void ScaleAll (double scale);
  vtkGetMacro (Scale, double);
  
  
  void AddLandmarkToLinker (vtkLandmark* landmark);

  
  void LinkerOn();
  void LinkerOff();

  void SetLinkerVisibility (bool val)
  {
    if (val)
      this->LinkerOn();
    else
      this->LinkerOff();
  }
  
  
  void UpdateLinker();

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


  vtkImageViewCollection* ViewList;
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



