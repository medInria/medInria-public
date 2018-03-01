/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

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

int vtkImageViewsynchronizedViewsTest(int argc, char* argv[])
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
  
  view1->SetSliceOrientation (vtkImageView2D::SLICE_ORIENTATION_XY);
  view2->SetSliceOrientation (vtkImageView2D::SLICE_ORIENTATION_XZ);
  view3->SetSliceOrientation (vtkImageView2D::SLICE_ORIENTATION_YZ);

  double bcolor[3] = {0.9,0.9,0.9};
  view4->SetBackground (bcolor);
  
  view4->SetRenderingModeToPlanar();
  view4->SetShowCube(1);
  view4->SetVolumeMapperTo3DTexture();
  
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

  pool->SetLinkSliceMove (1);
  pool->SetLinkTimeChange (0);
  pool->SetLinkColorWindowLevel (1);
  pool->SetLinkResetWindowLevel (0);
  pool->SetLinkResetViewer (0);
  pool->SetLinkRequestedPosition (0);
  pool->SetLinkCamera (0);
  pool->SetLinkZoom (0);
  pool->SetLinkPan (0);
  pool->SetLinkCurrentPoint (0);

  pool->SyncSetShowImageAxis (0);
  pool->SyncSetShowAnnotations (0);
  pool->SyncSetCursorFollowMouse (0);
  pool->SyncSetShowRulerWidget (0);
  pool->SyncSetShowScalarBar (0);
  // pool->SyncSetUseLookupTable (0);
  pool->SyncSetAnnotationStyle (vtkImageView2D::AnnotationStyle2);
  pool->SyncReset();
  view4->SetZoom (0.6);
  
  view4->SetShowActorX (0);
  view4->SetShowActorY (0);
  view4->SetShowActorZ (0);
  view4->AddExtraPlane (view1->GetImageActor());
  view4->AddExtraPlane (view2->GetImageActor());
  view4->AddExtraPlane (view3->GetImageActor());
  pool->SetShowSlicePlanes (0);
  
  const unsigned int N = 100;
  unsigned int n_2 = vtkMath::Round ((double)(N) / 2.0);
  double zfactor = 0.95;
  
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
  
  clock_t t0 = clock();
  
  int range[2];
  view1->GetSliceRange (range);
  double number_of_rounds = 1;
  double position[3]={64,64,64};
  pool->SyncSetCurrentPoint (position);
  view4->SetZoom (0.6);
  
  for (unsigned int j=0; j<(unsigned int)(number_of_rounds); j++)
  {
    for( int i=range[0]; i<range[1]; i++)
    {
      view1->GetWorldCoordinatesForSlice (i, position);
      pool->SyncSetCurrentPoint (position);
      pool->SyncRender();
    }
  }
    
  clock_t t1 = clock();
  double performance_settingslice = ( (double)(t1-t0) * 1000.0) / (number_of_rounds * (double)(range[1] - range[0]) * (double)CLOCKS_PER_SEC);  
  std::cout << "performance_settingslice: " << performance_settingslice << " ms."<< std::endl;
  view4->SetZoom (0.6);
  
  for( unsigned int i=0; i<N; i++)
  {
    pool->SyncSetCurrentPoint ( positions[i] );
    pool->SyncRender();
  }
  
  clock_t t2 = clock();
  
  double performance_settingpoint = ( (double)(t2-t1) * 1000.0) / (1.0 * (double)N * (double)CLOCKS_PER_SEC);  
  std::cout << "performance_settingpoint: " << performance_settingpoint << " ms."<< std::endl;
  
  pool->SyncReset();
  pool->SyncSetColorWindow (178);
  pool->SyncSetColorLevel (51);

  view4->GetCornerAnnotation()->SetText (3, "SyncSetWindow (decreasing)");
  view4->SetZoom (0.6);
  for( unsigned int i=0; i<n_2; i++){
    pool->SyncSetColorWindow ( view4->GetColorWindow() * zfactor );
    pool->SyncRender();
  }

  pool->SyncSetColorWindow (178);
  pool->SyncSetColorLevel (51);

  view4->GetCornerAnnotation()->SetText (3, "SyncSetWindow (increasing)");
  view4->SetZoom (0.6);
  for( unsigned int i=0; i<n_2; i++){
    pool->SyncSetColorWindow ( view4->GetColorWindow() / zfactor );
    pool->SyncRender();
  }

  pool->SyncSetColorWindow (178);
  pool->SyncSetColorLevel (51);

  view4->GetCornerAnnotation()->SetText (3, "SyncSetLevel (decreasing)");
  view4->SetZoom (0.6);
  for( unsigned int i=0; i<n_2; i++){
    pool->SyncSetColorLevel ( view4->GetColorLevel() * zfactor );
    pool->SyncRender();
  }

  pool->SyncSetColorWindow (178);
  pool->SyncSetColorLevel (51);

  view4->GetCornerAnnotation()->SetText (3, "SyncSetLevel (increasing)");
  view4->SetZoom (0.6);
  for( unsigned int i=0; i<n_2; i++){
    pool->SyncSetColorLevel ( view4->GetColorLevel() / zfactor );
    pool->SyncRender();
  }

  clock_t t3 = clock();
  
  double performance_windowlevel = ( (double)(t3-t2) * 1000.0) / (2.0 * (double)(N) * (double)CLOCKS_PER_SEC);  
  std::cout << "performance_windowlevel: " << performance_windowlevel << " ms."<< std::endl;
  
  view4->GetCornerAnnotation()->SetText (3, "SyncReset ()");
  view4->SetZoom (0.6);
  for( unsigned int i=0; i<N; i++){
    pool->SyncReset ();
    pool->SyncRender();
  }

  clock_t t4 = clock();
  
  double performance_reset = ( (double)(t4-t3) * 1000.0) / (1.0 * (double)(N) * (double)CLOCKS_PER_SEC);  
  std::cout << "performance_reset: " << performance_reset << " ms."<< std::endl;

  view4->GetCornerAnnotation()->SetText (3, "SyncRender ()");
  view4->SetZoom (0.6);
  for( unsigned int i=0; i<N; i++){
    pool->SyncRender();
  }

  clock_t t5 = clock();
  
  double performance_render = ( (double)(t5-t4) * 1000.0) / (1.0 * (double)(N) * (double)CLOCKS_PER_SEC);  
  std::cout << "performance_render: " << performance_render << " ms."<< std::endl;

  view4->SetRenderingModeToVR();
  pool->SyncReset();
  
  view4->GetCornerAnnotation()->SetText (3, "VR ()");
  view4->SetZoom (0.5);
  for( unsigned int i=0; i<n_2; i++){
    pool->SyncSetColorLevel ( view4->GetColorLevel() / zfactor );
    pool->SyncRender();
  }

  clock_t t6 = clock();
  
  double performance_volumerendering = ( (double)(t6-t5) * 1000.0) / (0.5 * (double)(N) * (double)CLOCKS_PER_SEC);  
  std::cout << "performance_volumerendering: " << performance_volumerendering << " ms."<< std::endl;

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
