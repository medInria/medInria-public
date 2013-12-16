/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkViewImage2D.h>
#include <vtkViewImage3D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageEllipsoidSource.h>
#include <vtkMath.h>
#include <cstdlib>

#include <time.h>

int vtkRenderingSynchronizedViewsTest(int argc, char* argv[])
{

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

  // view1->SetInteractionStyle (vtkViewImage2D::WINDOW_LEVEL_INTERACTION);
  // view2->SetInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
  // view3->SetInteractionStyle (vtkViewImage2D::ZOOM_INTERACTION);

  view1->SetLinkZoom (true);
  view2->SetLinkZoom (true);
  view3->SetLinkZoom (true);
  view4->SetLinkZoom (true);

  view1->Show2DAxisOn();
  view2->Show2DAxisOn();
  view3->Show2DAxisOn();
  view1->ShowAnnotationsOn();
  view2->ShowAnnotationsOn();
  view3->ShowAnnotationsOn();
  
  view1->SetOrientation (vtkViewImage2D::AXIAL_ID);
  view2->SetOrientation (vtkViewImage2D::CORONAL_ID);
  view3->SetOrientation (vtkViewImage2D::SAGITTAL_ID);

  double color[3] = {0.0,0.0,0.0};
  view4->SetTextColor (color);
  double bcolor[3] = {0.9,0.9,0.9};
  view4->SetBackgroundColor (bcolor[0], bcolor[1], bcolor[2]);
  
  view4->SetRenderingModeToPlanar();
  view4->SetCubeVisibility(1);
  

  // view1->SetAboutData ("Powered by vtkINRIA3D");
  // view2->SetAboutData ("Powered by vtkINRIA3D");
  // view3->SetAboutData ("Powered by vtkINRIA3D");
  // view4->SetAboutData ("Powered by vtkINRIA3D");
  view1->ScalarBarVisibilityOn();
  view2->ScalarBarVisibilityOn();
  view3->ScalarBarVisibilityOn();
  view4->ScalarBarVisibilityOn();
  
  /**
     Link the views together for synchronization.
   */
  view1->AddChild (view2);
  view2->AddChild (view3);
  view3->AddChild (view4);
  view4->AddChild (view1);


  vtkImageEllipsoidSource* imageSource = vtkImageEllipsoidSource::New();
  imageSource->SetWholeExtent (0, 127, 0, 127, 0, 127);
  imageSource->SetCenter (64,64,64);
  imageSource->SetRadius (64,32,32);
  imageSource->Update();

  vtkImageData* image = imageSource->GetOutput();

  view1->SetImage (image);
  view2->SetImage (image);
  view3->SetImage (image);
  view4->SetImage (image);

  rwin1->SetPosition (20, 20);
  rwin2->SetPosition (300, 20);
  rwin3->SetPosition (20, 300);
  rwin4->SetPosition (300, 300);

  
  /**
     Reset the window/level and the current position.
  */
  view1->SyncResetCurrentPoint();
  view1->SyncResetWindowLevel();
  view1->SyncReset();
  
  rwin1->Render();
  rwin2->Render();
  rwin3->Render();
  rwin4->Render();

  view1->SetShow2DAxis (0);
  view2->SetShow2DAxis (0);
  view3->SetShow2DAxis (0);
  view1->SetShowAnnotations (0);
  view2->SetShowAnnotations (0);
  view3->SetShowAnnotations (0);
  view1->SetRulerWidgetVisibility (0);
  view2->SetRulerWidgetVisibility (0);
  view3->SetRulerWidgetVisibility (0);
  
  
  //iren1->Start();

  view1->SyncResetCurrentPoint();
  view1->SyncResetWindowLevel();
  view1->SyncReset();

  std::vector<vtkViewImage*> vecViews;
  vecViews.push_back (view1);
  vecViews.push_back (view2);
  vecViews.push_back (view3);
  vecViews.push_back (view4);  

  iren1->Initialize();
  iren2->Initialize();
  iren3->Initialize();
  iren4->Initialize();

  rwin1->SetPosition (20, 20);
  rwin2->SetPosition (300, 20);
  rwin3->SetPosition (20, 300);
  rwin4->SetPosition (300, 300);
  
  const unsigned int N = 100;
  
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
    view1->SyncSetPosition ( positions[i] );
    view1->SyncRender();
    
    // // test if the other views have the same coordinate.
    // for( int j=0; j<4; j++)
    // {
    //   const double* pos = vecViews[j]->GetCurrentPoint();
    //   if( pos[0]!=positions[i][0] || pos[1]!=positions[i][1] || pos[2]!=positions[i][2] )
    //   {
    //     return 1;
    //   }
    // }
    
  }


  clock_t t2 = clock();
  
  double performance_settingpoint = ( (double)(t2-t1) * 1000.0) / (1.0 * (double)N * (double)CLOCKS_PER_SEC);  
  std::cout << "performance_settingpoint: " << performance_settingpoint << " ms."<< std::endl;

    
  double zfactor = 0.95;

  view4->SetWindow (178);
  view4->SetLevel (51);

  unsigned int n_2 = vtkMath::Round ((double)(N) / 2.0);
  
  view4->SetDownRightAnnotation ("SyncSetWindow (decreasing)");
  for( unsigned int i=0; i<n_2; i++){
    view4->SyncSetWindow ( view4->GetWindow() * zfactor );
    view4->SyncRender();
  }
  
  view4->SetDownRightAnnotation ("SetWindow (increasing)");
  for( unsigned int i=0; i<n_2; i++){
  view4->SyncSetWindow ( view4->GetWindow() / zfactor );
  view4->SyncRender();
  }
  view4->SetDownRightAnnotation ("SyncSetLevel (decreasing)");
  for( unsigned int i=0; i<n_2; i++){
    view4->SyncSetLevel ( view4->GetLevel() * zfactor );
    view4->SyncRender();
  }
  
  view4->SetDownRightAnnotation ("SyncSetLevel (increasing)");
  for( unsigned int i=0; i<n_2; i++){
    view4->SyncSetLevel ( view4->GetLevel () / zfactor );
  view4->SyncRender();
  }

  clock_t t3 = clock();
  
  double performance_windowlevel = ( (double)(t3-t2) * 1000.0) / (2.0 * (double)(N) * (double)CLOCKS_PER_SEC);  
  std::cout << "performance_windowlevel: " << performance_windowlevel << " ms."<< std::endl;
  
  view4->SetDownRightAnnotation ("SyncReset ()");
  for( unsigned int i=0; i<N; i++){
    view4->SyncReset ();
    view4->SyncRender();
  }
  
  clock_t t4 = clock();
  
  double performance_reset = ( (double)(t4-t3) * 1000.0) / (1.0 * (double)(N) * (double)CLOCKS_PER_SEC);  
  std::cout << "performance_reset: " << performance_reset << " ms."<< std::endl;
  
  view4->SetDownRightAnnotation ("SyncRender ()");
  for( unsigned int i=0; i<N; i++){
    view4->SyncRender();
  }

  clock_t t5 = clock();
  
  double performance_render = ( (double)(t5-t4) * 1000.0) / (1.0 * (double)(N) * (double)CLOCKS_PER_SEC);  
  std::cout << "performance_render: " << performance_render << " ms."<< std::endl;

  
  view4->SetDownRightAnnotation ("VR ()");
  view4->SetRenderingModeToVR();
  view4->SyncReset();

  for( unsigned int i=0; i<n_2; i++){
    view4->SyncSetLevel ( view4->GetLevel() / zfactor );
    view4->SyncRender();
  }  

  clock_t t6 = clock();
  
  double performance_volumerendering = ( (double)(t6-t5) * 1000.0) / (0.5 * (double)(N) * (double)CLOCKS_PER_SEC);  
  std::cout << "performance_volumerendering: " << performance_volumerendering << " ms."<< std::endl;


  
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
  imageSource->Delete();


  return 0;
}
