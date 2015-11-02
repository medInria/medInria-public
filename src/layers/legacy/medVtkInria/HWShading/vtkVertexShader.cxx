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
 * vtkVertexShader.cxx
 * by Tim Peeters
 *
 * 2005-05-03	Tim Peeters
 * - First version
 *
 * 2005-06-06	Tim Peeters
 * - Use OpenGL 2.0
 * - Renamed glCreateObject() to CreateGlShader()
 *
 * 2006-01-30	Tim Peeters
 * - Switch from glew to VTK OpenGL extension manager.
 *   Use vtkgl::CreateShader(vtkgl::VERTEX_SHADER) instead of
 *   glCreateShader(GL_VERTEX_SHADER).
 */

#include "vtkVertexShader.h"
#include <vtkObjectFactory.h>


vtkStandardNewMacro(vtkVertexShader);

vtkVertexShader::vtkVertexShader()
{
  // Nothing to do. Everything was already done in vtkShaderObject.
}

vtkVertexShader::~vtkVertexShader()
{
  // Nothing.
}

bool vtkVertexShader::CreateGlShader()
{
  if (this->GetHandleValid())
    {
    // no need to create a new handle.
    return true;
    }

  vtkDebugMacro("Calling glCreateShader(GL_VERTEX_SHADER)");
  GLuint handle = vtkgl::CreateShader(vtkgl::VERTEX_SHADER);
  vtkDebugMacro("glCreateShader() returned handle "<<handle<<".");
  this->SetHandle(handle);
  return true;
}

