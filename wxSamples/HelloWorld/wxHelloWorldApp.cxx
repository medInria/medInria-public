/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxHelloWorldApp.cxx 477 2007-11-20 17:46:10Z filus $
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
#include "wxHelloWorldApp.h"

//#include "wxHelloWorldMainFrame.h"
#include "wxHelloWorldMainFrameInteractor.h"

#include <vtkObject.h>

bool wxHelloWorldApp::OnInit()
{
  /**
     Deactivate the vtk warnings. They are cumbersome.
   */
  //vtkObject::GlobalWarningDisplayOff();

  /**
     Here we instanciate the main frame, we display it and THAT'S ALL.
   */
  
  //wxHelloWorldMainFrame* myMainFrame = new wxHelloWorldMainFrame ( (wxWindow*)(NULL) );
  wxHelloWorldMainFrameInteractor* myMainFrame = new wxHelloWorldMainFrameInteractor ( (wxWindow*)(NULL) );
  this->SetTopWindow (myMainFrame);
  myMainFrame->Show (true);
  return true;
  
}

IMPLEMENT_APP (wxHelloWorldApp)
