/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxImageViewerApp.cxx 1296 2009-10-21 17:25:16Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-10-21 19:25:16 +0200 (Wed, 21 Oct 2009) $
Version:   $Revision: 1296 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "wxImageViewerApp.h"

#include "wxImageViewerInteractor.h"
#include "itkNiftiImageIOFactory.h"
#include "itkGISImageIOFactory.h"
#ifdef ITK_USE_REVIEW
#include <itkPhilipsRECImageIOFactory.h>
#endif

#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
  #include "pixmaps/imageviewer.xpm"
#endif

bool wxImageViewerApp::OnInit()
{
  wxFrame* mainFrame = new wxFrame( (wxWindow*)(NULL), wxID_ANY,
                                    wxT("Wecome to ImageViewer"));
  
  mainFrame->SetIcon (wxICON (imageviewer));


  itk::NiftiImageIOFactory::RegisterOneFactory();
  itk::GISImageIOFactory::RegisterOneFactory();
#ifdef ITK_USE_REVIEW
  itk::PhilipsRECImageIOFactory::RegisterOneFactory();
#endif
  

  wxImageViewerInteractor* myMainFrame = new wxImageViewerInteractor ( mainFrame );

  wxBoxSizer* mainSizer = new wxBoxSizer (wxVERTICAL);
  mainSizer->Add ( myMainFrame, 1, wxEXPAND|wxALL, 0);
  mainFrame->CreateStatusBar (1);
  mainFrame->SetAutoLayout (true);
  mainFrame->SetSizerAndFit (mainSizer);
  mainFrame->Maximize(true);
  mainFrame->Show (true);
  mainFrame->Layout();
  mainFrame->SetSize (wxSize(1000,1000));
  this->SetTopWindow( mainFrame );
  
  return true;
}

IMPLEMENT_APP ( wxImageViewerApp )
