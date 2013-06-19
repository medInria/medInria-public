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
 * vtkUniformFloat.cxx
 *
 * 2005-05-17	Tim Peeters
 * - First version
 *
 * 2005-06-06	Tim Peeters
 * - Use OpenGL 2.0
 *
 * 2006-01-30	Tim Peeters
 * - Use vtkgl::Uniform1f() instead of glUniform1f()
 */

#include "vtkUniformFloat.h"
#include <vtkObjectFactory.h>

vtkCxxRevisionMacro(vtkUniformFloat, "$Revision: 540 $");
vtkStandardNewMacro(vtkUniformFloat);

vtkUniformFloat::vtkUniformFloat()
{
  this->Value = 0.0;
}

vtkUniformFloat::~vtkUniformFloat()
{
  // nothing to do.
}

void vtkUniformFloat::SetGlUniformSpecific()
{
  vtkDebugMacro(<<"Calling glUniform1f("<<this->Location<<", "
		<<this->Value<<").");
  vtkgl::Uniform1f(this->Location, this->Value);
}

