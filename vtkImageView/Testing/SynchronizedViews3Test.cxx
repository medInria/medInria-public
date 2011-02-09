/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: SynchronizedViewsTest.cxx 1211 2009-07-29 14:57:22Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-07-29 15:57:22 +0100 (Wed, 29 Jul 2009) $
Version:   $Revision: 1211 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkImageViewCollection.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageEllipsoidSource.h>
#include "vtkInteractorStyleImageView2D.h"
#include "vtkImageViewCornerAnnotation.h"
#include "vtkMath.h"

#include <time.h>
#include <cstdlib>
#include <vector>

int SynchronizedViews3Test(int argc, char* argv[])
{

  vtkImageView2D* view1 = vtkImageView2D::New();
  vtkImageView2D* view2 = vtkImageView2D::New();
  vtkImageView2D* view3 = vtkImageView2D::New();
  vtkImageView3D* view4 = vtkImageView3D::New();

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

  

  /*
    view1->SetInteractionStyle (vtkInteractorStyleImageView2D::MOUSE_INTERACTION_WINDOWING);
    view2->SetInteractionStyle (vtkInteractorStyleImageView2D::MOUSE_INTERACTION_SLICENAVIGATION);
    view3->SetInteractionStyle (vtkInteractorStyleImageView2D::MOUSE_INTERACTION_ZOOMING);
  */
  
  view1->SetSliceOrientation (vtkImageView2D::SLICE_ORIENTATION_XY);
  view2->SetSliceOrientation (vtkImageView2D::SLICE_ORIENTATION_XZ);
  view3->SetSliceOrientation (vtkImageView2D::SLICE_ORIENTATION_YZ);

  double bcolor[3] = {0.9,0.9,0.9};
  view4->SetBackground (bcolor);
  
  view4->SetRenderingModeToPlanar();
  view4->SetShowCube(1);  

  /*
  view1->SetAboutData ("Powered by vtkINRIA3D");
  view2->SetAboutData ("Powered by vtkINRIA3D");
  view3->SetAboutData ("Powered by vtkINRIA3D");
  view4->SetAboutData ("Powered by vtkINRIA3D");
  */
  
  /**
     Link the views together for synchronization.
   */
  vtkImageViewCollection* pool = vtkImageViewCollection::New();
  pool->AddItem (view1);
  pool->AddItem (view2);
  pool->AddItem (view3);
  pool->AddItem (view4);

  vtkImageEllipsoidSource* imageSource = vtkImageEllipsoidSource::New();
  imageSource->SetWholeExtent (0, 127, 0, 127, 0, 127);
  imageSource->SetCenter (64,64,64);
  imageSource->SetRadius (64,32,32);
  imageSource->Update();

  vtkImageData* image = imageSource->GetOutput();
  
  pool->SyncSetInput (image, 0);
  
  pool->SyncReset();
  pool->SyncRender();

  pool->SyncSetShowAnnotations (0);
  view1->ShowRulerWidgetOff();
  view2->ShowRulerWidgetOff();
  view3->ShowRulerWidgetOff();

  
//   view4->ShowActorXOff();
//   view4->ShowActorYOff();
//   view4->ShowActorZOff();
  

  std::vector<vtkImageView*> vecViews;
  vecViews.push_back (view1);
  vecViews.push_back (view2);
  vecViews.push_back (view3);
  vecViews.push_back (view4);
  
  rwin1->SetPosition (20, 20);
  rwin2->SetPosition (300, 20);
  rwin3->SetPosition (20, 300);
  rwin4->SetPosition (300, 300);

  rwin1->Render();
  rwin2->Render();
  rwin3->Render();
  rwin4->Render();

  pool->SyncSetShowImageAxis (1);
  pool->SyncReset();
  
  const unsigned int N = 500;
  
  double positions[N][3];
  srand( time(NULL) );

  for (unsigned int i=0; i<N; i++)
  {
    const float scale1 = rand()/float(RAND_MAX);
    const float scale2 = rand()/float(RAND_MAX);
    const float scale3 = rand()/float(RAND_MAX);
    
    positions[i][0] = scale1 * 128;
    positions[i][1] = scale2 * 128;
    positions[i][2] = scale3 * 128;
  }
	
	clock_t t1 = clock();
  
  for( unsigned int i=0; i<N; i++)
  {
    pool->SyncSetCurrentPoint ( positions[i] );
    pool->SyncRender();

    // test if the other views have the same coordinate.
    for( int j=0; j<4; j++)
    {
      const double* pos = vecViews[j]->GetCurrentPoint();
      if( pos[0]!=positions[i][0] || pos[1]!=positions[i][1] || pos[2]!=positions[i][2] )
      {
        return 1;
      }
    }
  }

  double zfactor = 0.95;

  pool->SyncReset();
  pool->SyncSetColorWindow (178);
  pool->SyncSetColorLevel (51);

  unsigned int n_2 = vtkMath::Round ((double)(N) / 2.0);

  view4->GetCornerAnnotation()->SetText (3, "SyncSetWindow (decreasing)");
  for( unsigned int i=0; i<n_2; i++){
    pool->SyncSetColorWindow ( view4->GetColorWindow() * zfactor );
    pool->SyncRender();
  }

  pool->SyncSetColorWindow (178);
  pool->SyncSetColorLevel (51);

  view4->GetCornerAnnotation()->SetText (3, "SyncSetWindow (increasing)");
  for( unsigned int i=0; i<n_2; i++){
  pool->SyncSetColorWindow ( view4->GetColorWindow() / zfactor );
  pool->SyncRender();
  }

  pool->SyncSetColorWindow (178);
  pool->SyncSetColorLevel (51);

  view4->GetCornerAnnotation()->SetText (3, "SyncSetLevel (decreasing)");
  for( unsigned int i=0; i<n_2; i++){
    pool->SyncSetColorLevel ( view4->GetColorLevel() * zfactor );
    pool->SyncRender();
  }

  pool->SyncSetColorWindow (178);
  pool->SyncSetColorLevel (51);

  view4->GetCornerAnnotation()->SetText (3, "SyncSetLevel (increasing)");
  for( unsigned int i=0; i<n_2; i++){
  pool->SyncSetColorLevel ( view4->GetColorLevel() / zfactor );
  pool->SyncRender();
  }

  view4->GetCornerAnnotation()->SetText (3, "SyncReset ()");
  for( unsigned int i=0; i<N; i++){
  pool->SyncReset ();
  pool->SyncRender();
  }
	
	clock_t t2 = clock();
	
	std::cout << "Time: " << (double)(t2-t1)/(double)CLOCKS_PER_SEC << std::endl;
  
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

  pool->Delete();
  
  imageSource->Delete();


  return 0;
}
