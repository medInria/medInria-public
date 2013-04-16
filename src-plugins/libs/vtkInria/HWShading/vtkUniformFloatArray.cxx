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
 * vtkUniformFloatArray.cxx
 *
 * 2005-05-17	Tim Peeters
 * - First version, not finished/working yet!
 */

#include "vtkUniformFloatArray.h"
#include <vtkObjectFactory.h>
#include <vtkFloatArray.h>

vtkCxxRevisionMacro(vtkUniformFloatArray, "$Revision: 540 $");
vtkStandardNewMacro(vtkUniformFloatArray);

vtkCxxSetObjectMacro(vtkUniformFloatArray, Value, vtkFloatArray);

vtkUniformFloatArray::vtkUniformFloatArray()
{
  //this->Count = 0;
  //this->Value = NULL;
  this->Value = NULL;
}

vtkUniformFloatArray::~vtkUniformFloatArray()
{
  if (this->Value)
    {
    this->Value->UnRegister(this);
    this->Value = NULL;
    }
}

void vtkUniformFloatArray::SetGlUniformSpecific()
{
  //glUniform1fvARB(this->Location, this->Count, this->Value);

  if (!this->Value)
    {
    vtkWarningMacro(<<"Not passing uniform float array with value NULL.");
    return;
    }

  int components = this->Value->GetNumberOfComponents();
  int tuples = this->Value->GetNumberOfTuples();

  float* array = this->Value->....//HIER VERDER
  switch(tuples)
    {
    case 1:
      {
      glUniform1fv
      break;
      }
    case 2:
      {
      glUniform2fv
      break;
      }
    case 3:
      {
      glUniform3fv
      break;
      }
    case 4:
      {
      glUniform4fv
      break;
      }
    default:
      {
      glUniform1fv
      break;
      }
}


