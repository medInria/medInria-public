/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxDICOMImporterApp.cxx 477 2007-11-20 17:46:10Z filus $
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
#include "wxDICOMImporterApp.h"

#include "wxVtkDICOMImporter.h"
#include "wxVtkDICOMImporterPage2.h"

#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
  #include "pixmaps/imageviewer.xpm"
#endif

bool wxDICOMImporterApp::OnInit()
{

  wxFrame* mainFrame = new wxFrame( (wxWindow*)(NULL), wxID_ANY,
                                    wxT("Wecome to DICOMImporter"));
  mainFrame->SetIcon (wxICON (imageviewer));
  wxVtkDICOMImporter* myMainFrame = new wxVtkDICOMImporter ( NULL );
  myMainFrame->Destroy();
  mainFrame->Destroy();  
  return true;
}

IMPLEMENT_APP ( wxDICOMImporterApp )
