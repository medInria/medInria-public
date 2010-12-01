/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkFibersManager.h 1467 2010-02-17 23:31:08Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2010-02-18 00:31:08 +0100 (Thu, 18 Feb 2010) $
Version:   $Revision: 1467 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtk_FibersManager_h_
#define _vtk_FibersManager_h_

#include "vtkINRIA3DConfigure.h"

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

class VTK_VISUMANAGEMENT_EXPORT vtkFibersManager : public vtkObject
{
 public:

  static vtkFibersManager* New();
  vtkTypeRevisionMacro(vtkFibersManager, vtkObject);

  enum vtkFiberRenderingMode
  {
    RENDER_IS_POLYLINES,
    RENDER_IS_TUBES,
    RENDER_IS_RIBBONS
  };

  vtkGetObjectMacro (BoxWidget, vtkBoxWidget);

  virtual void Enable (void);

  virtual void Disable (void);
  
  /** Reset the pipeline, deactivate the CP box and release memory.*/
  virtual void Initialize (void);
  
  /** Set the vtkPolyData input */
  virtual void SetInput (vtkPolyData*);
    
  /** Get the vtkPolyData input */
  vtkGetObjectMacro (Input, vtkPolyData);

  /** Get the generated actor */
  virtual vtkActor* GetOutput (void) const;

  /** Get the ROI Limiter */
  virtual vtkLimitFibersToROI* GetROILimiter(void) const;

  /** Get the VOI Limiter */
  virtual vtkLimitFibersToVOI* GetVOILimiter(void) const;
  
  /** Set the render window interactor */
  virtual void SetRenderWindowInteractor (vtkRenderWindowInteractor* rwin);
  vtkGetObjectMacro (RenderWindowInteractor, vtkRenderWindowInteractor);
  
  /** Set the default renderer to put actors in. Must be called before SetInput() */
  vtkSetObjectMacro(Renderer, vtkRenderer);
  vtkGetObjectMacro(Renderer, vtkRenderer);

  /** Remove all actors added by this manager. */
  virtual void RemoveAllActors (void);
  
  /** Set the rendering mode to poly lines */
  virtual void SetRenderingModeToPolyLines (void);

  /** Set the rendering mode to tubes */
  virtual void SetRenderingModeToTubes (void);

  /** Set the rendering mode to ribbons */
  virtual void SetRenderingModeToRibbons (void);

  /** Get the rendering mode. */
  static int GetRenderingMode (void);
  
  virtual void SetRenderingMode(int);
  
  static void UseHardwareShadersOn(void);
  
  static void UseHardwareShadersOff(void);
  
  static void SetUseHardwareShaders(int);
  
  static int GetUseHardwareShaders(void);

  virtual void ChangeMapperToUseHardwareShaders(void);
  virtual void ChangeMapperToDefault(void);

  
  /** Set the ouput of the callback to ite input. Thus, users
      can extract a subsample of fibers, and then another subsample,
      and so on. */
  virtual void SwapInputOutput(void);

  /** Reset the fiber manager to its first input. */
  virtual void Reset (void);

  /** Switch on/off the visibility of the fibers */
  virtual void SetVisibility (bool);

  /** Set the box widget on */
  virtual void BoxWidgetOn (void);
  
  /** Set the box widget on */
  virtual void BoxWidgetOff (void);

  /** Set the box widget on */
  virtual void SetBoxWidget (bool a);
  
  /** Get the box widget visibility */
  vtkGetMacro (BoxWidgetVisibility, int);

  /** Set Radius & ribbon width */
  virtual void SetRadius (double r);

  virtual double GetRadius (void) const;

  /** Return the fiber ids selected by the box widget */
  virtual vtkCellArray* GetSelectedCells (void) const;

  virtual vtkPolyData *GetCallbackOutput(void) const;

  virtual void SetMaskImage (vtkImageData* image);
  
  /**
     Set the color mode to either local or global fiber orientation.
  */
  virtual void SetColorModeToLocalFiberOrientation (void);
  
  /**
     Set the color mode to either local or global fiber orientation.
   */
  virtual void SetColorModelToGlobalFiberOrientation (void);
  
  virtual void SetColorModeToPointArray (const int& id);
  
  const char* GetPointArrayName (const int& id) const;
  
  virtual int GetNumberOfPointArrays (void) const;
  
  virtual void SetLookupTable (vtkScalarsToColors* lut);
  
  virtual vtkScalarsToColors* GetLookupTable (void) const;
  
  virtual void SetMaximumNumberOfFibers(int num);
  
  virtual int GetMaximumNumberOfFibers(void) const;

  virtual void ShowHelpMessage (void);
  
  virtual void HideHelpMessage (void);
  
  virtual void SetHelpMessageVisibility (int a);
  
  virtual int GetHelpMessageVisibility (void) const;
  
  /** Force a callback call to Execute() */
  virtual void Execute (void);
  
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

  static int vtkUseHardwareShaders;
  static int vtkFiberRenderingStyle;

};



#endif
