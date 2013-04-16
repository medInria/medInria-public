/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkLandmarkManager.h"

#include <vtkLandmark.h>
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

vtkCxxRevisionMacro(vtkLandmarkManager, "$Revision: 1315 $");
vtkStandardNewMacro(vtkLandmarkManager);



vtkLandmarkManager::vtkLandmarkManager()
{
  this->Callback = vtkLandmarkManagerCallback::New();
  this->ViewList = vtkImageViewCollection::New();  
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
    vtkImageView* view = vtkImageView::SafeDownCast (this->ViewList->GetItemAsObject(j));
    
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
    vtkImageView* view = vtkImageView::SafeDownCast (this->ViewList->GetItemAsObject(i));  
    vtkImageView2D* view2D = vtkImageView2D::SafeDownCast (view);
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


vtkLandmark* vtkLandmarkManager::CreateAndAddLandmark()
{

  if( !this->ViewList->GetNumberOfItems() )
  {
    return NULL;
  }
  
  vtkLandmark* landmark = vtkLandmark::New();
  landmark->SetLandmarkType (this->GetLandmarkType());

  vtkImageView2D* view = vtkImageView2D::SafeDownCast (this->ViewList->GetItemAsObject(this->GetViewIdToTrust()));
  const double* pos = view->GetCurrentPoint();

  
  landmark->SetPosition(pos);
  double currentpoint[3];
  currentpoint[0] = pos[0];
  currentpoint[1] = pos[1];
  currentpoint[2] = pos[2];

  int voxelcoord[3];
  view->GetImageCoordinatesFromWorldCoordinates(currentpoint, voxelcoord);

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

  vtkImageView3D* view3D = vtkImageView3D::SafeDownCast (this->ViewList->GetItemAsObject (3));
  if (!view3D)
    return;
  
  vtkSphereWidget* ptwidget = landmark->GetSphereWidget();
  
  ptwidget->SetInteractor(view3D->GetInteractor());
  ptwidget->SetEnabled (1);

  vtkProp3D* actor = view3D->FindDataSetActor (landmark);
  if (actor)
  {
    view3D->RemoveDataSet (landmark);
    landmark->RemoveActor (vtkActor::SafeDownCast (actor));
  }
  
  ptwidget->AddObserver (vtkCommand::InteractionEvent, this->Callback);
  ptwidget->AddObserver (vtkCommand::StartInteractionEvent, this->Callback);
  ptwidget->AddObserver (vtkCommand::EndInteractionEvent, this->Callback);

  landmark->Initialize();
  
}


void vtkLandmarkManager::RemoveAllLandmarks()
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
      vtkImageView* view = vtkImageView::SafeDownCast ( this->ViewList->GetItemAsObject(j) );
      
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
    vtkImageView* view = vtkImageView::SafeDownCast (this->ViewList->GetItemAsObject(j));  

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




void vtkLandmarkManager::AddView (vtkImageView* view)
{
  if (!this->HasView(view))
  {
    this->ViewList->AddItem (view);
  }
}



void vtkLandmarkManager::RemoveView (vtkImageView* view)
{
  this->ViewList->RemoveItem (view);
}


void vtkLandmarkManager::RemoveAllViews()
{
  this->ViewList->RemoveAllItems();
}



vtkImageView* vtkLandmarkManager::GetFirstView()
{
  vtkImageView* view = vtkImageView::SafeDownCast (this->ViewList->GetItemAsObject (0));
  return view;  
}



bool vtkLandmarkManager::HasView(vtkImageView* view)
{
  return this->ViewList->IsItemPresent (view);
}



vtkLandmark* vtkLandmarkManager::GetLandmark (unsigned int i)
{
  return vtkLandmark::SafeDownCast (this->LandmarkList->GetItemAsObject (i));
}

vtkLandmark* vtkLandmarkManager::GetLastLandmark()
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
  
void vtkLandmarkManager::ShowAll()
{
  for (int j=0; j<this->ViewList->GetNumberOfItems(); j++)
  {
    for (int i=0; i<this->LandmarkList->GetNumberOfItems(); i++)
    {
      vtkImageView* view = vtkImageView::SafeDownCast (this->ViewList->GetItemAsObject(j));
      vtkLandmark* landmark = vtkLandmark::SafeDownCast (this->LandmarkList->GetItemAsObject(i));
      vtkProp3D* actor = view->FindDataSetActor (landmark);
      if (actor)
  	actor->SetVisibility (true);
    }
  }
}

void vtkLandmarkManager::HideAll()
{
  for (int j=0; j<this->ViewList->GetNumberOfItems(); j++)
  {
    for (int i=0; i<this->LandmarkList->GetNumberOfItems(); i++)
    {
      vtkImageView* view = vtkImageView::SafeDownCast (this->ViewList->GetItemAsObject(j));
      vtkLandmark* landmark = vtkLandmark::SafeDownCast (this->LandmarkList->GetItemAsObject(i));
      vtkProp3D* actor = view->FindDataSetActor (landmark);
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

void vtkLandmarkManager::InteractionOn()
{
  for (int i=0; i<this->ViewList->GetNumberOfItems(); i++)
  {
    vtkImageView* view = vtkImageView::SafeDownCast (this->ViewList->GetItemAsObject(i));
    if (view && view->GetInteractor())
    {
      if ( !view->GetInteractor()->HasObserver(vtkCommand::MiddleButtonPressEvent, this->Callback) )
      {
	view->GetInteractor()->AddObserver(vtkCommand::MiddleButtonPressEvent, this->Callback);
	view->GetInteractor()->AddObserver(vtkCommand::MiddleButtonReleaseEvent, this->Callback);
	view->GetInteractor()->AddObserver(vtkCommand::RightButtonPressEvent, this->Callback);
	view->GetInteractor()->AddObserver(vtkCommand::RightButtonReleaseEvent, this->Callback);
      }
    }
    vtkImageView2D* view2d = vtkImageView2D::SafeDownCast (view);
    
    if (view2d)
    {
      view2d->SetMiddleButtonInteractionStyle (vtkInteractorStyleImageView2D::InteractionTypeNull);
      view2d->SetRightButtonInteractionStyle (vtkInteractorStyleImageView2D::InteractionTypeNull);
    }
  }
}

void vtkLandmarkManager::InteractionOff()
{
  for (int i=0; i<this->ViewList->GetNumberOfItems(); i++)
  {
    vtkImageView* view = vtkImageView::SafeDownCast (this->ViewList->GetItemAsObject(i));  
    if (view && view->GetInteractor())
      view->GetInteractor()->RemoveObserver(this->Callback);
    vtkImageView2D* view2d = vtkImageView2D::SafeDownCast (view);
    
    if (view2d)
    {
      view2d->SetMiddleButtonInteractionStyle (vtkInteractorStyleImageView2D::InteractionTypePan);
      view2d->SetRightButtonInteractionStyle (vtkInteractorStyleImageView2D::InteractionTypePan);
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
//     vtkImageView* view = vtkImageView::SafeDownCast (this->ViewList->GetItemAsObject(0));
//     view->SyncRender();
//   }  
}


vtkLandmark* vtkLandmarkManager::GetFirstSelectedLandmark()
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
  vtkImageView* view = vtkImageView::SafeDownCast(this->ViewList->GetItemAsObject(0));
  if ( !view )
    return;
  
  for (unsigned int i=0; i < this->GetNumberOfLandmarks(); i++)
  {
    double* coord = this->GetLandmark(i)->GetPosition();
    view->GetImageCoordinatesFromWorldCoordinates(coord, voxelcoord);
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


void vtkLandmarkManager::LinkerOn()
{
  this->LinkerVisibility = true;
  
  for (int i=0; i < this->ViewList->GetNumberOfItems(); i++)
  {
    vtkImageView* view = vtkImageView::SafeDownCast(this->ViewList->GetItemAsObject(i));
    if ( view )
    {
      vtkProp3D* actor = view->FindDataSetActor (this->Linker);
      if (actor)
  	actor->SetVisibility (this->LinkerVisibility);
    }
  }

}

void vtkLandmarkManager::LinkerOff()
{
  this->LinkerVisibility = false;
  
  for (int i=0; i < this->ViewList->GetNumberOfItems(); i++)
  {
    vtkImageView* view = vtkImageView::SafeDownCast(this->ViewList->GetItemAsObject(i));
    if ( view )
    {
      vtkProp3D* actor = view->FindDataSetActor (this->Linker);
      if (actor)
  	actor->SetVisibility (this->LinkerVisibility);
    }
  }
}

void vtkLandmarkManager::UpdateLinker()
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
      this->ViewList->SyncRemoveDataSet (this->Linker);
    }  
  }
  if (this->Linker->GetNumberOfPoints() == 2)
  {
    if (this->ViewList->GetNumberOfItems())
    {
      this->ViewList->SyncAddDataSet (this->Linker, this->GetSelectedProperty());
    }  
  }

}




  
