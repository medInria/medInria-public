/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxVtkImage3DImagePlaneCallback.cxx 1116 2009-03-06 17:07:50Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-03-06 18:07:50 +0100 (Fri, 06 Mar 2009) $
Version:   $Revision: 1116 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "wxVtkImage3DImagePlaneCallback.h"

#include <vtkViewImage2D.h>
#include <vtkRenderWindow.h>

void wxVtkImage3DImagePlaneCallback::Execute ( vtkObject *caller, unsigned long, void*)
{

  if( !this->ViewImage2D )
  {
    return;
  }

  vtkImage3DImagePlaneCallback::Execute (caller, 0, NULL);
  
  if( this->FirstRender )
  {
    this->ViewImage2D->GetVtkViewImage2D()->SetImage ( this->GetOutput() );
//     this->ViewImage2D->GetVtkViewImage2D()->ResetCurrentPoint();
//     this->ViewImage2D->GetVtkViewImage2D()->ResetWindowLevel();
    this->ViewImage2D->GetVtkViewImage2D()->Reset();
    
    this->FirstRender = false;
  }

  this->ViewImage2D->Show (true);
  this->ViewImage2D->GetVtkViewImage2D()->Update();
  if( !this->ViewImage2D->GetVtkViewImage2D()->GetRenderWindow()->GetNeverRendered() )
  {
    this->ViewImage2D->GetVtkViewImage2D()->Render();
  }
}
