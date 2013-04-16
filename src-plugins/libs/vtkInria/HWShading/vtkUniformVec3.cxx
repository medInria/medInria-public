/*============================================================================

Program:   vtkINRIA3D
Module:    $Id: HWShading 480 2007-11-21 08:48:21Z $
Language:  C++
Author:    $Author: Tim Peeters $
Date:      $Date: 2007-11-21 09:48:21 +0100 (Wed, 21 Nov 2007) $
Version:   $Revision: 480 $

The Hardware Shading (HWShading) module of vtkINRIA3D is protected by the
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
 * vtkUniformVec3.cxx
 *
 * 2005-05-17	Tim Peeters
 * - First version
 *
 * 2005-06-06	Tim Peeters
 * - Use OpenGL 2.0
 *
 * 2006-01-30	Tim Peeters
 * - Use vtkgl::Uniform3f() instead of glUniform3f().
 */

#include "vtkUniformVec3.h"
#include <vtkObjectFactory.h>

vtkCxxRevisionMacro(vtkUniformVec3, "$Revision: 540 $");
vtkStandardNewMacro(vtkUniformVec3);

vtkUniformVec3::vtkUniformVec3()
{
  this->Value[0] = 0.0;
  this->Value[1] = 0.0;
  this->Value[2] = 0.0;
}

vtkUniformVec3::~vtkUniformVec3()
{
  // nothing to do.
}

void vtkUniformVec3::SetGlUniformSpecific()
{
  vtkDebugMacro(<<"Calling glUniform3f("<<this->Location<<", "<<this->Value[0]
	<<", "<<this->Value[1]<<", "<<this->Value[2]<<").");
  vtkgl::Uniform3f(this->Location, this->Value[0], this->Value[1], this->Value[2]);
}

