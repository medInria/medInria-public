/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkRenderingAddOn/vtkViewImage2D.h>
#include <vtkRenderingAddOn/vtkViewImage3D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageEllipsoidSource.h>
#include <vtkCallbackCommand.h>


static double positions[4][3]={
  {0.0, 0.0, 0.0},
  {64.0,  0.0,  0.0}, // test a point outside
  {63.0, 63.0, 63.0},
  {32.0, 32.0, 32.0}
};



void handle_callback (vtkObject* caller, unsigned long id, void *clientdata, void *calldata)
{
  /*
    vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::SafeDownCast( caller );
    if( !iren )
    return;
  */

  int N=500;
  
  vtkViewImage2D* view = static_cast< vtkViewImage2D* >( clientdata );

  for( int i=0; i<4; i++)
  {
    view->SetCurrentPoint ( positions[i] );
    view->Render();
  }
  
  
  int min_z = view->GetWholeZMin();
  int max_z = view->GetWholeZMax();
  for( int i=0; i<3; i++)
  {
    for( int j=min_z; j<=max_z; j++ )
    {
      view->SetZSlice ( j );
      view->Render();
    }
    for( int j=max_z-1; j>min_z; j-- )
    {
      view->SetZSlice ( j );
      view->Render();
    }
  }

  double zfactor = 0.99;
  for( int i=0; i<N; i++)
  {
    view->SetZoom ( view->GetZoom()*zfactor );
    view->Render();
  }
  for( int i=0; i<N; i++)
  {
    view->SetZoom ( view->GetZoom()/zfactor );
    view->Render();
  }
  for( int i=0; i<N; i++)
  {
    view->SetZoom ( view->GetZoom()/zfactor );
    view->Render();
  }
  for( int i=0; i<N; i++)
  {
    view->SetZoom ( view->GetZoom()*zfactor );
    view->Render();
  }


  view->SetDownRightAnnotation ("SetShowDirections(0)");
  for( int i=0; i<N; i++){
    view->SetShowDirections (0);
    view->Render();
  }

  view->SetDownRightAnnotation ("SetShowDirections(1)");
  for( int i=0; i<N; i++){
    view->SetShowDirections (1);
    view->Render();
  }

  view->SetDownRightAnnotation ("SetShowSliceNumber(0)");
  for( int i=0; i<N; i++){
    view->SetShowSliceNumber (0);
    view->Render();
  }

  view->SetDownRightAnnotation ("SetShowSliceNumber(1)");
  for( int i=0; i<N; i++){
    view->SetShowSliceNumber (1);
    view->Render();
  }

  view->SetDownRightAnnotation ("SetShow2DAxis(0)");
  for( int i=0; i<N; i++){
    view->SetShow2DAxis (0);
    view->Render();
  }

  view->SetDownRightAnnotation ("SetShow2DAxis(1)");
  for( int i=0; i<N; i++){
    view->SetShow2DAxis (1);
    view->Render();
  }

  view->SetDownRightAnnotation ("SetOrientation (vtkViewImage::CORONAL_ID)");
  for( int i=0; i<N; i++){
    view->SetOrientation (vtkViewImage::CORONAL_ID);
    view->Render();
  }

  view->SetDownRightAnnotation ("SetOrientation (vtkViewImage::SAGITTAL_ID)");
  for( int i=0; i<N; i++){
    view->SetOrientation (vtkViewImage::SAGITTAL_ID);
    view->Render();
  }

  view->SetDownRightAnnotation ("SetOrientation (vtkViewImage::AXIAL_ID)");
  for( int i=0; i<N; i++){
    view->SetOrientation (vtkViewImage::AXIAL_ID);
    view->Render();
  }

  view->SetDownRightAnnotation ("Reset()");
  for( int i=0; i<N; i++){
    view->Reset();
    view->Render();
  }

  view->SetDownRightAnnotation ("SetShowAnnotations (0)");
  for( int i=0; i<N; i++){
  view->SetShowAnnotations (0);
  view->Render();
  }

  view->SetDownRightAnnotation ("SetShowAnnotations (1)");
  for( int i=0; i<N; i++){
  view->SetShowAnnotations (1);
  view->Render();
  }

  view->SetDownRightAnnotation ("SetShowDirections (0)");
  for( int i=0; i<N; i++){
  view->SetShowDirections (0);
  view->Render();
  }

  view->SetDownRightAnnotation ("SetShowDirections (1)");
  for( int i=0; i<N; i++){
    view->SetShowDirections (1);
    view->Render();
  }

  view->SetDownRightAnnotation ("SetConventionsToNeurological ()");
  for( int i=0; i<N; i++){
    view->SetConventionsToNeurological ();
    view->Render();
  }

  view->SetDownRightAnnotation ("SetConventionsToRadiological ()");
  for( int i=0; i<N; i++){
    view->SetConventionsToRadiological ();
    view->Render();
  }

  view->SetDownRightAnnotation ("SetInterpolationMode (0)");
  for( int i=0; i<N; i++){
    view->SetInterpolationMode (0);
    view->Render();
  }

  view->SetDownRightAnnotation ("SetInterpolationMode (1)");
  for( int i=0; i<N; i++){
  view->SetInterpolationMode (1);
  view->Render();
  }
  
  view->SetDownRightAnnotation ("SetVisibility (0)");
  for( int i=0; i<N; i++){
  view->SetVisibility (0);
  view->Render();
  }

  view->SetDownRightAnnotation ("SetVisibility (1)");
  for( int i=0; i<N; i++){
  view->SetVisibility (1);
  view->Render();
  }

  view->SetDownRightAnnotation ("SetOpacity (decreasing)");
  for( int i=0; i<N; i++){
  view->SetOpacity ( view->GetOpacity() * zfactor );
  view->Render();
  }

  view->SetDownRightAnnotation ("SetOpacity (increasing)");
  for( int i=0; i<N; i++){
  view->SetOpacity ( view->GetOpacity() / zfactor );
  view->Render();
  }

  view->SetWindow (178);
  view->SetLevel (51);

  
  view->SetDownRightAnnotation ("SetWindow (decreasing)");
  for( int i=0; i<N; i++){
    view->SetWindow ( view->GetWindow() * zfactor );
  view->Render();
  }
  
  view->SetDownRightAnnotation ("SetWindow (increasing)");
  for( int i=0; i<N; i++){
  view->SetWindow ( view->GetWindow() / zfactor );
  view->Render();
  }

  view->SetDownRightAnnotation ("SetLevel (decreasing)");
  for( int i=0; i<N; i++){
    view->SetLevel ( view->GetLevel() * zfactor );
  view->Render();
  }
  
  view->SetDownRightAnnotation ("SetLevel (increasing)");
  for( int i=0; i<N; i++){
  view->SetLevel ( view->GetLevel() / zfactor );
  view->Render();
  }

  view->SetZoom (0.5);
  
  view->SetDownRightAnnotation ("SetBackground (more blue)");
  for( int i=0; i<N; i++){
  view->SetBackgroundColor ( 0,0,(double)(i)/(double)(N) );
  view->Render();
  }

  view->SetDownRightAnnotation ("SetBackground (more blue)");
  for( int i=0; i<N; i++){
  view->SetBackgroundColor ( 0,0,(double)(i)/(double)(N) );
  view->Render();
  }
  
  view->SetDownRightAnnotation ("SetShowScalarBar (0)");
  for( int i=0; i<N; i++){
    view->SetScalarBarVisibility (0);
    view->Render();
  }

  view->SetDownRightAnnotation ("SetShowScalarBar (1)");
  for( int i=0; i<N; i++){
  view->SetScalarBarVisibility (1);
  view->Render();
  }
  
}



int TestViewImage2D (int argc, char* argv[])
{

  vtkViewImage2D* view = vtkViewImage2D::New();

  vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();

  vtkRenderWindow* rwin = vtkRenderWindow::New();

  vtkRenderer* renderer = vtkRenderer::New();

  iren->SetRenderWindow (rwin);

  rwin->AddRenderer (renderer);

  view->SetRenderWindow ( rwin );

  view->SetRenderer ( renderer );

  view->SetInteractionStyle (vtkViewImage2D::WINDOW_LEVEL_INTERACTION);

  view->SetOrientation (vtkViewImage2D::AXIAL_ID);

  view->SetAboutData ("Powered by vtkINRIA3D");

  view->ScalarBarVisibilityOn();
  

  
  vtkImageEllipsoidSource* imageSource = vtkImageEllipsoidSource::New();
  imageSource->SetWholeExtent (0, 255, 0, 255, 0, 120);
  imageSource->SetCenter (128, 128, 60);
  imageSource->SetRadius (128, 64, 64);
  imageSource->Update();

  vtkImageData* image = imageSource->GetOutput();


  view->SetImage (image);
  view->ResetCurrentPoint();
  view->ResetWindowLevel();
  

  // put our timer handler on the first window
  vtkCallbackCommand* callback = vtkCallbackCommand::New();
  callback->SetClientData( view );
  callback->SetCallback(handle_callback);

  iren->AddObserver(vtkCommand::StartEvent, callback, 0.0);
  
  iren->Initialize();

  view->Render();

  iren->Start();

  
  view->Delete();
  rwin->Delete();
  iren->Delete();
  renderer->Delete();
  imageSource->Delete();
  callback->Delete();
  

  return 0;
}
