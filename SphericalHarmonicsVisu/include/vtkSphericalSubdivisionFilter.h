/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSphericalSubdivisionFilter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSphericalSubdivisionFilter - generate a subdivision surface using the Linear Scheme
// .SECTION Description
// vtkSphericalSubdivisionFilter is a filter that generates output by
// subdividing its input polydata. Each subdivision iteration create 4
// new triangles for each triangle in the polydata.

// .SECTION Thanks
// This work was supported by PHS Research Grant No. 1 P41 RR13218-01
// from the National Center for Research Resources.

// .SECTION See Also
// vtkInterpolatingSubdivisionFilter vtkButterflySubdivisionFilter

#ifndef __vtkSphericalSubdivisionFilter_h
#define __vtkSphericalSubdivisionFilter_h

#include "vtkInterpolatingSubdivisionFilter.h"

class vtkIntArray;
class vtkPointData;
class vtkPoints;
class vtkPolyData;

class VTK_GRAPHICS_EXPORT vtkSphericalSubdivisionFilter : public vtkInterpolatingSubdivisionFilter
{
public:
  // Description:
  // Construct object with NumberOfSubdivisions set to 1.
  static vtkSphericalSubdivisionFilter *New();
  vtkTypeMacro(vtkSphericalSubdivisionFilter,vtkInterpolatingSubdivisionFilter);

protected:
  vtkSphericalSubdivisionFilter () {};
  ~vtkSphericalSubdivisionFilter () {};

  void GenerateSubdivisionPoints (vtkPolyData *inputDS, 
                                  vtkIntArray *edgeData, 
                                  vtkPoints *outputPts, 
                                  vtkPointData *outputPD);

  vtkIdType InterpolatePositionNorm (vtkPoints *inputPts, vtkPoints *outputPts,
                                 vtkIdList *stencil, double *weights);

private:
  vtkSphericalSubdivisionFilter(const vtkSphericalSubdivisionFilter&);  // Not implemented.
  void operator=(const vtkSphericalSubdivisionFilter&);  // Not implemented.
};

#endif


