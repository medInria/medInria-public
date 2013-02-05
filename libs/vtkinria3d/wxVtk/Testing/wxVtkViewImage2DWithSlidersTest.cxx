/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: SynchronizedViewsTest.cxx 477 2007-11-20 17:46:10Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2007-11-20 18:46:10 +0100 (Tue, 20 Nov 2007) $
Version:   $Revision: 477 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// For compilers that supports precompilation , includes  wx/wx.h
#include  "wx/wxprec.h"

#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

#include "wxVtkViewImage2DWithSliders.h"
#include <vtkRenderingAddOn/vtkViewImage2DWithTracer.h>

#include <vtkImageEllipsoidSource.h>

void wxVtkViewImage2DWithSlidersTestMainBody()
{
  wxFrame* mainFrame = new wxFrame ((wxWindow*)(NULL), -1, wxT ("wxVtkViewImage2DWithSlidersTest"), wxDefaultPosition, wxDefaultSize);
  wxVtkViewImage2DWithSliders* window1 = new wxVtkViewImage2DWithSliders (mainFrame, -1);
  wxVtkViewImage2DWithSliders* window2 = new wxVtkViewImage2DWithSliders (mainFrame, -1);




  vtkImageEllipsoidSource* source = vtkImageEllipsoidSource::New();
  source->SetWholeExtent(0,31,0,31,0,31);
  source->SetCenter (15,15,15);
  source->SetRadius (10,5,10);
  source->SetInValue ( 255.0 );
  source->SetOutValue ( 0.0 );

  source->Update();


  double *range = source->GetOutput()->GetScalarRange();



  window1->GetViewImage2D()->SetLeftButtonInteractionStyle (vtkViewImage2D::ZOOM_INTERACTION);
  window1->GetViewImage2D()->SetMiddleButtonInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
  window1->GetViewImage2D()->SetWheelInteractionStyle        (vtkViewImage2D::SELECT_INTERACTION);
  window1->GetViewImage2D()->SetRightButtonInteractionStyle  (vtkViewImage2D::WINDOW_LEVEL_INTERACTION);


  window2->GetViewImage2D()->SetLeftButtonInteractionStyle (vtkViewImage2D::ZOOM_INTERACTION);
  window2->GetViewImage2D()->SetMiddleButtonInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
  window2->GetViewImage2D()->SetWheelInteractionStyle        (vtkViewImage2D::SELECT_INTERACTION);
  window2->GetViewImage2D()->SetRightButtonInteractionStyle  (vtkViewImage2D::WINDOW_LEVEL_INTERACTION);
  window2->GetViewImage2D()->SetOrientation (1);
  
  
  window1->GetViewImage2D()->SetImage ( source->GetOutput() );
  window2->GetViewImage2D()->SetImage ( source->GetOutput() );
  
  
  window1->GetZSlider()->SetRange ( window1->GetViewImage2D()->GetWholeZMin(), window1->GetViewImage2D()->GetWholeZMax());
  window1->GetWindowSlider()->SetRange ((int)range[0], (int)range[1]);
  window1->GetLevelSlider()->SetRange ((int)range[0], (int)range[1]);

  window2->GetZSlider()->SetRange ( window2->GetViewImage2D()->GetWholeZMin(), window2->GetViewImage2D()->GetWholeZMax());
  window2->GetWindowSlider()->SetRange ((int)range[0], (int)range[1]);
  window2->GetLevelSlider()->SetRange ((int)range[0], (int)range[1]);


  window2->GetViewImage2D()->AddChild (window1->GetViewImage2D());
  window1->GetViewImage2D()->AddChild (window2->GetViewImage2D());
  window1->GetViewImage2D()->SetLinkZoom(1);
  window2->GetViewImage2D()->SetLinkZoom(1);
  
  
  window1->GetViewImage2D()->SyncResetWindowLevel();
  window1->GetViewImage2D()->SyncResetCurrentPoint();
  window1->GetViewImage2D()->SyncResetZoom();


  
  
  
  wxBoxSizer* sizer = new wxBoxSizer (wxHORIZONTAL);
  sizer->Add (window1, 1, wxALL|wxEXPAND, 0);
  sizer->Add (window2, 1, wxALL|wxEXPAND, 0);

  // window->SetLayoutMode(wxVtkPageView::LAYOUT_NO_3D);

  mainFrame->SetSizerAndFit (sizer);
  mainFrame->Layout();
  
  mainFrame->SetSize(800,600);
  
  mainFrame->Show(true);
  
  mainFrame->Layout();

  source->Delete();
}


int wxVtkViewImage2DWithSlidersTest (int argc, char* argv[])
{
  int narg = 1;
  const char *arg[1]={"wxVtkViewImage2DWithSlidersTest"};
  wxEntry(narg, const_cast<char**>(arg));
  return 0;
}
