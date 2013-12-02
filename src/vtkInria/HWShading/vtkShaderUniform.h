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
 * vtkShaderUniform.h
 * by Tim Peeters
 *
 * 2005-05-17	Tim Peeters
 * - First version
 *
 * 2005-06-06	Tim Peeters
 * - Switch to OpenGL 2.0
 * - Renamed glUniform() to SetGlUniform()
 * - Renamed glSpecificUniform() to SetGlUniformSpecific()
 */

#pragma once

#include "vtkHWShadingExport.h"

#include "vtkShaderBaseHandle.h"
#include <vtkObjectFactory.h>

#include <vtkgl.h> // for vtkgl::GLchar


/**
 * Representation of a uniform variable for GLSL.
 * Subclass of vtkShaderBaseHandle for easy setting of the handle of the
 * associated shader program. This means that a vtkShaderUniform object
 * should only be associated to ONE shader program and not multiple to
 * avoid confusion!
 * Setting of the handle is done by the shader program using this uniform.
 */
class VTK_HWSHADING_EXPORT vtkShaderUniform : public vtkShaderBaseHandle
{
public:

  vtkTypeRevisionMacro (vtkShaderUniform, vtkShaderBaseHandle);
  /**
   * Set/Get the name of the uniform. This must be a null-terminated string.
   * The array element operator ``[]'' and the structure field operator ``.''
   * may be used in the name to select elements of an array or fields of a
   * structure. White spaces are not allowed.
   */
  vtkSetStringMacro(Name);
  vtkGetStringMacro(Name);

  /**
   * Pass this uniform to the shader.
   * Always call this->glGetLocation() first to initialize this->Location.
   * Returns false if there was an error.
   */
  virtual bool SetGlUniform();

protected:
  vtkShaderUniform();
  ~vtkShaderUniform();

  //vtkGetMacro(Location, GLint);

  /**
   * Type-specific setting of uniform. Must be implemented in subclasses.
   * Can assume that this->Location is valid (not -1).
   */
  virtual void SetGlUniformSpecific() = 0;

  /**
   * The location of this uniform variable.
   */
  GLint Location;

private:
  /**
   * The name associated with this uniform.
   */
	vtkgl::GLchar* Name;

};
