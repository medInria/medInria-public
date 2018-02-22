/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
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

#include <vtkImageView2D.h>
#include <vtkTransferFunctionPresets.h>
#include <vtkLookupTableManager.h>

int vtkImageViewTest3(int argc, char *argv[])
{
  vtkImageView2D*            view     = vtkImageView2D::New();
  vtkRenderWindowInteractor* iren     = vtkRenderWindowInteractor::New();
  vtkRenderWindow*           rwin     = vtkRenderWindow::New();
  vtkRenderer*               renderer = vtkRenderer::New();
  
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




  ////////////////////////////////////////////////////////////////////
  //// Test with identity matrix: i.e. for an axial native image /////
  ////////////////////////////////////////////////////////////////////
  
  std::cout<<" testing axial native orientation..."<<std::endl;
  vtkMatrix4x4* matrix1 = vtkMatrix4x4::New();
  matrix1->Identity();
  view->SetOrientationMatrix (matrix1);
  
  view->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_AXIAL);
  if (view->GetSliceOrientation() != vtkImageView2D::SLICE_ORIENTATION_XY)
  {
    std::cerr<<"wrong parametrization :"<<std::endl;
    std::cerr<<"view->GetViewOrientation() : " <<view->GetViewOrientation() <<std::endl;
    std::cerr<<"view->GetSliceOrientation() : "<<view->GetSliceOrientation()<<std::endl;
    return EXIT_FAILURE;
  }
  view->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_CORONAL);
  if (view->GetSliceOrientation() != vtkImageView2D::SLICE_ORIENTATION_XZ)
  {
    std::cerr<<"wrong parametrization :"<<std::endl;
    std::cerr<<"view->GetViewOrientation() : " <<view->GetViewOrientation() <<std::endl;
    std::cerr<<"view->GetSliceOrientation() : "<<view->GetSliceOrientation()<<std::endl;
    return EXIT_FAILURE;
  }
  view->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_SAGITTAL);
  if (view->GetSliceOrientation() != vtkImageView2D::SLICE_ORIENTATION_YZ)
  {
    std::cerr<<"wrong parametrization :"<<std::endl;
    std::cerr<<"view->GetViewOrientation() : " <<view->GetViewOrientation() <<std::endl;
    std::cerr<<"view->GetSliceOrientation() : "<<view->GetSliceOrientation()<<std::endl;
    return EXIT_FAILURE;
  }
  matrix1->Delete();
  std::cout<<" success." <<std::endl;




  

  ////////////////////////////////////////////////////////////////////
  ////          Test with a sagittal native image                /////
  ////////////////////////////////////////////////////////////////////

  std::cout<<" testing sagittal native orientation..."<<std::endl;
  vtkMatrix4x4* matrix2 = vtkMatrix4x4::New();
  matrix2->Zero();

  matrix2->SetElement(1,0, 1);
  matrix2->SetElement(2,1, 1);
  matrix2->SetElement(0,2, 1);
  
  view->SetOrientationMatrix (matrix2);
  
  view->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_SAGITTAL);
  if (view->GetSliceOrientation() != vtkImageView2D::SLICE_ORIENTATION_XY)
  {
    std::cerr<<"wrong parametrization :"<<std::endl;
    std::cerr<<"view->GetViewOrientation() : " <<view->GetViewOrientation() <<std::endl;
    std::cerr<<"view->GetSliceOrientation() : "<<view->GetSliceOrientation()<<std::endl;
    return EXIT_FAILURE;
  }
  view->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_AXIAL);
  if (view->GetSliceOrientation() != vtkImageView2D::SLICE_ORIENTATION_XZ)
  {
    std::cerr<<"wrong parametrization :"<<std::endl;
    std::cerr<<"view->GetViewOrientation() : " <<view->GetViewOrientation() <<std::endl;
    std::cerr<<"view->GetSliceOrientation() : "<<view->GetSliceOrientation()<<std::endl;
    return EXIT_FAILURE;
  }
  view->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_CORONAL);
  if (view->GetSliceOrientation() != vtkImageView2D::SLICE_ORIENTATION_YZ)
  {
    std::cerr<<"wrong parametrization :"<<std::endl;
    std::cerr<<"view->GetViewOrientation() : " <<view->GetViewOrientation() <<std::endl;
    std::cerr<<"view->GetSliceOrientation() : "<<view->GetSliceOrientation()<<std::endl;
    return EXIT_FAILURE;
  }
  matrix2->Delete();
  std::cout<<" success." <<std::endl;



  

  ////////////////////////////////////////////////////////////////////
  ////           Test with a coronal native image                /////
  ////////////////////////////////////////////////////////////////////

  std::cout<<" testing coronal native orientation..."<<std::endl;
  vtkMatrix4x4* matrix3 = vtkMatrix4x4::New();
  matrix3->Zero();

  matrix3->SetElement(2,0, 1);
  matrix3->SetElement(0,1, 1);
  matrix3->SetElement(1,2, 1);
  
  view->SetOrientationMatrix (matrix3);
  
  view->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_CORONAL);
  if (view->GetSliceOrientation() != vtkImageView2D::SLICE_ORIENTATION_XY)
  {
    std::cerr<<"wrong parametrization :"<<std::endl;
    std::cerr<<"view->GetViewOrientation() : " <<view->GetViewOrientation() <<std::endl;
    std::cerr<<"view->GetSliceOrientation() : "<<view->GetSliceOrientation()<<std::endl;
    return EXIT_FAILURE;
  }
  view->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_SAGITTAL);
  if (view->GetSliceOrientation() != vtkImageView2D::SLICE_ORIENTATION_XZ)
  {
    std::cerr<<"wrong parametrization :"<<std::endl;
    std::cerr<<"view->GetViewOrientation() : " <<view->GetViewOrientation() <<std::endl;
    std::cerr<<"view->GetSliceOrientation() : "<<view->GetSliceOrientation()<<std::endl;
    return EXIT_FAILURE;
  }
  view->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_AXIAL);
  if (view->GetSliceOrientation() != vtkImageView2D::SLICE_ORIENTATION_YZ)
  {
    std::cerr<<"wrong parametrization :"<<std::endl;
    std::cerr<<"view->GetViewOrientation() : " <<view->GetViewOrientation() <<std::endl;
    std::cerr<<"view->GetSliceOrientation() : "<<view->GetSliceOrientation()<<std::endl;
    return EXIT_FAILURE;
  }
  matrix3->Delete();
  std::cout<<" success." <<std::endl;


  ////////////////////////////////////////////////////////////////////
  ////             Test with an oblique native acquisition       /////
  ////////////////////////////////////////////////////////////////////

  std::cout<<" testing oblique native orientation (but slice-encoding direction close to sagittal)..."<<std::endl;
  vtkMatrix4x4* matrix4 = vtkMatrix4x4::New();
  matrix4->Zero();

  matrix4->SetElement(0,0, 0.277064);  matrix4->SetElement(1,0, 0.798317);  matrix4->SetElement(2,0, -0.53472);
  matrix4->SetElement(0,1, -0.720176); matrix4->SetElement(1,1, -0.195862); matrix4->SetElement(2,1, -0.665571);
  matrix4->SetElement(0,2, -0.636068); matrix4->SetElement(1,2, 0.569498);  matrix4->SetElement(2,2, 0.520662);
  
  view->SetOrientationMatrix (matrix4);
  
  view->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_SAGITTAL);
  if (view->GetSliceOrientation() != vtkImageView2D::SLICE_ORIENTATION_XY)
  {
    std::cerr<<"wrong parametrization :"<<std::endl;
    std::cerr<<"view->GetViewOrientation() : " <<view->GetViewOrientation() <<std::endl;
    std::cerr<<"view->GetSliceOrientation() : "<<view->GetSliceOrientation()<<std::endl;
    return EXIT_FAILURE;
  }
  view->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_AXIAL);
  if (view->GetSliceOrientation() != vtkImageView2D::SLICE_ORIENTATION_XZ)
  {
    std::cerr<<"wrong parametrization :"<<std::endl;
    std::cerr<<"view->GetViewOrientation() : " <<view->GetViewOrientation() <<std::endl;
    std::cerr<<"view->GetSliceOrientation() : "<<view->GetSliceOrientation()<<std::endl;
    return EXIT_FAILURE;
  }
  view->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_CORONAL);
  if (view->GetSliceOrientation() != vtkImageView2D::SLICE_ORIENTATION_YZ)
  {
    std::cerr<<"wrong parametrization :"<<std::endl;
    std::cerr<<"view->GetViewOrientation() : " <<view->GetViewOrientation() <<std::endl;
    std::cerr<<"view->GetSliceOrientation() : "<<view->GetSliceOrientation()<<std::endl;
    return EXIT_FAILURE;
  }
  matrix4->Delete();
  std::cout<<" success." <<std::endl;

  view->Delete();
  iren->Delete();
  rwin->Delete();
  renderer->Delete();
  imageSource->Delete();
  
  return EXIT_SUCCESS;
}
