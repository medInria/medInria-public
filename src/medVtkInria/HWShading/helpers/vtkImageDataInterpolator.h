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
 * vtkImageDataInterpolator.h
 * by Tim Peeters
 * 
 * 2005-01-25	Tim Peeters
 * - First version
 *
 * 2005-01-26	Tim Peeters
 * - Moved this functionality from an imagedata subclass to a class
 *   that has imagedata as input (renamed vtkInterpolatedImageData to
 *   vtkImageDataInterpolator)
 *
 * 2005-02-23	Tim Peeters
 * - Added support for GetInterpolated*At(double x[3])
 *
 * 2005-03-09	Tim Peeters
 * - Added GetInterpolatedTensorAt_Nearest() and made
 *   SetMinTupleSize() public for vtkTensorRayCastMapper.
 *
 * 2005-06-03	Tim Peeters
 * - Use bmia namespace
 * - Removed inclusion of vtkImageData.h
 */

#pragma once

#include "vtkHelpersExport.h"

#include <vtkObject.h>

class vtkImageData;
class vtkDataArray;

/**
 * Convenience class for imagedata interpolation.
 * Note that interpolation can be done faster in the rendering methods
 * themselves.
 */
class VTK_HELPERS_EXPORT vtkImageDataInterpolator : public vtkObject
{
public:
  static vtkImageDataInterpolator *New();

  void SetImageData(vtkImageData* imageData);
  vtkGetObjectMacro(ImageData, vtkImageData);


  void SetInput(vtkImageData *input);
  vtkImageData* GetInput();


  /**
   * Return an interpolated value for the specified attribute of the
   * dataset. The amount of attributes for Scalars is not limited;
   * Vectors and Normals have 3 attributes; TCoords have at most 3
   * attributes; and Tensors have 9 attributes (this is specified in
   * vtkDataSetAttributes.cxx).
   * If no input imagedata dataset was set, if the specified coordinates
   * are out of range, or if the dataset does not have attributes of the
   * requested type, then NULL (or 0 for GetInterpolatedScalar1At)
   * is returned.
   */
  virtual double* GetInterpolatedScalarAt(double x, double y, double z);
  virtual double GetInterpolatedScalar1At(double x, double y, double z);
  virtual double* GetInterpolatedVectorAt(double x, double y, double z);
  virtual double* GetInterpolatedNormalAt(double x, double y, double z);
  virtual double* GetInterpolatedTCoordAt(double x, double y, double z);
  virtual double* GetInterpolatedTensorAt(double x, double y, double z);
  virtual double* GetInterpolatedScalarAt(double x[3]);
  virtual double GetInterpolatedScalar1At(double x[3]);
  virtual double* GetInterpolatedVectorAt(double x[3]);
  virtual double* GetInterpolatedNormalAt(double x[3]);
  virtual double* GetInterpolatedTCoordAt(double x[3]);
  virtual double* GetInterpolatedTensorAt(double x[3]);

  /**
   * ONLY FOR vtkTensorRayCastMapper.
   * Call SetMinTupleSize(9) first.
   */
  virtual double* GetInterpolatedTensorAt_Nearest(double x[3]);

  /**
   * Set the interpolation type for sampling the dataset.
   */
  vtkSetClampMacro( InterpolationType, int,
        VTK_NEAREST_INTERPOLATION, VTK_LINEAR_INTERPOLATION);
  vtkGetMacro(InterpolationType,int);
  void SetInterpolationTypeToNearest() 
        {this->SetInterpolationType(VTK_NEAREST_INTERPOLATION);};
  void SetInterpolationTypeToLinear() 
        {this->SetInterpolationType(VTK_LINEAR_INTERPOLATION);};
  //const char *GetInterpolationTypeAsString();

  void SetMinTupleSize(int tupleSize);

protected:
  vtkImageDataInterpolator();
  ~vtkImageDataInterpolator();

  virtual double * GetInterpolatedTupleNAt(double x, double y, double z, vtkDataArray* dataArray, int n);
  virtual double * GetInterpolatedTupleNAt(double x[3], vtkDataArray* dataArray, int n);
  virtual double * GetInterpolatedTupleAt(double x, double y, double z, vtkDataArray * dataArray);
  virtual double * GetInterpolatedTupleAt(double x[3], vtkDataArray * dataArray);
  virtual double * GetInterpolatedTupleAt_Linear(double x[3], vtkDataArray * dataArray);
  virtual double * GetInterpolatedTupleAt_Nearest(double x[3], vtkDataArray * dataArray);

  int InterpolationType;

  /**
   * Used in the GetInterpolatedTupleAt_Linear and _Nearest(double x[3], dataArray) function.
   */
  int TupleSize;
  double * Tuple;

  vtkImageData * ImageData;

private:
//  vtkImageDataInterpolator(const vtkImageDataInterpolator&);  // Not implemented.
//  void operator=(const vtkImageDataInterpolator&);  // Not implemented.

};



