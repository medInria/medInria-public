/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkVectorVisuManager.h 804 2008-04-23 13:28:57Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-04-23 15:28:57 +0200 (Wed, 23 Apr 2008) $
Version:   $Revision: 804 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtk_VectorVisuManager_h_
#define _vtk_VectorVisuManager_h_

#include "vtkINRIA3DConfigure.h"

#include <vtkCommand.h>
#include <vtkBoxWidget.h>
#include <vtkDataSet.h>
#include <vtkPolyData.h>
#include <vtkPointSet.h>
#include <vtkUnstructuredGrid.h>

#include <vtkLimitVectorsToVOI.h>

#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkActor.h>
#include <vtkBox.h>

#include <vtkGlyph3D.h>

#include <vtkExtractGeometry.h>


class VTK_VISUMANAGEMENT_EXPORT vtkVectorVisuManagerCallback: public vtkCommand
{

 public:
  static vtkVectorVisuManagerCallback* New()
  { return new vtkVectorVisuManagerCallback; }

  virtual void Execute ( vtkObject *caller, unsigned long, void* );

  vtkUnstructuredGrid* GetOutput (void) const
  { return this->VectorLimiter->GetOutput(); }  

/*   vtkLimitVectorsToVOI* GetVectorLimiter (void) const */
/*   { return this->VectorLimiter; }   */
  vtkExtractGeometry* GetVectorLimiter (void) const
  { return this->VectorLimiter; }  

  
 protected:
  vtkVectorVisuManagerCallback()
  {
/*     this->VectorLimiter = vtkLimitVectorsToVOI::New();  */
    this->VectorLimiter = vtkExtractGeometry::New();
    this->Box = vtkBox::New();

    this->VectorLimiter->SetImplicitFunction (this->Box);
    
  }
  ~vtkVectorVisuManagerCallback()
  {
    this->VectorLimiter->Delete();
    this->Box->Delete();
    
  }

  
  
 private:
  //vtkLimitVectorsToVOI* VectorLimiter;
  vtkExtractGeometry* VectorLimiter;
  vtkBox* Box;
  
  
  
};


class VTK_VISUMANAGEMENT_EXPORT vtkVectorVisuManager : public vtkObject
{
 public:

  static vtkVectorVisuManager* New();
  vtkTypeRevisionMacro(vtkVectorVisuManager, vtkObject);

  enum RenderingModeIds
  {
    RENDER_IS_POLYLINES,
    RENDER_IS_TUBES,
    RENDER_IS_RIBBONS
  };

  /** Reset the pipeline, deactivate the CP box and release memory.*/
  void Initialize (void);
  
  /** Set the vtkDataSet input */
  void SetInput (vtkDataSet*);
    
  /** Get the vtkDataSet input */
  vtkDataSet* GetInput (void) const
  { return this->Input; }

  /** Set the generated actor */
  vtkDataSet* GetOutput (void) const
  { return this->Callback->GetOutput(); }
  vtkPolyData* GetOutput2 (void) const
  { return this->VectorFilter->GetOutput(); }
  vtkActor* GetActor(void)
  { return this->Actor; }

  vtkRenderWindowInteractor* GetRenderWindowInteractor (void)
  {
    return this->RWin;
  }
  

  /** Set the render window interactor */
  void SetRenderWindowInteractor (vtkRenderWindowInteractor* rwin)
  {
    if( !rwin )
    {
      return;
    }

    if( rwin != this->RWin )
    {

      if( this->RWin != NULL )
      {
        this->RWin->UnRegister (this);
      }

      this->BoxWidget->SetInteractor (rwin);
      this->RWin = rwin;
      this->RWin->Register(this);
    }
    
  }

  /** Set the rendering mode to poly lines */
  void SetRenderingModeToPolyLines (void)
  {
    this->RenderingMode = RENDER_IS_POLYLINES;
    this->Mapper->SetInput (this->Callback->GetOutput());
  }

/*   /\** Set the rendering mode to tubes *\/ */
/*   void SetRenderingModeToTubes (void) */
/*   { */
/*     m_RenderingMode = RENDER_IS_TUBES;     */
/*     m_TubeFilter->Update(); */
/*     m_Mapper->SetInput (m_TubeFilter->GetOutput());     */
/*   } */

/*     /\** Set the rendering mode to tubes *\/ */
/*   void SetRenderingModeToRibbons (void) */
/*   { */
/*     m_RenderingMode = RENDER_IS_RIBBONS; */
/*     m_RibbonFilter->Update(); */
/*     m_Mapper->SetInput (m_RibbonFilter->GetOutput()); */
/*   } */

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
    this->BoxWidgetVisibility = true;
    this->BoxWidget->On();
  }

  /** Set the box widget on */
  void BoxWidgetOff (void)
  {
    this->BoxWidgetVisibility = false;
    this->BoxWidget->Off();
  }

  /** Set the box widget on */
  void SetBoxWidget (bool a)
  {
    this->BoxWidgetVisibility = a;
    if(a)
      this->BoxWidget->On();
    else
      this->BoxWidget->Off();
  }

  /** Get the box widget visibility */
  bool GetBoxWidgetVisibility (void) const
  { return this->BoxWidgetVisibility; }

  /** Set Radius & ribbon width */
  void SetRadius (int r)
  {
    std::cout<<"to do"<<std::endl;
    
  }

/*   /\** Return the fiber ids selected by the box widget *\/ */
/*   vtkCellArray* GetSelectedCells (void) const */
/*   { return this->Callback->GetVectorLimiter()->GetOutput()->GetLines(); } */
  
 protected:
  vtkVectorVisuManager();
  ~vtkVectorVisuManager();

  void ProvideColorsToVectors (void);
  void NormalizeVectors (void);
  void ParseVectorFromCellsToPoints (void);
  void SetInputInternal(vtkDataSet* input);
  
  
   
 private:

  vtkVectorVisuManagerCallback* Callback;

  vtkDataSet*   Input;
  vtkBoxWidget* BoxWidget;
  bool          BoxWidgetVisibility;
  vtkGlyph3D*   VectorFilter;
  
  vtkDataSetMapper* Mapper;
  vtkActor*         Actor;
  RenderingModeIds  RenderingMode;

  vtkRenderWindowInteractor* RWin;

  //BTX
  std::string ScalarName;
  //ETX
  
};



#endif
