/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkDataManagerCallback.h"
#include <vtkObjectFactory.h>

#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCellPicker.h>
#include <vtkPointPicker.h>
#include <vtkDataSet.h>
#include <vtkActor.h>

#include <vtkMatrix4x4.h>
#include <vtkLinearTransform.h>

#include <vtkDataManager.h>
#include <vtkMetaDataSet.h>



vtkDataManagerCallback::vtkDataManagerCallback()
{
  this->DataManager = NULL;
}

vtkDataManagerCallback::~vtkDataManagerCallback()
{
  if (this->DataManager)
    this->DataManager->UnRegister (this);
}

void vtkDataManagerCallback::SetDataManager (vtkDataManager* manager)
{
  if (!manager)
    return;
  if (this->DataManager)
  {
    if (manager==this->DataManager)
      return;
    this->DataManager->UnRegister (this);
    this->DataManager = NULL;
  }

  this->DataManager = manager;
  this->DataManager->Register(this);
}




void vtkDataManagerCallback::Execute(vtkObject* caller, unsigned long event, void* callData)
{
  // std::cout<<"executin : event is "<<event<<std::endl;
  
  if (event == vtkCommand::LeftButtonPressEvent )
  {
    vtkRenderWindowInteractor* interactor = vtkRenderWindowInteractor::SafeDownCast(caller);
    if (!interactor)
      return;
    if (!interactor->GetRepeatCount())
      return;
      
    int X, Y;
    X = interactor->GetEventPosition()[0];
    Y = interactor->GetEventPosition()[1];
    
    vtkRenderer* renderer = interactor->FindPokedRenderer(X,Y);
    if (!renderer)
      return;

    vtkCellPicker*  cellpicker = vtkCellPicker::New();
    vtkPointPicker* pointpicker = vtkPointPicker::New();
    
    cellpicker->Pick(X,Y,0.0, renderer);
    pointpicker->Pick(X,Y,0.0, renderer);
    
    vtkDataSet* dataset1 = cellpicker->GetDataSet();
    vtkDataSet* dataset2 = pointpicker->GetDataSet();
    
    if (!dataset1 || !dataset2 || (dataset1 != dataset2))
    {
      cellpicker->Delete();
      pointpicker->Delete();
      return;
    }
    
    vtkActor* actor = cellpicker->GetActor();

    if (this->DataManager)
    {
      for (unsigned int i=0; i<this->DataManager->GetNumberOfMetaDataSet(); i++)
      {
	vtkMetaDataSet* metadataset = this->DataManager->GetMetaDataSet (i);
	for (unsigned int j=0; j<metadataset->GetNumberOfActors(); j++)
	{
	  
	  if (metadataset->GetActor (j) && (metadataset->GetActor (j) == actor))
	  {
	    metadataset->SetPickedPointId(pointpicker->GetPointId());
	    metadataset->SetPickedCellId(cellpicker->GetCellId());
	    this->DataManager->InvokeEvent(vtkDataManager::MetaDataSetPickEvent, metadataset);
	    break;
	  }
	}
      }
    }
    
    
    
    pointpicker->Delete();
    cellpicker->Delete();

    interactor->InvokeEvent(vtkCommand::LeftButtonReleaseEvent);
    

  }
  
}


