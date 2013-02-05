/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWPageViewTest.cxx 477 2007-11-20 17:46:10Z filus $
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
#include <vtkViewImage2D.h>
#include <vtkViewImage3D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkImageData.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkKWPageView.h>

#include <vtkImageEllipsoidSource.h>
#include <vtkKWWindow.h>
#include "vtkKWApplication.h"
#include <vtksys/SystemTools.hxx>
#include <vtksys/CommandLineArguments.hxx>
#include "vtkKWWidgetsPaths.h"


int vtkKWPageViewTest(int argc, char* argv[])
{


  
  std::string name = "vtkKWPageViewTests";
  
  char** args = new char*[ 1 ];
  args[0] = new char[name.size()+1];
  strcpy(args[0], name.c_str());
  

  Tcl_Interp *interp = vtkKWApplication::InitializeTcl(1, args, &cerr);
  if (!interp)
  {
    cerr << "Error: InitializeTcl failed" << endl ;
    return 1;
  }
  
  vtkKWApplication *app = vtkKWApplication::New();
  app->SetName("vtkKWPageViewTests");
  app->PromptBeforeExitOff ();
  
  vtkKWWindow *win = vtkKWWindow::New();
  win->SetTitle ("vtkKWPageViewTests");
  app->AddWindow(win);
  win->Create();

  
  vtkKWPageView* viewframe = vtkKWPageView::New();
  viewframe->SetParent(win->GetViewFrame());
  viewframe->Create();
  viewframe->SetTag ("tag-name");
  app->Script("pack %s -fill both -side top -expand t", 
              viewframe->GetWidgetName());

  


  vtkImageEllipsoidSource* imageSource = vtkImageEllipsoidSource::New();
  imageSource->SetWholeExtent (0, 63, 0, 63, 0, 63);
  imageSource->SetCenter (32,32,32);
  imageSource->SetRadius (32,16,16);
  imageSource->Update();
  vtkImageData* image = imageSource->GetOutput();

  
  viewframe->SetImage (image);
  

  //win->Display();

  if (strcmp (viewframe->GetTag(), "tag-name") != 0)
  {
    std::cerr<<"error in GetTag() method "<<std::endl;
    return 1;
  }

  if (viewframe->GetImage() != image ||
      viewframe->GetView1()->GetImage() != image)
  {
    std::cerr<<"error in GetImage() method "<<std::endl;
    return 1;
  }
  viewframe->ToggleFullScreenView4();
  if (viewframe->GetIsFullScreen() != 4)
  {
    std::cerr<<"error in ToggleFullScreen() method "<<std::endl;
    return 1;
  }
  viewframe->ToggleFullScreenView4();
  if (viewframe->GetIsFullScreen())
  {
    std::cerr<<"error in ToggleFullScreen() method "<<std::endl;
    return 1;
  }
  viewframe->SetVisibility (image, false);
  if (viewframe->GetVisibility(image))
  {
    std::cerr<<"error in Set/GetVisibility() method "<<std::endl;
    return 1;
  }
  viewframe->SetVisibility (image, true);
  if (!viewframe->GetVisibility(image))
  {
    std::cerr<<"error in Set/GetVisibility() method "<<std::endl;
    return 1;
  }

  //app->Start(1, args);
  
  win->Close();
  imageSource->Delete();
  viewframe->Delete();
  app->RemoveWindow(win);
  win->RemoveAllChildren();
  win->Delete();
  app->Delete();  
  
  return 0;
}
