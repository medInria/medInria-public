/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWPageView.cxx 483 2007-11-21 09:03:23Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-11-21 10:03:23 +0100 (Wed, 21 Nov 2007) $
Version:   $Revision: 483 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkKWViewImage2DDialog.h"

#include "vtkKWApplication.h"
#include "vtkKWRenderWidget.h"
#include "vtkKWIcon.h"
#include "vtkObjectFactory.h"

#include <vtkWindowToImageFilter.h>
#include <vtkImageClip.h>
#include <vtkImagePermute.h>
#include <vtkImageResample.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageView2D.h>
#include <vtkImageView2DWithTracer.h>
#include <vtkImageView2DWithOrientedPoint.h>
#include <vtkImageView3D.h>

#include <vtkDataSet.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkLookupTable.h>

#include <vtkActor.h>



//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWViewImage2DDialog );
vtkCxxRevisionMacro( vtkKWViewImage2DDialog, "$Revision: 483 $");


//----------------------------------------------------------------------------
vtkKWViewImage2DDialog::vtkKWViewImage2DDialog()
{
  this->View         = vtkImageView2D::New();
  this->RenderWidget = vtkKWRenderWidget::New();
  
}



//----------------------------------------------------------------------------
vtkKWViewImage2DDialog::~vtkKWViewImage2DDialog()
{
  std::cout<<"Deleting 2d view dialog : "<<this->GetTitle()<<std::endl;

  this->View->Detach();
  this->View->Delete();
  this->RenderWidget->SetParent (NULL);
  this->RenderWidget->Delete();
}




//----------------------------------------------------------------------------
void vtkKWViewImage2DDialog::CreateWidget()
{
  // Check if already created

  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();


  
  this->RenderWidget->SetParent(this);
  this->RenderWidget->Create();

  this->View->SetRenderWindow(this->RenderWidget->GetRenderWindow());
  this->View->SetRenderer(this->RenderWidget->GetRenderer());
  this->View->GetRenderWindowInteractor()->LightFollowCameraOn();
  this->View->SetBackgroundColor (0,0,0);

  this->Script("pack %s -fill both -side top -expand t", 
	       this->RenderWidget->GetWidgetName());


  this->SetSize (500,500);

  
}

