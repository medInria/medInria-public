/*============================================================================

Program:   vtkINRIA3D
Module:    $Id: HWShading 480 2007-11-21 08:48:21Z $
Language:  C++
Author:    $Author: Tim Peeters $
Date:      $Date: 2007-11-21 09:48:21 +0100 (Wed, 21 Nov 2007) $
Version:   $Revision: 480 $

The Hardware Shading (HWShading) module of vtkINRIA3D is protected by the
following copyright:

Copyright (c) 2007 Biomedical Image Analysis (BMIA) - Department of
Biomedical Engineering - Eindhoven University of Technology.
All rights reserved. See Copyright.txt for details.

The HWShading implementation was originally written by Tim Peeters (BMIA - TUe)
and published at the "11th International Fall Workshop on Vision, Modeling,
and Visualization 2006" (VMV'06):
"Visualization of the Fibrous Structure of the Heart", by T. Peeters et al.
See http://timpeeters.com/research/vmv2006 for more information.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

============================================================================*/
/**
 * interpolation.cxx
 * by Tim Peeters
 *
 * 2005-01-26	Tim Peeters	First version
 * 2005-01-31	Tim Peeters	Added test for interpolating tensors
 */

#include "vtkImageDataInterpolator.h"
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>

vtkImageDataInterpolator* interpolator;

void testInterpolation(double xmin, double ymin, double zmin,
                       double xmax, double ymax, double zmax, int steps)
{
  double x = xmin; double y = ymin; double z = zmin;
  double xinc = (xmax-xmin)/(double)steps;
  double yinc = (ymax-ymin)/(double)steps;
  double zinc = (zmax-zmin)/(double)steps;

  cout<<"-- Going from ("<<xmin<<", "<<ymin<<", "<<zmin<<") to (";
  cout<<xmax<<", "<<ymax<<", "<<zmax<<"), step ("<<xinc<<", "<<yinc<<", "<<zinc<<")."<<endl;

  
  //while ( (x <= xmax+0.01) && (y <= ymax+0.01) && (z<=zmax+0.01) )
  for (int i = -1; i < steps; i++)
    {
    cout<<"Value at ("<<x<<", "<<y<<", "<<z<<") is "<<interpolator->GetInterpolatedScalar1At(x, y, z)<<endl;
    x += xinc; y += yinc; z += zinc;
    }
}

void tensorInterpol(double xmin, double ymin, double zmin,
		double xmax, double ymax, double zmax, int steps)
{
  double x = xmin; double y = ymin; double z = zmin;
  double xinc = (xmax-xmin)/(double)steps;
  double yinc = (ymax-ymin)/(double)steps;
  double zinc = (zmax-zmin)/(double)steps;

  cout<<"-- Interpolating tensors from ("<<xmin<<", "<<ymin<<", "<<zmin<<") to (";
  cout<<xmax<<", "<<ymax<<", "<<zmax<<"), step ("<<xinc<<", "<<yinc<<", "<<zinc<<")."<<endl;

  double* tensor = NULL;
  for (int i=-1; i < steps; i++)
    {
    cout<<"Tensor at ("<<x<<", "<<y<<", "<<z<<") is:"<<endl;

    tensor = interpolator->GetInterpolatedTensorAt(x, y, z);
    if (tensor == NULL) cout << "   NULL"<<endl;
    else for (int j=0; j < 3; j++)
      {
      cout<<"   ( "<<tensor[3*j]<<", "<<tensor[3*j+1]<<", "<<tensor[3*j+2]<<" )"<<endl;
      } // for
    x += xinc; y += yinc; z += zinc;
  } // for
}

int interpolation(int argc, char *argv[]) {

  if( argc<2 )
  {
    cout << "Usage: ";
    cout << argv[0] << " VTK VOLUME" << endl;
    return -1;
  }
  
  
  // Create the reader for the data
  vtkStructuredPointsReader* reader = vtkStructuredPointsReader::New();
  reader->SetFileName(argv[1]);

  //vtkStructuredPoints* data = reader->GetOutput();
  //data->Update();
  //data->Print(cout);
  //data = NULL;

  cout<<"Creating interpolator..."<<endl;
  interpolator = vtkImageDataInterpolator::New();
  interpolator->SetInput(reader->GetOutput());
  reader->Delete(); reader = NULL;

  cout<<"========== Nearest-neighbour interpolation ==========="<<endl;
  interpolator->SetInterpolationTypeToNearest();
  testInterpolation(0, 0, 0, 4, 0, 0, 20);
  testInterpolation(0, 0, 0, 4, 4, 4, 10);
  cout<<"================ Linear interpolation ================"<<endl;
  interpolator->SetInterpolationTypeToLinear();
  testInterpolation(0, 0, 0, 4, 0, 0, 20);
  testInterpolation(0, 0, 0, 4, 4, 4, 10);

  cout<<"Interpolating tensors..."<<endl; 
  reader = vtkStructuredPointsReader::New();
  reader->SetFileName("tensors.vtk");
  interpolator->SetInput(reader->GetOutput());
  reader->Delete(); reader = NULL;

  cout<<"========== Nearest-neighbour interpolation ==========="<<endl;
  interpolator->SetInterpolationTypeToNearest();
  tensorInterpol(0, 0, 0, 3, 0, 0, 6);
  cout<<"================ Linear interpolation ================"<<endl;
  interpolator->SetInterpolationTypeToLinear();
  tensorInterpol(0, 0, 0, 3, 0, 0, 6);
  tensorInterpol(0, 0, 0, 2, 1, 0, 4);

  cout<<"Deleting interpolator..."<<endl;
  interpolator->Delete();
  interpolator = NULL;
  cout<<"Done."<<endl;

  return 0;
}
