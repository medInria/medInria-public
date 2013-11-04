/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifdef _MSC_VER
#  pragma warning (disable : 4018) 
#endif

#include <vtkInteractorStyle.h>
#include <vtkRenderingAddOn/vtkViewImage2D.h>
#include <vtkRenderingAddOn/vtkInteractorStyleImage2D.h>
#include "vtkAbstractPropPicker.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageData.h"
#include <string.h> 

#include <vtkRenderingAddOn/vtkViewImage2DCommand.h>


vtkViewImage2DCommand::vtkViewImage2DCommand() :
  View(NULL), InitialWindow(0), InitialLevel(0)
{
}


void vtkViewImage2DCommand::Execute(vtkObject*    caller, 
                                 unsigned long event, 
                                 void*         callData)
{
  if ( !this->View || !this->View->GetImage() ){
    return;
  }
  
  if (event == vtkCommand::KeyPressEvent)
  {
    vtkRenderWindowInteractor *rwi = this->View->GetRenderWindow()->GetInteractor();
    char key = rwi->GetKeyCode();
    
    if ( key=='i' || key=='I')
    {
      int mode;      
      mode = this->View->GetInterpolationMode();
      this->View->SetInterpolationMode((++mode)%2);
      //this->View->Update();
      return;      
    }
    
    return;
  }

  if (event == vtkCommand::StartWindowLevelEvent) {
    this->StartWindowing();
    this->PropagateStartWindowing();
    return;    
  }
  if (event == vtkCommand::EndWindowLevelEvent) {
    this->EndWindowing();
    this->PropagateEndWindowing();
    return;    
  }
  if (event == vtkCommand::WindowLevelEvent) {
    this->Windowing(vtkInteractorStyleImage2D::SafeDownCast(caller));
    this->PropagateWindowLevel();
    return;    
  }
  if (event == vtkCommand::ResetWindowLevelEvent) {
    this->StartWindowing();
    this->View->ResetWindowLevel();
    this->PropagateResetWindowLevel();
    this->EndWindowing();
    return;    
  }
  if (event == vtkViewImage2DCommand::ResetPositionEvent) {
    this->View->ResetCurrentPoint();
    this->PropagateResetCurrentPoint();
    return;    
  }
  if (event == vtkViewImage2DCommand::ZoomEvent) {
    this->Zoom(vtkInteractorStyleImage2D::SafeDownCast(caller));
    this->PropagateZoom();
    return;    
  }
  if (event == vtkViewImage2DCommand::ResetZoomEvent) {
    this->View->ResetZoom();
    this->PropagateResetZoom();
    return;    
  }
  if (event == vtkCommand::StartPickEvent) {
    this->StartPicking(vtkInteractorStyleImage::SafeDownCast(caller));
    this->PropagateCurrentPoint();
    return;    
  }
  if (event == vtkCommand::EndPickEvent) {
    this->EndPicking(/*static_cast<vtkInteractorStyleImage *>(caller)*/);
    return;    
  }
  if (event == vtkViewImage2DCommand::StartMeasureEvent) {
    std::cout<<"StartMeasureEvent\n";    
    return;    
  }
  if (event == vtkViewImage2DCommand::MeasureEvent) {
    std::cout<<"MeasureEvent\n";    
    return;    
  }
  if (event == vtkViewImage2DCommand::EndMeasureEvent) {
    std::cout<<"EndMeasureEvent\n";    
    return;    
  }
  if (event == vtkViewImage2DCommand::StartZSliceMoveEvent) {
    return;    
  }
  if (event == vtkViewImage2DCommand::ZSliceMoveEvent) {
    this->ChangeZSlice(vtkInteractorStyleImage2D::SafeDownCast(caller));
    this->PropagateCurrentPoint();
    return;    
  }
  if (event == vtkViewImage2DCommand::EndZSliceMoveEvent) {
    return;    
  }
  if (event == vtkViewImage2DCommand::FullPageEvent) {
    return;    
  }
  if( event == vtkViewImage2DCommand::DefaultMoveEvent) {
    this->DefaultMove(vtkInteractorStyleImage::SafeDownCast(caller));
  }
  

}


void vtkViewImage2DCommand::StartWindowing()
{
  this->InitialWindow = this->View->GetColorWindow();
  this->InitialLevel  = this->View->GetColorLevel();
  this->View->StartWindowing();
}


void vtkViewImage2DCommand::Windowing(vtkInteractorStyleImage2D* p_isi)
{

  if( !p_isi )
  {
    return;
  }
    
  double window = this->InitialWindow;
  double level  = this->InitialLevel;
  double EPS    = 1e-8;
  
  
  double dx = p_isi->GetWindowStep();
  double dy = p_isi->GetLevelStep();
  
  // Scale by current values
  if (fabs(window) > EPS)
  {
    dx = dx * window;
  }
  else
  {
    dx = dx * (window < 0 ? -EPS : EPS);
  }
  
  if (fabs(level) > EPS)
  {
    dy = dy * level;
  }
  else
  {
    dy = dy * (level < 0 ? -EPS : EPS);
  }
  
  // Abs so that direction does not flip
  if (window < 0.0f) 
  {
    dx = -1*dx;
  }
  if (level < 0.0f)
  {
    dy = -1*dy;
  }
  
  // Compute new window level
  double newWindow = dx + window;
  double newLevel  = level - dy;
    
  // Stay away from zero and really
  if (fabs(newWindow) < EPS)
  {
    newWindow = EPS * (newWindow < 0 ? -1 : 1);
  }
  
  if (fabs(newLevel) < EPS)
  {
    newLevel = EPS * (newLevel < 0 ? -1 : 1);
  }
  
  this->View->SetWindow(newWindow);
  this->View->SetLevel(newLevel);	
  //this->View->Update();
}


void vtkViewImage2DCommand::EndWindowing()
{
  this->View->EndWindowing();
}


void vtkViewImage2DCommand::StartPicking(vtkInteractorStyle* p_isi)
{

  vtkRenderWindowInteractor *rwi = p_isi->GetInteractor();
  vtkAssemblyPath *path=NULL;
  p_isi->FindPokedRenderer(rwi->GetEventPosition()[0],
                           rwi->GetEventPosition()[1]);
  rwi->GetPicker()->Pick(rwi->GetEventPosition()[0],
                         rwi->GetEventPosition()[1], 0.0, 
                         p_isi->GetCurrentRenderer());
  vtkAbstractPropPicker *picker;
  if ( (picker=vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker())) )
  {
    path = picker->GetPath();
  }

  if ( path != NULL )
  {
    double* world = picker->GetPickPosition();

    double p_pos[3];
    this->View->ScreenCoordinatesToPosition (world, p_pos);

    // Treat extrem positions
    /*
    for (unsigned int i=0; i<3; i++)
    {
      if (pos[i] < this->View->GetWholeMinPosition(i)) pos[i] = this->View->GetWholeMinPosition(i)+0.0005;
      if (pos[i] > this->View->GetWholeMaxPosition(i)) pos[i] = this->View->GetWholeMaxPosition(i)-0.0005;
    }
    */
    // Set the position
    this->View->SetCurrentPoint(p_pos);
    //this->View->Update();
  }
  
}


void vtkViewImage2DCommand::EndPicking()
{
}


void vtkViewImage2DCommand::ChangeZSlice(vtkInteractorStyleImage2D* p_isi)
{

  if( !p_isi )
  {
    return;
  }
  
  int p_nbSlices = p_isi->GetZSliceStep();
  
  int current_slice = this->View->GetZSlice();

  int dest_slice    = current_slice + this->View->GetSliceNavigation() * p_nbSlices;

  double pos[3];
  this->View->GetPositionForSlice ( dest_slice, this->View->GetOrientation(), pos);

  this->View->SetPosition ( pos );
  //this->View->Update();
}


void vtkViewImage2DCommand::SetView(vtkViewImage2D *p_view)
{
  this->View = p_view;  
}


void vtkViewImage2DCommand::Zoom (vtkInteractorStyleImage2D* p_isi)
{
  if( !p_isi )
  {
    return;
  }
  
  this->View->SetZoom ( pow((double)1.1, p_isi->GetZoomFactor())*this->View->GetZoom() );
  //this->View->Update();
}


void vtkViewImage2DCommand::DefaultMove(vtkInteractorStyle* p_isi)
{

  if( this->View->GetCursorFollowMouse() ){
    
    vtkRenderWindowInteractor *rwi = p_isi->GetInteractor();
    vtkAssemblyPath *path=NULL;
    p_isi->FindPokedRenderer(rwi->GetEventPosition()[0],
			     rwi->GetEventPosition()[1]);
    rwi->GetPicker()->Pick(rwi->GetEventPosition()[0],
			   rwi->GetEventPosition()[1], 0.0, 
			   p_isi->GetCurrentRenderer());
    vtkAbstractPropPicker *picker;
    if ( (picker=vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker())) )
    {
      path = picker->GetPath();
    }
    
    if ( path != NULL )
    {
      double* world = picker->GetPickPosition();
      
      double p_pos[3];
      this->View->ScreenCoordinatesToPosition (world, p_pos);
      this->View->SetCurrentPoint(p_pos);    
      //this->View->Update();
    }
  }
  
}

void vtkViewImage2DCommand::PropagateWindowLevel()
{
  if( !this->View )
  {
    return;
  }

  if( !this->View->GetRenderer() )
  {
    return;
  }

  double window = this->View->GetWindow();
  double  level = this->View->GetLevel();
  
  bool LinkWindowLevel = this->View->GetLinkWindowLevel();
  if (LinkWindowLevel)
  {
    this->View->SetLinkWindowLevel ( 0 );
    this->View->SyncSetWindow (window);
    this->View->SyncSetLevel  (level);
    this->View->SetLinkWindowLevel ( LinkWindowLevel );
  }
}

void vtkViewImage2DCommand::PropagateResetWindowLevel()
{
  if( !this->View )
  {
    return;
  }

  if( !this->View->GetRenderer() )
  {
    return;
  }

  bool LinkWindowLevel = this->View->GetLinkWindowLevel();
  if (LinkWindowLevel)
  {
    this->View->SetLinkWindowLevel ( 0 );
    this->View->SyncResetWindowLevel ();
    this->View->SetLinkWindowLevel ( LinkWindowLevel );
  }
}

void vtkViewImage2DCommand::PropagateStartWindowing()
{
  if( !this->View )
  {
    return;
  }

  if( !this->View->GetRenderer() )
  {
    return;
  }

  bool LinkWindowLevel = this->View->GetLinkWindowLevel();
  if (LinkWindowLevel)
  {
    this->View->SetLinkWindowLevel ( 0 );
    this->View->SyncStartWindowing ();
    this->View->SetLinkWindowLevel ( LinkWindowLevel );
  }
}

void vtkViewImage2DCommand::PropagateEndWindowing()
{
  if( !this->View )
  {
    return;
  }

  if( !this->View->GetRenderer() )
  {
    return;
  }

  bool LinkWindowLevel = this->View->GetLinkWindowLevel();
  if (LinkWindowLevel)
  {
    this->View->SetLinkWindowLevel ( 0 );
    this->View->SyncEndWindowing ();
    this->View->SetLinkWindowLevel ( LinkWindowLevel );
  }
}

void vtkViewImage2DCommand::PropagateCurrentPoint()
{
  if( !this->View )
  {
    return;
  }

  if( !this->View->GetRenderer() )
  {
    return;
  }

  double pos[3];
  this->View->GetCurrentPoint (pos);
  
  bool LinkPosition = this->View->GetLinkPosition();
  if (LinkPosition)
  {
    this->View->SetLinkPosition ( 0 );
    this->View->SyncSetCurrentPoint  (pos);
    this->View->SetLinkPosition ( LinkPosition );
  }
}

void vtkViewImage2DCommand::PropagateResetCurrentPoint()
{
  if( !this->View )
  {
    return;
  }

  if( !this->View->GetRenderer() )
  {
    return;
  }

  bool LinkPosition = this->View->GetLinkPosition();
  if (LinkPosition)
  {
    this->View->SetLinkPosition ( 0 );
    this->View->SyncResetCurrentPoint  ();
    this->View->SetLinkPosition ( LinkPosition );
  }
}

void vtkViewImage2DCommand::PropagateZoom()
{
  if( !this->View )
  {
    return;
  }

  if( !this->View->GetRenderer() )
  {
    return;
  }

  double zoom = this->View->GetZoom();
  
  bool LinkZoom = this->View->GetLinkZoom();
  if (LinkZoom)
  {
    this->View->SetLinkZoom ( 0 );
    this->View->SyncSetZoom  (zoom);
    this->View->SetLinkZoom ( LinkZoom );
  }
}

void vtkViewImage2DCommand::PropagateResetZoom()
{
  if( !this->View )
  {
    return;
  }

  if( !this->View->GetRenderer() )
  {
    return;
  }

  bool LinkZoom = this->View->GetLinkZoom();
  if (LinkZoom)
  {
    this->View->SetLinkZoom ( 0 );
    this->View->SyncResetZoom  ();
    this->View->SetLinkZoom ( LinkZoom );
  }
}
