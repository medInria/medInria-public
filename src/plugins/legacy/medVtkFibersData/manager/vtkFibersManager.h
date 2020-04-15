/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkFibersDataPluginExport.h>

#include <vtkObject.h>
#include <vtkRenderer.h>

class vtkPolyData;
class vtkLimitFibersToVOI;
class vtkLimitFibersToROI;
class vtkBoxWidget;
class vtkActor;
class vtkRenderWindowInteractor;
class vtkCellArray;
class vtkImageData;
class vtkScalarsToColors;
class vtkMaskPolyData;
class vtkCleanPolyData;
class vtkTubeFilter;
class vtkRibbonFilter;
class vtkPolyDataMapper;
class vtkCornerAnnotation;
class vtkFiberPickerCallback;
class vtkFiberKeyboardCallback;
class vtkFibersManagerCallback;


/**
   
   \class vtkFibersManager.
   
   This class provides a full set of rendering and interactions with so-called
   fibers, which are nothing else than VTK poly-lines stored in a vtkPolyData.
   It takes as input a vtkPolyData (the actual lines) and a vtkRenderWindowInteractor
   and does everything for the user.
   
   Different modes of rendering the lines are possible:
   - Polylines (SetRenderingModeToPolyLines()): the default mode;
   - Ribbons (SetRenderingModeToRibbons()): renders each line as a flat ribbon;
   - Tubes (SetRenderingModeToTubes()): renders each line as a tube;
   
   Note that when using the ribbon or tube mode, a vtkMaskPolyData is also used
   to lower the number of fibers depending on the maximum number of fibers allowed
   (default is 20000 - change it with SetMaximumNumberOfFibers() ).
   
   Different type of interactions are possible. By default, a cropping box
   (vtkBoxWidget) is used to limit the fibers that go through it.    
*/

class MEDVTKFIBERSDATAPLUGIN_EXPORT vtkFibersManager : public vtkObject
{
 public:

  static vtkFibersManager* New();
  vtkTypeMacro(vtkFibersManager, vtkObject);

  //BTX
  enum vtkFiberRenderingMode
  {
    RENDER_IS_POLYLINES,
    RENDER_IS_TUBES,
    RENDER_IS_RIBBONS
  };
  //ETX
  
  vtkGetObjectMacro (BoxWidget, vtkBoxWidget);

  virtual void Enable();

  virtual void Disable();
  
  /** Reset the pipeline, deactivate the CP box and release memory.*/
  virtual void Initialize();
  
  /** Set the vtkPolyData input */
  virtual void SetInput (vtkPolyData*);
    
  /** Get the vtkPolyData input */
  vtkGetObjectMacro (Input, vtkPolyData);

  /** Get the generated actor */
  virtual vtkActor* GetOutput() const;

  /** Get the ROI Limiter */
  virtual vtkLimitFibersToROI* GetROILimiter() const;

  /** Get the VOI Limiter */
  virtual vtkLimitFibersToVOI* GetVOILimiter() const;
  
  /** Set the render window interactor */
  virtual void SetRenderWindowInteractor (vtkRenderWindowInteractor* rwin);
  vtkGetObjectMacro (RenderWindowInteractor, vtkRenderWindowInteractor);
  
  /** Set the default renderer to put actors in. Must be called before SetInput() */
  vtkSetObjectMacro(Renderer, vtkRenderer);
  vtkGetObjectMacro(Renderer, vtkRenderer);

  /** Remove all actors added by this manager. */
  virtual void RemoveAllActors();
  
  /** Set the rendering mode to poly lines */
  virtual void SetRenderingModeToPolyLines();

  /** Set the rendering mode to tubes */
  virtual void SetRenderingModeToTubes();

  /** Set the rendering mode to ribbons */
  virtual void SetRenderingModeToRibbons();

  /** Get the rendering mode. */
  static int GetRenderingMode();
  
  virtual void SetRenderingMode(int);

  
  /** Set the ouput of the callback to ite input. Thus, users
      can extract a subsample of fibers, and then another subsample,
      and so on. */
  virtual void SwapInputOutput();

  /** Reset the fiber manager to its first input. */
  virtual void Reset();

  /** Switch on/off the visibility of the fibers */
  virtual void SetVisibility (bool);

  /** Set the box widget on */
  virtual void BoxWidgetOn();
  
  /** Set the box widget on */
  virtual void BoxWidgetOff();

  /** Set the box widget on */
  virtual void SetBoxWidget (bool a);
  
  /** Get the box widget visibility */
  vtkGetMacro (BoxWidgetVisibility, int);

  /** Set Radius & ribbon width */
  virtual void SetRadius (double r);

  virtual double GetRadius() const;

  /** Return the fiber ids selected by the box widget */
  virtual vtkCellArray* GetSelectedCells() const;

  virtual vtkPolyData *GetCallbackOutput() const;

  virtual void SetMaskImage (vtkImageData* image);
  
  /**
     Set the color mode to either local or global fiber orientation.
  */
  virtual void SetColorModeToLocalFiberOrientation();
  
  /**
     Set the color mode to either local or global fiber orientation.
   */
  virtual void SetColorModelToGlobalFiberOrientation();
  
  virtual void SetColorModeToPointArray (const int& id);
  
  const char* GetPointArrayName (const int& id) const;
  
  virtual int GetNumberOfPointArrays() const;
  
  virtual void SetLookupTable(vtkScalarsToColors* lut);

  virtual vtkScalarsToColors* GetLookupTable() const;
  
  virtual void SetMaximumNumberOfFibers(int num);
  
  virtual int GetMaximumNumberOfFibers() const;

  virtual void ShowHelpMessage();
  
  virtual void HideHelpMessage();
  
  virtual void SetHelpMessageVisibility (int a);
  
  virtual int GetHelpMessageVisibility() const;
  
  /** Force a callback call to Execute() */
  virtual void Execute();
  
 protected:
  vtkFibersManager();
  ~vtkFibersManager();
  
   
 private:

  vtkPolyData     *Input;
  
  vtkBoxWidget *BoxWidget;
  bool          BoxWidgetVisibility;

  vtkMaskPolyData          *Squeezer;
  vtkCleanPolyData         *Cleaner;
  vtkTubeFilter            *TubeFilter;
  vtkRibbonFilter          *RibbonFilter;
  vtkPolyDataMapper        *Mapper;
  vtkActor                 *Actor;
  vtkCornerAnnotation      *HelpMessage;
  int                       MaximumNumberOfFibers;
  
  vtkFibersManagerCallback *Callback;
  vtkFiberPickerCallback   *PickerCallback;
  vtkFiberKeyboardCallback *KeyboardCallback;
  
  vtkRenderWindowInteractor *RenderWindowInteractor;
  vtkRenderer               *Renderer;

  static int vtkFiberRenderingStyle;
};
