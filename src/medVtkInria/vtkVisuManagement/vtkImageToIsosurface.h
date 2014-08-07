/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkInriaExport.h>

#include <vtkObject.h>
#include <vtkDataObject.h>
#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkImageThreshold.h>
#include <vtkContourFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkImageCast.h>
#include <vtkDecimatePro.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkPolyDataNormals.h>

class MEDVTKINRIA_EXPORT vtkImageToIsosurface: public vtkObject
{
 public:

  static vtkImageToIsosurface *New();
  vtkTypeRevisionMacro(vtkImageToIsosurface, vtkObject);

  vtkSetObjectMacro (Input, vtkImageData);
  vtkGetObjectMacro (Input, vtkImageData);
  
  vtkActor* GetActor() const
  { return this->Actor; }

  vtkPolyData* GetPolyData() const
  { return this->Mapper->GetInput(); }

  void SetParameters (int val, double color[4]);

  void ReverseNormals();
  
  void Update();

  void SetDecimationOff()
  {
    this->Mapper->SetInputData (this->Smoother->GetOutput());
  }

  void SetDecimationOn()
  {
    this->Mapper->SetInputData (this->Decimate->GetOutput());
  }
  

 protected:
  
  vtkImageToIsosurface();
  ~vtkImageToIsosurface();

 private:

  vtkImageCast*            Caster;
  vtkImageThreshold*       Thresholder;
  vtkContourFilter*        ContourFilter;
  vtkDecimatePro*          Decimate;
  vtkSmoothPolyDataFilter* Smoother;
  vtkPolyDataNormals*      Normals;

  vtkImageData*      Input;
  vtkPolyDataMapper* Mapper;
  vtkActor*          Actor;
  
};



