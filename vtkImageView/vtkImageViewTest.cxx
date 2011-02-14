/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: SynchronizedViews.cxx 1080 2009-02-18 13:27:02Z acanale $
Language:  C++
Author:    $Author: acanale $
Date:      $Date: 2009-02-18 14:27:02 +0100 (mer, 18 fév 2009) $
Version:   $Revision: 1080 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkImageView2D.h"
#include "vtkImageView3D.h"
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkStructuredPointsReader.h>
#include <vtkStructuredPoints.h>
#include <vtkImageCast.h>
#include "vtkImageViewCollection.h"
#include "vtkLookupTableManager.h"

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
  vtkImageView2D* view1 = vtkImageView2D::New();
  view1->SetSliceOrientation (vtkImageView2D::SLICE_ORIENTATION_XY );
  //view1->SetSliceOrientation (vtkImageView2D::SLICE_ORIENTATION_AXIAL );
    view1->SetLeftButtonInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypeZoom);
  view1->SetPatientName ("Patient");
  view1->SetStudyName ("Study");
  view1->SetSeriesName ("Series");


  vtkImageView2D* view2 = vtkImageView2D::New();
  view2->SetSliceOrientation (vtkImageView2D::SLICE_ORIENTATION_XZ );
  view2->SetPatientName ("Patient");
  view2->SetStudyName ("Study");
  view2->SetSeriesName ("Series");


  vtkImageView2D* view3 = vtkImageView2D::New();
  view3->SetSliceOrientation (vtkImageView2D::SLICE_ORIENTATION_YZ );
  view3->SetPatientName ("Patient");
  view3->SetStudyName ("Study");
  view3->SetSeriesName ("Series");

  vtkImageView3D* view4 = vtkImageView3D::New();
  view4->SetPatientName ("Patient");
  view4->SetStudyName ("Study");
  view4->SetSeriesName ("Series");

  
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
  
  vtkStructuredPointsReader* reader = vtkStructuredPointsReader::New();
  reader->SetFileName (argv[1]);
  reader->GetOutput()->Update();

  view1->SetInput  (reader->GetOutput());
  view2->SetInput  (reader->GetOutput());
  view3->SetInput  (reader->GetOutput());
  view4->SetInput  (reader->GetOutput());
  
    vtkStructuredPointsReader* reader2 = vtkStructuredPointsReader::New();
    reader2->SetFileName (argv[2]);
    reader2->GetOutput()->Update();

    view1->SetInput  (reader2->GetOutput(), 1);
    view2->SetInput  (reader2->GetOutput(), 1);
    view3->SetInput  (reader2->GetOutput(), 1);


    vtkStructuredPointsReader* reader3 = vtkStructuredPointsReader::New();
    reader3->SetFileName (argv[3]);
    reader3->GetOutput()->Update();

    view1->SetInput  (reader3->GetOutput(), 2);
    view2->SetInput  (reader3->GetOutput(), 2);
    view3->SetInput  (reader3->GetOutput(), 2);


  vtkImageViewCollection *collection = vtkImageViewCollection::New();
  collection->AddItem (view1);
  collection->AddItem (view2);
  collection->AddItem (view3);
  collection->AddItem (view4);


  double color[3]={0.9,0.9,0.9};
  int position[2] = {0, 0};  
  collection->SyncSetShowAnnotations (1);
  view1->SetPosition (position);
  position[0] = 400; position[1] = 0;
  view2->SetPosition (position);
  position[0] = 0; position[1] = 400;
  view3->SetPosition (position);
  position[0] = 400; position[1] = 400;
  view4->SetPosition (position);
  view4->SetBackground (color);
  // view4->SetRenderingModeToVR();
  // view4->SetVolumeRayCastFunctionToMIP();

//   collection->SyncResetCurrentPoint();
//   collection->SyncResetWindowLevel();
//   collection->SyncResetCamera();
  collection->SyncReset();

  collection->SyncSetShowImageAxis (1);
  
//   collection->SyncRender();
  
  rwin1->Render();
  rwin2->Render();
  rwin3->Render();
  rwin4->Render();
  
  iren1->Start();

  view1->Delete();

  rwin1->Delete();
  iren1->Delete();
  renderer1->Delete();
  reader->Delete();
  
  return 0;
}
