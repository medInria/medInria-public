/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkImageViewCollection.h>

#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkInteractorStyleImage.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkMatrix4x4.h"
#include "vtkScalarBarActor.h"
#include <vtkOrientationAnnotation.h>
#include "vtkCornerAnnotation.h"
#include "vtkTextProperty.h"
#include "vtkLookupTable.h"
#include "vtkMath.h"
#include "vtkPlane.h"
#include "vtkCutter.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkProp3DCollection.h"
#include "vtkDataSetCollection.h"
#include "vtkPoints.h"
#include "vtkIdList.h"
#include "vtkOutlineSource.h"
#include "vtkMatrixToLinearTransform.h"
#include "vtkPointData.h"
#include "vtkUnsignedCharArray.h"
#include "vtkIntArray.h"
#include "vtkImageAccumulate.h"


#include <vtkImageView2DCommand.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>

#include "vtkInteractorStyleImageView2D.h"

#include <vector>
#include <string>
#include <sstream>

vtkCxxRevisionMacro(vtkImageViewCollection, "$Revision: 2 $");
vtkStandardNewMacro(vtkImageViewCollection);


vtkImageViewCollection::vtkImageViewCollection()
{
  this->Command = vtkImageViewCollectionCommand::New();
  this->Command->SetCollection (this);

  this->LinkSliceMove = 1;
  this->LinkTimeChange = 1;
  this->LinkColorWindowLevel = 1;
  this->LinkResetWindowLevel = 1;
  this->LinkResetViewer = 1;
  this->LinkRequestedPosition = 1;
  this->LinkCamera = 0;
  this->LinkZoom = 1;
  this->LinkPan = 1;
  this->LinkCurrentPoint = 0;

  this->ShowSlicePlanes = 0;
}


vtkImageViewCollection::~vtkImageViewCollection()
{
  this->RemoveAllItems();  
  this->Command->Delete();
}


void vtkImageViewCollection::AddItem(vtkImageView *a)
{
  if (!a || this->IsItemPresent (a))
    return;

	
  vtkImageView2D* a2d = vtkImageView2D::SafeDownCast (a);
  
  if (a2d && a->GetInteractorStyle())
  {
    a->GetInteractorStyle()->AddObserver (vtkImageView2DCommand::CharEvent, this->Command);
    
    if (this->LinkSliceMove)
      a->GetInteractorStyle()->AddObserver (vtkImageView2DCommand::SliceMoveEvent, this->Command);
    if (this->LinkColorWindowLevel)
      a->GetInteractorStyle()->AddObserver (vtkCommand::WindowLevelEvent, this->Command);
    if (this->LinkResetWindowLevel)
      a->GetInteractorStyle()->AddObserver (vtkCommand::ResetWindowLevelEvent, this->Command);
    if (this->LinkResetViewer)
      a->GetInteractorStyle()->AddObserver (vtkImageView2DCommand::ResetViewerEvent, this->Command);
    if (this->LinkRequestedPosition)
      a->GetInteractorStyle()->AddObserver (vtkImageView2DCommand::RequestedPositionEvent, this->Command);
    if (this->LinkCamera)
      a->GetInteractorStyle()->AddObserver (vtkImageView2DCommand::CameraMoveEvent, this->Command);
    if (this->LinkZoom)
      a->GetInteractorStyle()->AddObserver (vtkImageView2DCommand::CameraZoomEvent, this->Command);
    if (this->LinkPan)
      a->GetInteractorStyle()->AddObserver (vtkImageView2DCommand::CameraPanEvent, this->Command);
    if (this->LinkCurrentPoint)
      a->GetInteractorStyle()->AddObserver (vtkImageView2DCommand::DefaultMoveEvent, this->Command);
    if (this->LinkTimeChange)
      a->GetInteractorStyle()->AddObserver (vtkImageView2DCommand::TimeChangeEvent, this->Command);
  }

	this->Superclass::AddItem (a);
  
}



void vtkImageViewCollection::RemoveItem(vtkImageView *a)
{
  if (!a)
    return;
  a->GetInteractorStyle()->RemoveObserver ( this->Command );
  this->Superclass::RemoveItem (a);
}

void vtkImageViewCollection::RemoveAllItems()
{
  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();    
  while(item)
  {
    std::cout<<"removing the observer"<<std::endl;
    
    item->GetInteractorStyle()->RemoveObserver ( this->Command );;
    item = this->GetNextItem();
  }
  this->Superclass::RemoveAllItems ();
}



void vtkImageViewCollection::ReplaceItem(int i, vtkImageView *a)
{
  this->Superclass::ReplaceItem (i, a);
}


// Remove the i'th item in the list.
// Be careful if using this function during traversal of the list using
// GetNextItemAsObject (or GetNextItem in derived class).  The list WILL
// be shortened if a valid index is given!  If this->Current is equal to the
// element being removed, have it point to then next element in the list.
void vtkImageViewCollection::RemoveItem(int i)
{
  this->Superclass::RemoveItem (i);
}

void vtkImageViewCollection::SetShowSlicePlanes(unsigned int arg)
{
  this->ShowSlicePlanes = arg;

  for (int i=0; i<this->GetNumberOfItems(); i++)
  {
    for (int j=0; j<this->GetNumberOfItems(); j++)
    {
      vtkImageView2D* v1 = vtkImageView2D::SafeDownCast (this->GetItem(i));
      vtkImageView2D* v2 = vtkImageView2D::SafeDownCast (this->GetItem(j));
      if (v1 && v2)
      {
	if (arg)
	{
	  v1->AddDataSet (v2->GetSlicePlane());
	  v2->AddDataSet (v1->GetSlicePlane());
	}
	else
	{
	  v1->RemoveDataSet (v2->GetSlicePlane());
	  v2->RemoveDataSet (v1->GetSlicePlane());
	}
      }
    }
  }
}
void vtkImageViewCollection::SyncSetPan(double* arg, vtkImageView* caller)
{
  this->InitTraversal();						
  vtkImageView* item = this->GetNextItem();				
  while(item)								
  {									
    if(item!=caller) {						
      vtkImageView2D* item2D = vtkImageView2D::SafeDownCast (item);	
      if (item2D)							
	item2D->SetPan (arg);
    }									
    item = this->GetNextItem();					
  }				
}

void vtkImageViewCollection::SyncRender(vtkImageView* caller, int checkForInteractor)
{
  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();

  while(item)
  {
    if (item!=caller && (!checkForInteractor || (checkForInteractor&&item->GetIsInteractorInstalled())))
    {
      
      vtkImageView3D* item3d = vtkImageView3D::SafeDownCast (item);
      
      if ( caller && item3d && item3d->GetRenderingMode() == vtkImageView3D::VOLUME_RENDERING)
      {
    if (caller->GetInteractorStyle()->GetState() == VTKIS_WINDOW_LEVEL )
	  item->Render ();
      }
      else
      item->Render ();
    }
    item = this->GetNextItem();
  }
}


void vtkImageViewCollection::SyncReset(vtkImageView *caller)
{
  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();
  while(item)
  {
	  if (item!=caller) {
      item->Reset ();
	  }
    item = this->GetNextItem();
  }
}

void vtkImageViewCollection::SyncStart()
{

  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();
  while(item)
  {
    item->Start ();
    item = this->GetNextItem();
  }  
}

void vtkImageViewCollection::SyncResetCurrentPoint(vtkImageView *caller)
{
  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();
  while(item)
  {
    if (item!=caller)
    {
      vtkImageView2D* item2D = vtkImageView2D::SafeDownCast (item);
      if (item2D)
	item2D->ResetCurrentPoint ();
    }
    
    item = this->GetNextItem();
  }
}


void vtkImageViewCollection::SyncResetWindowLevel(vtkImageView *caller)
{
  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();
  while(item)
  {
    if (item!=caller)
      item->ResetWindowLevel ();
    item = this->GetNextItem();
  }
}


void vtkImageViewCollection::SyncResetCamera(vtkImageView *caller)
{
  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();
  while(item)
  {
    if (item!=caller)
      item->ResetCamera ();
    item = this->GetNextItem();
  }
}


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
////////////////////// Collection Callback ////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/*
void vtkImageViewCollection::SyncStart()
{
  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();
  while(item)
  {
    item->GetInteractor()->Start();
    item = this->GetNextItem();
  }
}
*/



vtkImageViewCollectionCommand::vtkImageViewCollectionCommand()
{
  this->Collection = NULL;
}


void vtkImageViewCollectionCommand::SetCollection(vtkImageViewCollection* p)
{
  this->Collection = p;
  
}


void vtkImageViewCollectionCommand::Execute(vtkObject *caller,
					    unsigned long event,
					    void *callData)
{
  if (!this->Collection)
    return;
	
  this->GetCollection()->InitTraversal();
	
  vtkImageView*   v      = this->GetCollection()->GetNextItem();
  vtkImageView*   view   = NULL;
  vtkImageView2D* viewer = NULL;
	
  while(v)
  {
    if (caller == v->GetInteractorStyle()) {
	  view = v;
	}
    v = this->GetCollection()->GetNextItem();
  }

  if (!view)
	return;
	
  // Time change
  if (event == vtkImageView2DCommand::TimeChangeEvent)
  {
	this->Collection->SyncSetTimeIndex (view->GetTimeIndex(), view, 1);
	this->Collection->SyncRender(view, 1);
  }
	
  viewer = vtkImageView2D::SafeDownCast (view);
  vtkInteractorStyleImageView2D *isi = vtkInteractorStyleImageView2D::SafeDownCast(caller);
	
  if (!isi || !viewer || !viewer->GetInput())
    return;

  vtkRenderWindowInteractor *rwi = viewer->GetInteractor();
  
  // Move Slice
  if (event == vtkImageView2DCommand::SliceMoveEvent)
  {
    this->Collection->SyncSetCurrentPoint (viewer->GetCurrentPoint(), viewer, 1);
    this->Collection->SyncRender(viewer, 1);
  }

  // Adjust the window level here
  if (event == vtkCommand::WindowLevelEvent)
  {
    this->Collection->SyncSetColorWindow(viewer->GetColorWindow(), viewer, 1);
    this->Collection->SyncSetColorLevel(viewer->GetColorLevel(), viewer, 1);
    this->Collection->SyncRender(viewer, 1);
  }
  
  // Reset
  if (event == vtkCommand::ResetWindowLevelEvent)
  {
    this->Collection->SyncResetWindowLevel(viewer);
    this->Collection->SyncRender(viewer, 1);
  }

  // Char events
  if (event == vtkCommand::CharEvent)
  {}

  // Reset
  if (event == vtkImageView2DCommand::ResetViewerEvent)
  {
    this->Collection->SyncReset(viewer);
    this->Collection->SyncRender(viewer, 1);
    return;
  }

  // default move
  if (event == vtkImageView2DCommand::DefaultMoveEvent)
  {
    double* position = viewer->GetCurrentPoint();
    this->Collection->SyncSetCurrentPoint(position, viewer, 1);
    
    if (isi->GetState() == VTKIS_NONE)
      this->Collection->SyncRender(viewer, 1);
  }
  
  // Position requested
  if (event == vtkImageView2DCommand::RequestedPositionEvent)
  {
    double position[3];
    viewer->GetWorldCoordinatesFromDisplayPosition (isi->GetRequestedPosition (), position);
    this->Collection->SyncSetCurrentPoint(position, 0, 1);
    this->Collection->SyncRender(viewer, 1);
    viewer->Render();
  }  

  if (event == vtkImageView2DCommand::CameraZoomEvent)
  {
    double z = viewer->GetZoom();
    this->Collection->SyncSetZoom (z, viewer);
    this->Collection->SyncRender(viewer, 1);
  }


  if (event == vtkImageView2DCommand::CameraPanEvent)
  {
    double* pan = viewer->GetPan();
    this->Collection->SyncSetPan (pan, viewer);
    this->Collection->SyncRender(viewer, 1);
  }

  if (event == vtkImageView2DCommand::CameraMoveEvent)
  {
    double* p = viewer->GetCameraPosition();
    double* f = viewer->GetCameraFocalPoint();
    double* v = viewer->GetCameraViewUp();
    double s = viewer->GetCameraParallelScale();

    this->Collection->SyncSetCameraPosition (p, viewer);
    this->Collection->SyncSetCameraFocalPoint (f, viewer);
    this->Collection->SyncSetCameraViewUp (v, viewer);
    this->Collection->SyncSetCameraParallelScale (s, viewer);
    this->Collection->SyncRender(viewer, 1);
  }
  

}

//----------------------------------------------------------------------------
void vtkImageViewCollection::SetLinkSliceMove(unsigned int v)
{
  this->LinkSliceMove = v;
  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();
  while(item)
  {
    if (vtkImageView2D::SafeDownCast(item))
    {
      if (v)
      {
    if(!item->GetInteractorStyle()->HasObserver (vtkImageView2DCommand::SliceMoveEvent, this->Command) )
      item->GetInteractorStyle()->AddObserver (vtkImageView2DCommand::SliceMoveEvent, this->Command);
      }
      else
    item->GetInteractorStyle()->RemoveObservers (vtkImageView2DCommand::SliceMoveEvent, this->Command);
    }
    
    item = this->GetNextItem();
  }    
}

//----------------------------------------------------------------------------
void vtkImageViewCollection::SetLinkTimeChange(unsigned int v)
{
	this->LinkTimeChange = v;
	this->InitTraversal();
	vtkImageView* item = this->GetNextItem();
	while(item)
	{
		if (vtkImageView2D::SafeDownCast(item))
		{
			if (v)
			{
                if(!item->GetInteractorStyle()->HasObserver (vtkImageView2DCommand::TimeChangeEvent, this->Command) )
                    item->GetInteractorStyle()->AddObserver (vtkImageView2DCommand::TimeChangeEvent, this->Command);
			}
			else
                item->GetInteractorStyle()->RemoveObservers (vtkImageView2DCommand::TimeChangeEvent, this->Command);
		}
		
		item = this->GetNextItem();
	}    
}

//----------------------------------------------------------------------------
void vtkImageViewCollection::SetLinkColorWindowLevel(unsigned int v)
{
  this->LinkColorWindowLevel = v;
  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();
  while(item)
  {
    if (vtkImageView2D::SafeDownCast(item))
    {
      if (v)
      {
    if (!item->GetInteractorStyle()->HasObserver (vtkCommand::WindowLevelEvent, this->Command) )
      item->GetInteractorStyle()->AddObserver (vtkCommand::WindowLevelEvent, this->Command);
      }
      else
    item->GetInteractorStyle()->RemoveObservers (vtkCommand::WindowLevelEvent, this->Command);
    }
    
    item = this->GetNextItem();
  }    
}



//----------------------------------------------------------------------------
void vtkImageViewCollection::SetLinkResetWindowLevel(unsigned int v)
{
  this->LinkResetWindowLevel = v;
  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();
  while(item)
  {
    if (vtkImageView2D::SafeDownCast(item))
    {
      if (v)
      {
    if(!item->GetInteractorStyle()->HasObserver (vtkCommand::ResetWindowLevelEvent, this->Command) )
      item->GetInteractorStyle()->AddObserver (vtkCommand::ResetWindowLevelEvent, this->Command);
      }
      else
    item->GetInteractorStyle()->RemoveObservers (vtkCommand::ResetWindowLevelEvent, this->Command);
    }
    
    item = this->GetNextItem();
  }    
}



//----------------------------------------------------------------------------
void vtkImageViewCollection::SetLinkResetViewer(unsigned int v)
{
  this->LinkResetViewer = v;
  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();
  while(item)
  {
    if (vtkImageView2D::SafeDownCast(item))
    {
      if (v)
      {
    if (!item->GetInteractorStyle()->HasObserver (vtkImageView2DCommand::ResetViewerEvent, this->Command) )
      item->GetInteractorStyle()->AddObserver (vtkImageView2DCommand::ResetViewerEvent, this->Command);
      }
      else
    item->GetInteractorStyle()->RemoveObservers (vtkImageView2DCommand::ResetViewerEvent, this->Command);
    }
    
    item = this->GetNextItem();
  }    
}



//----------------------------------------------------------------------------
void vtkImageViewCollection::SetLinkRequestedPosition(unsigned int v)
{
  this->LinkRequestedPosition = v;
  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();
  while(item)
  {
    if (vtkImageView2D::SafeDownCast(item))
    {
      if (v)
      {
    if (!item->GetInteractorStyle()->HasObserver (vtkImageView2DCommand::RequestedPositionEvent, this->Command) )
      item->GetInteractorStyle()->AddObserver (vtkImageView2DCommand::RequestedPositionEvent, this->Command);
      }
      else
    item->GetInteractorStyle()->RemoveObservers (vtkImageView2DCommand::RequestedPositionEvent, this->Command);
    }
    
    item = this->GetNextItem();
  }    
}



//----------------------------------------------------------------------------
void vtkImageViewCollection::SetLinkCamera(unsigned int v)
{
  this->LinkCamera = v;
  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();
  while(item)
  {
    if (vtkImageView2D::SafeDownCast(item))
    {
      if (v)
      {
    if(!item->GetInteractorStyle()->HasObserver (vtkImageView2DCommand::CameraMoveEvent, this->Command) )
      item->GetInteractorStyle()->AddObserver (vtkImageView2DCommand::CameraMoveEvent, this->Command, -10);
      }
      else
    item->GetInteractorStyle()->RemoveObservers (vtkImageView2DCommand::CameraMoveEvent, this->Command);
    }
    
    item = this->GetNextItem();
  }    
}

//----------------------------------------------------------------------------
void vtkImageViewCollection::SetLinkZoom(unsigned int v)
{
  this->LinkZoom = v;
  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();
  while(item)
  {
    if (vtkImageView2D::SafeDownCast(item))
    {
      if (v)
      {
    if(!item->GetInteractorStyle()->HasObserver (vtkImageView2DCommand::CameraZoomEvent, this->Command) )
      item->GetInteractorStyle()->AddObserver (vtkImageView2DCommand::CameraZoomEvent, this->Command, -10);
      }
      else
    item->GetInteractorStyle()->RemoveObservers (vtkImageView2DCommand::CameraZoomEvent, this->Command);
    }
    
    item = this->GetNextItem();
  }    
}


//----------------------------------------------------------------------------
void vtkImageViewCollection::SetLinkPan(unsigned int v)
{
  this->LinkPan = v;
  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();
  while(item)
  {
    if (vtkImageView2D::SafeDownCast(item))
    {
      if (v)
      {
    if(!item->GetInteractorStyle()->HasObserver (vtkImageView2DCommand::CameraPanEvent, this->Command) )
      item->GetInteractorStyle()->AddObserver (vtkImageView2DCommand::CameraPanEvent, this->Command, -10);
      }
      else
    item->GetInteractorStyle()->RemoveObservers (vtkImageView2DCommand::CameraPanEvent, this->Command);
    }
    
    item = this->GetNextItem();
  }    
}

//----------------------------------------------------------------------------
void vtkImageViewCollection::SetLinkCurrentPoint(unsigned int v)
{
  this->LinkCurrentPoint = v;
  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();
  while(item)
  {
    if (vtkImageView2D::SafeDownCast(item))
    {
      if( item->GetInteractorStyle() )
      {
	if (v)
	{
      if(!item->GetInteractorStyle()->HasObserver (vtkImageView2DCommand::DefaultMoveEvent, this->Command) )
        item->GetInteractorStyle()->AddObserver (vtkImageView2DCommand::DefaultMoveEvent, this->Command);
	}
	else
      item->GetInteractorStyle()->RemoveObservers (vtkImageView2DCommand::DefaultMoveEvent, this->Command);
      }
    }
    
    item = this->GetNextItem();
  }    
}




//----------------------------------------------------------------------------
void vtkImageViewCollection::SyncAddDataSet (vtkPointSet* arg, vtkProperty* prop)
{
  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();
  while(item)
  {
    item->AddDataSet (arg, prop);
    item = this->GetNextItem();
  }  
}

//----------------------------------------------------------------------------
void vtkImageViewCollection::SyncRemoveDataSet (vtkPointSet* arg)
{
  this->InitTraversal();
  vtkImageView* item = this->GetNextItem();
  while(item)
  {
    item->RemoveDataSet (arg);
    item = this->GetNextItem();
  }  
}

