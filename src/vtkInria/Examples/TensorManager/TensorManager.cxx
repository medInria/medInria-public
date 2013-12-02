/*=========================================================================

 medInria

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

#include "vtkTensorManager.h"


int main (int argc, char*argv[])
{

  if( argc<3 )
  {
    std::cout << "Usage: " << std::endl;
    std::cout << "\t" << argv[0] << " <image file> <tensor file>" << std::endl;
    std::cout << "Example: " << std::endl;
    std::cout << "\t" << argv[0] << " [vtkINRIA3D_DATA_DIR]/MRI.vtk [vtkINRIA3D_DATA_DIR]/Tensors.vtk" << std::endl;
    exit (-1);
  }


  /**
     In this example, we illustrate the use of the vtkTensorVisuManager.
     We first set up a vtkImageView3D to display an image.
   */

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
     We read the tensor field == vtkStructuredPoints with tensor data.
  */
  vtkStructuredPointsReader* reader2 = vtkStructuredPointsReader::New();
  reader2->SetFileName (argv[2]);
  
  vtkStructuredPoints* tensors = reader2->GetOutput();
  tensors->Update();


  /**
     We create the vtkTensorManager, feed it with the interactor
     and the tensor data, choose the glyph shape, and finally reset
     the position and update it to make the actors up to data.
  */
  vtkTensorManager* manager = vtkTensorManager::New();
  manager->SetRenderWindowInteractor (iren);
  manager->SetInput (tensors);
  manager->SetGlyphShapeToCube();
  manager->ResetPosition();
  manager->Update();


  rwin->Render();
  iren->Start();

  view->Delete();
  iren->Delete();
  rwin->Delete();
  renderer->Delete();
  manager->Delete();
  reader->Delete();
  reader2->Delete();
  

  return 0;
}
