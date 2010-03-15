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

#include <vtkCommand.h>
#include <vtkBoxWidget.h>
#include <vtkPolyData.h>
#include <vtkLimitFibersToVOI.h>
#include <vtkLimitFibersToROI.h>
#include <vtkSqueezeFibers.h>
#include <vtkTubeFilter.h>
#include <vtkRibbonFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkScalarsToColors.h>
#include <vtkMaskPolyData.h>
#include <vtkProperty.h>
#include <vtkCornerAnnotation.h>
#include <vtkCleanPolyData.h>

class vtkFibersManager;

class VTK_VISUMANAGEMENT_EXPORT vtkFibersManagerCallback: public vtkCommand
{

 public:
  static vtkFibersManagerCallback* New()
  { return new vtkFibersManagerCallback; }

  virtual void Execute ( vtkObject *caller, unsigned long, void* );

  vtkPolyData* GetOutput (void) const
  { return this->FiberLimiter->GetOutput(); }
  

  vtkLimitFibersToVOI* GetFiberLimiter (void) const
  { return this->FiberLimiter; }
  
  vtkLimitFibersToROI* GetROIFiberLimiter (void) const
  { return this->ROIFiberLimiter; }
  
    
 protected:
  vtkFibersManagerCallback()
  {
    this->FiberLimiter    = vtkLimitFibersToVOI::New();
    this->ROIFiberLimiter = vtkLimitFibersToROI::New();
    this->FiberLimiter->SetInputConnection ( this->ROIFiberLimiter->GetOutputPort() );
    
  }
  ~vtkFibersManagerCallback()
  {
    this->FiberLimiter->Delete();
    this->ROIFiberLimiter->Delete();
  }
  
  
 private:
  vtkLimitFibersToVOI* FiberLimiter;
  vtkLimitFibersToROI* ROIFiberLimiter;
  
};



class VTK_VISUMANAGEMENT_EXPORT vtkFiberPickerCallback: public vtkCommand
{
 public:
  static vtkFiberPickerCallback* New()
  { return new vtkFiberPickerCallback; }
  
  virtual void Execute ( vtkObject*, unsigned long, void*);
  
  void SetInput (vtkPolyData* input)
  {
    if( !input )
    {
      return;
    }
    
    m_Input = input;
  }
  
  vtkPolyData* GetInput (void) const
  { return m_Input; }

  void SetFiberImage (vtkPolyData* image)
  { m_FiberImage = image; }

  vtkPolyData* GetFiberImage (void) const
  { return m_FiberImage; }

  void SetFibersManager (vtkFibersManager* manager)
  { m_FibersManager = manager; }
  
  
  vtkActor* GetPickedActor (void) const
  { return m_PickedActor; }

  void DeletePickedCell();
  
  
 protected:
  vtkFiberPickerCallback()
  {
    m_Input        = 0;
    m_FiberImage   = 0;
    m_FibersManager= 0;
    m_PickedFiber  = vtkPolyData::New();
    m_PickedMapper = vtkPolyDataMapper::New();
    m_PickedActor  = vtkActor::New();

    m_PickedMapper->SetInput ( m_PickedFiber );
    m_PickedActor->SetMapper ( m_PickedMapper );
    m_PickedActor->GetProperty()->SetColor ( 1.0, 0.0, 0.0 );
    m_PickedActor->GetProperty()->SetLineWidth ( 5.0 );
	
    m_PickedFiber->Initialize();
    m_PickedFiber->Allocate();
    
    m_PickedCellId = -1;
    
  }
  ~vtkFiberPickerCallback()
  {
    m_PickedFiber->Delete();
    m_PickedMapper->Delete();
    m_PickedActor->Delete();
  }
  
 private:
  
  vtkPolyData*       m_Input;
  vtkPolyData*       m_FiberImage;

  vtkPolyData*       m_PickedFiber;
  vtkPolyDataMapper* m_PickedMapper;
  vtkActor*          m_PickedActor;

  int                m_PickedCellId;

  vtkFibersManager*  m_FibersManager;
};



class VTK_VISUMANAGEMENT_EXPORT vtkFiberKeyboardCallback: public vtkCommand
{
  public:
  static vtkFiberKeyboardCallback* New()
  { return new vtkFiberKeyboardCallback; }
  
  virtual void Execute ( vtkObject*, unsigned long, void*);

  void SetFiberPickerCallback (vtkFiberPickerCallback* callback)
  { m_FiberPickerCallback = callback; }
  
  void SetFiberManager(vtkFibersManager* man)
  { m_FibersManager = man; }
  

 protected:
  vtkFiberKeyboardCallback()
  {
    m_FiberPickerCallback = 0;
    m_FibersManager = 0;
  }
  ~vtkFiberKeyboardCallback(){};
  

 private:
  vtkFiberPickerCallback* m_FiberPickerCallback;
  vtkFibersManager*       m_FibersManager;
  
};


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


  vtkBoxWidget* GetBoxWidget (void)
  { return this->m_BoxWidget; }
  
  /** Reset the pipeline, deactivate the CP box and release memory.*/
  void Initialize (void);
  
  /** Set the vtkPolyData input */
  void SetInput (vtkPolyData*);
    
  /** Get the vtkPolyData input */
  vtkPolyData* GetInput (void) const
  { return m_Input; }

  /** Set the generated actor */
  vtkActor* GetOutput (void) const
  { return m_Actor; }

  /** Get the ROI Limiter */
  vtkLimitFibersToROI* GetROILimiter(void) const
  { return m_Callback->GetROIFiberLimiter(); }

  /** Get the VOI Limiter */
  vtkLimitFibersToVOI* GetVOILimiter(void) const
  { return m_Callback->GetFiberLimiter(); }

  vtkRenderWindowInteractor* GetRenderWindowInteractor (void)
  {
    return this->m_RWin;
  }
  
  /** Set the render window interactor */
  void SetRenderWindowInteractor (vtkRenderWindowInteractor* rwin);
	
	/** Set the default renderer to put actors in. Must be called before SetInput() */
	vtkSetObjectMacro(Renderer, vtkRenderer);
	vtkGetObjectMacro(Renderer, vtkRenderer);

  /** Remove all actors added by this manager. */
  void RemoveAllActors (void);
  
  
  /** Set the rendering mode to poly lines */
  void SetRenderingModeToPolyLines (void);


  /** Set the rendering mode to tubes */
  void SetRenderingModeToTubes (void);


  /** Set the rendering mode to ribbons */
  void SetRenderingModeToRibbons (void);


  /** Get the rendering mode. */
  static int GetRenderingMode (void);

  
  void SetRenderingMode(int);
  
  static void UseHardwareShadersOn(void)
  { vtkFibersManager::SetUseHardwareShaders(1); }
  
  static void UseHardwareShadersOff(void)
  { vtkFibersManager::SetUseHardwareShaders(0); }
  
  static void SetUseHardwareShaders(int);
  
  static int GetUseHardwareShaders(void);

  void ChangeMapperToUseHardwareShaders(void);
  void ChangeMapperToDefault(void);
  

  /** Set the ouput of the callback to ite input. Thus, users
      can extract a subsample of fibers, and then another subsample,
      and so on. */
  void SwapInputOutput(void);

  /** Reset the fiber manager to its first input. */
  void Reset (void);

  /** Switch on/off the visibility of the fibers */
  void SetVisibility (bool);

  /** Set the box widget on */
  void BoxWidgetOn (void)
  {
    m_BoxWidgetVisibility = true;
    m_BoxWidget->On();
  }

  /** Set the box widget on */
  void BoxWidgetOff (void)
  {
    m_BoxWidgetVisibility = false;
    m_BoxWidget->Off();
  }

  /** Set the box widget on */
  void SetBoxWidget (bool a)
  {
    m_BoxWidgetVisibility = a;
    if(a)
      m_BoxWidget->On();
    else
      m_BoxWidget->Off();
  }

  /** Get the box widget visibility */
  bool GetBoxWidgetVisibility (void) const
  { return m_BoxWidgetVisibility; }

  /** Set Radius & ribbon width */
  void SetRadius (double r)
  {
    m_TubeFilter->SetRadius (r);
    m_RibbonFilter->SetWidth (r);
  }

  /** Return the fiber ids selected by the box widget */
  vtkCellArray* GetSelectedCells (void) const
  { return m_Callback->GetFiberLimiter()->GetOutput()->GetLines(); }
  
	vtkPolyData *GetCallbackOutput(void) const
	{ return m_Callback->GetOutput(); }
	
  void SetMaskImage (vtkImageData* image)
  {
    m_Callback->GetROIFiberLimiter()->SetMaskImage (image);
    m_Callback->GetROIFiberLimiter()->Modified();
  }


  /**
     Set the color mode to either local or global fiber orientation.
   */
  void SetColorModeToLocalFiberOrientation (void)
  {
    m_Mapper->SetScalarModeToUsePointData();
  }
  
  /**
     Set the color mode to either local or global fiber orientation.
   */
  void SetColorModelToGlobalFiberOrientation (void)
  {
    m_Mapper->SetScalarModeToUseCellData();
  }

  void SetColorModeToPointArray (const int& id)
  {
    if( id >= m_Mapper->GetInput()->GetPointData()->GetNumberOfArrays() )
    {
      return;
    }

    m_Mapper->SetScalarModeToUsePointFieldData();
    m_Mapper->SelectColorArray (id);
  }

  const char* GetPointArrayName (const int& id) const
  {
    if( id >= m_Mapper->GetInput()->GetPointData()->GetNumberOfArrays() )
    {
      return 0;
    }

    return m_Mapper->GetInput()->GetPointData()->GetArrayName (id);
  }

  int GetNumberOfPointArrays (void) const
  {
    return m_Mapper->GetInput()->GetPointData()->GetNumberOfArrays();
  }
  
  void SetLookupTable (vtkScalarsToColors* lut)
  {
    m_Mapper->SetLookupTable (lut);
  }

  vtkScalarsToColors* GetLookupTable (void) const
  {
    return m_Mapper->GetLookupTable();
  }
  
  void SetMaximumNumberOfFibers(int num)
  {
	this->MaximumNumberOfFibers = num;
  }

  int GetMaximumNumberOfFibers(void) const
  {
	return this->MaximumNumberOfFibers;
  }


  void ShowHelpMessage (void)
  {
    m_HelpMessage->SetVisibility (1);
  }

  void HideHelpMessage (void)
  {
    m_HelpMessage->SetVisibility (0);
  }

  void SetHelpMessageVisibility (int a)
  {
    if( a )
    {
      this->ShowHelpMessage();
    }
    else
    {
      this->HideHelpMessage();
    }
  }

  int GetHelpMessageVisibility (void) const
  { return m_HelpMessage->GetVisibility(); }
  
  
  /** Force a callback call to Execute() */
  void Execute (void)
  {
    m_Callback->GetROIFiberLimiter()->Modified();
    m_Callback->GetFiberLimiter()->Modified();
    m_Callback->GetFiberLimiter()->Update();
    //m_Callback->Execute(m_BoxWidget, 0, 0);
  }

  
 protected:
  vtkFibersManager();
  ~vtkFibersManager();
  
   
 private:

  vtkPolyData*              m_Input;
  vtkBoxWidget*             m_BoxWidget;
  bool                      m_BoxWidgetVisibility;

  vtkFibersManagerCallback* m_Callback;
  //vtkSqueezeFibers*         m_Squeezer;
  vtkMaskPolyData*          m_Squeezer;
  vtkCleanPolyData*         m_Cleaner;
  vtkTubeFilter*            m_TubeFilter;
  vtkRibbonFilter*          m_RibbonFilter;
  vtkPolyDataMapper*        m_Mapper;
  vtkActor*                 m_Actor;
  vtkCornerAnnotation*      m_HelpMessage;
  //vtkFiberRenderingMode     m_RenderingMode;

  vtkFiberPickerCallback*   m_PickerCallback;

  vtkFiberKeyboardCallback*  m_KeyboardCallback;
  
  vtkRenderWindowInteractor* m_RWin;
	vtkRenderer *Renderer;

  int MaximumNumberOfFibers;
  
};



#endif
