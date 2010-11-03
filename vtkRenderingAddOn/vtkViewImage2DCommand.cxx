/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkViewImage2DCommand.cxx 1368 2009-11-30 18:58:27Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-11-30 19:58:27 +0100 (Mon, 30 Nov 2009) $
Version:   $Revision: 1368 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// version vtkRenderingAddOn
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
  View(NULL), InitialWindow(0), InitialLevel(0),
  WindowEventStatus(false)
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
    return;    
  }
  if (event == vtkCommand::EndWindowLevelEvent) {
    this->EndWindowing();
    return;    
  }
  if (event == vtkCommand::WindowLevelEvent) {
    this->Windowing(vtkInteractorStyleImage2D::SafeDownCast(caller));
    return;    
  }
  if (event == vtkCommand::ResetWindowLevelEvent) {
    this->StartWindowing();
    this->View->SyncResetWindowLevel();
    //this->View->SyncSetWindow ( this->View->GetWindow() );
    //this->View->SyncSetLevel ( this->View->GetLevel() );
    this->EndWindowing();
    //this->View->Update();
    return;    
  }
  if (event == vtkViewImage2DCommand::ResetPositionEvent) {
    this->View->SyncResetCurrentPoint();
    //this->View->Update();
    return;    
  }
  if (event == vtkViewImage2DCommand::ZoomEvent) {
    this->Zoom(vtkInteractorStyleImage2D::SafeDownCast(caller));
    return;    
  }
  if (event == vtkViewImage2DCommand::ResetZoomEvent) {
    this->View->SyncResetZoom();
    //this->View->Update();
    return;    
  }
  if (event == vtkCommand::StartPickEvent) {
    this->StartPicking(vtkInteractorStyleImage::SafeDownCast(caller));
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
  this->View->SyncStartWindowing();
}


void vtkViewImage2DCommand::Windowing(vtkInteractorStyleImage2D* p_isi)
{

  if( !p_isi )
  {
    return;
  }
    
  double window = this->InitialWindow;
  double level  = this->InitialLevel;
  double EPS    = 0.01;
  
  
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
  
  if (this->View->GetLinkWindowLevel() )
  {
    this->View->SyncSetWindow(newWindow);
    this->View->SyncSetLevel(newLevel);
  }
  //this->View->Update();
}


void vtkViewImage2DCommand::EndWindowing()
{
  this->View->SyncEndWindowing();
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
    this->View->SyncSetCurrentPoint(p_pos);
    //this->View->SyncSetCurrentPoint(world);
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

  this->View->SyncSetPosition ( pos );
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
  
  this->View->SyncSetZoom ( pow((double)1.1, p_isi->GetZoomFactor())*this->View->GetZoom() );
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
