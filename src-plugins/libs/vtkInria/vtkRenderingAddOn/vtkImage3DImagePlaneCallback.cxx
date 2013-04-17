/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkRenderingAddOn/vtkImage3DImagePlaneCallback.h>

#include <vtkPlaneWidget.h>
#include <vtkImageReslice.h>
#include <vtkImageData.h>
#include <vtkMath.h>
#include <vtkRenderingAddOn/vtkViewImage2D.h>
#include <vtkPlane.h>



void vtkImage3DImagePlaneCallback::Execute ( vtkObject *caller, unsigned long, void* )
{
  /*
  if( !this->ViewImage2D )
  {
    return;
  }
  */
  
  // get the box widget
//   vtkPlaneWidget *widget = reinterpret_cast<vtkPlaneWidget*>(caller);
  vtkPlaneWidget *widget = vtkPlaneWidget::SafeDownCast(caller);
  if( !widget )
  {
    return;
  }

  vtkImageData* imageData = vtkImageData::SafeDownCast( widget->GetInput() );
  
  if( !imageData )
  {
    this->Reslice->SetInput (NULL);
    return;
  }

  this->Reslice->SetInput ( imageData );


  // Calculate appropriate pixel spacing for the reslicing
  imageData->UpdateInformation();
  double spacing[3];
  imageData->GetSpacing(spacing);

  
  double imOrigin[3];
  imageData->GetOrigin(imOrigin);
  int extent[6];
  imageData->GetWholeExtent(extent);
  double bounds[] = {imOrigin[0] + spacing[0]*extent[0], //xmin
                     imOrigin[0] + spacing[0]*extent[1], //xmax
                     imOrigin[1] + spacing[1]*extent[2], //ymin
                     imOrigin[1] + spacing[1]*extent[3], //ymax
                     imOrigin[2] + spacing[2]*extent[4], //zmin
                     imOrigin[2] + spacing[2]*extent[5]};//zmax


  int i = 0;
  
  for (i = 0; i <= 4; i += 2 ) // reverse bounds if necessary
  {
    if ( bounds[i] > bounds[i+1] )
    {
      double t = bounds[i+1];
      bounds[i+1] = bounds[i];
      bounds[i] = t;
    }
  }
  
  double abs_normal[3];
  widget->GetNormal(abs_normal);
  double planeCenter[3];
  widget->GetCenter(planeCenter);
  double nmax = 0.0;
  int k = 0;
  for (i = 0; i < 3; i++ )
  {
    abs_normal[i] = fabs(abs_normal[i]);
    if ( abs_normal[i]>nmax )
    {
      nmax = abs_normal[i];
      k = i;
    }
  }
  
  // Force the plane to lie within the true image bounds along its normal
  //
  
  if ( planeCenter[k] > bounds[2*k+1] )
  {
    planeCenter[k] = bounds[2*k+1];
  }
  else if ( planeCenter[k] < bounds[2*k] )
  {
    planeCenter[k] = bounds[2*k];
  }

  widget->SetCenter(planeCenter);
  
  
  
  // get the planes
  double point1[3], point2[3], origin[4], normal[3];
  widget->GetPoint1 (point1);
  widget->GetPoint2 (point2);
  widget->GetOrigin (origin);
  widget->GetNormal (normal);

  double axis1[3], axis2[3];
  for(i=0; i<3; i++)
  {
    axis1[i] = point1[i]-origin[i];
    axis2[i] = point2[i]-origin[i];
  }

  double planeSizeX = vtkMath::Normalize(axis1);
  double planeSizeY = vtkMath::Normalize(axis2);

  this->ResliceAxes->Identity();
  for (i = 0; i < 3; i++ )
  {
    this->ResliceAxes->SetElement(0,i,axis1[i]);
    this->ResliceAxes->SetElement(1,i,axis2[i]);
    this->ResliceAxes->SetElement(2,i,normal[i]);
  }

  origin[3] = 1.0;
  double originXYZW[4];
  this->ResliceAxes->MultiplyPoint(origin,originXYZW);

  this->ResliceAxes->Transpose();
  double neworiginXYZW[4];
  double point[] =  {originXYZW[0],originXYZW[1],originXYZW[2],originXYZW[3]};
  this->ResliceAxes->MultiplyPoint(point,neworiginXYZW);

  this->ResliceAxes->SetElement(0,3,neworiginXYZW[0]);
  this->ResliceAxes->SetElement(1,3,neworiginXYZW[1]);
  this->ResliceAxes->SetElement(2,3,neworiginXYZW[2]);

  this->Reslice->SetResliceAxes (this->ResliceAxes);
  this->Reslice->SetInterpolationModeToLinear();
  this->Reslice->SetOutputSpacing (1.0,1.0,1.0);
  this->Reslice->SetOutputOrigin (0,0,0);
  this->Reslice->SetOutputExtent (0, (int)planeSizeX-1, 0, (int)planeSizeY-1,0,0);

  this->Reslice->Update();
  
  /*
  if( this->FirstRender )
  {
    this->ViewImage2D->SetImage ( this->Reslice->GetOutput() );
    this->ViewImage2D->ResetCurrentPoint();
    this->ViewImage2D->ResetWindowLevel();
    this->FirstRender = false;
  }
  
  this->ViewImage2D->Update();
  this->ViewImage2D->Render();
  */
}
