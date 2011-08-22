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

#include "wxMainApp.h"

// list test here
//extern void wxVtkLandmarkWindowTestMainBody(void);
extern void wxVtkPageViewTestMainBody(void);
extern void wxVTKRenderWindowInteractorTestMainBody(void);
extern void wxVtkViewImage2DWithSlidersTestMainBody(void);

bool wxMainApp::OnInit( )
{

  for( int i=0; i<this->argc; i++)
  {
    if( wxStrcmp(this->argv[i], wxT("wxVTKRenderWindowInteractorTest"))==0 )
    {
      wxVTKRenderWindowInteractorTestMainBody();
    }
    // else if( wxStrcmp(this->argv[i], wxT("wxVtkLandmarkWindowTest"))==0 )
    // {
    //   wxVtkLandmarkWindowTestMainBody();
    // }
    else if( wxStrcmp(this->argv[i], wxT("wxVtkPageViewTest"))==0 )
    {
      wxVtkPageViewTestMainBody();
    }
    else if( wxStrcmp(this->argv[i], wxT("wxVtkViewImage2DWithSlidersTest"))==0 )
    {
      wxVtkViewImage2DWithSlidersTestMainBody();
    }
  }
  
  wxYield();
  
  return false; // exit main loop
  //return true;
  
}

IMPLEMENT_APP_NO_MAIN(wxMainApp)
