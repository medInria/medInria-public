/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkFlipTensorImageFilter.h 608 2008-01-14 08:21:23Z filus $
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
#ifndef _vtk_FlipTensorImageFilter_h_
#define _vtk_FlipTensorImageFilter_h_

#include "vtkINRIA3DConfigure.h"

//#include <vtkDataSetAlgorithm.h>
#include <vtkThreadedImageAlgorithm.h>

class VTK_VISUMANAGEMENT_EXPORT vtkFlipTensorImageFilter: public vtkThreadedImageAlgorithm
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


#endif
