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

#include "vtkImageFuse.h"

#include "vtkImageMapToColors.h"
#include "vtkLookupTable.h"
#include "vtkImageBlend.h"
#include "vtkImageCheckerboard.h"
#include "vtkImageClamp.h"
#include "vtkImageMapToColors.h"
#include "vtkLookupTableManager.h"



int main (int argc, char*argv[])
{

  if( argc<3 )
  {
    std::cout << "Usage: " << std::endl;
    std::cout << "\t" << argv[0] << " <image file 1> <image file 2>" << std::endl;
    std::cout << "Example: " << std::endl;
    std::cout << "\t" << argv[0] << " [vtkINRIA3D_DATA_DIR]/MRI.vtk vtkINRIA3D_DATA_DIR]/MRI2.vtk" << std::endl;
    exit (-1);
  }


  /**
     In this example, we illustrate the use of the vtkCompareImageManager.
     We first set up four vtkImageView2D/3D to display an image.
   */

  vtkViewImage2D* view1 = vtkViewImage2D::New();
  vtkViewImage2D* view2 = vtkViewImage2D::New();
  vtkViewImage2D* view3 = vtkViewImage2D::New();
  vtkViewImage3D* view4 = vtkViewImage3D::New();
  

  vtkRenderWindowInteractor* iren1 = vtkRenderWindowInteractor::New();
  vtkRenderWindowInteractor* iren2 = vtkRenderWindowInteractor::New();
  vtkRenderWindowInteractor* iren3 = vtkRenderWindowInteractor::New();
  vtkRenderWindowInteractor* iren4 = vtkRenderWindowInteractor::New();

  
  vtkRenderWindow* rwin1 = vtkRenderWindow::New();
  vtkRenderWindow* rwin2 = vtkRenderWindow::New();
  vtkRenderWindow* rwin3 = vtkRenderWindow::New();
  vtkRenderWindow* rwin4 = vtkRenderWindow::New();
  

  vtkRenderer* renderer1 = vtkRenderer::New();
  vtkRenderer* renderer2 = vtkRenderer::New();
  vtkRenderer* renderer3 = vtkRenderer::New();
  vtkRenderer* renderer4 = vtkRenderer::New();

  
  iren1->SetRenderWindow (rwin1);
  iren2->SetRenderWindow (rwin2);
  iren3->SetRenderWindow (rwin3);
  iren4->SetRenderWindow (rwin4);

  
  rwin1->AddRenderer (renderer1);
  rwin2->AddRenderer (renderer2);
  rwin3->AddRenderer (renderer3);
  rwin4->AddRenderer (renderer4);

  
  view1->SetRenderWindow ( rwin1 );
  view2->SetRenderWindow ( rwin2 );
  view3->SetRenderWindow ( rwin3 );
  view4->SetRenderWindow ( rwin4 );
  

  view1->SetRenderer ( renderer1 );
  view2->SetRenderer ( renderer2 );
  view3->SetRenderer ( renderer3 );
  view4->SetRenderer ( renderer4 );

  
  view1->SetAboutData ("Powered by vtkINRIA3D");
  view2->SetAboutData ("Powered by vtkINRIA3D");
  view3->SetAboutData ("Powered by vtkINRIA3D");
  view4->SetAboutData ("Powered by vtkINRIA3D");


  view1->AddChild (view2);
  view2->AddChild (view3);
  view3->AddChild (view4);
  view4->AddChild (view1);

  view1->SetLeftButtonInteractionStyle   (vtkViewImage2D::ZOOM_INTERACTION);
  view1->SetMiddleButtonInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
  view1->SetWheelInteractionStyle        (vtkViewImage2D::SELECT_INTERACTION);
  view1->SetRightButtonInteractionStyle  (vtkViewImage2D::WINDOW_LEVEL_INTERACTION);

  view2->SetLeftButtonInteractionStyle   (vtkViewImage2D::ZOOM_INTERACTION);
  view2->SetMiddleButtonInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
  view2->SetWheelInteractionStyle        (vtkViewImage2D::SELECT_INTERACTION);
  view2->SetRightButtonInteractionStyle  (vtkViewImage2D::WINDOW_LEVEL_INTERACTION);

  view3->SetLeftButtonInteractionStyle   (vtkViewImage2D::ZOOM_INTERACTION);
  view3->SetMiddleButtonInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
  view3->SetWheelInteractionStyle        (vtkViewImage2D::SELECT_INTERACTION);
  view3->SetRightButtonInteractionStyle  (vtkViewImage2D::WINDOW_LEVEL_INTERACTION);
  
  view1->SetLinkZoom (true);
  view2->SetLinkZoom (true);
  view3->SetLinkZoom (true);

  view1->SetLinkWindowLevel (0);
  view2->SetLinkWindowLevel (0);
  view3->SetLinkWindowLevel (0);
  

  vtkStructuredPointsReader* reader1 = vtkStructuredPointsReader::New();
  reader1->SetFileName (argv[1]);
  reader1->GetOutput()->Update();
  
  
  vtkStructuredPointsReader* reader2 = vtkStructuredPointsReader::New();
  reader2->SetFileName (argv[2]);
  reader2->GetOutput()->Update();


  view1->SetImage (reader1->GetOutput());
  view2->SetImage (reader2->GetOutput());

  vtkLookupTable* spectrumlut = vtkLookupTableManager::GetSpectrumLookupTable();
  vtkLookupTable* gelut = vtkLookupTableManager::GetGEColorLookupTable();
  view2->SetLookupTable (gelut);

  view1->SyncReset();
  
  vtkImageFuse* manager1 = vtkImageFuse::New();
  vtkImageFuse* manager2 = vtkImageFuse::New();

  manager1->UseClampOff();
  manager1->UseColorMapsOn();
  
  manager1->SetLookupTable1 (vtkLookupTable::SafeDownCast (view1->GetLookupTable()));
  manager1->SetLookupTable2 (vtkLookupTable::SafeDownCast (view2->GetLookupTable()));
  manager2->SetLookupTable1 (vtkLookupTable::SafeDownCast (view1->GetLookupTable()));
  manager2->SetLookupTable2 (vtkLookupTable::SafeDownCast (view2->GetLookupTable()));
  
  manager1->SetInput ( 0, reader1->GetOutput() );
  manager1->SetInput ( 1, reader2->GetOutput() );
  manager2->SetInput ( 0, reader1->GetOutput() );
  manager2->SetInput ( 1, reader2->GetOutput() );
  manager1->SetFuseMode ( vtkImageFuse::FUSEMODE_BLEND );
  manager2->SetFuseMode ( vtkImageFuse::FUSEMODE_GRID );
  manager1->SetBlendAlpha (0.5);
  manager2->SetCheckerAlpha (0.06);
  
  manager1->Update();
  manager2->Update();

  view3->SetImage (manager1->GetOutput());
  view4->SetImage (manager2->GetOutput());

  view1->SyncReset();

  rwin1->Render();
  rwin2->Render();
  rwin3->Render();
  rwin4->Render();
  
  iren1->Start();
  iren2->Start();
  iren3->Start();
  iren4->Start();


  spectrumlut->Delete();
  gelut->Delete();

  manager1->Delete();
  manager2->Delete();
  
  view1->Detach();
  view2->Detach();
  view3->Detach();
  view4->Detach();
  
  view1->Delete();
  view2->Delete();
  view3->Delete();
  view4->Delete();
  
  iren1->Delete();
  iren2->Delete();
  iren3->Delete();
  iren4->Delete();
  
  rwin1->Delete();
  rwin2->Delete();
  rwin3->Delete();
  rwin4->Delete();
  
  renderer1->Delete();
  renderer2->Delete();
  renderer3->Delete();
  renderer4->Delete();  
  
  reader1->Delete();
  reader2->Delete();
  

  return 0;
}
