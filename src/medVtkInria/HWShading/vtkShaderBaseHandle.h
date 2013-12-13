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
 * vtkShaderBaseHandle.h
 * by Tim Peeters
 *
 * 2005-05-17	Tim Peeters
 * - First version, based on the (old) vtkShaderBase.h
 *
 * 2006-06-06	Tim Peeters
 * - Switched to OpenGL 2.0 (removed ARB in function calls and types)
 * - Removed glDelete() function. Now implemented in vtkShaderObject
 *   and vtkBMIAShaderProgram as DeleteGlShader() and DeleteGlProgram().
 */

#pragma once

#include "vtkHWShadingExport.h"

#include "vtkShaderBase.h"

/**
 * Base class for GLSL shader related subclasses with GLhandles.
 */
class VTK_HWSHADING_EXPORT vtkShaderBaseHandle : public vtkShaderBase
{
public:

  static vtkShaderBaseHandle* New();
  vtkTypeRevisionMacro (vtkShaderBaseHandle, vtkShaderBase);
  /**
   * Gets/Sets the handle for the current shader object or program.
   */
  GLuint GetHandle();
  void SetHandle(GLuint newhandle);
  void UnsetHandle();

  /**
   * True if a handle was given a value using SetHandle() and UnsetHandle()
   * was not called afterwards.
   */
  bool GetHandleValid();

protected:
  vtkShaderBaseHandle();
  ~vtkShaderBaseHandle();

  /**
   * Deletes the shader program or object associated with this->Handle
   * if this->Handle is valid (GetHandleValid()).
   */
  //void glDelete();

private:

  /**
   * Handle for shader or vertex object. Initialize in subclasses!
   */
  GLuint Handle;

  /**
   * True if this->Handle was given a value using SetHandle and UnsetHandle
   * was not called afterwards.
   */
  bool HandleValid;

};


