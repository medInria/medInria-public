/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkImageToIsosurface.h 608 2008-01-14 08:21:23Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-01-14 09:21:23 +0100 (Mon, 14 Jan 2008) $
Version:   $Revision: 608 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#pragma once

#include "vtkVisuManagementExport.h"

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

class VTK_VISUMANAGEMENT_EXPORT vtkImageToIsosurface: public vtkObject
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
    this->Mapper->SetInput (this->Smoother->GetOutput());
  }

  void SetDecimationOn()
  {
    this->Mapper->SetInput (this->Decimate->GetOutput());
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



