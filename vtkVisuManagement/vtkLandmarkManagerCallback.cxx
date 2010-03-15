/*=========================================================================

  Program:   vtkINRIA3D
  Module:    $Id: vtkLandmarkManagerCallback.cxx 1306 2009-10-28 18:51:32Z ntoussaint $
  Language:  C++
  Author:    $Author: ntoussaint $
  Date:      $Date: 2009-10-28 19:51:32 +0100 (Wed, 28 Oct 2009) $
  Version:   $Revision: 1306 $

  Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
  See Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

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



#include <vtkCollection.h>
#include <vtkViewImage2D.h>
#include <vtkViewImage3D.h>

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
  //       vtkViewImage* view = vtkViewImage::SafeDownCast (this->ViewList->GetItemAsObject(0));
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

    vtkViewImage* view = vtkViewImage::SafeDownCast (this->ViewList->GetItemAsObject(0));

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
 	if (view)
 	  view->SyncSetCurrentPoint (pos);
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
	view->GetVoxelCoordinates(movinglandmark->GetPosition(), voxelpos);
	movinglandmark->SetVoxelCoord(voxelpos);
      }
    }

    if (view)
      view->SyncRender();
    
    return;
    
  }
  

  
  vtkViewImage* view = NULL;
  
  for (int i=0; i<this->ViewList->GetNumberOfItems(); i++)
  {
    vtkViewImage* testview = vtkViewImage::SafeDownCast (this->ViewList->GetItemAsObject(i));  
    if (testview->GetRenderWindowInteractor() == interactor)
      view = testview;
  }
  
  if (!view)
  {
    return;
  }
   
  //if ((event == vtkCommand::RightButtonPressEvent) && (interactor->GetControlKey()))
  if ((event == vtkCommand::RightButtonPressEvent) && (interactor->GetShiftKey()))
  {

    vtkViewImage2D* view2d = vtkViewImage2D::SafeDownCast (view);
    vtkViewImage3D* view3d = vtkViewImage3D::SafeDownCast (view);
    std::cout<<"found view "<<(*view)<<std::endl;
    if (view2d)
    {
      std::cout<<"found view2d "<<std::endl;
      // Tricky part to recover the new position of the landmark
      
      vtkInteractorStyle* p_isi = vtkInteractorStyle::SafeDownCast(view2d->GetRenderWindowInteractor()->GetInteractorStyle());
      if (p_isi)
      {
	vtkRenderWindowInteractor *rwi = p_isi->GetInteractor();
        //vtkAssemblyPath *path=NULL;
        p_isi->FindPokedRenderer(rwi->GetEventPosition()[0],
				 rwi->GetEventPosition()[1]);
        rwi->GetPicker()->Pick(rwi->GetEventPosition()[0],
			       rwi->GetEventPosition()[1], 0.0, 
			       p_isi->GetCurrentRenderer());
        vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker());
	if (!picker)
	{
	  return;
	
	}
      
	double* world = picker->GetPickPosition();
	double  pos[3];
	view2d->GetCurrentPoint(pos);    
      
	switch(view2d->GetOrientation())
	{
	    case vtkViewImage2D::SAGITTAL_ID:
	    
	      pos [1] = world[0];//*1.0;
	      pos [2] = world[1];//*1.0;
	      break;
	    
	    
	    case vtkViewImage2D::CORONAL_ID:
	    
	      pos [0] = world[0];//*-1.0;
	      pos [2] = world[1];
	      break;
	    
	    
	    case vtkViewImage2D::AXIAL_ID:
	    
	      pos [0] = world[0];
	      pos [1] = world[1]*-1.0;
	      break;
	    
	}
      
	// Treat extrem positions
	for (unsigned int i=0; i<3; i++)
	{
	  if (pos[i] < view2d->GetWholeMinPosition(i)) pos[i] = view2d->GetWholeMinPosition(i)+0.0005;
	  if (pos[i] > view2d->GetWholeMaxPosition(i)) pos[i] = view2d->GetWholeMaxPosition(i)-0.0005;
	}
      
	int voxelpos[3] = {0,0,0};
	view2d->GetVoxelCoordinates(pos, voxelpos);
      
	vtkLandmark* landmark = this->LandmarkManager->CreateAndAddLandmark();
	if (landmark)
	{
	
	  landmark->SetPosition (pos);
	  landmark->SetVoxelCoord (voxelpos);
	  //this->LandmarkManager->InvokeEvent (vtkLandmarkManagerCallback::LandmarkHasMovedEvent, landmark);
	  this->LandmarkManager->UpdateLinker();
	}
      
	view2d->SyncRender();
      }
      
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
	
	view3d->SyncRender();
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
    //     if (!vtkViewImage2D::SafeDownCast (view))
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

    vtkLandmark* landmark = vtkLandmark::SafeDownCast (view->GetDataSetFromActor (actor));
    if (!landmark)
    {
      cellpicker->Delete();
      return;
    }

    cellpicker->Delete();

    if (vtkViewImage2D::SafeDownCast (view))
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
      
      view->SyncSetCurrentPoint (pos);
    }
    view->SyncRender();

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

    vtkViewImage2D* view2d = vtkViewImage2D::SafeDownCast (view);
    if (!view2d)
    {
      this->SelectedLandmark->UnSelect();
      this->SelectedLandmark = NULL;
      
      return;
    }

    
    // Tricky part to recover the new position of the landmark
    
    vtkInteractorStyle* p_isi = vtkInteractorStyle::SafeDownCast( view2d->GetRenderWindowInteractor()->GetInteractorStyle() );
    if (p_isi )
    {
      vtkRenderWindowInteractor *rwi = p_isi->GetInteractor();
      vtkAssemblyPath *path=NULL;
      p_isi->FindPokedRenderer(rwi->GetEventPosition()[0],
			       rwi->GetEventPosition()[1]);
      rwi->GetPicker()->Pick(rwi->GetEventPosition()[0],
			     rwi->GetEventPosition()[1], 0.0, 
			     p_isi->GetCurrentRenderer());
      vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker());
      if (!picker)
      {
	return;
      
      }
      path = picker->GetPath();
    
      if (!path)
      {
	return;
      }
    
    
    
      double* world = picker->GetPickPosition();
      double  pos[3];
      view2d->GetCurrentPoint(pos);    
    
      switch(view2d->GetOrientation())
      {
	  case vtkViewImage2D::SAGITTAL_ID:
	  
	    pos [1] = world[0];//*1.0;
	    pos [2] = world[1];//*1.0;
	    break;
	  
        
	  case vtkViewImage2D::CORONAL_ID:
	  
	    pos [0] = world[0];//*-1.0;
	    pos [2] = world[1];
	    break;
	  
	  
	  case vtkViewImage2D::AXIAL_ID:
	  
	    pos [0] = world[0];
	    pos [1] = world[1]*-1.0;
	    break;
	  
      }
    
      // Treat extrem positions
      for (unsigned int i=0; i<3; i++)
      {
	if (pos[i] < view2d->GetWholeMinPosition(i)) pos[i] = view2d->GetWholeMinPosition(i)+0.0005;
	if (pos[i] > view2d->GetWholeMaxPosition(i)) pos[i] = view2d->GetWholeMaxPosition(i)-0.0005;
      }

      int voxelpos[3] = {0,0,0};
      view2d->GetVoxelCoordinates(pos, voxelpos);
      this->SelectedLandmark->SetPosition (pos);
      this->SelectedLandmark->SetVoxelCoord (voxelpos);    

    
      if (this->LandmarkManager)
      {
      
	this->LandmarkManager->InvokeEvent (vtkLandmarkManagerCallback::LandmarkHasMovedEvent, this->SelectedLandmark);
	this->LandmarkManager->UpdateLinker();
      }
      this->SelectedLandmark->Modified();
    
      view->SyncRender();
    
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

    vtkLandmark* landmark = vtkLandmark::SafeDownCast (view->GetDataSetFromActor (actor));
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
      
      view->SyncSetCurrentPoint (pos);
    }
    view->SyncRender();
    
  }
  
}
  

