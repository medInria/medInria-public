/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkRenderingAddOn/vtkDataSet3DCroppingPlaneCallback.h>

#include <vtkPlaneWidget.h>
#include <vtkDataSet.h>
#include <vtkUnstructuredGrid.h>
#include <vtkCutter.h>

void vtkDataSet3DCroppingPlaneCallback::Execute ( vtkObject *caller, unsigned long, void* )
{
  
  if( !this->PlaneWidget )
  {
    return;
  }

  std::cout<<"here 1"<<std::endl;
  
  this->PlaneWidget->GetPlane (this->Plane);
  
}


unsigned int vtkDataSet3DCroppingPlaneCallback::AddInput (vtkDataSet* dataset)
{
  if (!dataset)
    return 9999;
  
  if( !this->PlaneWidget )
    return 9999;

  if (!this->Plane)
    this->Plane = vtkPlane::New();
  
  this->PlaneWidget->GetPlane (this->Plane);
  if (!this->Plane)
    return 9999;
  
  vtkCutter* clipper = vtkCutter::New();
  clipper->SetInput (dataset);
  clipper->SetCutFunction (this->Plane);
  clipper->Update();
  std::cout<<"cutting"<<std::endl;

  this->InputList.push_back (dataset);
  if (!clipper->GetOutput())
  {
    return 9999;
  }
  
  this->OutputList.push_back (clipper->GetOutput());
  clipper->GetOutput()->Register(this);
  
  clipper->Delete();
  
  return (this->InputList.size()-1);
  
}

unsigned int  vtkDataSet3DCroppingPlaneCallback::RemoveInput (vtkDataSet* dataset)
{
  return 9999;
}

vtkDataSet* vtkDataSet3DCroppingPlaneCallback::GetInput (unsigned int i)
{
  if (i >= this->InputList.size())
    return NULL;

  return this->InputList[i];
  
}
vtkDataSet* vtkDataSet3DCroppingPlaneCallback::GetOutput (unsigned int i)
{
  
  if (i >= this->OutputList.size())
    return NULL;

  return this->OutputList[i];
  
}
