/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkViewImage3D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkStructuredPointsReader.h>
#include <vtkStructuredPoints.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyData.h>

#include "vtkFibersManager.h"


int main (int argc, char*argv[])
{

  if( argc<3 )
  {
    std::cout << "Usage: " << std::endl;
    std::cout << "\t" << argv[0] << " <image file> <fiber file>" << std::endl;
    std::cout << "Example: " << std::endl;
    std::cout << "\t" << argv[0] << " [vtkINRIA3D_DATA_DIR]/MRI.vtk [vtkINRIA3D_DATA_DIR]/Fibers.vtk" << std::endl;
    exit (-1);
  }


  /**
     In this example, we illustrate the use of the vtkFibersManager.
     We first set up a vtkImageView3D to display an image.
   */

  vtkObject::GlobalWarningDisplayOn();

  vtkViewImage3D*            view = vtkViewImage3D::New();
  vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
  vtkRenderWindow*           rwin = vtkRenderWindow::New();
  vtkRenderer*           renderer = vtkRenderer::New();

  iren->SetRenderWindow ( rwin );
  rwin->AddRenderer ( renderer );
  view->SetRenderWindow ( rwin );
  view->SetRenderer ( renderer );

  
  double color[3] = {0.0,0.0,0.0};
  view->SetTextColor (color);
  view->SetRenderingModeToPlanar();
  view->SetCubeVisibility(1);
  view->SetAboutData ("Powered by vtkINRIA3D");


  vtkStructuredPointsReader* reader = vtkStructuredPointsReader::New();
  reader->SetFileName (argv[1]);
  reader->GetOutput()->Update();
  

  view->SetImage (reader->GetOutput());
  view->SyncResetCurrentPoint();
  view->SyncResetWindowLevel();


  /**
     We start by creating the vtkFibersManager and setting it
     the vtkRenderWindowInteractor of the view.
  */
  vtkFibersManager* manager = vtkFibersManager::New();
  manager->SetRenderWindowInteractor (iren);
  manager->SetRenderingModeToRibbons();  
  

  /**
     We simply read the fibers (i.e., vtkPolyData).
  */
  vtkPolyDataReader* reader2 = vtkPolyDataReader::New();
  reader2->SetFileName (argv[2]);

  vtkPolyData* fibers = reader2->GetOutput();
  fibers->Update();
  
 
  rwin->Render();

  manager->UseHardwareShadersOn();  

  /**
     We simply feed the manager with the fibers and activate
     the box widget for fiber manipulation.
   */
  manager->SetInput (fibers);  
  manager->BoxWidgetOn();
  
  
  view->Render();

  iren->Start();
  
  //manager->UseHardwareShadersOff();

  manager->Delete();
  view->Delete();
  iren->Delete();
  rwin->Delete();
  renderer->Delete();
  reader->Delete();
  reader2->Delete();  

  return 0;
}
