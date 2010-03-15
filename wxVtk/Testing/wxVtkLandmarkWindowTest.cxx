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

#ifdef wxUSE_UNICODE
    #define _U(x) wxString((x),wxConvUTF8)
    #define _UU(x,y) wxString((x),y)
#else
    #define _U(x) (x)
    #define _UU(x,y) (x)
#endif

#include "wxVtkLandmarkWindow.h"


void wxVtkLandmarkWindowTestMainBody()
{
  wxFrame* mainFrame = new wxFrame ((wxWindow*)(NULL), -1, wxT ("wxVtkLandmarkWindowTest"), wxDefaultPosition, wxDefaultSize);
  wxVtkLandmarkWindow* window = new wxVtkLandmarkWindow (mainFrame, -1);

  wxBoxSizer* sizer = new wxBoxSizer (wxHORIZONTAL);
  sizer->Add (window, 1, wxALL|wxEXPAND, 0);

  mainFrame->SetSizerAndFit (sizer);
  mainFrame->Layout();
  
  mainFrame->Show(true);
  
  wxCommandEvent junk;
  wxSpinEvent junk2;
  window->OnButtonAddLandmark(junk);
  window->OnButtonRemoveLandmark(junk);
  window->OnButtonRemoveAllLandmarks(junk);
  window->OnButtonUpdate(junk);
  window->OnLandmarkFlagText(junk);
  window->OnLandmarkFlagButton(junk2);
  window->OnLandmarkChoice(junk);
}


int wxVtkLandmarkWindowTest (int argc, char* argv[])
{
  int narg = 1;
  const char *arg = "wxVtkLandmarkWindowTest";

  const wxChar * args[1];

 
  wxString argStr =_U(const_cast<char*>(arg) );
  args[0] = argStr.GetData();

  wxEntry(narg, const_cast<wxChar**>(args));
  return 0;
}
