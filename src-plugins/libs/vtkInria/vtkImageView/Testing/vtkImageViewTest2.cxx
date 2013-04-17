/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageEllipsoidSource.h>
#include <vtkImageMandelbrotSource.h>
#include <vtkImageData.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkConeSource.h>
#include <vtkMatrix4x4.h>
#include <vtkImageViewCornerAnnotation.h>

#include <vtkImageView3D.h>
#include <vtkTransferFunctionPresets.h>
#include <vtkLookupTableManager.h>

int vtkImageViewTest2(int argc, char *argv[])
{
  vtkImageView3D*            view     = vtkImageView3D::New();
  vtkRenderWindowInteractor* iren     = vtkRenderWindowInteractor::New();
  vtkRenderWindow*           rwin     = vtkRenderWindow::New();
  vtkRenderer*               renderer = vtkRenderer::New();
  
  iren->SetRenderWindow (rwin);
  rwin->AddRenderer (renderer);
  view->SetRenderWindow (rwin);
  view->SetRenderer (renderer);
  
  view->SetShowScalarBar(0);
  view->SetRenderingModeToVR();
  //view->SetVolumeMapperToRayCast();
  view->SetCroppingModeToOff();

  double color[3] = {1.0, 1.0, 1.0};
  view->SetBackground (color);
  
  vtkImageEllipsoidSource* imageSource = vtkImageEllipsoidSource::New();
  imageSource->SetWholeExtent (0, 63, 0, 63, 0, 63);
  imageSource->SetCenter (32,32,32);
  imageSource->SetRadius (32,16,16);
  imageSource->Update();
  
  vtkImageData* image = imageSource->GetOutput();
  
  vtkImageEllipsoidSource* imageSource2 = vtkImageEllipsoidSource::New();
  imageSource2->SetWholeExtent (0, 127, 0, 127, 0, 127);
  imageSource2->SetCenter (32, 32, 32);
  imageSource2->SetRadius (16, 16, 16);
  imageSource2->Update();
  
  vtkImageData* image2 = imageSource2->GetOutput();

  vtkImageEllipsoidSource* imageSource3 = vtkImageEllipsoidSource::New();
  imageSource3->SetWholeExtent (0, 31, 0, 31, 0, 31);
  imageSource3->SetCenter (31, 31, 31);
  imageSource3->SetRadius (16, 16, 16);
  imageSource3->Update();
  
  vtkImageData* image3 = imageSource3->GetOutput();

  vtkConeSource *coneSource = vtkConeSource::New();
  coneSource->SetCenter(32,32,32);
  coneSource->SetResolution (8);
  coneSource->SetRadius (8.0);
  coneSource->SetHeight (8.0);
  coneSource->SetDirection (0.0,0.0,1.0);
  coneSource->Update();

  vtkPolyData *cone = coneSource->GetOutput();

  double cos30 = 0.86603;
  double sin30 = 0.5;
  
  // create rotation matrices:
  double rot1[16] = {1, 0, 0, 0, 0, cos30, sin30, 0, 0, -sin30, cos30, 0, 32, 32, 32, 1}; // rotation in X
  vtkMatrix4x4 *mat1 = vtkMatrix4x4::New();
  mat1->DeepCopy (rot1);
  mat1->Transpose();

  double rot2[16] = {cos30, 0, sin30, 0, 0, 1, 0, 0, -sin30, 0, cos30, 0, 0, 0, 0, 1}; // rotation in Y
  vtkMatrix4x4 *mat2 = vtkMatrix4x4::New();
  mat2->DeepCopy (rot2);
  mat2->Transpose();

  double rot3[16] = {cos30, sin30, 0, 0, -sin30, cos30, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}; // rotation in Z
  vtkMatrix4x4 *mat3 = vtkMatrix4x4::New();
  mat3->DeepCopy (rot3);
  mat3->Transpose();


  view->SetInput (image, mat1);
  view->Reset();
  view->Render();
  
  // same image twice
  view->SetInput (image, mat1);
  view->Reset();
  view->Render();
  
  // other image
  view->SetInput (image2, mat2);
  view->Reset();
  view->Render();

  // other image
  view->SetInput (image3, mat3);
  view->Reset();
  view->Render();

  // other image, no matrix
  view->SetInput (image);
  view->GetCornerAnnotation()->SetText (3, "No matrix");
  view->Reset();
  view->Render();

  // add layers
  view->SetInput (image2, mat2, 1);
  view->SetInput (image3, mat3, 2);
  view->GetCornerAnnotation()->SetText (3, "Add layers");
  view->Render();

  view->RemoveAllLayers();

  // skip one layer
  view->SetInput (image, mat1, 0);
  view->Render();
  view->SetInput (image2, mat2, 2);
  view->Render();

  // change transfer functions
  vtkColorTransferFunction *rgb1   = vtkColorTransferFunction::New();
  vtkPiecewiseFunction     *alpha1 = vtkPiecewiseFunction::New();
  vtkTransferFunctionPresets::GetTransferFunction("Hot Metal", rgb1, alpha1);
  view->SetTransferFunctions(rgb1, alpha1, 0);
  view->Render();

  vtkColorTransferFunction *rgb2   = vtkColorTransferFunction::New();
  vtkPiecewiseFunction     *alpha2 = vtkPiecewiseFunction::New();
  vtkTransferFunctionPresets::GetTransferFunction("Flow", rgb2, alpha2);
  view->SetTransferFunctions(rgb2, alpha2, 1);
  view->Render();

  vtkColorTransferFunction *rgb3   = vtkColorTransferFunction::New();
  vtkPiecewiseFunction     *alpha3 = vtkPiecewiseFunction::New();
  vtkTransferFunctionPresets::GetTransferFunction("Hot Green", rgb3, alpha3);
  view->SetTransferFunctions(rgb3, alpha3, 2);
  view->Render();

  // add/remove  cone
  view->AddDataSet (cone);
  view->Render();
  
  view->RemoveDataSet (cone);
  view->Render();
  
  // opacity / visibility
  view->SetVisibility (0, 0);
  view->Render();

  view->SetVisibility (1, 0);
  view->Render();

  view->SetVisibility (0, 1);
  view->Render();
 
  view->SetVisibility (1, 1);
  view->Render();
  
  view->SetVisibility (0, 2);
  view->Render();
 
  view->SetVisibility (1, 2);
  view->Render();
 
  view->SetVisibility (0, 1000);
  view->Render();
  
  view->SetVisibility (1, 1000);
  view->Render();

  view->SetOpacity (0.5, 0);
  view->Render();
  
  view->SetOpacity (1.0, 0);
  view->Render();

  view->SetOpacity (0.5, 1);
  view->Render();

  view->SetOpacity (1.0, 1);
  view->Render();

  view->SetOpacity (0.5, 2);
  view->Render();

  view->SetOpacity (1.0, 2);
  view->Render();

  view->SetOpacity (0.5, 1000);   // dummy value
  view->Render();

  view->SetOpacity (1.0, 1000);   // dummy value
  view->Render();
  
  // remove layers
  view->RemoveLayer (1);
  view->RemoveLayer (2);
  view->GetCornerAnnotation()->SetText (3, "Remove layers");
  view->Render();

  view->RemoveLayer (0); // forbidden
  view->Render();
  
  view->RemoveLayer (10); // dummy
  view->Render();

  view->Delete();
  iren->Delete();
  rwin->Delete();
  renderer->Delete();
  imageSource->Delete();
  imageSource2->Delete();
  imageSource3->Delete();
  coneSource->Delete();
  mat1->Delete();
  mat2->Delete();
  mat3->Delete();
  rgb1->Delete();
  alpha1->Delete();
  rgb2->Delete();
  alpha2->Delete();
  rgb3->Delete();
  alpha3->Delete();
  
  return EXIT_SUCCESS;
}
