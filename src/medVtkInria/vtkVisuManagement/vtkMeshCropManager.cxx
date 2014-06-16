/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkMeshCropManager.h"

#include <vtkProperty.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkCellArray.h>
#include <vtkRenderWindow.h>

#include <vtkGlyph3D.h>

#include <sstream>
#include <assert.h>
#include <cmath>

#include <stdio.h>
#include <vtkCellData.h>
#include <vtkFloatArray.h>

vtkCxxRevisionMacro(vtkMeshCropManager, "$Revision: 805 $");
vtkStandardNewMacro(vtkMeshCropManager);

void vtkMeshCropManagerCallback::Execute ( vtkObject *caller, unsigned long, void* )
{
  // get the box widget
  vtkBoxWidget *widget = vtkBoxWidget::SafeDownCast (caller);

  // get the inputed vtkPolyData
  vtkUnstructuredGrid* input = (vtkUnstructuredGrid*)widget->GetInput();
  if( !input )
  {
    std::cout<<"ouch !..."<<std::endl;
    
    return;
  }
  
  // Get the poly data defined by the vtkBoxWidget
  vtkPolyData* myBox = vtkPolyData::New();
  widget->GetPolyData(myBox);
  
  // myBox contains 15 points and points 8 to 13
  // define the bounding box
  double xmin, xmax, ymin, ymax, zmin, zmax;
  double* pt = myBox->GetPoint(8);
  xmin = pt[0];
  pt = myBox->GetPoint(9);
  xmax = pt[0];
  pt = myBox->GetPoint(10);
  ymin = pt[1];
  pt = myBox->GetPoint(11);
  ymax = pt[1];
  pt = myBox->GetPoint(12);
  zmin = pt[2];
  pt = myBox->GetPoint(13);
  zmax = pt[2];

  this->Box->SetBounds(xmin, xmax, ymin, ymax, zmin, zmax);  
  this->MeshLimiter->Update();

  myBox->Delete();  
}



vtkMeshCropManager::vtkMeshCropManager()
{
  this->Input = 0;
  this->RWin = 0;
  this->BoxWidget    = vtkBoxWidget::New();
  this->Callback     = vtkMeshCropManagerCallback::New();
  this->RenderingMode = RENDER_IS_POLYLINES;

  this->BoxWidget->RotationEnabledOff();
  this->BoxWidget->SetPlaceFactor (1.0);
  this->BoxWidget->AddObserver (vtkCommand::InteractionEvent, this->Callback);
  this->BoxWidgetVisibility = true;

  this->BoxWidget->GetOutlineProperty()->SetColor (0.3,0.3,1.0);
  this->BoxWidget->GetHandleProperty()->SetColor (0.8,0.8,1.0);
}


vtkMeshCropManager::~vtkMeshCropManager()
{
  this->BoxWidget->RemoveObserver (this->Callback);
  this->BoxWidget->SetInteractor (NULL);

    
  this->BoxWidget->Delete();
  this->Callback->Delete();
  if (this->RWin)
  {
    this->RWin->Delete();
  }
  if (this->Input)
    this->Input->Delete();
  
  
}


void vtkMeshCropManager::SetInputInternal(vtkDataSet* input)
{

  
  if( !input )
    return;
  if( !input->GetNumberOfPoints() )
    return;
  vtkPointSet* pointset = vtkPointSet::SafeDownCast (input);
  if( !pointset )
    return;
  
  
  if (this->Input == input)
  {
    return;
  }
  
  if (this->Input)
  {
    this->Input->Delete();
    this->Input = NULL;
  }

  this->Input =  input;
  this->Input->Register(this);
  
  if (this->Input)
  {
    this->Initialize();
  }
  
}


void vtkMeshCropManager::SetInput(vtkDataSet* input)
{

  this->SetInputInternal (input);

  if(!this->GetInput())
    return;  
  
  this->Callback->GetMeshLimiter()->SetInput ( this->GetInput() );
  
  this->BoxWidget->SetInput ( this->GetInput() );
  this->BoxWidget->PlaceWidget();

  this->Callback->Execute (this->BoxWidget, 0, NULL);  
}


void vtkMeshCropManager::SwapInputOutput()
{
}


void vtkMeshCropManager::Reset()
{
  if( !this->GetInput() ) return;

  this->BoxWidget->SetInput ( this->GetInput() );
  this->Callback->GetMeshLimiter()->SetInput ( this->Callback->GetMeshLimiter()->GetOutput() );
  this->Callback->Execute (this->BoxWidget, 0, NULL);
}


void vtkMeshCropManager::SetVisibility (bool isVisible)
{
  if (isVisible)
    this->SetBoxWidget (this->BoxWidgetVisibility);
}

void vtkMeshCropManager::Initialize()
{
  if (this->Callback->GetMeshLimiter()->GetOutput())
    this->Callback->GetMeshLimiter()->GetOutput()->Initialize();
  this->Callback->GetMeshLimiter()->RemoveAllInputs();
  
}

