/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkRenderingAddOn/vtkViewImage3D.h>
#include <vtkRenderingAddOn/vtkViewImage2D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkStructuredPointsReader.h>
#include <vtkStructuredPoints.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyData.h>

#include "vtkIsosurfaceManager.h"
#include "vtkImageToIsosurface.h"


int main (int argc, char*argv[])
{

  if( argc<3 )
  {
    std::cout << "Usage: " << std::endl;
    std::cout << "\t" << argv[0] << " <image file>  <segmentation file>" << std::endl;
    std::cout << "Example: " << std::endl;
    std::cout << "\t" << argv[0] << " [vtkINRIA3D_DATA_DIR]/Atlas.vtk  [vtkINRIA3D_DATA_DIR]/AtlasSegmentation.vtk" << std::endl;
    exit (-1);
  }


  /**
     In this example, we illustrate the use of the vtkIsosurfaceManager.
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


  vtkViewImage2D*            view2 = vtkViewImage2D::New();
  vtkRenderWindowInteractor* iren2 = vtkRenderWindowInteractor::New();
  vtkRenderWindow*           rwin2 = vtkRenderWindow::New();
  vtkRenderer*           renderer2 = vtkRenderer::New();

  iren2->SetRenderWindow ( rwin2 );
  rwin2->AddRenderer ( renderer2 );
  view2->SetRenderWindow ( rwin2 );
  view2->SetRenderer ( renderer2 );

  view->AddChild (view2);
  view2->AddChild (view);


  
  double color[3] = {0.0,0.0,0.0};
  view->SetTextColor (color);
  view->SetRenderingModeToPlanar();
  view->SetCubeVisibility(1);
  view->SetAboutData ("Powered by vtkINRIA3D");

  view2->SetBackgroundColor ( 0.0,0.0,0.0 );
  view2->SetAboutData ("Powered by vtkINRIA3D");


  vtkStructuredPointsReader* reader = vtkStructuredPointsReader::New();
  reader->SetFileName ( argv[1] );
  reader->GetOutput()->Update();
  
  
  view->SetImage ( reader->GetOutput() );
  view2->SetImage ( reader->GetOutput() );
  view->SyncResetCurrentPoint();
  view->SyncResetWindowLevel();


  /**
     We start by creating the vtkIsosurfaceManager and setting it
     the vtkRenderWindowInteractor of the view.
   */
  vtkIsosurfaceManager* manager = vtkIsosurfaceManager::New();
  manager->SetRenderWindowInteractor (iren);
  manager->SetDecimationOff();
  

  /**
     We simply read the segmentation (i.e., image of labels).
   */
  vtkStructuredPointsReader* reader2 = vtkStructuredPointsReader::New();
  reader2->SetFileName (argv[2]);

  vtkImageData* segmentation = reader2->GetOutput();
  segmentation->Update();

  
  manager->SetInput (segmentation);

  /**
     The call to GenerateData() computes the number of labels, generates
     the isosurfaces, create one vtkActor per surface, and add the actors to
     the renderer.
   */
  manager->GenerateData();


  /**
     We set here the opacity of all isosurfaces to 0.5.
   */
  manager->SetOpacity (0.5);
  /*
  unsigned int n = manager->GetNumberOfIsoValues();
  for( unsigned int i=0; i<n; i++)
  {
    manager->SetOpacity (i, 0.5);
  }
  */


  for( unsigned int i=0; i<manager->GetIsosurfaces().size(); i++)
  {
    vtkProperty* property = vtkProperty::New();
    property->DeepCopy (manager->GetIsosurfaces()[i]->GetActor()->GetProperty());
    property->SetOpacity (1.0);
    property->SetAmbient (1.0);
    property->SetDiffuse (0.0);
    property->SetSpecular (0.0);
    property->SetLineWidth (3.0);
    
    view2->AddDataSet( manager->GetIsosurfaces()[i]->GetPolyData(), property );

    property->Delete();
    
  }
  

  rwin->Render();
  rwin2->Render();
  iren->Start();

  view->Detach();
  view2->Detach();
  
  view->Delete();
  iren->Delete();
  rwin->Delete();
  renderer->Delete();
  view2->Delete();
  iren2->Delete();
  rwin2->Delete();
  renderer2->Delete();
  manager->Delete();
  reader->Delete();
  reader2->Delete();
  

  return 0;
}
