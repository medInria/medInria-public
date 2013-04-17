/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkVisuManagementExport.h"

#include <vtkObject.h>
#include <vtkImageData.h>

/** EXPERIMENTAL CODE --- DO NOT USE FOR NOW -- Nicolas Toussaint */

/** This Class merges two images (any type of data) into a third one.
    Input image must have the same dimensions and Scalar Type */

class VTK_VISUMANAGEMENT_EXPORT vtkImageMerge: public vtkObject
{
 public:

  static vtkImageMerge *New();
  vtkTypeRevisionMacro(vtkImageMerge, vtkObject);

  vtkSetObjectMacro (Input1, vtkImageData);
  vtkGetObjectMacro (Input1, vtkImageData);
  vtkSetObjectMacro (Input2, vtkImageData);
  vtkGetObjectMacro (Input2, vtkImageData);

  vtkImageData* GetOutput() const
  { return Output; }

  /** Call this function to compute the data */
  void Update();

 protected:
  
  vtkImageMerge();
  ~vtkImageMerge();

 private:

  
  vtkImageData*      Input1;
  vtkImageData*      Input2;
  vtkImageData*      Output;
  
};



