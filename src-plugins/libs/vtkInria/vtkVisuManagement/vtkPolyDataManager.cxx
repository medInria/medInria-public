/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkPolyDataManager.h"
#include <vtkObjectFactory.h>


#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkActor.h>

#include "vtkRendererCollection.h"

vtkCxxRevisionMacro(vtkPolyDataManager, "$Revision: 477 $");
vtkStandardNewMacro(vtkPolyDataManager);

vtkPolyDataManager::vtkPolyDataManager()
{
  this->Renderer = 0;
  this->DefaultColor    = new double[4];
  this->DefaultColor[0] = 1;
  this->DefaultColor[1] = 0;
  this->DefaultColor[2] = 0;
  this->DefaultColor[3] = 1;
  
  this->Visibility      = 1;
  this->Opacity         = 1.0;  
}

vtkPolyDataManager::~vtkPolyDataManager()
{
  this->ResetData();
  
  delete [] this->DefaultColor;
  
  if( this->Renderer )
    this->Renderer->Delete();
  
}

void vtkPolyDataManager::AddPolyData (vtkPolyData* polydata)
{
  if (!polydata)
    return;
  
  // copy the polydata output

  vtkPolyDataNormals* algo = vtkPolyDataNormals::New();
//   algo->SplittingOn();
//   algo->ConsistencyOn();
//   algo->AutoOrientNormalsOn ();
  //algo->SetFeatureAngle(3.0);
  
  algo->SetInput (polydata);
  algo->Update();

  
  vtkPolyDataMapper* newMapper = vtkPolyDataMapper::New();
  newMapper->SetInput( algo->GetOutput() );
  
  vtkActor* newActor = vtkActor::New();
  newActor->SetMapper( newMapper );
  newActor->GetProperty()->SetColor( this->DefaultColor );
  newActor->SetVisibility (this->Visibility);
  newActor->GetProperty()->SetOpacity (this->Opacity);

  if( this->Renderer )
  {
    this->Renderer->AddActor ( newActor );
  }


  this->PolyDataList.push_back (polydata);
  this->ActorList.push_back ( newActor );
  

  newMapper->Delete();
  newActor->Delete();

}

void vtkPolyDataManager::RemovePolyData (vtkPolyData* polydata)
{

  std::vector<vtkPolyData*> temppolydatalist = this->PolyDataList;
  std::vector<vtkActor*>    tempactotrlist   = this->ActorList;
  this->PolyDataList.clear();
  this->ActorList.clear();

  vtkActor* actor = 0;
  
  for (unsigned int i=0;i<temppolydatalist.size();i++)
  {
    if (temppolydatalist[i] != polydata)
      this->PolyDataList.push_back (temppolydatalist[i]);
    else
      actor = this->GetActor (i);
  }
  
  for (unsigned int i=0;i<tempactotrlist.size();i++)
  {
    if (tempactotrlist[i] != actor)
      this->ActorList.push_back (tempactotrlist[i]);
    else
      if (this->Renderer && actor)
      {
	std::cout<<"removing"<<std::endl;
	this->Renderer->RemoveActor ( tempactotrlist[i] );
      }
    
  }
}



void vtkPolyDataManager::ResetData()
{  
  if (this->Renderer)
  {
    for (unsigned int i=0;i<this->ActorList.size();i++)
      this->Renderer->RemoveActor (this->ActorList[i]);
  }
  this->ActorList.clear();
  this->PolyDataList.clear();
  
}

void vtkPolyDataManager::SetVisibility (int IsVisible)
{
  this->Visibility = IsVisible;
  for (unsigned int i=0;i<this->ActorList.size();i++)
    this->ActorList[i]->SetVisibility (this->Visibility);
}


void vtkPolyDataManager::SetVisibility (unsigned int i, int IsVisible)
{
  if (i>this->ActorList.size())
    return;
  this->ActorList[i]->SetVisibility (IsVisible);
}


int vtkPolyDataManager::GetVisibility (unsigned int i) const
{
  if (i>this->ActorList.size())
    return -1;  
  return this->ActorList[i]->GetVisibility();
}


void vtkPolyDataManager::SetOpacity (double val)
{
  this->Opacity = val;
  for (unsigned int i=0;i<this->ActorList.size();i++)
    this->ActorList[i]->GetProperty()->SetOpacity (this->Opacity);
}


void vtkPolyDataManager::SetOpacity (unsigned int i, double val)
{
  if (i>this->ActorList.size())
    return;
  this->ActorList[i]->GetProperty()->SetOpacity (val);

}


double vtkPolyDataManager::GetOpacity(unsigned int i) const
{
  if (i>this->ActorList.size())
    return -1;  
  return this->ActorList[i]->GetProperty()->GetOpacity();
}




vtkActor* vtkPolyDataManager::GetActor (unsigned int i)
{
  if (i>this->ActorList.size())
    return NULL;  
  return this->ActorList[i];
}
 


vtkPolyData* vtkPolyDataManager::GetPolyData (unsigned int i)
{
  if (i>this->PolyDataList.size())
    return NULL;  
  return this->PolyDataList[i];
}
 
int vtkPolyDataManager::HasPolyData    (vtkPolyData* polydata)
{
  for (unsigned int i=0;i<this->PolyDataList.size();i++)
    if (this->PolyDataList[i] == polydata)
      return i;
  return -1;
}

  
