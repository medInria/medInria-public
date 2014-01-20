/*============================================================================

The Hardware Shading (HWShading) module is protected by the
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
 * vtkDoubleToUCharFilter.h
 * by Tim Peeters
 *
 * 2005-03-29	Tim Peeters
 * - First version
 *
 * 2005-06-03	Tim Peeters
 * - Use bmia namespace
 */

#pragma once

#include "medVtkInriaExport.h"

#include <vtkSimpleImageToImageFilter.h>

/**
 * Datasets containing double data in the range <0..1> are converted
 * to unsigned char datasets with values from 0 to 255.
 *
 * vtkImageShiftScale is not used because that always assumes scalar
 * data with one component per tuple, while this is not always the case.
 * I didn't try to patch that one because it already has changes in
 * CVS version and upgrading to CVS is not practical for me.
 */
class MEDVTKINRIA_EXPORT vtkDoubleToUCharFilter : public vtkSimpleImageToImageFilter
{
public:
  static vtkDoubleToUCharFilter* New();

protected:
  vtkDoubleToUCharFilter();
  ~vtkDoubleToUCharFilter();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output);

private:
  vtkDoubleToUCharFilter(const vtkDoubleToUCharFilter&); // Not implemented
  void operator=(const vtkDoubleToUCharFilter&); // Not implemented
};


