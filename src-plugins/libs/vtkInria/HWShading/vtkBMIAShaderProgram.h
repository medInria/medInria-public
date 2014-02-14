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
 * vtkBMIAShaderProgram.h
 * by Tim Peeters
 *
 * 2005-05-04	Tim Peeters
 * - First version
 *
 * 2005-05-17	Tim Peeters
 * - Added various functions for dealing with uniform variables.
 * - Removed functions for reading the shader from file. This is now
 *   implemented in vtkBMIAShaderProgramReader.
 *
 * 2005-06-03	Tim Peeters
 * - Use bmia namespace
 * - Removed #include <string>
 *
 * 2005-06-06	Tim Peeters
 * - Renamed glCreateProgram() to CreateGlProgram
 * - Renamed glAttach() to AttachGlShader()
 * - Renamed glDetach() to DetachGlShader()
 * - Renamed glAttachAll() to AttachAllGlShaders()
 * - Renamed glDetachAll() to DetachAllGlShaders()
 * - Renamed glLink() to LinkGlProgram()
 * - Renamed glUniform() to SetGlUniform()
 * - Renamed UniformAll() to SetAllGlUniforms()
 *
 * 2005-07-01	Tim Peeters
 * - Added Validate()
 *
 * 2005-07-04	Tim Peeters
 * - Added Activate() and Deactivate() functions. vtkShaderManager class
 *   is now no longer needed for activating a shader program.
 */

#pragma once

#include "vtkHWShadingExport.h"

#include <vtkShaderBaseHandle.h>

class vtkShaderObject;
class vtkShaderObjectCollection;
class vtkShaderUniform;
class vtkShaderUniformCollection;

/**
 * Class for representing a GLSL hardware shader program.
 * This class also takes care of linking shader objects 
 * to the shader program.
 * Subclass of vtkCollection because a shader program is a set of shader
 * objects which are linked.
 */
class VTK_HWSHADING_EXPORT vtkBMIAShaderProgram : public vtkShaderBaseHandle
{
public:
  static vtkBMIAShaderProgram *New();
  vtkTypeRevisionMacro(vtkBMIAShaderProgram, vtkShaderBaseHandle);
  
  virtual void AddShaderObject(vtkShaderObject* object);
  virtual void RemoveShaderObject(vtkShaderObject* object);
  vtkShaderObjectCollection* GetShaderObjects()
    {
    return this->ShaderObjects;
    };

  virtual void AddShaderUniform(vtkShaderUniform* uniform);
  virtual void RemoveShaderUniform(vtkShaderUniform* uniform);
  vtkShaderUniformCollection* GetShaderUniforms()
    {
    return this->ShaderUniforms;
    }

  /**
   * Link the shader program. This automatically compiles associated shader
   * objects if needed. This is called by vtkShaderManager if that is used
   * for managing the shaders.
   */
  virtual void Link();
  virtual void ForceLink()
    {
    this->Linked = false;
    this->Link();
    }

  /**
   * Shortcut to do some stuff faster. Used in vtkFiberMapper.
   * TODO: Do this in a nicer way in a newer version.
   */
  virtual void ForceReLink();

  bool IsLinked()
    {
    return this->Linked;
    }
  bool GetLinked()
    {
    return this->IsLinked();
    }

  /**
   * Get the shader program's mtime plus consider its shader objects.
   */
  unsigned long int GetMTime();

  /**
   * Must be called after calling glUseShaderProgram. Applies all
   * uniform variables attached to this shader program to the actual
   * program running on the GPU.
   */
  void ApplyShaderUniforms();

  /**
   * Validate this shader program. If you want to validate your program,
   * call this function after linking and not between glBegin() and glEnd().
   * If there are problems, false is returned and a description is
   * output as text to the console. If validation is successfull, true is
   * returned.
   */
  bool Validate();

  /**
   * Deactivate all other shader programs and activate this one as the
   * active shader program.
   */
  void Activate();

  /**
   * Deactivate the use of any shader program.
   */
  static void Deactivate();

protected:
  vtkBMIAShaderProgram();
  ~vtkBMIAShaderProgram();

  bool CreateGlProgram();
  bool DeleteGlProgram();
  bool AttachGlShader(vtkShaderObject* object);
  bool DetachGlShader(vtkShaderObject* object);
  bool AttachAllGlShaders(vtkShaderObjectCollection* objects);
  bool DetachAllGlShaders(vtkShaderObjectCollection* objects);
  bool LinkGlProgram();
  bool SetGlUniform(vtkShaderUniform* uniform);
  bool SetAllGlUniforms(vtkShaderUniformCollection* uniforms);

private:
  vtkShaderObjectCollection* ShaderObjects;
  vtkShaderUniformCollection* ShaderUniforms;

  bool Linked;
  vtkTimeStamp LinkTime;

};


