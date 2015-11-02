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
 * vtkBMIAShaderProgram.cxx
 *
 * 2005-05-04	Tim Peeters
 * - First version
 *
 * 2005-05-17	Tim Peeters
 * - Simplified Link()
 * - Added support for uniform values
 * - Removed support for reading values from file. This is now implemented
 *   in vtkBMIAShaderProgramReader.
 *
 * 2005-06-03	Tim Peeters
 * - Use namespace bmia
 * - Include string
 *
 * 2005-06-06	Tim Peeters
 * - Switch to OpenGL 2.0
 * - Added DeleteGlProgram()
 *
 * 2005-07-01	Tim Peeters
 * - Added validation of program after linking in LinkGlProgram, and
 *   output the infolog if linking fails or debugging is enabled.
 * - Added Validate() function.
 *
 * 2006-01-30	Tim Peeters
 * - Use vtkOpenGLExtensionManager and vtkgl.h instead of GLEW.
 */

#include "vtkBMIAShaderProgram.h"

#include "vtkShaderObject.h"
#include "vtkShaderObjectCollection.h"
#include "vtkShaderUniform.h"
#include "vtkShaderUniformCollection.h"
#include <vtkObjectFactory.h>
#include <string>
#include <vtkgl.h>


vtkStandardNewMacro(vtkBMIAShaderProgram);

vtkBMIAShaderProgram::vtkBMIAShaderProgram()
{
  this->ShaderObjects = vtkShaderObjectCollection::New();
  this->ShaderUniforms = vtkShaderUniformCollection::New();
  this->Linked = false;
}

vtkBMIAShaderProgram::~vtkBMIAShaderProgram()
{
  this->DetachAllGlShaders(this->ShaderObjects);
  this->ShaderObjects->Delete();
  this->ShaderUniforms->Delete();
  this->DeleteGlProgram();
}

void vtkBMIAShaderProgram::AddShaderObject(vtkShaderObject* object)
{
  vtkDebugMacro(<<"Adding shader object "<<object);
  if (object == NULL)
    {
    vtkDebugMacro(<<"Don't add NULL shader objects!");
    return;
    }
  this->ShaderObjects->AddItem(object);
  this->Modified();
}

void vtkBMIAShaderProgram::RemoveShaderObject(vtkShaderObject* object)
{
  this->ShaderObjects->RemoveItem(object);
  this->Modified();
}

void vtkBMIAShaderProgram::AddShaderUniform(vtkShaderUniform* uniform)
{
  if (uniform == NULL)
    {
    vtkDebugMacro(<<"Not adding the uniform with value NULL.");
    return;
    }
  this->ShaderUniforms->AddItem(uniform);
  this->Modified();
}

void vtkBMIAShaderProgram::RemoveShaderUniform(vtkShaderUniform* uniform)
{
  this->ShaderUniforms->RemoveItem(uniform);
  this->Modified();
}

unsigned long int vtkBMIAShaderProgram::GetMTime()
{
  unsigned long mTime = this-> vtkShaderBase::GetMTime();
  unsigned long time;

  this->ShaderObjects->InitTraversal();
  vtkShaderObject* object = this->ShaderObjects->GetNextItem();
  while (object != NULL)
    {
    time = object->GetMTime();
    mTime = ( time > mTime ? time : mTime );
    object = this->ShaderObjects->GetNextItem();
    }
  //object == NULL

  return mTime;
}

void vtkBMIAShaderProgram::Link()
{
  if (this->Linked && this->LinkTime > this->GetMTime())
    {
    vtkDebugMacro(<<"Link time > MTime and linked, so not re-linking.");
    }
  else
    {
    vtkDebugMacro(<<"(Re)linking.");
    this->Linked = false;

    if (!this->SupportsOpenGLVersion(2, 0))
      {
      vtkErrorMacro(<<"OpenGL 2.0 not supported, so shader program cannot "
        <<"be used. Terminating program.");
      exit(1);
      }

    if (!this->CreateGlProgram()) return;
    vtkgl::BindAttribLocation(this->GetHandle(), 1, "Tangent");
    vtkgl::BindAttribLocation(this->GetHandle(), 2, "LineID");
    if (!this->AttachAllGlShaders(this->ShaderObjects)) return;
    if (!this->LinkGlProgram())
      {
      return;
      }

    this->LinkTime.Modified();
    this->Linked = true;

    //this->glUniformAll(this->ShaderUniforms);
    // must be done after calling glUseProgramObject.
    // XXX: Now handled in vtkShaderManager.
  } // else

  return;
}

void vtkBMIAShaderProgram::ForceReLink()
{
  if (!this->LinkGlProgram()) return;
  this->LinkTime.Modified();
  this->Linked = true;
}

bool vtkBMIAShaderProgram::CreateGlProgram()
{
  if (this->GetHandleValid())
    {
    // no need to create a new handle.
    vtkDebugMacro(<<"Handle is already valid. Not creating a new one");
    }
  else
    {
    // create a new  handle.
    vtkDebugMacro(<<"No valid handle found. Creating new one...");
    GLuint handle = vtkgl::CreateProgram();
    vtkDebugMacro(<<"Handle created. Setting handle...");
    this->SetHandle(handle);
    }
  return true;
}

bool vtkBMIAShaderProgram::AttachGlShader(vtkShaderObject* object)
{
  if (!this->GetHandleValid())
    {
    vtkErrorMacro(<<"Do not try to attach shader objects to a shader program "
        <<"that does not have a handle yet!");
    return false;
    }

  if (object == NULL)
    {
    vtkWarningMacro(<<"Cannot attach a NULL shader object!");
    //return true; // why not continue with the other shader programs?
    return false;
    }

  // make sure the shader object is compiled
  object->Compile();

  vtkgl::AttachShader(this->GetHandle(), object->GetHandle());
  // TODO: check whether the attaching was successful?

  return true;
}

bool vtkBMIAShaderProgram::DetachGlShader(vtkShaderObject* object)
{
  if (!this->GetHandleValid())
    {
    vtkErrorMacro(<<"How can you detach a shader object if the shader program "
        <<"does not even have a handle??");
    return false;
    }

  if (object == NULL)
    {
    vtkWarningMacro(<<"Cannot detach a NULL shader object!");
    return false; // or true, if glAttach(NULL) does this.
    }

  if (object->GetHandleValid())
    {
    vtkgl::DetachShader(this->GetHandle(), object->GetHandle());
    return true;
    }
  else
    { // !object->GetHandleValid()
    vtkErrorMacro(<<"Trying to detach a shader object that does not"
        <<" have a handle!");
    return false;
    }
}

bool vtkBMIAShaderProgram::AttachAllGlShaders(vtkShaderObjectCollection* objects)
{
  if (!this->GetHandleValid())
    {
    vtkErrorMacro(<<"Cannot attach shader objects to a shader program that"
        <<" does not have a handle yet!");
    return false;
    }

  if (objects == NULL)
    {
    vtkErrorMacro(<<"Don't call glAttachAll(NULL)!");
    return false;
    }

  if (objects->GetNumberOfItems() < 1)
    {
    vtkErrorMacro(<<"No shader objects specified to link to shader program!");
    return false;
    }

  bool result = true;
  this->ShaderObjects->InitTraversal();
  vtkShaderObject* object = this->ShaderObjects->GetNextItem();
  while (object != NULL)
    {
    result = (result && this->AttachGlShader(object));
    object = this->ShaderObjects->GetNextItem();
    }
  //object == NULL

  return result;
}

bool vtkBMIAShaderProgram::DetachAllGlShaders(vtkShaderObjectCollection* objects)
{
  if (!this->GetHandleValid())
    {
    vtkDebugMacro(<<"Cannot detach shader objects if the shader program"
        <<" does not have a valid handle.");
    return false;
    }

  if (objects == NULL)
    {
    vtkErrorMacro(<<"Don't call glDetachAll(NULL)!");
    return false;
    }

  bool result = true;
  this->ShaderObjects->InitTraversal();
  vtkShaderObject* object = this->ShaderObjects->GetNextItem();
  while (object != NULL)
    {
    result = (this->DetachGlShader(object) && result);
    object = this->ShaderObjects->GetNextItem();
    }
  // object == NULL

  return result;
}

bool vtkBMIAShaderProgram::LinkGlProgram()
{
  GLint success;
  GLuint handle = this->GetHandle();
  vtkgl::LinkProgram(handle);
  //glGetShaderiv(handle, GL_LINK_STATUS, &success);
  vtkgl::GetProgramiv(handle, vtkgl::LINK_STATUS, &success);

  if (this->GetDebug() || (success != GL_TRUE))
    {
    if (success != GL_TRUE)
      {
      vtkWarningMacro(<<"Linking of shader program failed!");
      }

    GLint InfoLogLength;
    vtkgl::GetProgramiv(handle, vtkgl::INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength == 0)
      {
      vtkWarningMacro(<<"OpenGL info log has length 0!");
      return false;
      }
    else // InfoLogLength != 0
      {
          vtkgl::GLchar* InfoLog = (vtkgl::GLchar *)malloc(InfoLogLength);
      if (InfoLog == NULL)
        {
        vtkWarningMacro(<<"Could not allocate InfoLog buffer!");
        return false;
        }
      GLint CharsWritten = 0;
      vtkgl::GetProgramInfoLog(handle, InfoLogLength, &CharsWritten, InfoLog);
      vtkWarningMacro("Shader InfoLog for shader with handle "<<handle<<":\n"<<InfoLog);
      free(InfoLog);
      }
    //this->Validate();
    if (success != GL_TRUE) return false;
    }
  vtkDebugMacro(<<"Linking of shader program succeeded.");

  if (this->GetDebug()) this->Validate();

  return true;
}

bool vtkBMIAShaderProgram::SetAllGlUniforms(vtkShaderUniformCollection* uniforms)
{
  if (!this->GetHandleValid())
    {
    vtkErrorMacro(<<"Cannot set uniform values for a shader program that"
        <<" does not have a handle yet!");
    return false;
    }

  if (uniforms == NULL)
    {
    vtkErrorMacro(<<"Don't call glUniformAll(NULL)!");
    return false;
    }

  if (uniforms->GetNumberOfItems() < 1)
    {
    vtkDebugMacro(<<"No uniform values specified.");
    return true;
    }

  bool result = true;
  this->ShaderUniforms->InitTraversal();
  vtkShaderUniform* uniform = this->ShaderUniforms->GetNextItem();
  while (uniform != NULL)
    {
    vtkDebugMacro("Handling uniform "<<uniform<<"...");
    //uniform->DebugOn();
    result = (this->SetGlUniform(uniform) && result);
    uniform = this->ShaderUniforms->GetNextItem();
    }
  /* uniform == NULL */

  return result;
}

bool vtkBMIAShaderProgram::SetGlUniform(vtkShaderUniform* uniform)
{
  if (!uniform)
    {
    vtkErrorMacro(<<"Don't call glUniform with parameter NULL!");
    return false;
    }
  //uniform->DebugOn();
  uniform->SetHandle(this->GetHandle());
  return uniform->SetGlUniform();
}

bool vtkBMIAShaderProgram::DeleteGlProgram()
{
  if (!this->GetHandleValid())
    {
    vtkDebugMacro(<<"Calling DeleteGlProgram() without a valid handle!");
    // nothing to delete.
    return false;
    }
  vtkgl::DeleteProgram(this->GetHandle());
  return true;
}

bool vtkBMIAShaderProgram::Validate()
{
  if (!this->GetHandleValid())
    {
    vtkErrorMacro(<<"Cannot validate shader program without a handle!");
    return false;
    }

  GLint handle = this->GetHandle();
  GLint success;
  vtkgl::ValidateProgram(handle);
  vtkgl::GetProgramiv(handle, vtkgl::VALIDATE_STATUS, &success);

  if (success)
    {
    vtkWarningMacro(<<"Shader program successfully validated.");
    return true;
    }
  else // !success
    {
    GLint InfoLogLength;
    vtkgl::GetProgramiv(handle, vtkgl::INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength == 0)
      {
      vtkWarningMacro(<<"OpenGL info log has length 0!");
      }
    else // InfoLogLength != 0
      {
          vtkgl::GLchar* InfoLog = (vtkgl::GLchar *)malloc(InfoLogLength);
      if (InfoLog == NULL)
        {
        vtkWarningMacro(<<"Could not allocate InfoLog buffer!");
        return false;
        }
      GLint CharsWritten = 0;
      vtkgl::GetProgramInfoLog(handle, InfoLogLength, &CharsWritten, InfoLog);
      vtkWarningMacro("Validation of shader program failed. InfoLog:\n"<<InfoLog);
      free(InfoLog);
      }
    }

  return false;
}

void vtkBMIAShaderProgram::ApplyShaderUniforms()
{
  this->SetAllGlUniforms(this->ShaderUniforms);
}

void vtkBMIAShaderProgram::Activate()
{
  vtkDebugMacro(<<"Activating shader program...");
  this->Link();
  vtkgl::UseProgram(this->GetHandle());
  this->ApplyShaderUniforms();
}

void vtkBMIAShaderProgram::Deactivate()
{
  vtkgl::UseProgram(0);
}

