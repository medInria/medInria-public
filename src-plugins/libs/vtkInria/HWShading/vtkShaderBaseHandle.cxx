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
 * vtkShaderBaseHandle.cxx
 * by Tim Peeters
 *
 * 2005-05-17	Tim Peeters
 * - First version, based on the (old) vtkShaderBase.cxx
 *
 * 2005-06-06	Tim Peeters
 * - Switch to OpenGL 2.0
 * - Removed glDelete()
 */

#include "vtkShaderBaseHandle.h"
#include <vtkObjectFactory.h>

vtkCxxRevisionMacro (vtkShaderBaseHandle, "$Revision: 540 $");
vtkStandardNewMacro (vtkShaderBaseHandle);

vtkShaderBaseHandle::vtkShaderBaseHandle()
{
  this->HandleValid = false;
  // this->Handle is not initialized here
}

vtkShaderBaseHandle::~vtkShaderBaseHandle()
{
  // nothing was created by this class, so don't destroy anything either.
  // do that in the subclasses that were creating things.
}

GLuint vtkShaderBaseHandle::GetHandle()
{
  if (!this->GetHandleValid())
    {
    vtkErrorMacro(<<"Calling GetHandle() without a valid handle!");
    }
  return this->Handle;
}

void vtkShaderBaseHandle::SetHandle(GLuint newhandle)
{
  this->Handle = newhandle;
  this->HandleValid = true;
}

void vtkShaderBaseHandle::UnsetHandle()
{
  this->HandleValid = false;
}

bool vtkShaderBaseHandle::GetHandleValid()
{
  return this->HandleValid;
}

