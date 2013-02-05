/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxVtkPageView.h 705 2008-02-23 14:04:14Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-02-23 15:04:14 +0100 (Sat, 23 Feb 2008) $
Version:   $Revision: 705 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "wxVtkViewImage2DWithSliders.h"



#include <vtkRenderingAddOn/vtkViewImage2D.h>
#include <wxVTKRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <vtkInteractorStyleImage2D.h>
#include <vtkViewImage2DCommand.h>

#include <wx/recguard.h>

wxSliderChangeObserver::wxSliderChangeObserver()
{
  this->ZSlider = 0;
  this->WindowSlider = 0;
  this->LevelSlider = 0;
  this->ZoomSlider = 0;

  this->ZLock = 0;
  this->ZoomLock = 0;
  this->WindowLevelLock = 0;
}

wxSliderChangeObserver::~wxSliderChangeObserver()
{}


void wxSliderChangeObserver::Execute (vtkObject *   caller, 
                                      unsigned long event, 
                                      void *        callData)
{
  vtkViewImage2D* view = vtkViewImage2D::SafeDownCast (caller);
  if( !view )
  {
    return;
  }

  if( event == vtkViewImage::ViewImagePositionChangeEvent && !this->ZLock)
  {
    if( this->ZSlider )
    {
      unsigned int zslice = view->GetZSlice();
      //      this->ZSlider->SetRange ( view->GetWholeZMin(), view->GetWholeZMax());
      this->ZSlider->SetValue (zslice);
    }
  }
  else if ( event==vtkViewImage::ViewImageWindowLevelChangeEvent && !this->WindowLevelLock)
  {
    if( this->WindowSlider && this->LevelSlider )
    {
      double window = view->GetWindow();
      double level  = view->GetLevel();
      
      this->WindowSlider->SetValue ( (int)(level - window*0.5) );
      this->LevelSlider->SetValue  ( (int)(level + window*0.5) );
    }
  }
  else if ( event==vtkViewImage::ViewImageZoomChangeEvent && !this->ZoomLock)
  {
    if( this->ZoomSlider )
    {
      double zoom = view->GetZoom();
      this->ZoomSlider->SetValue ( (int)(log (zoom)*10.0) );
    }
  }
  
}



BEGIN_EVENT_TABLE (wxVtkViewImage2DWithSliders, wxPanel)
  EVT_COMMAND_SCROLL(SL_ZSLIDER,      wxVtkViewImage2DWithSliders::OnZSlider)
  EVT_COMMAND_SCROLL(SL_WINDOWSLIDER, wxVtkViewImage2DWithSliders::OnWindowSlider)
  EVT_COMMAND_SCROLL(SL_LEVELSLIDER,  wxVtkViewImage2DWithSliders::OnLevelSlider)
  EVT_COMMAND_SCROLL(SL_ZOOMSLIDER,   wxVtkViewImage2DWithSliders::OnZoomSlider)
END_EVENT_TABLE()



wxVtkViewImage2DWithSliders::wxVtkViewImage2DWithSliders (wxWindow* parent, int id,
                                                          const wxPoint& pos, const wxSize& size,
                                                          long style, const wxString& title,
                                                          unsigned int orientation)
  : wxPanel (parent, id, pos, size, style, title)
{

  this->ViewInteractor = new wxVTKRenderWindowInteractor (this, wxID_ANY,
                                                          wxDefaultPosition,
                                                          wxDefaultSize,
                                                          wxWANTS_CHARS | wxNO_FULL_REPAINT_ON_RESIZE,
                                                          wxT(""));
  
  this->ViewImage2D = vtkViewImage2D::New();

  vtkRenderer* rend = vtkRenderer::New();


  this->ViewImage2D->SetRenderWindow ( this->ViewInteractor->GetRenderWindow() );
  this->ViewImage2D->SetRenderer (rend);
  this->ViewImage2D->SetOrientation (orientation);

  this->ZSlider      = new wxSlider (this, SL_ZSLIDER,      50, 0, 100);
  this->WindowSlider = new wxSlider (this, SL_WINDOWSLIDER, 50, 0, 100);
  this->LevelSlider  = new wxSlider (this, SL_LEVELSLIDER,  50, 0, 100);
  this->ZoomSlider   = new wxSlider (this, SL_ZOOMSLIDER,   0, -50, 50);


  this->SliderObserver = wxSliderChangeObserver::New();
  this->SliderObserver->SetZSlider      ( this->ZSlider );
  this->SliderObserver->SetWindowSlider ( this->WindowSlider );
  this->SliderObserver->SetLevelSlider  ( this->LevelSlider );
  this->SliderObserver->SetZoomSlider   ( this->ZoomSlider );
  
  this->ViewImage2D->AddObserver ( vtkViewImage::ViewImagePositionChangeEvent,    this->SliderObserver );
  this->ViewImage2D->AddObserver ( vtkViewImage::ViewImageWindowLevelChangeEvent, this->SliderObserver );
  this->ViewImage2D->AddObserver ( vtkViewImage::ViewImageZoomChangeEvent,        this->SliderObserver );

  rend->Delete();

  this->SetProperties();
  this->DoLayout();
  
}


wxVtkViewImage2DWithSliders::~wxVtkViewImage2DWithSliders()
{
  this->ViewImage2D->Detach();
  this->ViewImage2D->Delete();
  this->ViewInteractor->Delete();
  this->SliderObserver->Delete();
}


void wxVtkViewImage2DWithSliders::SetProperties()
{ 
}


void wxVtkViewImage2DWithSliders::DoLayout()
{
  wxBoxSizer* sizer = new wxBoxSizer (wxVERTICAL);
  sizer->Add (this->ViewInteractor, 1, wxEXPAND|wxALL, 0);
  sizer->Add (this->ZSlider, 0, wxEXPAND|wxALL, 0);
  sizer->Add (this->WindowSlider, 0, wxEXPAND|wxALL, 0);
  sizer->Add (this->LevelSlider, 0, wxEXPAND|wxALL, 0);
  sizer->Add (this->ZoomSlider, 0, wxEXPAND|wxALL, 0);

  this->SetSizer (sizer);
  this->Fit();
  this->SetAutoLayout(true);
  this->Layout();
}


void wxVtkViewImage2DWithSliders::OnZSlider (wxScrollEvent& event)
{
  int pos = event.GetPosition();

  this->SliderObserver->LockZ();
  this->ViewImage2D->SyncSetSlice (this->ViewImage2D->GetOrientation(), pos);
  this->SliderObserver->UnLockZ();
  
  this->ViewImage2D->Render();
}


void wxVtkViewImage2DWithSliders::OnWindowSlider (wxScrollEvent& event)
{
  int X = event.GetPosition();
  int Y = this->LevelSlider->GetValue();
  int level = (int)((X+Y)*0.5);
  int window  = Y-X;

  this->SliderObserver->LockWindowLevel();
  this->ViewImage2D->SyncSetWindow (window);
  this->ViewImage2D->SyncSetLevel  (level);
  this->SliderObserver->UnLockWindowLevel();
  
  this->ViewImage2D->Render();
}


void wxVtkViewImage2DWithSliders::OnLevelSlider (wxScrollEvent& event)
{
  int Y = event.GetPosition();
  int X = this->WindowSlider->GetValue();
  int level = (int)((X+Y)*0.5);
  int window  = Y-X;

  this->SliderObserver->LockWindowLevel();
  this->ViewImage2D->SyncSetWindow (window);
  this->ViewImage2D->SyncSetLevel  (level);
  this->SliderObserver->UnLockWindowLevel();
  
  this->ViewImage2D->Render();
}


void wxVtkViewImage2DWithSliders::OnZoomSlider (wxScrollEvent& event)
{ 
  int pos = event.GetPosition();
  // double z = this->ViewImage2D->GetZoom();

  this->SliderObserver->LockZoom();
  this->ViewImage2D->SyncSetZoom ( exp ((double)(pos)*0.1) );
  this->SliderObserver->UnLockZoom();

  this->ViewImage2D->Render();

}
