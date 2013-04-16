/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkLandmarkManagerCallback.h>

#include <vtkObjectFactory.h>

#include <vtkProperty.h>
#include <vtkWorldPointPicker.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCellPicker.h>
#include <vtkPolyData.h>
#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkErrorCode.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorObserver.h>
#include <vtkImageData.h>
#include <vtkCellArray.h>
#include <vtkLandmarkManager.h>
#include <vtkPointPicker.h>
#include <sstream>
#include <assert.h>
#include <cmath>
#include <stdio.h>
#include <string>
#include <vector>



#include <vtkImageViewCollection.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>

#include <vtkSphereWidget.h>


void vtkLandmarkManagerCallback::Execute ( vtkObject *caller, unsigned long event, void*)
{

  // Middle button was pressed :
  // Select the landmark (go to its position if double clicked
  // if single click : prepare to drag landmark !!
  
  //   if (event == vtkCommand::ModifiedEvent)
  //   {
  //     for(unsigned int i = 0; i < this->ViewList->GetNumberOfItems(); i++)
  //     {
  //       vtkImageView* view = vtkImageView::SafeDownCast (this->ViewList->GetItemAsObject(0));
  //       view->SyncRender();
  //     }  

  //     return;
  
  //   }
  
  vtkSphereWidget* sphere = vtkSphereWidget::SafeDownCast(caller);
  vtkRenderWindowInteractor* interactor = vtkRenderWindowInteractor::SafeDownCast(caller);
  if (!interactor && !sphere)
  {
    return;
  }
  if (!this->ViewList)
  {
    return;
  }

  if (!this->LandmarkManager)
    return;

  if (sphere)
  {
    vtkLandmark* movinglandmark = 0;
    for (unsigned int i=0; i<this->LandmarkManager->GetNumberOfLandmarks(); i++)
    {
      vtkLandmark* landmark =  this->LandmarkManager->GetLandmark(i);
      
      if (!landmark)
      {
	continue;
      }
      if (sphere == landmark->GetSphereWidget())
      {
	movinglandmark = landmark;
	break;
      }
    }
    if (!movinglandmark)
      return;

    vtkImageView* view = vtkImageView::SafeDownCast (this->ViewList->GetItemAsObject(0));

    if (event == vtkCommand::StartInteractionEvent)
    {
      
      this->LandmarkManager->SelectLandmark (movinglandmark);
      this->LandmarkManager->InvokeEvent (vtkLandmarkManagerCallback::LandmarkPickedEvent);

      if (sphere->GetInteractor()->GetRepeatCount())
      {
	double pos[3];
	pos[0] = movinglandmark->GetPosition()[0];
	pos[1] = movinglandmark->GetPosition()[1];
	pos[2] = movinglandmark->GetPosition()[2];
 	this->ViewList->SyncSetCurrentPoint (pos);
      }
    }
    else if (event == vtkCommand::InteractionEvent)
    {
      movinglandmark->SetPosition (sphere->GetCenter());
      this->LandmarkManager->InvokeEvent (vtkLandmarkManagerCallback::LandmarkHasMovedEvent, this->SelectedLandmark);
      this->LandmarkManager->UpdateLinker();
      if (view)
      {
	int voxelpos[3] = {0,0,0};
	view->GetImageCoordinatesFromWorldCoordinates(movinglandmark->GetPosition(), voxelpos);
	movinglandmark->SetVoxelCoord(voxelpos);
      }
    }

    this->ViewList->SyncRender();
    
    return;
    
  }
  
  vtkImageView* view = NULL;
  
  for (int i=0; i<this->ViewList->GetNumberOfItems(); i++)
  {
    vtkImageView* testview = vtkImageView::SafeDownCast (this->ViewList->GetItemAsObject(i));  
    if (testview->GetInteractor() == interactor)
      view = testview;
  }
  
  if (!view)
  {
    return;
  }
   
  //if ((event == vtkCommand::RightButtonPressEvent) && (interactor->GetControlKey()))
  if ((event == vtkCommand::RightButtonPressEvent) && (interactor->GetShiftKey()))
  {

    vtkImageView2D* view2d = vtkImageView2D::SafeDownCast (view);
    vtkImageView3D* view3d = vtkImageView3D::SafeDownCast (view);
    std::cout<<"found view "<<(*view)<<std::endl;
    if (view2d)
    {
      std::cout<<"found view2d "<<std::endl;
    
      vtkInteractorStyleImageView2D* p_isi = vtkInteractorStyleImageView2D::SafeDownCast (view2d->GetInteractorStyle());
      
      // Tricky part to recover the new position of the landmark
      double pos[3] = {0,0,0};
      view2d->GetWorldCoordinatesFromDisplayPosition (p_isi->GetRequestedPosition (), pos);
      int voxelpos[3] = {0,0,0};
      view2d->GetImageCoordinatesFromWorldCoordinates(pos, voxelpos);
      
      vtkLandmark* landmark = this->LandmarkManager->CreateAndAddLandmark();
      if (landmark)
      {
	
	landmark->SetPosition (pos);
	landmark->SetVoxelCoord (voxelpos);
	//this->LandmarkManager->InvokeEvent (vtkLandmarkManagerCallback::LandmarkHasMovedEvent, landmark);
	this->LandmarkManager->UpdateLinker();
      }
      
      this->ViewList->SyncRender();
    }
    else if (view3d)
    {
      std::cout<<"found view3d at "<<std::endl;
      int X, Y;
      X = interactor->GetEventPosition()[0];
      Y = interactor->GetEventPosition()[1];
      
      vtkRenderer* renderer = interactor->FindPokedRenderer(X,Y);
      if (!renderer)
	return;
      
      vtkCellPicker* pointpicker = vtkCellPicker::New();
      //pointpicker->SetTolerance (0.5);
      pointpicker->Pick(X,Y,0.0, renderer);
      
      vtkDataSet* dataset = pointpicker->GetDataSet();
      
      if (!dataset)
      {
	pointpicker->Delete();
	return;
      }
      
      vtkPoints* points = pointpicker->GetPickedPositions();
      
      if (!points || !points->GetNumberOfPoints())
      {
	pointpicker->Delete();
	return;
      }
      
      double* pos = points->GetPoint ((vtkIdType)0);
      std::cout<<"found point at "
	       <<pos[0]<<" "
	       <<pos[1]<<" "
	       <<pos[2]<<std::endl;
      
      vtkLandmark* landmark = this->LandmarkManager->CreateAndAddLandmark();
      if (landmark)
      {
	landmark->SetPosition (pos);
	//this->LandmarkManager->InvokeEvent (vtkLandmarkManagerCallback::LandmarkHasMovedEvent, landmark);
	this->LandmarkManager->UpdateLinker();
	
	this->ViewList->SyncRender();
	pointpicker->Delete();
      }
      
    }
    
    
    return;
    
  }


  // Middle button was pressed :
  // Select the landmark (go to its position if double clicked
  // if single click : prepare to drag landmark !!
  
  if ( (event == vtkCommand::MiddleButtonPressEvent) ||
       ( (event == vtkCommand::RightButtonPressEvent) && interactor->GetControlKey() ) )
  {

    // CAUTION : apparently VTK does not allow us to catch mouse release button
    // events in a 3D renderwindowinteractor...
    // but it is catched in 2D...
    // that is why we don't select anything if we are in 3D
    //     if (!vtkImageView2D::SafeDownCast (view))
    //       return;
    
    int X, Y;
    X = interactor->GetEventPosition()[0];
    Y = interactor->GetEventPosition()[1];
    
    vtkRenderer* renderer = interactor->FindPokedRenderer(X,Y);
    if (!renderer)
      return;
    
    vtkCellPicker*  cellpicker = vtkCellPicker::New();

    // TO DO :
    // restrict the actors that have to be picked to the actors
    // of the landmarks from the manager (if provided)
    // otherwise the picking can slow down the image rendering in 3D
    
    cellpicker->SetTolerance(0.01);
    cellpicker->Pick(X,Y,0.01, renderer);
    vtkActor* actor = cellpicker->GetActor();
    
    if (!actor)
    {
      cellpicker->Delete();
      return;
    }

    vtkLandmark* landmark = vtkLandmark::SafeDownCast (view->FindActorDataSet (actor));
    if (!landmark)
    {
      cellpicker->Delete();
      return;
    }

    cellpicker->Delete();

    if (vtkImageView2D::SafeDownCast (view))
      this->SelectedLandmark = landmark;
    
    this->LandmarkManager->SelectLandmark (landmark);

    //this->LandmarkManager->SetLastPickedLandmark (landmark); // no need to do it here
    this->LandmarkManager->InvokeEvent (vtkLandmarkManagerCallback::LandmarkPickedEvent);

    
    if (interactor->GetRepeatCount())
    {
      double pos[3];
      pos[0] = landmark->GetPosition()[0];
      pos[1] = landmark->GetPosition()[1];
      pos[2] = landmark->GetPosition()[2];
      
      this->ViewList->SyncSetCurrentPoint (pos);
    }
    this->ViewList->SyncRender();

    return;

  }


  
  // Middle button was released :
  // Drag the selected landmark if there is one !!
  if ( (event == vtkCommand::MiddleButtonReleaseEvent) || (event == vtkCommand::RightButtonReleaseEvent) )
  {
    // CAUTION : apparently VTK does not allow us to catch mouse release button
    // events in a 3D renderwindowinteractor...
    // but it is catched in 2D...
    if (!this->SelectedLandmark)
    {
      return;
    }

    if (!this->LandmarkManager)
    {
      this->SelectedLandmark->UnSelect();
    }

    vtkImageView2D* view2d = vtkImageView2D::SafeDownCast (view);
    if (!view2d)
    {
      this->SelectedLandmark->UnSelect();
      this->SelectedLandmark = NULL;
      
      return;
    }

    
    // Tricky part to recover the new position of the landmark
    
    vtkInteractorStyleImageView2D* p_isi = vtkInteractorStyleImageView2D::SafeDownCast ( view2d->GetInteractorStyle());
    if (p_isi )
    {
      // Tricky part to recover the new position of the landmark
      double pos[3] = {0,0,0};
      view2d->GetWorldCoordinatesFromDisplayPosition (p_isi->GetRequestedPosition (), pos);
      int voxelpos[3] = {0,0,0};
      view2d->GetImageCoordinatesFromWorldCoordinates(pos, voxelpos);

      this->SelectedLandmark->SetPosition (pos);
      this->SelectedLandmark->SetVoxelCoord (voxelpos);
    
      if (this->LandmarkManager)
      {
      
	this->LandmarkManager->InvokeEvent (vtkLandmarkManagerCallback::LandmarkHasMovedEvent, this->SelectedLandmark);
	this->LandmarkManager->UpdateLinker();
      }
      this->SelectedLandmark->Modified();
    
      this->ViewList->SyncRender();
    
      this->SelectedLandmark = NULL;

      return;
    }
  }

  if (event == vtkCommand::RightButtonPressEvent)
  {
    int X, Y;
    X = interactor->GetEventPosition()[0];
    Y = interactor->GetEventPosition()[1];
    
    vtkRenderer* renderer = interactor->FindPokedRenderer(X,Y);
    if (!renderer)
      return;
    
    vtkCellPicker*  cellpicker = vtkCellPicker::New();

    // TO DO :
    // restrict the actors that have to be picked to the actors
    // of the landmarks from the manager (if provided)
    // otherwise the picking can slow down the image rendering in 3D
    
    cellpicker->SetTolerance(0.01);
    cellpicker->Pick(X,Y,0.01, renderer);
    vtkActor* actor = cellpicker->GetActor();
    
    if (!actor)
    {
      cellpicker->Delete();
      return;
    }

    vtkLandmark* landmark = vtkLandmark::SafeDownCast (view->FindActorDataSet (actor));
    if (!landmark)
    {
      cellpicker->Delete();
      return;
    }

    cellpicker->Delete();

    this->LandmarkManager->SelectLandmark (landmark);
    
    this->LandmarkManager->SelectLandmark (landmark);
    this->LandmarkManager->InvokeEvent (vtkLandmarkManagerCallback::LandmarkPickedEvent);
      
    
    if (interactor->GetRepeatCount())
    {
      double pos[3];
      pos[0] = landmark->GetPosition()[0];
      pos[1] = landmark->GetPosition()[1];
      pos[2] = landmark->GetPosition()[2];
      
      this->ViewList->SyncSetCurrentPoint (pos);
    }

    this->ViewList->SyncRender();
  }
  
}
  

