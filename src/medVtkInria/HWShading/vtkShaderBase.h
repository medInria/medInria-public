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
 * vtkShaderBase.h
 * by Tim Peeters
 *
 * 2005-05-03	Tim Peeters
 * - First version
 *
 * 2005-05-17	Tim Peeters
 * - Removed functionality dealing with GLhandles. This was moved to  a new
 *   vtkShaderBaseHandle subclass.
 *
 * 2005-06-06	Tim Peeters
 * - Switched to OpenGL 2.0
 *
 * 2005-07-04	Tim Peeters
 * - Instead of using glew, set GL_GLEXT_PROTOTYPES and then include gl.h
 *
 * 2005-07-14	Tim Peeters
 * - Use glew again. On Windows it seems to be needed.
 * - Add SupportsOpenGLVersion() function.
 *
 * 2006-01-30	Tim Peeters
 * - Removed #include <GL/glew.h>
 */

#pragma once

#include <medVtkInriaExport.h>

//#include <GL/glew.h> // for OpenGL types and some functions
		     // TODO: can this be done without glew?
#include <vtkgl.h>

#include <vtkObject.h>

//#define GL_GLEXT_PROTOTYPES 1
//#include "GL/gl.h"

/**
 * Base class for all GLSL shader related subclasses. Implements
 * printing of info logs, etc.
 * NOTE: include this header file before including any rendering header
 * files because glew.h must be included before gl.h.
 */
class MEDVTKINRIA_EXPORT vtkShaderBase : public vtkObject
{
public:

  static vtkShaderBase *New();
  vtkTypeRevisionMacro(vtkShaderBase, vtkObject);

  /**
   * Returns true if the specified version of OpenGL is supported, and
   * false otherwise.
   */
  static bool SupportsOpenGLVersion(int atLeastMajor, int atLeastMinor);

protected:
  vtkShaderBase();
  ~vtkShaderBase();

private:

};


