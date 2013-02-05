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

#include "wxVTKRenderWindowInteractor.h"


void wxVTKRenderWindowInteractorTestMainBody()
{
  wxFrame* mainFrame = new wxFrame ((wxWindow*)(NULL), -1, wxT ("wxVtkPageViewTest"), wxDefaultPosition, wxDefaultSize);
  wxVTKRenderWindowInteractor* window = new wxVTKRenderWindowInteractor (mainFrame, -1);

  wxBoxSizer* sizer = new wxBoxSizer (wxHORIZONTAL);
  sizer->Add (window, 1, wxALL|wxEXPAND, 0);

//  window->SetLayoutMode(wxVtkPageView::LAYOUT_NO_3D);


  mainFrame->SetSizerAndFit (sizer);
  mainFrame->Layout();
  
  mainFrame->SetSize(800,600);
  
  mainFrame->Show(true);
  mainFrame->Layout();
  
  window->Delete();
  
}


int wxVTKRenderWindowInteractorTest (int argc, char* argv[])
{
  int narg = 1;
  const char *arg[1]={"wxVTKRenderWindowInteractorTest"};
  wxEntry(narg, const_cast<char**>(arg));
  return 0;
}
