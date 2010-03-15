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
#include "wxImageViewsWithSlidersApp.h"

#include "wxImageViewsWithSlidersMainFrame.h"

bool wxImageViewsWithSlidersApp::OnInit()
{
  wxImageViewsWithSlidersMainFrame* myMainFrame = new wxImageViewsWithSlidersMainFrame ( (wxWindow*)(NULL) );
  this->SetTopWindow (myMainFrame);
  myMainFrame->Show (true);
  return true;
  
}

IMPLEMENT_APP (wxImageViewsWithSlidersApp)
