/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include <vtkRenderingAddOn/vtkInteractorStyleImage2D.h>
#include <vtkRenderingAddOn/vtkViewImage2D.h>

#include "vtkAbstractPropPicker.h"
#include "vtkAssemblyPath.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindowInteractor.h"
#include <vtkRenderingAddOn/vtkViewImage2DCommand.h>
#include <vtkImageData.h>
#include <vtkRenderWindow.h>

vtkCxxRevisionMacro (vtkInteractorStyleImage2D, "$Revision: 1368 $");
vtkStandardNewMacro (vtkInteractorStyleImage2D);

#include <vtkCamera.h>
#include <vtkRenderer.h>


vtkInteractorStyleImage2D::vtkInteractorStyleImage2D()
  : vtkInteractorStyleImage()
{
  this->View       = 0;
  this->ZSliceStep = 0;
  this->LevelStep  = 0.0;
  this->WindowStep = 0.0;
  this->ZoomFactor = 1.0;
}


vtkInteractorStyleImage2D::~vtkInteractorStyleImage2D() 
{
}


void vtkInteractorStyleImage2D::OnMouseMove() 
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];  
  
  switch (this->State) 
  {
      case VTKIS_WINDOW_LEVEL:
        this->FindPokedRenderer(x, y);
        this->WindowLevel();
        this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
        break;
        
      case VTKIS_PICK:
        // No Drag is allowed for picking. Toggle to ZSliceMove state
        // We force StartZSliceMove without ending the picking.
        // We don't want to pick at all
        this->StopState();
        this->StartZSliceMove();
        // There we don't break to let the hand to ZSliceMove
        
      case VTKIS_ZSLICE_MOVE:
        this->FindPokedRenderer(x, y);
	this->ZSliceMove();
        //this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
        break;
        
      case VTKIS_DOLLY:
        this->FindPokedRenderer(x, y);
        this->Dolly();
        this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
        break;
        
      case VTKIS_PAN:
        this->FindPokedRenderer(x, y);
        this->Pan();
        this->PropagateCameraFocalAndPosition();
        this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
	break;

      default:
	this->DefaultMoveAction();
  }
}


void vtkInteractorStyleImage2D::OnLeftButtonDown() 
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  this->FindPokedRenderer(x, y);
  if ( !this->View || !this->CurrentRenderer )
  {
    return;
  }

  if (this->Interactor->GetRepeatCount())
  {
    this->StartPick();
    return;
  }

  switch ( this->View->GetLeftButtonInteractionStyle() )
  {
      case vtkViewImage2D::WINDOW_LEVEL_INTERACTION :
        this->WindowLevelStartPosition[0] = x;
        this->WindowLevelStartPosition[1] = y;      
        this->StartWindowLevel();
        break;
      case vtkViewImage2D::SELECT_INTERACTION :          
        this->StartPick();
        break;    
      case vtkViewImage2D::FULL_PAGE_INTERACTION :
        this->FullPage();
        break;
      case vtkViewImage2D::MEASURE_INTERACTION :
        this->StartMeasure();
        break;
      case vtkViewImage2D::ZOOM_INTERACTION :
        if( this->Interactor->GetShiftKey() )
        {
          this->StartPan();
        }
        else
        {
          this->StartDolly(); // continuous zoom
        }
        break;
  }
}


void vtkInteractorStyleImage2D::OnMiddleButtonDown()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  this->FindPokedRenderer(x, y);
  if ( !this->View || !this->CurrentRenderer )
  {
    return;
  }

  switch ( this->View->GetMiddleButtonInteractionStyle() )
  {
      case vtkViewImage2D::WINDOW_LEVEL_INTERACTION :
        this->WindowLevelStartPosition[0] = x;
        this->WindowLevelStartPosition[1] = y;      
        this->StartWindowLevel();
        break;
      case vtkViewImage2D::SELECT_INTERACTION :          
        this->StartPick();
        break;    
      case vtkViewImage2D::FULL_PAGE_INTERACTION :
        this->FullPage();
        break;
      case vtkViewImage2D::MEASURE_INTERACTION :
        this->StartMeasure();
        break;

      case vtkViewImage2D::ZOOM_INTERACTION :
	/*
        if( this->Interactor->GetShiftKey() )
        {*/
          this->StartPan();
	  /*}
        else
        {
          this->StartDolly();// continuous zoom
	  }*/
        break;      
  }
}


void vtkInteractorStyleImage2D::OnMiddleButtonUp()
{
  switch (this->State)
  {
      case VTKIS_ZSLICE_MOVE:
        this->EndZSliceMove();    
        break;    
      case VTKIS_WINDOW_LEVEL:
        this->EndWindowLevel();
        break;
      case VTKIS_PICK:
        this->EndPick();    
        break;    
      case VTKIS_MEASURE:
        this->EndMeasure();    
        break;    
      case VTKIS_PAN :
        this->EndPan();
        break;
      case VTKIS_DOLLY:
        this->EndDolly();
        break;      
  }
  
}


void vtkInteractorStyleImage2D::OnLeftButtonUp()
{
  switch (this->State) 
  {
      case VTKIS_ZSLICE_MOVE:
        this->EndZSliceMove();    
        break;    
      case VTKIS_WINDOW_LEVEL:
        this->EndWindowLevel();
        break;
      case VTKIS_PICK:
        this->EndPick();    
        break;    
      case VTKIS_MEASURE:
        this->EndMeasure();    
        break;    
      case VTKIS_DOLLY:
        this->EndDolly();
        break;
      case VTKIS_PAN:
        this->EndPan();
        break;    
  }
}


void vtkInteractorStyleImage2D::OnRightButtonDown() 
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  this->FindPokedRenderer(x, y);
  if ( !this->View || !this->CurrentRenderer )
  {
    return;
  }
  
  switch ( this->View->GetRightButtonInteractionStyle() )
  {
      case vtkViewImage2D::WINDOW_LEVEL_INTERACTION :
        this->WindowLevelStartPosition[0] = x;
        this->WindowLevelStartPosition[1] = y;      
        this->StartWindowLevel();
        break;
      case vtkViewImage2D::SELECT_INTERACTION :          
        this->StartPick();
        break;    
      case vtkViewImage2D::FULL_PAGE_INTERACTION :
        this->FullPage();
        break;
      case vtkViewImage2D::MEASURE_INTERACTION :
        this->StartMeasure();
        break;
      case vtkViewImage2D::ZOOM_INTERACTION :
        if( this->Interactor->GetShiftKey() )
        {
          this->StartPan();
        }
        else
        {
          this->StartDolly();// continuous zoom
        }
        break;
  }
}


void vtkInteractorStyleImage2D::OnRightButtonUp() 
{
  switch (this->State) 
  {
      case VTKIS_ZSLICE_MOVE:
        this->EndZSliceMove();    
        break;    
      case VTKIS_WINDOW_LEVEL:
        this->EndWindowLevel();
        break;
      case VTKIS_PICK:
        this->EndPick();    
        break;    
      case VTKIS_MEASURE:
        this->EndMeasure();    
        break;    
      case VTKIS_DOLLY: // continuous zoom
        this->EndDolly();    
        break;
      case VTKIS_PAN:
        this->EndPan();
        break;
  }
}


void vtkInteractorStyleImage2D::OnChar() 
{
  if ( !this->View )
  {
    return;
  }
  
  vtkRenderWindowInteractor *rwi = this->Interactor;  
  int *size = rwi->GetRenderWindow()->GetSize();
  
  std::string key_sym (rwi->GetKeySym());
  
  if ((key_sym.compare("Up")==0) || (key_sym.compare("KP_Up")==0))
  {
    switch( this->View->GetInteractionStyle() )
    {
        case vtkViewImage2D::WINDOW_LEVEL_INTERACTION :
          this->StartWindowLevel();
          this->SetWindowStep (0.0);
          this->SetLevelStep  (4.0/size[1]);
          this->InvokeEvent(vtkCommand::WindowLevelEvent, this);
          this->EndWindowLevel();
          break;
          
        case vtkViewImage2D::SELECT_INTERACTION :
          this->StartZSliceMove();
          this->ZSliceStep = 1;    
          this->InvokeEvent(vtkViewImage2DCommand::ZSliceMoveEvent, this);
          this->EndZSliceMove();
          break;

        case vtkViewImage2D::ZOOM_INTERACTION :
          this->StartDolly();
          this->ZoomFactor = 2.0;
	  this->InvokeEvent(vtkViewImage2DCommand::ZoomEvent, this);
          this->EndDolly();
          break;

        default:
          break;
    } 
  }

  else if ((key_sym.compare("Right")==0) || (key_sym.compare("KP_Right")==0))
  {

    switch( this->View->GetInteractionStyle() )
    {
        case vtkViewImage2D::WINDOW_LEVEL_INTERACTION :
          this->StartWindowLevel();
          this->SetWindowStep (4.0/size[1]);
          this->SetLevelStep  (0.0);
          this->InvokeEvent(vtkCommand::WindowLevelEvent, this);
          this->EndWindowLevel();
          break;

        default:
          break;
    } 
  }
  
  else if ((key_sym.compare("Left")==0) || (key_sym.compare("KP_Left")==0))
  {

    switch( this->View->GetInteractionStyle() )
    {
        case vtkViewImage2D::WINDOW_LEVEL_INTERACTION :
          this->StartWindowLevel();
          this->SetWindowStep (-4.0/size[1]);
          this->SetLevelStep  (0.0);
          this->InvokeEvent(vtkCommand::WindowLevelEvent, this);
          this->EndWindowLevel();
          break;
          
        default:
          break;
    }
  }
  
  else if ((key_sym.compare("Down")==0) || (key_sym.compare("KP_Down")==0))
  {

    switch( this->View->GetInteractionStyle() )
    {
        case vtkViewImage2D::WINDOW_LEVEL_INTERACTION :
          this->StartWindowLevel();
          this->SetWindowStep (0.0);
          this->SetLevelStep  (-4.0/size[1]);
          this->InvokeEvent(vtkCommand::WindowLevelEvent, this);
          this->EndWindowLevel();
          break;
          
        case vtkViewImage2D::SELECT_INTERACTION :
          this->StartZSliceMove();
          this->ZSliceStep = -1;
          this->InvokeEvent(vtkViewImage2DCommand::ZSliceMoveEvent, this);
          this->EndZSliceMove();
          break;
          
        case vtkViewImage2D::ZOOM_INTERACTION :
          this->StartDolly();
          this->ZoomFactor = -2.0;
	  this->InvokeEvent(vtkViewImage2DCommand::ZoomEvent, this);
          this->EndDolly();
          break;
          
        default:
          break;
    }
  }
  
  else if ((key_sym.compare("Page_Down")==0) || (key_sym.compare("KP_Page_Down")==0))
  {
    switch( this->View->GetInteractionStyle() )
    {
        case vtkViewImage2D::WINDOW_LEVEL_INTERACTION :
          this->StartWindowLevel();
          this->SetWindowStep (0.0);
          this->SetLevelStep  (-40.0/size[1]);
          this->InvokeEvent(vtkCommand::WindowLevelEvent, this);
          this->EndWindowLevel();
          break;
          
        case vtkViewImage2D::SELECT_INTERACTION :
          this->StartZSliceMove();
          this->ZSliceStep = -10;    
          this->InvokeEvent(vtkViewImage2DCommand::ZSliceMoveEvent, this);
          this->EndZSliceMove();
          break;

        case vtkViewImage2D::ZOOM_INTERACTION :
          this->StartDolly();
          this->ZoomFactor = -20.0;
	  this->InvokeEvent(vtkViewImage2DCommand::ZoomEvent, this);
          this->EndDolly();
          break;

        default:
          break;
    }

  }

  else if ((key_sym.compare("Page_Up")==0) || (key_sym.compare("KP_Page_Up")==0))
  {
    switch( this->View->GetInteractionStyle() )
    {
        case vtkViewImage2D::WINDOW_LEVEL_INTERACTION :
          this->StartWindowLevel();
          this->SetWindowStep (0.0);
          this->SetLevelStep  (40.0/size[1]);
          this->InvokeEvent(vtkCommand::WindowLevelEvent, this);
          this->EndWindowLevel();
          break;
          
        case vtkViewImage2D::SELECT_INTERACTION :
          this->StartZSliceMove();
          this->ZSliceStep = 10;    
          this->InvokeEvent(vtkViewImage2DCommand::ZSliceMoveEvent, this);
          this->EndZSliceMove();
          break;

        case vtkViewImage2D::ZOOM_INTERACTION :
          this->StartDolly();
          this->ZoomFactor = 20.0;
	  this->InvokeEvent(vtkViewImage2DCommand::ZoomEvent, this);
          this->EndDolly();
          break;

        default:
          break;
    }
  }


  
  vtkAssemblyPath       *path   = 0;
  vtkAbstractPropPicker *picker = 0;

  switch (rwi->GetKeyCode()) 
  {
    
      case 'f' :      
      case 'F' :
      {
        this->AnimState = VTKIS_ANIM_ON;
        this->FindPokedRenderer(rwi->GetEventPosition()[0],
                                rwi->GetEventPosition()[1]);
        rwi->GetPicker()->Pick(rwi->GetEventPosition()[0],
                               rwi->GetEventPosition()[1], 0.0, 
                               this->CurrentRenderer);
        picker = vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker());
        if ( picker )
        {
          path = picker->GetPath();
        }
        if ( path  )
        {          
          rwi->FlyToImage(this->CurrentRenderer,picker->GetPickPosition());
        }
        this->AnimState = VTKIS_ANIM_OFF;
        break;
      }
        
      case 'r' :
      case 'R' :
        if ( this->View->GetLeftButtonInteractionStyle()   == vtkViewImage2D::WINDOW_LEVEL_INTERACTION ||
             this->View->GetRightButtonInteractionStyle()  == vtkViewImage2D::WINDOW_LEVEL_INTERACTION ||
             this->View->GetMiddleButtonInteractionStyle() == vtkViewImage2D::WINDOW_LEVEL_INTERACTION ||
             this->View->GetWheelInteractionStyle()  == vtkViewImage2D::WINDOW_LEVEL_INTERACTION )
        {
          this->InvokeEvent(vtkCommand::ResetWindowLevelEvent, this);
        }

        if ( this->View->GetLeftButtonInteractionStyle()   == vtkViewImage2D::SELECT_INTERACTION ||
             this->View->GetRightButtonInteractionStyle()  == vtkViewImage2D::SELECT_INTERACTION ||
             this->View->GetMiddleButtonInteractionStyle() == vtkViewImage2D::SELECT_INTERACTION ||
             this->View->GetWheelInteractionStyle()  == vtkViewImage2D::SELECT_INTERACTION )
        {
          this->InvokeEvent(vtkViewImage2DCommand::ResetPositionEvent, this);
        }
        if( this->View->GetLeftButtonInteractionStyle()   == vtkViewImage2D::ZOOM_INTERACTION ||
            this->View->GetRightButtonInteractionStyle()  == vtkViewImage2D::ZOOM_INTERACTION ||
            this->View->GetMiddleButtonInteractionStyle() == vtkViewImage2D::ZOOM_INTERACTION ||
            this->View->GetWheelInteractionStyle()  == vtkViewImage2D::ZOOM_INTERACTION )
        {
          this->InvokeEvent(vtkViewImage2DCommand::ResetZoomEvent, this);
        }
        
        break;
  }
  
}


void vtkInteractorStyleImage2D::StartZSliceMove()
{
  if ((this->State != VTKIS_NONE) && (this->State != VTKIS_PICK)) {
    return;
  }
  this->StartState(VTKIS_ZSLICE_MOVE);
  this->InvokeEvent(vtkViewImage2DCommand::StartZSliceMoveEvent, this);
}


void vtkInteractorStyleImage2D::ZSliceMove()
{
  vtkRenderWindowInteractor *rwi = this->Interactor;
  int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
  this->ZSliceStep = dy;  
  this->InvokeEvent(vtkViewImage2DCommand::ZSliceMoveEvent, this);
	rwi->Render();
}


void vtkInteractorStyleImage2D::ZSliceWheelForward()
{
  int dy = (int)this->MouseWheelMotionFactor;
	this->ZSliceStep = dy;  
  this->InvokeEvent(vtkViewImage2DCommand::ZSliceMoveEvent, this);
}


void vtkInteractorStyleImage2D::ZSliceWheelBackward()
{
  int dy = (int)(-1.0*this->MouseWheelMotionFactor);
	this->ZSliceStep = dy;  
  this->InvokeEvent(vtkViewImage2DCommand::ZSliceMoveEvent, this);
}


void vtkInteractorStyleImage2D::EndZSliceMove()
{
  if (this->State != VTKIS_ZSLICE_MOVE) {
    return;
  }
  this->InvokeEvent (vtkViewImage2DCommand::EndZSliceMoveEvent, this);
  this->StopState();
}


void vtkInteractorStyleImage2D::Dolly()
{
  if( this->State != VTKIS_DOLLY) {
    return;
  }

  double *center = this->GetCurrentRenderer()->GetCenter();
  int dy = this->Interactor->GetEventPosition()[1] - this->Interactor->GetLastEventPosition()[1];
  double factor = 10.0 * (double)(dy) / (double)(center[1]);
  this->SetZoomFactor ( factor );
  this->InvokeEvent (vtkViewImage2DCommand::ZoomEvent, this);
	vtkRenderWindowInteractor *rwi = this->Interactor;
	rwi->Render();
}


void vtkInteractorStyleImage2D::DollyWheelForward()
{
  if( this->State != VTKIS_DOLLY) {
    return;
  }
  
  double factor = 10.0 * 0.2 * this->MouseWheelMotionFactor;
  this->SetZoomFactor ( factor );
  this->InvokeEvent (vtkViewImage2DCommand::ZoomEvent, this);
}


void vtkInteractorStyleImage2D::DollyWheelBackward()
{
  if( this->State != VTKIS_DOLLY) {
    return;
  }

  double factor = 10.0 * -0.2 * this->MouseWheelMotionFactor;
  this->SetZoomFactor ( factor );
  this->InvokeEvent (vtkViewImage2DCommand::ZoomEvent, this);
}


void vtkInteractorStyleImage2D::FullPage()
{
  this->InvokeEvent(vtkViewImage2DCommand::FullPageEvent, this);
}


void vtkInteractorStyleImage2D::StartMeasure()
{
  if (this->State != VTKIS_NONE) {
    return;
  }
  this->StartState(VTKIS_MEASURE);
  this->InvokeEvent(vtkViewImage2DCommand::StartMeasureEvent, this);
}


void vtkInteractorStyleImage2D::Measure()
{
  this->InvokeEvent(vtkViewImage2DCommand::MeasureEvent, this);
}


void vtkInteractorStyleImage2D::EndMeasure()
{
  if (this->State != VTKIS_MEASURE) {
    return;
  }
  this->InvokeEvent(vtkViewImage2DCommand::EndMeasureEvent, this);
  this->StopState();
}


void vtkInteractorStyleImage2D::WindowLevel()
{
  vtkRenderWindowInteractor *rwi = this->Interactor;
  
  this->WindowLevelCurrentPosition[0] = rwi->GetEventPosition()[0];
  this->WindowLevelCurrentPosition[1] = rwi->GetEventPosition()[1];

  int *size = rwi->GetRenderWindow()->GetSize();
   
  // Compute normalized delta
  double dx = 4.0 * 
    (this->GetWindowLevelCurrentPosition()[0] - 
     this->GetWindowLevelStartPosition()[0]) / size[0];
  double dy = 4.0 * 
    (this->GetWindowLevelStartPosition()[1] - 
     this->GetWindowLevelCurrentPosition()[1]) / size[1];

  this->SetWindowStep (dx);
  this->SetLevelStep (dy);
  
  this->InvokeEvent(vtkCommand::WindowLevelEvent, this);
	rwi->Render();
}


void vtkInteractorStyleImage2D::WindowLevelWheelForward()
{
  int *size = this->Interactor->GetRenderWindow()->GetSize();
  
  double dy = 4.0*(double)(this->MouseWheelMotionFactor)/size[1];
  
  this->SetWindowStep (0.0);
  this->SetLevelStep  (dy);
  
  this->InvokeEvent(vtkCommand::WindowLevelEvent, this);
}


void vtkInteractorStyleImage2D::WindowLevelWheelBackward()
{
  int *size = this->Interactor->GetRenderWindow()->GetSize();
  
  double dy = 4.0*(double)(-1.0*this->MouseWheelMotionFactor)/size[1];
  
  this->SetWindowStep (0.0);
  this->SetLevelStep  (dy);
  
  this->InvokeEvent(vtkCommand::WindowLevelEvent, this);
}


void vtkInteractorStyleImage2D::OnMouseWheelForward()
{

  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];
  
  this->FindPokedRenderer(x,y);
  
  if( this->CurrentRenderer == NULL )
  {
    return;
  }
  
  switch ( this->View->GetWheelInteractionStyle() )
  {
      case vtkViewImage2D::WINDOW_LEVEL_INTERACTION :
        this->StartWindowLevel();
        this->WindowLevelWheelForward ();
        this->EndWindowLevel();
        break;
        
      case vtkViewImage2D::SELECT_INTERACTION :
        this->StartZSliceMove();
        this->ZSliceWheelForward();
        this->EndZSliceMove();
        break;    
        
      case vtkViewImage2D::FULL_PAGE_INTERACTION :
        break;
        
      case vtkViewImage2D::ZOOM_INTERACTION :
        this->StartDolly();
	this->DollyWheelForward();
        this->EndDolly();
        break;
	
      default:
        break;
  }
  
}


void vtkInteractorStyleImage2D::OnMouseWheelBackward()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];
  
  this->FindPokedRenderer(x,y);
  
  if( this->CurrentRenderer == NULL )
  {
    return;
  }

  
  switch ( this->View->GetWheelInteractionStyle() )
  {
      case vtkViewImage2D::WINDOW_LEVEL_INTERACTION :
        this->StartWindowLevel();
        this->WindowLevelWheelBackward ();
        this->EndWindowLevel();
        break;
        
      case vtkViewImage2D::SELECT_INTERACTION :          
        this->StartZSliceMove();
        this->ZSliceWheelBackward();
        this->EndZSliceMove();
        break;
        break;    
        
      case vtkViewImage2D::FULL_PAGE_INTERACTION :
        
        break;
        
      case vtkViewImage2D::ZOOM_INTERACTION :
        this->StartDolly();
	this->DollyWheelBackward();
        this->EndDolly();
        break;
	
      default:
        break;
  }
  
}




void vtkInteractorStyleImage2D::PropagateCameraFocalAndPosition()
{
  if( !this->GetView() )
  {
    return;
  }

  if( !this->GetView()->GetRenderer() )
  {
    return;
  }

  vtkCamera* camera = this->GetView()->GetRenderer()->GetActiveCamera();
  double* focal = camera->GetFocalPoint ();
  double* pos   = camera->GetPosition ();

  bool LinkCamera = this->GetView()->GetLinkCameraFocalAndPosition();
  if (LinkCamera)
  {
    this->GetView()->SetLinkCameraFocalAndPosition ( 0 );
    this->GetView()->SyncSetCameraFocalAndPosition (focal, pos);
    this->GetView()->SetLinkCameraFocalAndPosition ( LinkCamera );
  }
}

void vtkInteractorStyleImage2D::DefaultMoveAction()
{
  this->InvokeEvent (vtkViewImage2DCommand::DefaultMoveEvent ,this);
  vtkRenderWindowInteractor *rwi = this->Interactor;
  rwi->Render();
}
