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
 * vtkShaderUniform.cxx
 * by Tim Peeters
 *
 * 2005-05-17	Tim Peeters
 * - First version
 *
 * 2005-06-06	Tim Peeters
 * - Switch to OpenGL 2.0
 *
 * 2006-01-30	Tim Peeters
 * - Switch from using glew to vtkOpenGLExtensionManager and vtkgl.h
 */

#include "vtkShaderUniform.h"

#include <vtkObjectFactory.h>

vtkCxxRevisionMacro (vtkShaderUniform, "$Revision: 540 $");

vtkShaderUniform::vtkShaderUniform()
{
  this->Name = NULL;
}

vtkShaderUniform::~vtkShaderUniform()
{
  if (this->Name)
    {
    delete [] this->Name;
    this->Name = NULL;
    }
}

bool vtkShaderUniform::SetGlUniform()
{
  if (!this->GetHandleValid())
    {
    this->Location = -1;
    vtkErrorMacro(<<"No handle set!");
    return false;
    }

  if (!this->Name)
    {
    this->Location = -1;
    vtkWarningMacro(<<"Uniform has no name!");
    return false;
    }

  this->Location = vtkgl::GetUniformLocation(this->GetHandle(), this->Name);
  vtkDebugMacro(<<"Location of uniform "<<this->Name<<" with handle "<<this->GetHandle()<<" is: "<<this->Location);

  if (this->Location == -1)
    {
    vtkWarningMacro(<<"Location of uniform with name "<<this->Name<<" could not be determined!");
    return false;
    }

  // this->Location has a valid value.
  this->SetGlUniformSpecific();
  return true;
}

