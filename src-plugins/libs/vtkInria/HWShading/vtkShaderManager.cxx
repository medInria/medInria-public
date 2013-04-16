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
 * vtkShaderManager.cxx
 * by Tim Peeters
 *
 * 2005-05-04	Tim Peeters
 * - First version
 *
 * 2005-06-06	Tim Peeters
 * - Switched to OpenGL 2.0
 * - Renamed glUseProgram() to UseGlProgram()
 */

#include "vtkShaderManager.h"

#include "vtkBMIAShaderProgram.h"
#include "vtkBMIAShaderProgramReader.h"

#include <vtkObjectFactory.h>

vtkCxxRevisionMacro (vtkShaderManager, "$Revision: 540 $");
vtkStandardNewMacro(vtkShaderManager);

vtkShaderManager::vtkShaderManager()
{
  this->ActiveShaderProgram = NULL;
  this->Initialized = false;
}

vtkShaderManager::~vtkShaderManager()
{
  this->SetActiveShaderProgram(NULL);
}

void vtkShaderManager::Initialize()
{
  // TODO: check for working OpenGL environment to use.
  // otherwise segfaults will pop up when using gl*() later.
  cout<<"vtkShaderManager::Initialize(): calling glewInit()."<<endl;
  glewInit();
  if (this->GetHWShaderSupport())
    {
    vtkDebugMacro(<<"Ready for GLSL.");
    this->Initialized = true;
    }
  else
    {
    vtkWarningMacro(<<"GLSL not supported!");
    this->Initialized = false;
    }
}

bool vtkShaderManager::GetHWShaderSupport()
{
  // note: glewInit() or this->Initialize() must have been called first.
  //  return (	(glewGetExtension("GL_ARB_fragment_shader") == GL_TRUE) &&
  //		(glewGetExtension("GL_ARB_vertex_shader") == GL_TRUE)
  //	 );
  //  return true; // TODO: REALLY check :)
        //glewGetExtension("GL_ARB_shader_objects")       != GL_TRUE ||
        //glewGetExtension("GL_ARB_shading_language_100") != GL_TRUE)

  // Make sure that OpenGL 2.0 is supported by the driver
  int gl_major, gl_minor;
  this->GetGlVersion(&gl_major, &gl_minor);
  vtkDebugMacro(<<"GL_VERSION major="<<gl_major<<" minor="<< gl_minor);

  if (gl_major < 2)
  {
    vtkErrorMacro(<<"GL_VERSION major="<<gl_major<<" minor="<< gl_minor<<" "
	<<"Support for OpenGL 2.0 is required!");
    return false;
  }
  return true;
}

void vtkShaderManager::SetActiveShaderProgram(vtkBMIAShaderProgram* program)
{
  vtkDebugMacro(<<"Setting active shader program to "<<program);
  if (this->ActiveShaderProgram != program)
    {
    if (this->ActiveShaderProgram != NULL)
      {
      this->ActiveShaderProgram->UnRegister(this);
      }
    this->ActiveShaderProgram = program;
    if (program != NULL)
      {
      program->Register(this);
      //program->DebugOn();
      }
    this->UpdateActiveShaderProgram();
    this->Modified();
    }
}

void vtkShaderManager::UpdateActiveShaderProgram()
{
  vtkDebugMacro(<<"Updating active shader program");
  if (this->ActiveShaderProgram != NULL)
    {
    // Link() handles everything for you :)
    this->ActiveShaderProgram->Link();
    }
  this->UseGlProgram();
}

bool vtkShaderManager::UseGlProgram()
{
  if (!this->Initialized)
    {
    vtkWarningMacro(<<"Shader manager was not initialized!");
    return false;
    }

  if (this->ActiveShaderProgram == NULL)
    {
    // disable the programmable processors, and use fixed functionality
    // for both vertex and fragment processing.
    vtkDebugMacro(<<"Calling glUseProgram(0)");
    glUseProgram(0);
    }
  else
    {
    vtkDebugMacro(<<"Calling glUseProgram("<<this->ActiveShaderProgram->GetHandle()<<");");
    glUseProgram(this->ActiveShaderProgram->GetHandle());
    this->ActiveShaderProgram->ApplyShaderUniforms();
    }
  return true;
}

void vtkShaderManager::UseShaderFromFile(const char* filename)
{
  if (!this->Initialized)
    {
    vtkWarningMacro(<<"Shader manager was not initialized! Cancelling loading of shader.");
    return;
    }
  vtkDebugMacro(<<"Creating shader program...");
  vtkBMIAShaderProgramReader* reader = vtkBMIAShaderProgramReader::New();
  reader->SetFileName(filename);
  reader->Execute();
  this->SetActiveShaderProgram(reader->GetOutput());
  reader->Delete(); reader = NULL;
}

void vtkShaderManager::GetGlVersion( int *major, int *minor )
{
  const char* verstr = (const char*)glGetString( GL_VERSION );
  if( (verstr == NULL) || (sscanf( verstr, "%d.%d", major, minor ) != 2) )
  {
      *major = *minor = 0;
      vtkErrorMacro(<<"Invalid GL_VERSION format!!!");
  }
}

// from http://developer.nvidia.com/object/nv_ogl2_support.html
bool vtkShaderManager::SupportsOpenGLVersion(int atLeastMajor, int atLeastMinor)
{
  const char* version;
  int major, minor;

  version = (const char *) glGetString(GL_VERSION);
  cout<<"OpenGL version is "<<version<<endl;

  if (sscanf(version, "%d.%d", &major, &minor) == 2) {
  if (major > atLeastMajor)
    return true;
  if (major == atLeastMajor && minor >= atLeastMinor)
    return true;
  } else {
    /* OpenGL version string malformed! */
  }
  return false;
}

