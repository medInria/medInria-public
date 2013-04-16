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
 * vtkImageOrthogonalSlicesActor.cxx
 * by Tim Peeters
 *
 * 2006-05-03	Tim Peeters
 * - First version
 */

#include "vtkImageOrthogonalSlicesActor.h"
#include "vtkImageSliceActor.h"

#include <vtkAlgorithmOutput.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkObjectFactory.h>

vtkStandardNewMacro(vtkImageOrthogonalSlicesActor);

vtkImageOrthogonalSlicesActor::vtkImageOrthogonalSlicesActor()
{
  int axis;
  for (axis=0; axis < 3; axis++)
    {
    this->ImageSliceActors[axis] = vtkImageSliceActor::New();
    //this->ImageSliceActors[axis]->DebugOn();
    }
  this->ImageSliceActors[0]->SetSliceOrientationToYZ();
  this->ImageSliceActors[1]->SetSliceOrientationToXZ();
  this->ImageSliceActors[2]->SetSliceOrientationToXY();

  for (axis=0; axis < 3; axis++)
    {
    this->AddPart(this->ImageSliceActors[axis]);
    this->IsSliceVisible[axis] = true;
    }
}

vtkImageOrthogonalSlicesActor::~vtkImageOrthogonalSlicesActor()
{
  for (int axis=0; axis < 3; axis++)
    {
    this->SetSliceVisible(axis, false);
    this->ImageSliceActors[axis]->Delete();
    this->ImageSliceActors[axis] = NULL;
    }
}

void vtkImageOrthogonalSlicesActor::SetInput(vtkImageData* in)
{
  for (int i=0; i < 3; i++) this->ImageSliceActors[i]->SetInput(in);
}

vtkImageData* vtkImageOrthogonalSlicesActor::GetInput()
{
  return this->ImageSliceActors[0]->GetInput();
}

void vtkImageOrthogonalSlicesActor::SetInputConnection(vtkAlgorithmOutput* input)
{
  for (int i=0; i < 3; i++) this->ImageSliceActors[i]->SetInputConnection(input);
}

void vtkImageOrthogonalSlicesActor::SetLookupTable(vtkLookupTable* lut)
{
  for (int i=0; i < 3; i++) this->ImageSliceActors[i]->SetLookupTable(lut);
}

vtkLookupTable* vtkImageOrthogonalSlicesActor::GetLookupTable()
{
  return this->ImageSliceActors[0]->GetLookupTable();
}

int vtkImageOrthogonalSlicesActor::GetMapColorScalarsThroughLookupTable()
{
  return this->ImageSliceActors[0]->GetMapColorScalarsThroughLookupTable();
}

void vtkImageOrthogonalSlicesActor::SetMapColorScalarsThroughLookupTable(int map)
{
  for (int i=0; i < 3; i++) this->ImageSliceActors[i]->SetMapColorScalarsThroughLookupTable(map);
}

int vtkImageOrthogonalSlicesActor::GetInterpolate()
{
  return this->ImageSliceActors[0]->GetInterpolate();
}

void vtkImageOrthogonalSlicesActor::SetInterpolate(int interpolate)
{
  for (int i=0; i < 3; i++) this->ImageSliceActors[i]->SetInterpolate(interpolate);
}

void vtkImageOrthogonalSlicesActor::SetX(int x)
{
  this->ImageSliceActors[0]->SetSlice(x);
}

void vtkImageOrthogonalSlicesActor::SetY(int y)
{
  this->ImageSliceActors[1]->SetSlice(y);
}

void vtkImageOrthogonalSlicesActor::SetZ(int z)
{
  this->ImageSliceActors[2]->SetSlice(z);
}

int vtkImageOrthogonalSlicesActor::GetX()
{
  return this->ImageSliceActors[0]->GetSlice();
}

int vtkImageOrthogonalSlicesActor::GetY()
{
  return this->ImageSliceActors[1]->GetSlice();
}

int vtkImageOrthogonalSlicesActor::GetZ()
{
  return this->ImageSliceActors[2]->GetSlice();
}

int vtkImageOrthogonalSlicesActor::GetXMin()
{
  return this->ImageSliceActors[0]->GetSliceMin();
}

int vtkImageOrthogonalSlicesActor::GetYMin()
{
  return this->ImageSliceActors[1]->GetSliceMin();
}

int vtkImageOrthogonalSlicesActor::GetZMin()
{
  return this->ImageSliceActors[2]->GetSliceMin();
}

int vtkImageOrthogonalSlicesActor::GetXMax()
{
  return this->ImageSliceActors[0]->GetSliceMax();
}

int vtkImageOrthogonalSlicesActor::GetYMax()
{
  return this->ImageSliceActors[1]->GetSliceMax();
}

int vtkImageOrthogonalSlicesActor::GetZMax()
{
  return this->ImageSliceActors[2]->GetSliceMax();
}

void vtkImageOrthogonalSlicesActor::GetXRange(int &min, int &max)
{
  this->ImageSliceActors[0]->GetSliceRange(min, max);
}

void vtkImageOrthogonalSlicesActor::GetYRange(int &min, int &max)
{
  this->ImageSliceActors[1]->GetSliceRange(min, max);
}

void vtkImageOrthogonalSlicesActor::GetZRange(int &min, int &max)
{
  this->ImageSliceActors[2]->GetSliceRange(min, max);
}

int* vtkImageOrthogonalSlicesActor::GetXRange()
{
  return this->ImageSliceActors[0]->GetSliceRange();
}

int* vtkImageOrthogonalSlicesActor::GetYRange()
{
  return this->ImageSliceActors[1]->GetSliceRange();
}

int* vtkImageOrthogonalSlicesActor::GetZRange()
{
  return this->ImageSliceActors[2]->GetSliceRange();
}

void vtkImageOrthogonalSlicesActor::CenterSlices()
{
  for (int i=0; i < 3; i++) this->ImageSliceActors[i]->CenterSlice();
}

void vtkImageOrthogonalSlicesActor::UpdateInput()
{
  for (int i=0; i < 3; i++) this->ImageSliceActors[i]->UpdateInput();
}

void vtkImageOrthogonalSlicesActor::SetSliceVisible(int axis, bool visible)
{
  if (axis > 2 || axis < 0) return;
  if (this->IsSliceVisible[axis] == visible) return;
  this->IsSliceVisible[axis] = visible;
  if (this->IsSliceVisible[axis])
    {
    this->AddPart(this->ImageSliceActors[axis]);
    }
  else
    {
    this->RemovePart(this->ImageSliceActors[axis]);
    }
}

bool vtkImageOrthogonalSlicesActor::GetSliceVisible(int axis)
{
  if (axis > 2 || axis < 0) return false;
  return this->IsSliceVisible[axis];
}

vtkImageSliceActor* vtkImageOrthogonalSlicesActor::GetSliceActor(int axis)
{
  if (axis > 2 || axis < 0) return NULL;
  return this->ImageSliceActors[axis];
}

