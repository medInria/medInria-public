/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id:  1182 2009-06-30 09:19:05Z  $
Language:  C++
Author:    $Author:  $
Date:      $Date: 2009-06-30 11:19:05 +0200 (mar, 30 jun 2009) $
Version:   $Revision: 1182 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifdef WXMAKINGDLL_ADDON
#undef WXMAKINGDLL_ADDON
#endif

#include "wxFlatNotebookTest.h"
#include <wx/image.h>
#include <wx/xrc/xmlres.h>
#include "Frame.h"
#include <wx/sysopt.h>
 
IMPLEMENT_APP(App)

Frame *pMainFrame = (Frame *) NULL;
wxPrintData *g_printData = (wxPrintData*) NULL;
wxPageSetupData *g_pageSetupData = (wxPageSetupData*) NULL;

// Externs
extern void InitXmlResource();

App::App(void)
{
}

App::~App(void)
{
}

// Initialize our application
bool App::OnInit(void)
{ 
	// Init resources and add the PNG handler
	wxSystemOptions::SetOption(wxT("msw.remap"), 0); 
	wxXmlResource::Get()->InitAllHandlers(); 
 	wxImage::AddHandler( new wxPNGHandler ); 
	wxImage::AddHandler( new wxCURHandler ); 
	wxImage::AddHandler( new wxBMPHandler );
	wxImage::AddHandler( new wxXPMHandler );

	InitXmlResource();

	pMainFrame = new Frame(NULL, wxT("Frame"));
	 
	// initialize print data and setup
    g_printData = new wxPrintData;
    g_pageSetupData = new wxPageSetupDialogData;

	// show the main frame
	pMainFrame->Show(TRUE);

	// set the main frame to be the main window
	SetTopWindow(pMainFrame);
	return TRUE;	
}

int App::OnExit(void) 
{
    // delete global print data and setup
    if (g_printData) delete g_printData;
    if (g_pageSetupData) delete g_pageSetupData;
	return 0;
}

