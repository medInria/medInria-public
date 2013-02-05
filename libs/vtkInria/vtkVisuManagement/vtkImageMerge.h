/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkImageMerge.h 608 2008-01-14 08:21:23Z filus $
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
#ifndef _vtk_ImageMerge_h_
#define _vtk_ImageMerge_h_

#include "vtkINRIA3DConfigure.h"

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

  vtkImageData* GetOutput (void) const
  { return Output; }

  /** Call this function to compute the data */
  void Update(void);

 protected:
  
  vtkImageMerge();
  ~vtkImageMerge();

 private:

  
  vtkImageData*      Input1;
  vtkImageData*      Input2;
  vtkImageData*      Output;
  
};


#endif
