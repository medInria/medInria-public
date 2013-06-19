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
 * vtkUniformInt.cxx
 *
 * 2005-05-17	Tim Peeters
 * - First version
 *
 * 2005-06-06	Tim Peeters
 * - Use OpenGL 2.0
 *
 * 2006-01-30	Tim Peeters
  - Use vtkgl::Uniform1i() instead of glUniform1i().
 */

#include "vtkUniformInt.h"
#include <vtkObjectFactory.h>

vtkCxxRevisionMacro(vtkUniformInt, "$Revision: 540 $");
vtkStandardNewMacro(vtkUniformInt);

vtkUniformInt::vtkUniformInt()
{
  this->Value = 0;
}

vtkUniformInt::~vtkUniformInt()
{
  // nothing to do.
}

void vtkUniformInt::SetGlUniformSpecific()
{
  vtkgl::Uniform1i(this->Location, this->Value);
}

