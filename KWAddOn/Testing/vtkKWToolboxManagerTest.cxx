/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWToolboxManagerTest.cxx 477 2007-11-20 17:46:10Z filus $
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
#include <vtkRenderWindowInteractor.h>
#include <vtkKWPageView.h>
#include <vtkMetaImageData.h>
#include <vtkImageData.h>
#include <vtkKWToolBox.h>
#include <vtkKWToolboxManager.h>
#include <vtkKWIcon.h>
#include <vtkImageEllipsoidSource.h>
#include <vtkKWWindow.h>
#include "vtkKWApplication.h"
#include <vtksys/SystemTools.hxx>
#include <vtksys/CommandLineArguments.hxx>
#include "vtkKWWidgetsPaths.h"

#include <vtkErrorCode.h>
int vtkKWToolboxManagerTest(int argc, char* argv[])
{


  std::string name = "vtkKWToolboxManagerTests";
  
  char** args = new char*[ 1 ];
  args[0] = new char[name.size()+1];
  strcpy(args[0], name.c_str());
  
  Tcl_Interp *interp = vtkKWApplication::InitializeTcl(1, args, &cerr);
  if (!interp)
  {
    cerr << "Error: InitializeTcl failed" << endl ;
    return -1;
  }
  
  vtkKWApplication *app = vtkKWApplication::New();
  app->SetName("vtkKWToolboxManagerTest");
  app->PromptBeforeExitOff ();
  vtkKWWindow *win = vtkKWWindow::New();
  win->SetTitle ("vtkKWToolboxManagerTest");
  app->AddWindow(win);
  win->Create();



  //win->Display();
  
  

  vtkImageEllipsoidSource* imageSource = vtkImageEllipsoidSource::New();
  imageSource->SetWholeExtent (0, 63, 0, 63, 0, 63);
  imageSource->SetCenter (32,32,32);
  imageSource->SetRadius (32,16,16);
  imageSource->Update();
  vtkImageData* image = imageSource->GetOutput();


  vtkMetaImageData* metaimage = vtkMetaImageData::New();
  metaimage->SetName("m-metaimage");
  try
  {
    std::cerr<<"error in Set/GetDataSet method "<<std::endl;
    metaimage->SetDataSet(image);
  }
  catch (vtkErrorCode::ErrorIds)
  {
    return -1;
  }
  
    
  vtkKWToolboxManager* tbmanager = vtkKWToolboxManager::New();
  tbmanager->SetParent(win->GetViewFrame());
  tbmanager->Create();
  app->Script("pack %s -fill both -side top -expand t", 
              tbmanager->GetWidgetName());

  
  vtkKWToolBox* tb1 = vtkKWToolBox::New();  
  tb1->SetName("m-toolbox-1");
  vtkKWToolBox* tb2= vtkKWToolBox::New();
  tb2->SetName("m-toolbox-2");

  tbmanager->InsertToolbox (tb1);
  tbmanager->InsertToolbox (tb2);
  tbmanager->SelectToolbox(tb1);
  
  
  
  if (!tb1->GetName() ||
      strcmp (tb1->GetName(), "m-toolbox-1") != 0)
  {
    std::cerr<<"error in Set/GetName() method "<<std::endl;
    return -1;
  }

  
  tb1->SetMetaDataSet(metaimage);
  if (!tb1->GetMetaDataSet() ||
      tb1->GetMetaDataSet() != metaimage)
  {
    std::cerr<<"error in Set/GetMetaDataSet() method "<<std::endl;
    return -1;
  }

  

  vtkKWIcon* icon1 = vtkKWIcon::New();
  icon1->SetImage (vtkKWIcon::IconTrashcan);
  tb1->SetIcon (icon1);
  if (!tb1->GetIcon() ||
      tb1->GetIcon() != icon1)
  {
    std::cerr<<"error in Set/GetIcon() method "<<std::endl;
    return -1;
  }
  icon1->Delete();

  vtkKWIcon* icon2 = vtkKWIcon::New();
  icon2->SetImage (vtkKWIcon::IconWindowLevel);
  tb2->SetIcon (icon2);
  if (!tb2->GetIcon() ||
      tb2->GetIcon() != icon2)
  {
    std::cerr<<"error in Set/GetIcon() method "<<std::endl;
    return -1;
  }
  icon2->Delete();

  //app->Start(1, args);

  
  win->Close();
    
  imageSource->Delete();
  metaimage->Delete();
  tb1->Delete();
  tb2->Delete();
  tbmanager->Delete();

  app->RemoveWindow(win);
  win->RemoveAllChildren();
  win->Delete();
  app->Delete();   
  
  return 0;
}
