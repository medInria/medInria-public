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
 * vtkUniformBool.cxx
 *
 * 2005-05-17	Tim Peeters
 * - First version
 *
 * 2005-06-06	Tim Peeters
 * - Use OpenGL 2.0
 *
 * 2006-01-30	Tim PEeters
 * - Use vtkgl::Unifrom1i() instead of glUniform1i().
 */

#include "vtkUniformBool.h"
#include <vtkObjectFactory.h>

vtkCxxRevisionMacro(vtkUniformBool, "$Revision: 540 $");
vtkStandardNewMacro(vtkUniformBool);

vtkUniformBool::vtkUniformBool()
{
  this->Value = false;
}

vtkUniformBool::~vtkUniformBool()
{
  // nothing to do.
}

void vtkUniformBool::SetGlUniformSpecific()
{
  // Bools may be passed as either integers or floats where 0 or 0.0f is
  // equivealent to false, and other values are equivalent to true.
  // Here we use an integer value to pass a bool. 
  if (this->Value)
    {
    vtkgl::Uniform1i(this->Location, 1);
    }
  else
    {
    vtkgl::Uniform1i(this->Location, 0);
    }
}

