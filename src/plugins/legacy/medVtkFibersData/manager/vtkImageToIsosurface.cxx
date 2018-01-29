/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkImageToIsosurface.h"

#include <vtkProperty.h>
#include <vtkObjectFactory.h>
#include <vtkReverseSense.h>

#include <sstream>
#include <assert.h>
#include <cmath>


vtkStandardNewMacro(vtkImageToIsosurface);

vtkImageToIsosurface::vtkImageToIsosurface()
{
  //allocate
  this->Thresholder        = vtkImageThreshold::New();
  this->ContourFilter      = vtkContourFilter::New();
  this->Decimate           = vtkDecimatePro::New();
  this->Smoother           = vtkSmoothPolyDataFilter::New();
  this->Normals            = vtkPolyDataNormals::New();
  this->Mapper             = vtkPolyDataMapper::New();
  this->Actor              = vtkActor::New();
  this->Caster             = vtkImageCast::New();
  this->ContourFilter->ComputeScalarsOff();
  
  this->Input = 0;
  this->Caster->SetOutputScalarTypeToFloat();
  this->Thresholder->SetInputConnection   (this->Caster->GetOutputPort());
  this->ContourFilter->SetInputConnection (this->Thresholder->GetOutputPort());

  //this->Smoother->FeatureEdgeSmoothingOn();
  this->Smoother->SetNumberOfIterations(200);
  //this->Smoother->SetInput ( this->Decimate->GetOutput() );
  this->Smoother->SetInputConnection (this->ContourFilter->GetOutputPort());
  
  //this->Decimate->SetInput ( this->ContourFilter->GetOutput() );
  this->Decimate->SetInputConnection ( this->Smoother->GetOutputPort() );
  this->Decimate->SetTargetReduction(0.9);
  //this->Decimate->PreserveTopologyOn();

  this->Normals->SetInputConnection (this->Decimate->GetOutputPort());

  this->Mapper->SetInputConnection (this->Normals->GetOutputPort());
  //this->Mapper->SetInput (this->Smoother->GetOutput());
    
  this->Actor->GetProperty()->SetAmbient (0.1);
  this->Actor->GetProperty()->SetDiffuse (0.6);
  this->Actor->GetProperty()->SetSpecularPower(10.0);
  this->Actor->GetProperty()->SetInterpolationToGouraud();
  this->Actor->SetVisibility (true);
}

vtkImageToIsosurface::~vtkImageToIsosurface()
{
  this->Caster->Delete();
  this->Thresholder->Delete();
  this->Decimate->Delete();
  this->Smoother->Delete();
  this->ContourFilter->Delete();
  this->Normals->Delete();
  this->Mapper->Delete();
  this->Actor->Delete();

  if( this->Input )
  {
    Input->Delete();
  }
}

void vtkImageToIsosurface::Update()
{
  if (!this->Input)
  {
    return;
  }

  // compute data
  this->Caster->SetInputData (Input);
  
  //this->ContourFilter->Update();
  //this->Decimate->Update();
  this->Mapper->Update();
  
  this->Actor->SetMapper ( this->Mapper );
}

void vtkImageToIsosurface::ReverseNormals ()
{
  vtkReverseSense *reverse = vtkReverseSense::New();
  reverse->SetInputConnection ( this->Normals->GetOutputPort() );
  reverse->ReverseNormalsOn();
  reverse->ReverseCellsOff();
  this->Mapper->SetInputConnection (reverse->GetOutputPort());
  reverse->Delete();
}

void vtkImageToIsosurface::SetParameters (int  val, double color[4])
{
  double dval = (double)val;

  this->Thresholder->ThresholdBetween (dval-0.5, dval+0.5);
  this->Thresholder->SetInValue (dval);
  this->Thresholder->SetOutValue (0.0);
  this->ContourFilter->SetValue(0, dval-0.5);

  this->Actor->GetProperty()->SetColor ( color[0],
                                     color[1],
                                     color[2] );
  this->Actor->GetProperty()->SetOpacity (color[3]);
  
}
