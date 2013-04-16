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
 * vtkFragmentShader.cxx
 * by Tim Peeters
 *
 * 2005-05-03	Tim Peeters
 * - First version
 *
 * 2005-06-06	Tim Peeters
 * - Switched to OpenGL 2.0 (removed ARB in OpenGL types and functions)
 * - Renamed glCreate() to CreateGlShader()
 *
 * 2006-01-30	Tim Peeters
 * - Switch from glew to vtkOpenGLExtensionManager and vtkgl.h
 */

#include "vtkFragmentShader.h"
#include <vtkObjectFactory.h>

vtkCxxRevisionMacro (vtkFragmentShader, "$Revision: 540 $");
vtkStandardNewMacro(vtkFragmentShader);

vtkFragmentShader::vtkFragmentShader()
{
  // Nothing to do. Everything was already done in vtkShaderObject.
}

vtkFragmentShader::~vtkFragmentShader()
{
  // Nothing.
}

bool vtkFragmentShader::CreateGlShader()
{
  if (this->GetHandleValid())
    {
    // no need to create a new handle.
    return true;
    }

  vtkDebugMacro("Calling glCreateShader(GL_FRAGMENT_SHADER)");
  GLuint handle = vtkgl::CreateShader(vtkgl::FRAGMENT_SHADER);
  vtkDebugMacro("glCreateShader() returned handle "<<handle<<".");
  this->SetHandle(handle);
  return true;
}

