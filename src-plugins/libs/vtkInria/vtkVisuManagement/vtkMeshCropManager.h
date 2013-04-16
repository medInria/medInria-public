/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkVisuManagementExport.h"

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
#include <iostream>


//BTX
class VTK_VISUMANAGEMENT_EXPORT vtkMeshCropManagerCallback: public vtkCommand
{

 public:
  static vtkMeshCropManagerCallback* New()
  { return new vtkMeshCropManagerCallback; }

  virtual void Execute ( vtkObject *caller, unsigned long, void* );

  vtkUnstructuredGrid* GetOutput() const
  { return this->MeshLimiter->GetOutput(); }  

/*   vtkLimitVectorsToVOI* GetMeshLimiter() const */
/*   { return this->MeshLimiter; }   */
  vtkExtractGeometry* GetMeshLimiter() const
  { return this->MeshLimiter; }  

  
 protected:
  vtkMeshCropManagerCallback()
  {
/*     this->MeshLimiter = vtkLimitVectorsToVOI::New();  */
    this->MeshLimiter = vtkExtractGeometry::New();
    this->Box = vtkBox::New();

    this->MeshLimiter->SetImplicitFunction (this->Box);
    
  }
  ~vtkMeshCropManagerCallback()
  {
    this->MeshLimiter->Delete();
    this->Box->Delete();
    
  }

  
  
 private:
  //vtkLimitVectorsToVOI* MeshLimiter;
  vtkExtractGeometry* MeshLimiter;
  vtkBox* Box;
  
  
  
};
//ETX

class VTK_VISUMANAGEMENT_EXPORT vtkMeshCropManager : public vtkObject
{
 public:

  static vtkMeshCropManager* New();
  vtkTypeRevisionMacro(vtkMeshCropManager, vtkObject);

  //BTX
  enum RenderingModeIds
  {
    RENDER_IS_POLYLINES,
    RENDER_IS_TUBES,
    RENDER_IS_RIBBONS
  };
  //ETX

  /** Reset the pipeline, deactivate the CP box and release memory.*/
  void Initialize();
  
  /** Set the vtkDataSet input */
  void SetInput (vtkDataSet*);
    
  /** Get the vtkDataSet input */
  vtkDataSet* GetInput() const
  { return this->Input; }

  /** Set the generated actor */
  vtkDataSet* GetOutput() const
  { return this->Callback->GetOutput(); }
  vtkPolyData* GetOutput2() const
  { return this->VectorFilter->GetOutput(); }
  vtkActor* GetActor()
  { return this->Actor; }

  vtkRenderWindowInteractor* GetRenderWindowInteractor()
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
  void SetRenderingModeToPolyLines()
  {
    this->RenderingMode = RENDER_IS_POLYLINES;
    this->Mapper->SetInput (this->Callback->GetOutput());
  }

/*   /\** Set the rendering mode to tubes *\/ */
/*   void SetRenderingModeToTubes() */
/*   { */
/*     m_RenderingMode = RENDER_IS_TUBES;     */
/*     m_TubeFilter->Update(); */
/*     m_Mapper->SetInput (m_TubeFilter->GetOutput());     */
/*   } */

/*     /\** Set the rendering mode to tubes *\/ */
/*   void SetRenderingModeToRibbons() */
/*   { */
/*     m_RenderingMode = RENDER_IS_RIBBONS; */
/*     m_RibbonFilter->Update(); */
/*     m_Mapper->SetInput (m_RibbonFilter->GetOutput()); */
/*   } */

  /** Set the ouput of the callback to ite input. Thus, users
      can extract a subsample of fibers, and then another subsample,
      and so on. */
  void SwapInputOutput();

  /** Reset the fiber manager to its first input. */
  void Reset();

  /** Switch on/off the visibility of the fibers */
  void SetVisibility (bool);

  /** Set the box widget on */
  void BoxWidgetOn()
  {
    this->BoxWidgetVisibility = true;
    this->BoxWidget->On();
  }

  /** Set the box widget on */
  void BoxWidgetOff()
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
  bool GetBoxWidgetVisibility() const
  { return this->BoxWidgetVisibility; }

  /** Set Radius & ribbon width */
  void SetRadius (int r)
  {
    std::cout<<"to do"<<std::endl;
    
  }

/*   /\** Return the fiber ids selected by the box widget *\/ */
/*   vtkCellArray* GetSelectedCells() const */
/*   { return this->Callback->GetMeshLimiter()->GetOutput()->GetLines(); } */
  
 protected:
  vtkMeshCropManager();
  ~vtkMeshCropManager();

  void SetInputInternal(vtkDataSet* input);
  
  
   
 private:

  vtkMeshCropManagerCallback* Callback;

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




