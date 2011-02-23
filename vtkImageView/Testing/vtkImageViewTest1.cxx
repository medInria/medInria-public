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
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageEllipsoidSource.h>
#include <vtkImageMandelbrotSource.h>
#include <vtkImageData.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>

#include <vtkImageView2D.h>
#include <vtkTransferFunctionPresets.h>

int vtkImageViewTest1(int argc, char *argv[])
{
  vtkImageView2D* view = vtkImageView2D::New();
  vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
  vtkRenderWindow* rwin = vtkRenderWindow::New();
  vtkRenderer* renderer = vtkRenderer::New();
  
  iren->SetRenderWindow (rwin);
  rwin->AddRenderer (renderer);
  view->SetRenderWindow (rwin);
  view->SetRenderer (renderer);
  
  view->SetShowScalarBar(0);
  
  vtkImageEllipsoidSource* imageSource = vtkImageEllipsoidSource::New();
  imageSource->SetWholeExtent (0, 63, 0, 63, 0, 63);
  imageSource->SetCenter (32,32,32);
  imageSource->SetRadius (32,16,16);
  imageSource->Update();
  
  vtkImageData* image = imageSource->GetOutput();
  
  view->SetInput (image);
  view->Reset();
  
  view->Render();
  
  vtkImageEllipsoidSource* imageSource2 = vtkImageEllipsoidSource::New();
  imageSource2->SetWholeExtent (0, 127, 0, 127, 0, 127);
  imageSource2->SetCenter (32, 32, 64);
  imageSource2->SetRadius (32, 32, 32);
  imageSource2->Update();
  
  vtkImageData* image2 = imageSource2->GetOutput();

  vtkImageEllipsoidSource* imageSource3 = vtkImageEllipsoidSource::New();
  imageSource3->SetWholeExtent (0, 31, 0, 31, 0, 31);
  imageSource3->SetCenter (16, 16, 16);
  imageSource3->SetRadius (16, 16, 16);
  imageSource3->Update();
  
  vtkImageData* image3 = imageSource3->GetOutput();
  
  view->SetInput(image2, 0, 1);

  view->SetInput(image3, 0, 2);
  
  /*
  vtkColorTransferFunction *rgb = vtkColorTransferFunction::New();
  vtkPiecewiseFunction *alpha = vtkPiecewiseFunction::New();
  vtkTransferFunctionPresets::GetTransferFunction("Spectrum", rgb, alpha);
  
  view->SetTransferFunctions(rgb, alpha, 1);
  */
  //view->Reset();
  
  view->Render();
  
  view->SetSlice(0);
  view->Render();

  view->SetSlice(127);
  view->Render();

  view->SetSlice(-1000);
  view->Render();
  
  view->SetSlice(1000);
  view->Render();

  view->RemoveLayer (1);
  view->Render();

  view->RemoveLayer (0);
  view->Render();

  view->RemoveLayer (10);
  view->Render();

  view->SetInput(image2, 0, 1);
  view->Render();
  
  view->RemoveLayer (1);
  view->Render();

  view->SetInput(image2, 0, 0);
  view->ResetCurrentPoint();
  view->Render();

  view->Delete();
  iren->Delete();
  rwin->Delete();
  renderer->Delete();
  imageSource->Delete();

  return EXIT_SUCCESS;
}