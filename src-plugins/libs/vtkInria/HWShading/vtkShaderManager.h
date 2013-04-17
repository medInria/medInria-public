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
 * vtkShaderManager.h
 * by Tim Peeters
 *
 * 2005-05-04	Tim Peeters
 * - First version
 *
 * 2005-06-06	Tim Peeters
 * - Added GetGlVersion()
 * - Renamed glUseProgram() to UseGlProgram().
 *
 * 2005-07-04	Tim Peeters
 * - DEPRECATED this class in favor of vtkBMIAShaderProgramReader
 *   and the vtkBMIAShaderProgram->Activate() function.
 */

#pragma once

#include "vtkShaderBase.h"

class vtkBMIAShaderProgram;

/**
 * Class for setting the currently active shader.
 * DEPRECATED. It still works, but don't use it anyway. Shader programs can
 * be read using vtkBMIAShaderProgramReader. Shader programs can be activated
 * with vtkBMIAShaderProgram->Activate(). Calling of the Initialize() function of
 * the shader manager is no longer needed if the renderwindow is correctly
 * initialized.
 */
class VTK_HWSHADING_EXPORT vtkShaderManager: public vtkShaderBase
// shader manager doesn't have/need a handle, but I made it a subclass of
// vtkShaderBase anyway for convenience (include glew.h etc).
{
public:
  static vtkShaderManager *New();
  vtkTypeRevisionMacro (vtkShaderManager, vtkShaderBase);

  /**
   * Inititialize glew and check for the extensions required
   * for hardware shading.
   */
  void Initialize();

  /**
   * Returns true if the specified version of OpenGL is supported, and
   * false otherwise.
   */
  static bool SupportsOpenGLVersion(int atLeastMajor, int atLeastMinor);

  /**
   * Returns true if GLSL is supported and false otherwise.
   */
  bool GetHWShaderSupport();

  /**
   * Set/Get the shader to be used when rendering.
   * Value of shader may be NULL to disable custom shaders
   * and to enable the OpenGL fixed-pipeline functionality.
   * NOTE: This function does not (yet) start monitoring of changes
   * in the shader program or its associated shader objects. So if
   * you change any of those, execute UpdateActiveShader() again.
   */
  void UseShaderProgram(vtkBMIAShaderProgram* program)
    {
    this->SetActiveShaderProgram(program);
    }
  void SetActiveShaderProgram(vtkBMIAShaderProgram* program);
  vtkGetObjectMacro(ActiveShaderProgram, vtkBMIAShaderProgram);

  /**
   * Read a shader from file and activate that shader.
   */
  virtual void UseShaderFromFile(const char* filename);

  /**
   * Checks for changes in the active shader and recompiles and
   * relinks it if needed.
   */
  void UpdateActiveShaderProgram();

protected:
  vtkShaderManager();
  ~vtkShaderManager();

  bool UseGlProgram();

  bool Initialized;

private:
  vtkBMIAShaderProgram* ActiveShaderProgram;

  /**
   * Parse GL_VERSION and return the major and minor numbers in the supplied
   * integers.
   * If it fails for any reason, major and minor will be set to 0.
   * Assumes a valid OpenGL context.
   */
  void GetGlVersion(int *major, int *minor);
};


