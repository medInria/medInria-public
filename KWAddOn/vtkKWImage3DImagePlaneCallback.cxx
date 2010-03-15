/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxVtkImage3DImagePlaneCallback.cxx 477 2007-11-20 17:46:10Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-11-20 18:46:10 +0100 (Tue, 20 Nov 2007) $
Version:   $Revision: 477 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkKWImage3DImagePlaneCallback.h"

#include <vtkViewImage2D.h>
#include <vtkRenderWindow.h>
#include <vtkDataSet.h>
#include <vtkDataSet3DCroppingPlaneCallback.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkMetaDataSet.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkActor2D.h>
#include <vtkPolyData.h>
#include <vtkCutter.h>
#include <vtkPolyDataMapper.h>


vtkKWImage3DImagePlaneCallback::vtkKWImage3DImagePlaneCallback()
{
  this->ViewImage2D = 0;
  this->FirstRender = true;
//   this->DataSetCropper = vtkDataSet3DCroppingPlaneCallback::New();
}

vtkKWImage3DImagePlaneCallback::~vtkKWImage3DImagePlaneCallback()
{
//   this->DataSetCropper->Delete();
  this->ActorList.clear();
}

void vtkKWImage3DImagePlaneCallback::SetPlaneWidget (vtkPlaneWidget* widget)
{
//   this->DataSetCropper->SetPlaneWidget (widget);  
}
 
void vtkKWImage3DImagePlaneCallback::AddMetaDataSet (vtkMetaDataSet* metadataset)
{
  

  if( !this->ViewImage2D )
  {
    return;
  }


//   vtkMatrix4x4* matrix = vtkMatrix4x4::New();
//   for (unsigned int i=0; i<3; i++)
//   {
//     for (unsigned int j=0; j<3; j++)
//     {
//       matrix->SetElement(i,j,this->GetReslice()->GetResliceAxes()->GetElement(j,i));
//     }
//     matrix->SetElement(i,3,0);
//   }
//   matrix->SetElement(3,3,1);

//   vtkCutter* cutter = vtkCutter::New();
//   cutter->SetCutFunction (this->DataSetCropper->GetPlane());
  
//   // Very strangely in some cases (ex : landmarks)
//   // the cutter increments the RefCount of the input dataset by 2
//   // making some memory leek...
//   // I could not manage to know what is wrong here
  
//   cutter->SetInput (metadataset->GetDataSet());
//   cutter->Update();
  
//   if (!cutter->GetOutput())
//   {
//     std::cerr<<"unable to cut dataset"<<std::endl;
    
//     matrix->Delete();
//     cutter->Delete();
//     return;
//   }
  
  
//   vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
//   mapper->SetInput (cutter->GetOutput());
  
//   vtkActor* actor = vtkActor::New();
//   actor->SetUserMatrix (matrix);
//   actor->SetMapper (mapper);
//   actor->SetProperty (vtkProperty::SafeDownCast (metadataset->GetProperty()));
//   actor->PickableOff();
  
//   this->ViewImage2D->GetView()->AddActor (actor);

//   this->ActorList.push_back (actor);
  
//   this->ViewImage2D->GetView()->AddDataSet(metadataset->GetDataSet(), vtkProperty::SafeDownCast (metadataset->GetProperty()));
  
  
//   this->MetaDataSetList.push_back (metadataset);
//   unsigned int id = this->DataSetCropper->AddInput (metadataset->GetDataSet());

//   vtkPolyData* polydata = vtkPolyData::SafeDownCast (this->DataSetCropper->GetOutput (id));

//   vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
//   mapper->SetInput (polydata);
//   vtkActor* actor = vtkActor::New();
//   actor->SetMapper (mapper);
//   actor->SetProperty (vtkProperty::SafeDownCast (metadataset->GetProperty()));

//   vtkMatrix4x4* matrix = vtkMatrix4x4::New();
  
//   for (unsigned int i=0; i<3; i++)
//   {
//     for (unsigned int j=0; j<3; j++)
//     {
//       matrix->SetElement(i,j,this->GetReslice()->GetResliceAxes()->GetElement(j,i));
//     }
//     matrix->SetElement(i,3,0);
//   }
//   matrix->SetElement(3,3,1);

//   actor->SetUserMatrix (matrix);
  
//   this->ViewImage2D->GetView()->AddActor (actor);

//   this->ActorList.push_back (actor);
  
  
//   vtkPolyDataMapper2D* mapper = vtkPolyDataMapper2D::New();
//   vtkPolyData* polydata = vtkPolyData::SafeDownCast (this->DataSetCropper->GetOutput (id));
//   std::cout<<"1"<<std::endl;

//   this->ViewImage2D->GetView()->AddDataSet (polydata, vtkProperty::SafeDownCast (metadataset->GetProperty()));
  
//   mapper->SetInput (polydata);
  
//   vtkActor2D * actor = vtkActor2D::New();
//   std::cout<<"2"<<std::endl;
//   actor->SetMapper (mapper);

  
// //   vtkProperty* property = vtkProperty::SafeDownCast (metadataset->GetProperty());
// //   if (property)
// //     actor->SetProperty (property);

//   this->ViewImage2D->GetView()->AddActor (actor);
  
//   mapper->Delete();
//   actor->Delete();
  
  
  //metadataset->AddActor (actor);
}

void vtkKWImage3DImagePlaneCallback::RemoveMetaDataSet (vtkMetaDataSet* metadataset)
{
  std::cerr<<"not implemented"<<std::endl;
}


void vtkKWImage3DImagePlaneCallback::Execute ( vtkObject *caller, unsigned long, void*)
{

  if( !this->ViewImage2D )
  {
    return;
  }

  vtkImage3DImagePlaneCallback::Execute (caller, 0, NULL);
  // this->DataSetCropper->Execute (caller, 0, NULL);
  
  if( this->FirstRender )
  {
    this->ViewImage2D->GetView()->SetImage ( this->GetOutput() );
    this->ViewImage2D->GetView()->ResetCurrentPoint();
    this->ViewImage2D->GetView()->ResetWindowLevel();

    
    this->FirstRender = false;
  }

  /*  for (unsigned int n=0; n<this->ActorList.size(); n++)
  {
    
    vtkMatrix4x4* matrix = this->ActorList[n]->GetUserMatrix();
    matrix->DeepCopy (this->GetReslice()->GetResliceAxes());
    
    for (unsigned int i=0; i<3; i++)
    {
      for (unsigned int j=0; j<3; j++)
      {
	matrix->SetElement(i,j, this->GetReslice()->GetResliceAxes()->GetElement(j,i));
      }
//      matrix->SetElement(i,3,this->GetReslice()->GetResliceAxes()->GetElement(i,3));
    }
//     matrix->SetElement(3,3,this->GetReslice()->GetResliceAxes()->GetElement(3,3));
//     matrix->SetElement(3,3,1);
    
    matrix->Modified();

//     vtkMatrix4x4* matrix = vtkMatrix4x4::New();
//     matrix->DeepCopy (this->GetMatrix());
//     matrix->Transpose();
    
//     this->ActorList[n]->SetUserMatrix (matrix);
    
  }
  
//   for (unsigned int i=0
*/
  this->ViewImage2D->DeIconify();
  this->ViewImage2D->Raise();
  
  this->ViewImage2D->GetView()->Update();
  if( !this->ViewImage2D->GetView()->GetRenderWindow()->GetNeverRendered() )
  {
    this->ViewImage2D->GetView()->Render();
  }
}
