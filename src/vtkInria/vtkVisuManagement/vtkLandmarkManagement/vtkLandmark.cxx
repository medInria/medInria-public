/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkLandmarkManager.h"
#include <vtkLandmarkManagerCallback.h>

#include <vtkObjectFactory.h>

#include <vtkWorldPointPicker.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkMapper.h>
#include <vtkErrorCode.h>
#include <vtkInteractorStyle.h>
#include <vtkImageData.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkCellType.h>
#include <vtkLookupTableManager.h>
#include <vtkPolyDataWriter.h>
#include "vtkSphereSource.h"
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSphereWidget.h>
#include <vtkTextSource.h>
#include <vtkMatrixToLinearTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkFollower.h>
#include <vtkVectorText.h>
#include <vtkPolyDataMapper.h>

#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkImageViewCollection.h>


#include <sstream>
#include <fstream>
#include <time.h>

vtkStandardNewMacro(vtkLandmark);
vtkCxxRevisionMacro(vtkLandmark, "$Revision: 1315 $");


vtkLandmark::vtkLandmark()
{
  this->SetName("Landmark#0");

  this->Property = vtkProperty::New();
  this->Property->SetColor(1.0,1.0,0.0);
  this->Property->SetPointSize(4);
  this->Property->SetLineWidth(4);
  this->Property->SetRepresentationToSurface();

  this->Property2D = vtkProperty::New();
  this->Property2D->DeepCopy (this->Property);  
  this->Property2D->SetAmbient (1);
  this->Property2D->SetDiffuse (0);
  this->Property2D->SetSpecular (0);
  
  this->SelectedProperty = vtkProperty::New();  
  this->SelectedProperty->SetColor(1.0,0.0,0.0);
  this->SelectedProperty->SetPointSize(4);
  this->SelectedProperty->SetLineWidth(7);
  this->SelectedProperty->SetRepresentationToSurface();
  
  this->SelectedProperty2D = vtkProperty::New();
  this->SelectedProperty2D->DeepCopy (this->SelectedProperty);  
  this->SelectedProperty2D->SetAmbient (1);
  this->SelectedProperty2D->SetDiffuse (0);
  this->SelectedProperty2D->SetSpecular (0);
  
  this->ActorList = vtkActorCollection::New();
  this->Actor2DList = vtkActorCollection::New();

  this->SetLandmarkType (vtkLandmark::LANDMARK_CROSS);

  this->VoxelCoord[0] = 0;
  this->VoxelCoord[1] = 0;
  this->VoxelCoord[2] = 0;

  this->Position[0] = 0;
  this->Position[1] = 0;
  this->Position[2] = 0;
  
  this->Scale = 2;
  this->SelectFlag = 0;
  
  this->Source = vtkSphereSource::New();
  this->SphereWidget = vtkSphereWidget::New();

  this->TextMaker = vtkVectorText::New();
  this->TextActor = vtkFollower::New();
  
  this->Initialize();
  

}

vtkLandmark::~vtkLandmark()
{
  vtkDebugMacro (<<"deleting landmark "<<this->GetName());
  this->Property->Delete();
  this->SelectedProperty->Delete();
  this->Property2D->Delete();
  this->SelectedProperty2D->Delete();

  this->Source->Delete();
  
  this->ActorList->Delete();
  this->Actor2DList->Delete();

  this->SphereWidget->Delete();
  this->TextActor->Delete();

  this->TextMaker->Delete();
   
}




void vtkLandmark::SetUseComment (int val)
{
  this->UseComment = val;
  this->ConfigureComment();
}



void vtkLandmark::ConfigureComment ()
{
  
  this->TextMaker->SetText (this->Comment.c_str());
  this->TextActor->SetVisibility (this->UseComment);
  
}


void vtkLandmark::Initialize()
{
  
  if (this->GetLandmarkType() == vtkLandmark::LANDMARK_SPHERE)
  {
    this->Source->SetThetaResolution(16);
    this->Source->SetPhiResolution (8);
    this->Source->LatLongTessellationOn();
    this->Source->SetCenter (0,0,0);
    this->Source->SetRadius (2);
    this->Source->Update();
    this->DeepCopy (this->Source->GetOutput());
    
    if (this->SphereWidget->GetInteractor())
      this->SphereWidget->SetEnabled (1);
    this->SphereWidget->SetRepresentationToSurface();
    this->SphereWidget->GetSphereProperty()->DeepCopy (this->GetProperty());
    this->SphereWidget->GetSelectedSphereProperty()->DeepCopy (this->GetSelectedProperty());  
    this->SphereWidget->ScaleOff();
    if (this->SphereWidget->GetInteractor())
      this->SphereWidget->PlaceWidget(this->GetBounds());
    this->SphereWidget->SetCenter (this->GetPosition());
    this->SphereWidget->SetRadius (this->GetScale());
    this->SphereWidget->SetCenter (this->GetPosition());
  }
  else
  {
    vtkPoints* points = vtkPoints::New();
    double pt[3];
    pt[0] = 0;
    pt[1] = 0;
    pt[2] = 0;
    points->InsertNextPoint(pt);
    
    pt[0] = -0.5;
    pt[1] = 0.5;
    pt[2] = 0;
    points->InsertNextPoint(pt);
    pt[0] = 0.5;
    pt[1] = 0.5;
    pt[2] = 0;
    points->InsertNextPoint(pt);
    pt[0] = 0.5;
    pt[1] = -0.5;
    pt[2] = 0;
    points->InsertNextPoint(pt);
    pt[0] = -0.5;
    pt[1] = -0.5;
    pt[2] = 0;
    points->InsertNextPoint(pt);

  
    pt[0] = -0.5;
    pt[1] = 0;
    pt[2] = 0.5;
    points->InsertNextPoint(pt);
    pt[0] = 0.5;
    pt[1] = 0;
    pt[2] = 0.5;
    points->InsertNextPoint(pt);
    pt[0] = 0.5;
    pt[1] = 0;
    pt[2] = -0.5;
    points->InsertNextPoint(pt);
    pt[0] = -0.5;
    pt[1] = 0;
    pt[2] = -0.5;
    points->InsertNextPoint(pt);

  
    pt[0] = 0;
    pt[1] = -0.5;
    pt[2] = 0.5;
    points->InsertNextPoint(pt);
    pt[0] = 0;
    pt[1] = 0.5;
    pt[2] = 0.5;
    points->InsertNextPoint(pt);
    pt[0] = 0;
    pt[1] = 0.5;
    pt[2] = -0.5;
    points->InsertNextPoint(pt);
    pt[0] = 0;
    pt[1] = -0.5;
    pt[2] = -0.5;
    points->InsertNextPoint(pt);

    this->SetPoints (points);
    points->Delete();

    this->Allocate();
  

    vtkIdList* ids = vtkIdList::New();
    ids->InsertNextId (1);
    ids->InsertNextId (2);
    ids->InsertNextId (3);
    ids->InsertNextId (4);
    this->InsertNextCell(VTK_QUAD, ids);
    ids->Delete();
  
    vtkIdList* ids2 = vtkIdList::New();
    ids2->InsertNextId (5);
    ids2->InsertNextId (6);
    ids2->InsertNextId (7);
    ids2->InsertNextId (8);
    this->InsertNextCell(VTK_QUAD, ids2);
    ids2->Delete();
  
    vtkIdList* ids3 = vtkIdList::New();
    ids3->InsertNextId (9);
    ids3->InsertNextId (10);
    ids3->InsertNextId (11);
    ids3->InsertNextId (12);
    this->InsertNextCell(VTK_QUAD, ids3);
    ids3->Delete();
  }

  this->TextMaker->SetText (this->Comment.c_str());
  vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
  mapper->SetInput(this->TextMaker->GetOutput());
  mapper->ScalarVisibilityOff();
  this->TextActor->SetMapper (mapper);
  this->TextActor->GetProperty()->SetColor (0,0,0);
  this->TextActor->SetScale (this->Scale);
  this->TextActor->SetPosition (this->GetPosition());
  this->TextActor->SetVisibility (this->GetUseComment());
  mapper->Delete();
}

    
  

double* vtkLandmark::GetPosition()
{
  return this->Position;
}


void vtkLandmark::UpdateCommentPosition()
{
  this->TextActor->SetPosition (this->GetPosition());  
  this->TextActor->SetScale (this->GetScale());  
}


void vtkLandmark::UpdateCrossPosition()
{
  double* position = this->GetPosition();
  
  double pt[3];

  this->GetPoints()->SetPoint (0, position);

  pt[0] = position[0] - this->Scale;
  pt[1] = position[1];
  pt[2] = position[2];
  this->GetPoints()->SetPoint (1, pt);
  pt[0] = position[0] ;
  pt[1] = position[1] + this->Scale;
  pt[2] = position[2];
  this->GetPoints()->SetPoint (2, pt);
  pt[0] = position[0] + this->Scale;
  pt[1] = position[1] ;
  pt[2] = position[2];
  this->GetPoints()->SetPoint (3, pt);
  pt[0] = position[0] ;
  pt[1] = position[1] - this->Scale;
  pt[2] = position[2];
  this->GetPoints()->SetPoint (4, pt);

  pt[0] = position[0] - this->Scale;
  pt[1] = position[1];
  pt[2] = position[2] ;
  this->GetPoints()->SetPoint (5, pt);
  pt[0] = position[0] ;
  pt[1] = position[1];
  pt[2] = position[2] + this->Scale;
  this->GetPoints()->SetPoint (6, pt);
  pt[0] = position[0] + this->Scale;
  pt[1] = position[1];
  pt[2] = position[2] ;
  this->GetPoints()->SetPoint (7, pt);
  pt[0] = position[0] ;
  pt[1] = position[1];
  pt[2] = position[2] - this->Scale;
  this->GetPoints()->SetPoint (8, pt);

  pt[0] = position[0] ;
  pt[1] = position[1] - this->Scale;
  pt[2] = position[2] ;
  this->GetPoints()->SetPoint (9, pt);
  pt[0] = position[0] ;
  pt[1] = position[1] ;
  pt[2] = position[2] + this->Scale;
  this->GetPoints()->SetPoint (10, pt);
  pt[0] = position[0];
  pt[1] = position[1] + this->Scale;
  pt[2] = position[2] ;
  this->GetPoints()->SetPoint (11, pt);
  pt[0] = position[0];
  pt[1] = position[1] ;
  pt[2] = position[2] - this->Scale;
  this->GetPoints()->SetPoint (12, pt);

}


void vtkLandmark::UpdateSpherePosition()
{
  this->Source->SetCenter (this->GetPosition());
  this->Source->SetRadius (this->GetScale());
  
  this->Source->Update();
  
//   this->GetPoints()->SetData (this->Source->GetOutput()->GetPoints()->GetData());
//   this->SetPoints(this->Source->GetOutput()->GetPoints());
  this->ShallowCopy(this->Source->GetOutput());
  
  this->SphereWidget->SetCenter (this->GetPosition());
  this->SphereWidget->SetRadius (this->GetScale());
  
}


void vtkLandmark::SetColor(double r, double g, double b)
{ 
  this->Property->SetColor(r,g,b); 
  this->Property2D->SetColor(r,g,b);

  
  this->SphereWidget->GetSphereProperty()->SetColor (r,g,b);
  
  
}
void vtkLandmark::SetSelectedColor(double r, double g, double b)
{ 
  this->SelectedProperty->SetColor(r,g,b); 
  this->SelectedProperty2D->SetColor(r,g,b); 
  if (this->SphereWidget)
  {
    this->SphereWidget->GetSelectedSphereProperty()->SetColor (r,g,b);
  }
  
}

//----------------------------------------------------------------------------
void vtkLandmark::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "Name : " << this->Name.c_str() <<std::endl;
  os << indent << "Scale : " << this->Scale <<std::endl;
  os << indent << "Select Flag : " << this->SelectFlag <<std::endl;
  os << indent << "Position : [ " << this->GetPosition()[0] <<"; "<< this->GetPosition()[1] <<"; "<<this->GetPosition()[2] <<" ]"<<std::endl;
  os << indent << "Voxel coord : [ " << this->GetVoxelCoord()[0] <<"; "<< this->GetVoxelCoord()[1] <<"; "<<this->GetVoxelCoord()[2] <<" ]"<<std::endl;
  std::vector<std::string> keys = this->GetFlagKeys();
  os << indent << "Flags :"<<std::endl;
  for (unsigned int j=0; j<keys.size(); j++)
  {
    os << indent << indent << keys[j].c_str() <<" ";
    os << indent << indent << this->GetFlag (keys[j].c_str()) << endl;
  }
  os << indent << "DataSet :"<<std::endl;
  this->Superclass::PrintSelf(os,indent);
  
}


void vtkLandmark::SetPosition (const double val[3])
{
  this->Position[0] = val[0];
  this->Position[1] = val[1];
  this->Position[2] = val[2];
  
  switch(this->GetLandmarkType())
  {
      case vtkLandmark::LANDMARK_SPHERE :	
	this->UpdateSpherePosition();
	break;
      case vtkLandmark::LANDMARK_CROSS :
	this->UpdateCrossPosition();
	break;
      default :
	break;
  }

  if (this->UseComment)
    this->UpdateCommentPosition();
  
  this->Modified();  
}

void vtkLandmark::SetFlag (const char* key, double val)
{
  std::string skey = key;
  this->FlagMap[skey] = val;
}

double vtkLandmark::GetFlag (const char* key)
{
  std::string skey = key;
  return this->FlagMap[skey];
}

bool vtkLandmark::HasFlag (const char* key)
{
  return (this->FlagMap.find(key) != this->FlagMap.end());
}

std::vector<std::string> vtkLandmark::GetFlagKeys()
{
  std::vector<std::string> ans;

  for (std::map<std::string, double>::const_iterator it = this->FlagMap.begin(); it != this->FlagMap.end(); ++it)
  {
    ans.push_back( (*it).first );
  }

  return ans;
}

void vtkLandmark::AddActor (vtkActor* actor)
{
  actor->PickableOn();
  actor->DragableOn();
  actor->SetProperty (this->Property);
  this->ActorList->AddItem (actor);
}

void vtkLandmark::AddActor2D (vtkActor* actor)
{
  actor->PickableOn();
  actor->DragableOn();
  actor->SetProperty (this->Property2D);
  this->Actor2DList->AddItem (actor);
}

void vtkLandmark::RemoveActor (vtkActor* actor)
{
  this->ActorList->RemoveItem (actor);
  this->Actor2DList->RemoveItem (actor);
  
}

void vtkLandmark::RemoveAllActors()
{
  this->ActorList->RemoveAllItems ();
  this->Actor2DList->RemoveAllItems ();
}


void vtkLandmark::SetScale (double scale)
{
  this->Scale = scale;
  
  switch(this->GetLandmarkType())
  {
      case vtkLandmark::LANDMARK_SPHERE :
	this->UpdateSpherePosition();
	break;
      case vtkLandmark::LANDMARK_CROSS :
	this->UpdateCrossPosition();
	break;
      default :
	break;
  }
  
  if (this->UseComment)
    this->UpdateCommentPosition();
  
  this->Modified();  
  
}

double vtkLandmark::GetScale()
{
  return this->Scale;
}

void vtkLandmark::Select (bool changecolor)
{
  if (!changecolor)
  {
    this->SelectedProperty->SetColor (this->Property->GetColor());
    this->SelectedProperty2D->SetColor (this->Property2D->GetColor());
  }
  
  for (int i=0; i<this->ActorList->GetNumberOfItems(); i++)
  {
    vtkActor* actor = vtkActor::SafeDownCast (this->ActorList->GetItemAsObject (i));
    actor->SetProperty (this->SelectedProperty);
  }
  for (int i=0; i<this->Actor2DList->GetNumberOfItems(); i++)
  {
    vtkActor* actor = vtkActor::SafeDownCast (this->Actor2DList->GetItemAsObject (i));
    actor->SetProperty (this->SelectedProperty2D);
  }

  this->SphereWidget->GetSphereProperty()->SetColor (this->SelectedProperty->GetColor());
  this->SelectFlag = 1;
}

void vtkLandmark::UnSelect()
{
  for (int i=0; i<this->ActorList->GetNumberOfItems(); i++)
  {
    vtkActor* actor = vtkActor::SafeDownCast (this->ActorList->GetItemAsObject (i));
    actor->SetProperty (this->Property);
  }

  for (int i=0; i<this->Actor2DList->GetNumberOfItems(); i++)
  {
    vtkActor* actor = vtkActor::SafeDownCast (this->Actor2DList->GetItemAsObject (i));
    actor->SetProperty (this->Property2D);
  }

  this->SphereWidget->GetSphereProperty()->SetColor (this->Property->GetColor());
  this->SelectFlag = 0;
}
