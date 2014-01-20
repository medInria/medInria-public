/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medVtkInriaExport.h"

//#include <vtkDataSetAlgorithm.h>
#include <vtkThreadedImageAlgorithm.h>

class MEDVTKINRIA_EXPORT vtkFlipTensorImageFilter: public vtkThreadedImageAlgorithm
{
  
 public:
  static vtkFlipTensorImageFilter *New();
  vtkTypeRevisionMacro (vtkFlipTensorImageFilter, vtkThreadedImageAlgorithm);
  void PrintSelf (ostream &os, vtkIndent indent)
  {}
  

  vtkSetMacro (FlipX, bool);
  vtkGetMacro (FlipX, bool);
  vtkSetMacro (FlipY, bool);
  vtkGetMacro (FlipY, bool);
  vtkSetMacro (FlipZ, bool);
  vtkGetMacro (FlipZ, bool);
  
  
 protected:
  vtkFlipTensorImageFilter();
  ~vtkFlipTensorImageFilter();
 
  virtual int RequestData(vtkInformation *request, 
                          vtkInformationVector **inputVector, 
                          vtkInformationVector *outputVector);
  
 private:
  vtkFlipTensorImageFilter (const vtkFlipTensorImageFilter&);
  void operator=(const vtkFlipTensorImageFilter&);

  bool FlipX;
  bool FlipY;
  bool FlipZ;
  
};



