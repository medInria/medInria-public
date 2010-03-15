/*=========================================================================

  Program:   vtkINRIA3D
  Module:    $Id: vtkLandmarkManager.cxx 1315 2009-11-03 12:13:28Z acanale $
  Language:  C++
  Author:    $Author: acanale $
  Date:      $Date: 2009-11-03 13:13:28 +0100 (Tue, 03 Nov 2009) $
  Version:   $Revision: 1315 $

  Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
  See Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

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

#include <vtkViewImage2D.h>
#include <vtkViewImage3D.h>

#include <sstream>
#include <fstream>
#include <time.h>

vtkCxxRevisionMacro(vtkLandmarkManager, "$Revision: 1315 $");
vtkStandardNewMacro(vtkLandmark);
vtkCxxRevisionMacro(vtkLandmark, "$Revision: 1315 $");
vtkStandardNewMacro(vtkLandmarkManager);


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


void vtkLandmark::Initialize (void)
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


void vtkLandmark::UpdateCommentPosition (void)
{
  this->TextActor->SetPosition (this->GetPosition());  
  this->TextActor->SetScale (this->GetScale());  
}


void vtkLandmark::UpdateCrossPosition (void)
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


void vtkLandmark::UpdateSpherePosition (void)
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

std::vector<std::string> vtkLandmark::GetFlagKeys (void)
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

void vtkLandmark::RemoveAllActors (void)
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

double vtkLandmark::GetScale (void)
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

void vtkLandmark::UnSelect (void)
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













vtkLandmarkManager::vtkLandmarkManager()
{
  this->Callback = vtkLandmarkManagerCallback::New();
  this->ViewList = vtkCollection::New();  
  this->LandmarkList = vtkCollection::New();
  this->Callback->SetViewList (this->ViewList);
  this->Callback->SetLandmarkManager (this);
  
  this->SelectedProperty = vtkProperty::New();
  this->Property = vtkProperty::New();

  this->SelectedProperty->SetColor (1,0,0);
  this->Property->SetColor (0,1,0);
  this->SelectedProperty->SetPointSize(6);
  this->Property->SetPointSize(4);
  this->SelectedProperty->SetLineWidth(6);
  this->Property->SetLineWidth(4);
  this->SelectedProperty->SetRepresentationToSurface();
  this->Property->SetRepresentationToSurface();

  this->AutoSelectOn();
  this->ChangeColorOnSelectedOn();
  this->SetColorMode(vtkLandmarkManager::LANDMARK_COLOR_NONE);  

  this->Linker = vtkPolyData::New();

  vtkPoints* points = vtkPoints::New();
  this->Linker->SetPoints (points);
  points->Delete();
  this->Linker->Allocate();

  this->LinkerVisibility = 0;
  this->ChangeColorOnSelected = 1;

  this->Scale = 2;

  this->LastPickedLandmark   = 0;
  this->LandmarkBeingRemoved = 0;
  this->ViewIdToTrust = 0;
  this->LandmarkType = vtkLandmark::LANDMARK_CROSS;
  
}


vtkLandmarkManager::~vtkLandmarkManager()
{

  vtkDebugMacro (<<"deleting landmark manager");
  
  this->InteractionOff();  
  
  this->Reset();
  this->RemoveAllViews();
  
  this->Property->Delete();
  this->SelectedProperty->Delete();
  this->Callback->Delete();
  this->LandmarkList->Delete();
  this->ViewList->Delete();

  this->Linker->Delete();

}

void vtkLandmarkManager::Reset()
{
  this->RemoveAllLandmarks();

  for (int j=0; j<this->ViewList->GetNumberOfItems(); j++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->ViewList->GetItemAsObject(j));  
    
    if (view)
      view->RemoveDataSet (this->Linker);
  }


}



void vtkLandmarkManager::ReadFile (const char* filename)
{


  // open the file:
  std::ifstream buffer (filename);
    
  if(buffer.fail())
  {
    std::cerr << "Unable to open file: " << filename << std::endl;
    throw vtkErrorCode::CannotOpenFileError;
  }

  this->Reset();
  char header[1024];
  
  buffer.getline(header, 1024);
  
  char tmp[26];
  strncpy(tmp, header, 25);
  tmp[25]='\0';
  
  if (strcmp(tmp, "LANDMARK MANAGER FILE 1.4") != 0)
  {
      buffer.close();
    this->ReadFileOld (filename);
    return;
    }

  // Get number of landmarks
  unsigned int N_landmarks = 0;
  buffer >> N_landmarks;

  for (unsigned int i=0; i<N_landmarks; i++)
  {

    // Create the new landmark
    vtkLandmark* landmark = vtkLandmark::New();
    landmark->SetLandmarkType (this->GetLandmarkType());
    
//     landmark->GetProperty()->DeepCopy(this->Property);
//     landmark->GetSelectedProperty()->DeepCopy(this->SelectedProperty);

    std::string name;
    buffer >> name;

    landmark->SetName (name.c_str());
    
    double coord[3];    
    for (unsigned int j = 0; j < 3; j++)
      buffer >> coord[j];
    landmark->SetPosition (coord);


    double color[3];    
    for (unsigned int j = 0; j < 3; j++)
      buffer >> color[j];
    landmark->SetColor (color[0], color[1], color[2]);

    double scale = 2;
    buffer >> scale;
    landmark->SetScale (scale);
    
    unsigned int N_Flags = 0;
    buffer >> N_Flags;

    // Set its flags
    for (unsigned int j = 0; j < N_Flags; j++)
    {
      std::string key;
      double val = 0;
      buffer >> key >> val;
      landmark->SetFlag(key.c_str(), val);
    }
    
    this->AddLandmark (landmark);

    if (landmark->GetLandmarkType() == vtkLandmark::LANDMARK_SPHERE)
    {
      this->AddSphereWidgetToLandmark (landmark);
    }
    
    landmark->Delete();
    
  }

  buffer.close();

  this->Update();
}


void vtkLandmarkManager::ReadFileOld (const char* filename)
{
  // open the file:
  std::ifstream buffer (filename);

  if(buffer.fail())
  {
    std::cerr << "Unable to open file: " << filename << std::endl;
    throw vtkErrorCode::CannotOpenFileError;
  }

  this->Reset();
  
  // Get number of landmarks
  unsigned int N_landmarks = 0;
  buffer >> N_landmarks;

  // Get number of flags
  unsigned int N_flags = 0;
  buffer >> N_flags;

	
  for (unsigned int i=0; i<N_landmarks; i++)
  {
    double coord[3];
    
    // Create the new landmark
    for (unsigned int j = 0; j < 3; j++)
    {
      if(buffer.fail())
      {
	std::cerr << "Unable to read in file: " << filename << std::endl;
	buffer.close();
	throw vtkErrorCode::CannotOpenFileError;
      }
      buffer >> coord[j];
    }

    vtkLandmark* landmark = this->CreateAndAddLandmark();
	if( !landmark )
	{
	  std::cerr << "Cannot create landmarks!" << std::endl;
	  return;
	}
    landmark->SetPosition (coord);

    // Set its flags
    for (unsigned int j = 0; j < N_flags; j++)
    {
      double v = 0;
      buffer >> v;
      std::ostringstream os;
      os << "flag-"<<j;
      
      landmark->SetFlag(os.str().c_str(), v);
    }
  }

  buffer.close();

  this->Update();
}




void vtkLandmarkManager::WriteFile (const char* filename)
{

  // open the file:
  std::ofstream buffer (filename);
  if( buffer.fail() )
  {
    std::cerr << "Unable to open file: " << filename << std::endl;
    buffer.close();
    throw vtkErrorCode::CannotOpenFileError;
  }  

  // header
  buffer << "LANDMARK MANAGER FILE 1.4" << std::endl;
  
  // Write the number of landmarks
  unsigned int N_landmarks = this->GetNumberOfLandmarks();
  buffer << N_landmarks << std::endl;

  for (unsigned int i=0; i<N_landmarks; i++)
  {
    std::string name = this->GetLandmark (i)->GetName();
    buffer << name.c_str() << std::endl;

    double* coord = this->GetLandmark (i)->GetPosition();    
    for (unsigned int j=0; j<3; j++)
      buffer << coord[j] <<" ";
    buffer << std::endl;

    double* color = this->GetLandmark (i)->GetColor();    
    for (unsigned int j=0; j<3; j++)
      buffer << color[j] <<" ";
    buffer << std::endl;

    double scale = this->GetLandmark (i)->GetScale();
    buffer << scale << std::endl;
    
    std::vector<std::string> keys = this->GetLandmark (i)->GetFlagKeys();
    buffer << keys.size() <<std::endl;
    
    for (unsigned int j=0; j<keys.size(); j++)
    {
      buffer << keys[j].c_str() <<" ";
	  buffer << this->GetLandmark (i)->GetFlag (keys[j].c_str())<<std::endl;
    }
    buffer << std::endl;
  }

  buffer.close();
  
}



void vtkLandmarkManager::AddLandmark (vtkLandmark* landmark)
{

  for (int i=0; i<this->ViewList->GetNumberOfItems(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->ViewList->GetItemAsObject(i));  
    vtkViewImage2D* view2D = vtkViewImage2D::SafeDownCast (view);
    if (view2D)
    {
      vtkActor* actor = view->AddDataSet(landmark, landmark->GetProperty2D());
      if (actor)
      {
	landmark->AddActor2D (actor);
      }
    }
    else if (landmark->GetLandmarkType() != vtkLandmark::LANDMARK_SPHERE)
    {
      vtkActor* actor = view->AddDataSet(landmark, landmark->GetProperty());
      if (actor)      
	landmark->AddActor (actor);
    }
//     view->Update();
  }

  this->LandmarkList->AddItem (landmark);

  this->UpdateLinker();
  
  landmark->AddObserver (vtkCommand::ModifiedEvent, this->Callback);
  this->InvokeEvent (vtkLandmarkManagerCallback::LandmarkAddedEvent, landmark);
  landmark->Modified();
  landmark->SetPosition (landmark->GetPosition());
  landmark->Modified();
}


vtkLandmark* vtkLandmarkManager::CreateAndAddLandmark (void)
{

  if( !this->ViewList->GetNumberOfItems() )
  {
    return NULL;
  }
  
  vtkLandmark* landmark = vtkLandmark::New();
  landmark->SetLandmarkType (this->GetLandmarkType());

  vtkViewImage2D* view = vtkViewImage2D::SafeDownCast (this->ViewList->GetItemAsObject(this->GetViewIdToTrust()));
  const double* pos = view->GetCurrentPoint();

  
  landmark->SetPosition(pos);
  double currentpoint[3];
  currentpoint[0] = pos[0];
  currentpoint[1] = pos[1];
  currentpoint[2] = pos[2];

  int voxelcoord[3];
  view->GetVoxelCoordinates(currentpoint, voxelcoord);

  landmark->SetVoxelCoord(voxelcoord);
  
  // Set a unique name to the new landmark (to be optimized)
  unsigned int ok = 0;
  unsigned int nb = 0;

  std::string name;
  while ( !ok )
  {
    std::ostringstream os;
    os << "Landmark#" << nb;
    if ( !this->FindLandmark(os.str().c_str()) )
    {
      ok = 1;
      name = os.str();
    }
    nb++;
  }

  
  landmark->SetName(name.c_str());

  // We check that there is no landmark at this position
  /**
     FIX-ME :
     if this test remains, then
     if the user add a landmark intentionnaly at 0,0,0
     then he cannot add any other landmarks...
     therefore : do not perform this test
  */
  /*
  for (int i = 0; i < this->LandmarkList->GetNumberOfItems(); i++)
  {
    vtkLandmark* testLandmark = vtkLandmark::SafeDownCast (this->LandmarkList->GetItemAsObject(i));
    
    double* c1 = landmark->GetPosition();
    double* c2 = testLandmark->GetPosition();
    if ( (c1[0] == c2[0]) && (c1[1] == c2[1]) && (c1[2] == c2[2]) )
    {
      landmark->Delete();
      return NULL;
    }
  }
  */

  if (this->ColorMode == vtkLandmarkManager::LANDMARK_COLOR_RANDOM)
  {
    srand (clock());
    
    // put a random color
    int i1 = 1 + (int) (100.0 * ((double)rand() / (RAND_MAX + 1.0)));
    int i2 = 1 + (int) (100.0 * ((double)rand() / (RAND_MAX + 1.0)));
    int i3 = 1 + (int) (100.0 * ((double)rand() / (RAND_MAX + 1.0)));
    double rand_1 = (double)(i1)/(100.0);
    double rand_2 = (double)(i2)/(100.0);
    double rand_3 = (double)(i3)/(100.0);
    double r = rand_1, g = rand_2, b = rand_3;
    if (i1 < 33)
      r = 1;
    else if (i1 < 66)
      g = 1;
    else if (i1 < 100)
      b = 1;

    landmark->SetColor (r,g,b);
  }
  else if (this->ColorMode == vtkLandmarkManager::LANDMARK_COLOR_LUT)
  {
    vtkLookupTable* lut = vtkLookupTableManager::GetROILookupTable();
    double color[4];
    lut->GetTableValue (this->GetNumberOfLandmarks()+1, color);
    landmark->SetColor (color[0], color[1], color[2]);
    lut->Delete();
  }

  
  landmark->SetScale (this->Scale);

  this->AddLandmark (landmark);

  if ( this->AutoSelect )
    this->SelectLandmark (landmark);

  if (landmark->GetLandmarkType() == vtkLandmark::LANDMARK_SPHERE)
  {
    this->AddSphereWidgetToLandmark (landmark);
  }
  
  landmark->Delete();
  
  //this->InvokeEvent (vtkLandmarkManagerCallback::LandmarkAddedEvent, landmark);
  
  
  return this->FindLandmark (name.c_str());

  //this->Update(); // a quoi sert un appel fonction apres un return???
}


void vtkLandmarkManager::AddSphereWidgetToLandmark (vtkLandmark* landmark)
{

  vtkViewImage3D* view3D = vtkViewImage3D::SafeDownCast (this->ViewList->GetItemAsObject (3));
  if (!view3D)
    return;
  
  vtkSphereWidget* ptwidget = landmark->GetSphereWidget();
  
  ptwidget->SetInteractor(view3D->GetRenderWindowInteractor());
  ptwidget->SetEnabled (1);

  vtkActor* actor = view3D->GetDataSetActor (landmark);
  if (actor)
  {
    view3D->RemoveDataSet (landmark);
    landmark->RemoveActor (actor);
  }
  
  ptwidget->AddObserver (vtkCommand::InteractionEvent, this->Callback);
  ptwidget->AddObserver (vtkCommand::StartInteractionEvent, this->Callback);
  ptwidget->AddObserver (vtkCommand::EndInteractionEvent, this->Callback);

  landmark->Initialize();
  
}


void vtkLandmarkManager::RemoveAllLandmarks(void)
{

  for (int i=0; i<this->LandmarkList->GetNumberOfItems(); i++)
  {
    vtkLandmark* landmark =  vtkLandmark::SafeDownCast (this->LandmarkList->GetItemAsObject(i));
  
    if (!landmark)
    {
      continue;
    }

    landmark->RemoveAllActors();
    
    for (int j=0; j<this->ViewList->GetNumberOfItems(); j++)
    {
      vtkViewImage* view = vtkViewImage::SafeDownCast ( this->ViewList->GetItemAsObject(j) );
      
      if (view)
      {
        view->RemoveDataSet (landmark);
	if (landmark->GetSphereWidget() && landmark->GetSphereWidget()->GetInteractor())
	  landmark->GetSphereWidget()->SetEnabled (0);
	if (landmark->GetUseComment())
	  landmark->UseCommentOff();      
      } 
    }
    
    landmark->RemoveObserver(this->Callback);
    
  }
  this->LandmarkList->RemoveAllItems();
  this->LastPickedLandmark = 0;

  this->UpdateLinker();
  
}



void vtkLandmarkManager::RemoveLandmark(vtkLandmark* landmark)
{
  if (!landmark)
  {
    return;
  }

  this->LandmarkBeingRemoved = landmark;

  this->InvokeEvent (vtkLandmarkManagerCallback::LandmarkRemovingEvent, landmark);
  
  landmark->RemoveAllActors();
  
  
  for (int j=0; j<this->ViewList->GetNumberOfItems(); j++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->ViewList->GetItemAsObject(j));  

    if (view)
    {
      view->RemoveDataSet (landmark);
      if (landmark->GetSphereWidget() && landmark->GetSphereWidget()->GetInteractor())
	landmark->GetSphereWidget()->SetEnabled (0);
      if (landmark->GetUseComment())
	landmark->UseCommentOff();      
    }
    
  }

  landmark->RemoveObserver(this->Callback);

  this->InvokeEvent (vtkLandmarkManagerCallback::LandmarkRemovedEvent, landmark);
 
  this->LandmarkList->RemoveItem (landmark);
  this->LandmarkBeingRemoved = 0;
  if( this->LastPickedLandmark == landmark )
  {
    this->LastPickedLandmark = 0;
  }
  

  this->UpdateLinker();

}




void vtkLandmarkManager::AddView (vtkViewImage* view)
{
  if (!this->HasView(view))
  {
    this->ViewList->AddItem (view);
  }
}



void vtkLandmarkManager::RemoveView (vtkViewImage* view)
{
  this->ViewList->RemoveItem (view);
}


void vtkLandmarkManager::RemoveAllViews (void)
{
  this->ViewList->RemoveAllItems();
}



vtkViewImage* vtkLandmarkManager::GetFirstView (void)
{
  vtkViewImage* view = vtkViewImage::SafeDownCast (this->ViewList->GetItemAsObject (0));
  return view;  
}



bool vtkLandmarkManager::HasView(vtkViewImage* view)
{
  return this->ViewList->IsItemPresent (view);
}



vtkLandmark* vtkLandmarkManager::GetLandmark (unsigned int i)
{
  return vtkLandmark::SafeDownCast (this->LandmarkList->GetItemAsObject (i));
}




vtkLandmark* vtkLandmarkManager::GetLastLandmark (void)
{
  if (!this->GetNumberOfLandmarks())
    return NULL;
  return this->GetLandmark(this->GetNumberOfLandmarks() - 1);
}




vtkLandmark* vtkLandmarkManager::FindLandmark(const char* name)
{
  for (int i=0; i<this->LandmarkList->GetNumberOfItems(); i++)
  {
    vtkLandmark* landmark = vtkLandmark::SafeDownCast (this->LandmarkList->GetItemAsObject(i));
    
    if ( strcmp (landmark->GetName(), name) == 0 )
      return landmark;
  }
  
  return NULL;
}


  
void vtkLandmarkManager::ShowAll(void)
{
  for (int j=0; j<this->ViewList->GetNumberOfItems(); j++)
  {
    for (int i=0; i<this->LandmarkList->GetNumberOfItems(); i++)
    {
      vtkViewImage* view = vtkViewImage::SafeDownCast (this->ViewList->GetItemAsObject(j));
      vtkLandmark* landmark = vtkLandmark::SafeDownCast (this->LandmarkList->GetItemAsObject(i));
      vtkActor* actor = view->GetDataSetActor (landmark);
      if (actor)
	actor->SetVisibility (true);
    }
  }
}

void vtkLandmarkManager::HideAll(void)
{
  for (int j=0; j<this->ViewList->GetNumberOfItems(); j++)
  {
    for (int i=0; i<this->LandmarkList->GetNumberOfItems(); i++)
    {
      vtkViewImage* view = vtkViewImage::SafeDownCast (this->ViewList->GetItemAsObject(j));
      vtkLandmark* landmark = vtkLandmark::SafeDownCast (this->LandmarkList->GetItemAsObject(i));
      vtkActor* actor = view->GetDataSetActor (landmark);
      if (actor)
	actor->SetVisibility (false);
    }
  }
}


void vtkLandmarkManager::ScaleAll (double scale)
{
  for (unsigned int i=0; i<this->GetNumberOfLandmarks(); i++)
  {
    vtkLandmark* t_landmark = this->GetLandmark (i);
    t_landmark->SetScale (scale);
  }
  
  this->Scale = scale;

  
}

  
    

void vtkLandmarkManager::InteractionOn(void)
{
  for (int i=0; i<this->ViewList->GetNumberOfItems(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->ViewList->GetItemAsObject(i));
    if (view && view->GetRenderWindowInteractor())
    {
      if ( !view->GetRenderWindowInteractor()->HasObserver(vtkCommand::MiddleButtonPressEvent, this->Callback) )
      {
	view->GetRenderWindowInteractor()->AddObserver(vtkCommand::MiddleButtonPressEvent, this->Callback);
	view->GetRenderWindowInteractor()->AddObserver(vtkCommand::MiddleButtonReleaseEvent, this->Callback);
	view->GetRenderWindowInteractor()->AddObserver(vtkCommand::RightButtonPressEvent, this->Callback);
	view->GetRenderWindowInteractor()->AddObserver(vtkCommand::RightButtonReleaseEvent, this->Callback);
      }
    }
    vtkViewImage2D* view2d = vtkViewImage2D::SafeDownCast (view);
    
    if (view2d)
    {
      view2d->SetMiddleButtonInteractionStyle (vtkViewImage2D::NO_INTERACTION);
      view2d->SetRightButtonInteractionStyle (vtkViewImage2D::NO_INTERACTION);
    }    
    
  }
}

void vtkLandmarkManager::InteractionOff(void)
{
  for (int i=0; i<this->ViewList->GetNumberOfItems(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast (this->ViewList->GetItemAsObject(i));  
    if (view && view->GetRenderWindowInteractor())
      view->GetRenderWindowInteractor()->RemoveObserver(this->Callback);
    vtkViewImage2D* view2d = vtkViewImage2D::SafeDownCast (view);
    
    if (view2d)
    {
      view2d->SetMiddleButtonInteractionStyle (view2d->GetInteractionStyle());
      view2d->SetRightButtonInteractionStyle (view2d->GetInteractionStyle());
    }
    
  }
}




void vtkLandmarkManager::SelectLandmark(vtkLandmark* landmark)
{

  this->SetLastPickedLandmark (landmark);
  for (unsigned int i=0; i<this->GetNumberOfLandmarks(); i++)
  {
    vtkLandmark* t_landmark = this->GetLandmark (i);
    if (landmark == t_landmark)
    {
      t_landmark->Select(this->GetChangeColorOnSelected());
    }
    else
    {
      t_landmark->UnSelect();
    }
  }
  landmark->Modified();

//   if (this->ViewList->GetNumberOfItems())
//   {
//     vtkViewImage* view = vtkViewImage::SafeDownCast (this->ViewList->GetItemAsObject(0));
//     view->SyncRender();
//   }  
}


vtkLandmark* vtkLandmarkManager::GetFirstSelectedLandmark (void)
{
  for (unsigned int i=0; i<this->GetNumberOfLandmarks(); i++)
  {
    vtkLandmark* t_landmark = this->GetLandmark (i);
    if (t_landmark->IsSelected())
      return t_landmark;
  }
  return NULL;
  
}


void vtkLandmarkManager::SetSelectedColor(double r, double g, double b)
{
  this->SelectedProperty->SetColor (r,g,b);
  for (unsigned int i=0; i<this->GetNumberOfLandmarks(); i++)
  {
    vtkLandmark* t_landmark = this->GetLandmark (i);
    t_landmark->SetSelectedColor(r,g,b);

  }
}

void vtkLandmarkManager::SetSelectedColor(double c[3])
{
  this->SetSelectedColor (c[0], c[1], c[2]);
}

  



void vtkLandmarkManager::Update()
{
  if (!this->ViewList->GetNumberOfItems())
    return;

  int voxelcoord[3];
  vtkViewImage* view = vtkViewImage::SafeDownCast(this->ViewList->GetItemAsObject(0));
  if ( !view )
    return;
  
  for (unsigned int i=0; i < this->GetNumberOfLandmarks(); i++)
  {
    double* coord = this->GetLandmark(i)->GetPosition();
    view->GetVoxelCoordinates(coord, voxelcoord);
    this->GetLandmark(i)->SetVoxelCoord(voxelcoord);
  }
  
}



void vtkLandmarkManager::AddLandmarkToLinker (vtkLandmark* landmark)
{


  vtkPoints* points = this->Linker->GetPoints();
  if (!points)
  {
    points = vtkPoints::New();
    this->Linker->SetPoints (points);
    points->Delete();
  }
  
  vtkIdType id = this->Linker->GetPoints()->InsertNextPoint (landmark->GetPosition());
  
  if (this->Linker->GetNumberOfPoints() <= 1)
    return;
  
  vtkIdList* ids = vtkIdList::New();
  ids->InsertNextId (id-1);
  ids->InsertNextId (id);
  this->Linker->InsertNextCell(VTK_LINE, ids);
  this->Linker->Modified();    
  ids->Delete();
  
}


void vtkLandmarkManager::LinkerOn(void)
{
  this->LinkerVisibility = true;
  
  for (int i=0; i < this->ViewList->GetNumberOfItems(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast(this->ViewList->GetItemAsObject(i));
    if ( view )
    {
      vtkActor* actor = view->GetDataSetActor (this->Linker);
      if (actor)
	actor->SetVisibility (this->LinkerVisibility);
    }
  }

}

void vtkLandmarkManager::LinkerOff(void)
{
  this->LinkerVisibility = false;
  
  for (int i=0; i < this->ViewList->GetNumberOfItems(); i++)
  {
    vtkViewImage* view = vtkViewImage::SafeDownCast(this->ViewList->GetItemAsObject(i));
    if ( view )
    {
      vtkActor* actor = view->GetDataSetActor (this->Linker);
      if (actor)
	actor->SetVisibility (this->LinkerVisibility);
    }
  }
}

void vtkLandmarkManager::UpdateLinker(void)
{
  if (!this->LinkerVisibility)
    return;
  
  this->Linker->GetPoints()->Initialize();
  this->Linker->GetLines()->Reset();
  
  for(unsigned int i=0;i<this->GetNumberOfLandmarks();i++)
  {
    this->AddLandmarkToLinker(this->GetLandmark(i));
  }

  if (this->Linker->GetNumberOfPoints() <= 1)
  {
    if (this->ViewList->GetNumberOfItems())
    {
      vtkViewImage* view = vtkViewImage::SafeDownCast (this->ViewList->GetItemAsObject(0));
      view->SyncRemoveDataSet(this->Linker);
    }  
  }
  if (this->Linker->GetNumberOfPoints() == 2)
  {
    if (this->ViewList->GetNumberOfItems())
    {
      
      vtkViewImage* view = vtkViewImage::SafeDownCast (this->ViewList->GetItemAsObject(0));
      view->SyncAddDataSet(this->Linker, this->GetSelectedProperty());
    }  
  }

}




  
