/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: SynchronizedViews.cxx 1080 2009-02-18 13:27:02Z acanale $
Language:  C++
Author:    $Author: acanale $
Date:      $Date: 2009-02-18 14:27:02 +0100 (Wed, 18 Feb 2009) $
Version:   $Revision: 1080 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <vtkRenderingAddOn/vtkViewImage2D.h>
#include <vtkRenderingAddOn/vtkViewImage3D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkStructuredPointsReader.h>
#include <vtkStructuredPoints.h>
#include <vtkImageCast.h>

int main (int argc, char* argv[])
{

  if (argc < 2)
  {
    std::cout << "Usage: " << std::endl;
    std::cout << "\t" << argv[0] << " <image file>" << std::endl;
    std::cout << "Example: " << std::endl;
    std::cout << "\t" << argv[0] << " [vtkINRIA3D_DATA_DIR]/MRI.vtk" << std::endl;
    exit (-1);
  }


  /**
     Create 3 views, each of them will have a different orientation, .i.e.
     axial, sagittal and coronal.
     Create one 3D view.
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


  /**
     Set some properties to the views, like the interaction style, orientation and
     background color.
   */

  /*
  view1->SetInteractionStyle (vtkViewImage2D::WINDOW_LEVEL_INTERACTION);
  view2->SetInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
  view3->SetInteractionStyle (vtkViewImage2D::ZOOM_INTERACTION);
  */

  // One can also associate to each button (left, middle, right and even wheel)
  // a specific interaction like this:
  
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
  view4->SetLinkZoom (true);

  view1->SetOrientation (vtkViewImage2D::AXIAL_ID);
  view2->SetOrientation (vtkViewImage2D::CORONAL_ID);
  view3->SetOrientation (vtkViewImage2D::SAGITTAL_ID);

  view1->SetBackgroundColor (0.0,0.0,0.0);
  view2->SetBackgroundColor (0.0,0.0,0.0);
  view3->SetBackgroundColor (0.0,0.0,0.0);

  double color[3] = {0.0,0.0,0.0};
  view4->SetTextColor (color);
  view4->SetRenderingModeToPlanar();
  view4->SetCubeVisibility(1);
  

  view1->SetAboutData ("Powered by vtkINRIA3D");
  view2->SetAboutData ("Powered by vtkINRIA3D");
  view3->SetAboutData ("Powered by vtkINRIA3D");
  view4->SetAboutData ("Powered by vtkINRIA3D");

  /**
     Link the views together for synchronization.
   */
  view1->AddChild (view2);
  view2->AddChild (view3);
  view3->AddChild (view4);
  view4->AddChild (view1);


  
  vtkStructuredPointsReader* reader = vtkStructuredPointsReader::New();
  reader->SetFileName (argv[1]);
  reader->GetOutput()->Update();
  vtkImageCast* caster = vtkImageCast::New();
  caster->SetInput (reader->GetOutput());
  caster->SetOutputScalarTypeToUnsignedShort ();
  caster->Update();
  

  view1->SetImage (caster->GetOutput());
  view2->SetImage (caster->GetOutput());
  view3->SetImage (caster->GetOutput());
  view4->SetImage (caster->GetOutput());

  /**
     Reset the window/level and the current position.
  */
  view1->SyncResetCurrentPoint();
  view1->SyncResetWindowLevel();

  rwin1->Render();
  rwin2->Render();
  rwin3->Render();
  rwin4->Render();
  
  iren1->Start();


  view1->Detach();
  view2->Detach();
  view3->Detach();
  view4->Detach();

  view1->Delete();
  view2->Delete();
  view3->Delete();
  view4->Delete();

  rwin1->Delete();
  rwin2->Delete();
  rwin3->Delete();
  rwin4->Delete();
  iren1->Delete();
  iren2->Delete();
  iren3->Delete();
  iren4->Delete();
  renderer1->Delete();
  renderer2->Delete();
  renderer3->Delete();
  renderer4->Delete();
  caster->Delete();
  reader->Delete();
  
  return 0;
}
