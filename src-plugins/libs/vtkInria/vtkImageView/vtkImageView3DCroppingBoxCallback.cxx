/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkImageView3DCroppingBoxCallback.cxx 1133 2009-03-18 17:37:24Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-03-18 18:37:24 +0100 (mer, 18 mar 2009) $
Version:   $Revision: 1133 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkImageView3DCroppingBoxCallback.h"

#include <vtkBoxWidget.h>
#include <vtkPolyData.h>

void vtkImageView3DCroppingBoxCallback::Execute ( vtkObject *caller, unsigned long event, void* callData)
{

  if( !this->VolumeMapper )
  {
    return;
  }
  
  // get the box widget
  vtkBoxWidget *widget = vtkBoxWidget::SafeDownCast( caller );

  if( !widget )
  {
    return;
  }
  

  // Get the poly data defined by the vtkBoxWidget
  vtkPolyData* myBox = vtkPolyData::New();
  widget->GetPolyData(myBox);

  double localBounds[6]={0.0,0.0,0.0,0.0,0.0,0.0};
  const double * bounds;
  if ( callData ) 
  {
      bounds = reinterpret_cast <const double * > (callData) ;
  } 
  else 
  {
      this->VolumeMapper->GetBounds(localBounds);
      bounds  = localBounds; 
  }
  
  // myBox contains 15 points and points 8 to 13
  // define the bounding box
  double xmin, xmax, ymin, ymax, zmin, zmax;
  double* pt = myBox->GetPoint(8);
  xmin = pt[0];
  pt = myBox->GetPoint(9);
  xmax = pt[0];
  pt = myBox->GetPoint(10);
  ymin = pt[1];
  pt = myBox->GetPoint(11);
  ymax = pt[1];
  pt = myBox->GetPoint(12);
  zmin = pt[2];
  pt = myBox->GetPoint(13);
  zmax = pt[2];

  if(xmin<bounds[0]) xmin=bounds[0];
  if(ymin<bounds[2]) ymin=bounds[2];
  if(zmin<bounds[4]) zmin=bounds[4];
  if(xmax>bounds[1]) xmax=bounds[1];
  if(ymax>bounds[3]) ymax=bounds[3];
  if(zmax>bounds[5]) zmax=bounds[5];

  
  this->VolumeMapper->SetCroppingRegionPlanes(xmin, xmax, ymin, ymax, zmin, zmax);
    
  myBox->Delete();
}
