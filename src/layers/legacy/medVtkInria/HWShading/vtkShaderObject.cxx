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
 * vtkShaderObject.cxx
 * by Tim Peeters
 *
 * 2005-05-03	Tim Peeters
 * - First version
 *
 * 2005-06-03	Tim Peeters
 * - Use bmia namespace
 * - Replaced including <iostream> and <fstream> by
 *   <vtkIOStream.h>
 *
 * 2005-06-06	Tim Peeters
 * - Switched to OpenGL 2.0 instead of using ARB extension for OpenGL 1.5
 * - Renamed function names (see changes in header file).
 * - Added DeleteGlShader()
 *
 * 2005-09-12	Tim Peeters
 * - Added printShaderInfoLog() from ogl2brick.
 *
 * 2006-01-30	Tim Peeters
 * - Make use of vtkgl header instead of glew.
 *   So, use vtgl::Function() instead of glFunction() for extensions.
 */
//#define GL_GLEXT_PROTOTYPES 1
//#include <GL/gl.h>
//#include "glext-tim.h"

#include "vtkShaderObject.h"
#include <vtkObjectFactory.h>

#include <vtkIOStream.h>



vtkShaderObject::vtkShaderObject()
{
  //this->SourceFileName = NULL;
  //this->SourceText = ""; //NULL;
  this->SourceText = NULL;
  this->Compiled = false;

  //this.CompileTime = vtkTimeStamp::New();
}

vtkShaderObject::~vtkShaderObject()
{
  if (this->Compiled)
    {
    this->Compiled = false;
    }

  if (this->SourceText != NULL)
    {
    delete [] this->SourceText;
    this->SourceText = NULL;
    }

  // check for HandleValid is in DeleteGlShader().
  this->DeleteGlShader();
}

void vtkShaderObject::Compile()
{
  if ( (this->Compiled) && (this->CompileTime > this->GetMTime()) )
    {
    vtkDebugMacro(<<"Compiled && CompileTime > MTime. Not recompiling.");
    // no need to recompile.
    return;
    }

  //vtkDebugMacro(<<"CompileTime not greater than MTime. Recompiling.");
  this->Compiled = false;

  bool success;
  success = this->CreateGlShader();
  if (!success)
    {
    vtkWarningMacro(<<"Could not create gl shader object!");
    return;
    }

  success = this->SetGlShaderSource();
  if (!success)
    {
    vtkWarningMacro(<<"Could not set shader source!");
    return;
    }

  success = this->CompileGlShader();
  if (!success)
    {
    vtkWarningMacro(<<"Could not compile shader object with source text:\n"
		<<this->SourceText);
    vtkShaderObject::PrintShaderInfoLog(this->GetHandle());
    return;
    }

  this->CompileTime.Modified();
  this->Compiled = true;
  return;
}

bool vtkShaderObject::SetGlShaderSource()
{
  if (this->SourceText == NULL)
    {
    vtkWarningMacro(<<"No source text was specified!");
    return false;
    // OR: just compile and have an empty shader object?
    }

  const char* text = this->SourceText;
  //glShaderSource(this->GetHandle(), 1, &text, NULL);
  glShaderSource(this->GetHandle(), 1, &text, NULL);

  // XXX: I think/assume text only copies the pointer to SourceText,
  // so it does not need to be deleted here.
  text = NULL;

  return true;
}

bool vtkShaderObject::CompileGlShader()
{
  // this->GetHandleValid() is checked in this->GetHandle().
  //if (!this->GetHandleValid())
  //  {
  //  vtkWarningMacro(<<"No valid handle. Cancelling compilation.");
  //  return false;
  //  }

  GLint success;

  glCompileShader(this->GetHandle());

  glGetShaderiv(this->GetHandle(), GL_COMPILE_STATUS, &success);
  if (success != GL_TRUE)
    {
    vtkWarningMacro(<<"Compilation of shader failed!");
    // TODO: find out why compilation failed and output that.
    return false;
    }

  return true;
}

void vtkShaderObject::ReadSourceTextFromFile(const char* filename)
{
  if ( !filename || (strlen(filename) == 0))
    {
    vtkErrorMacro(<< "No file specified!");
    return;
    }

  vtkDebugMacro("Reading source text from file "<<filename);
  // http://www.cplusplus.com/ref/iostream/istream/read.html

  int length;
  char * buffer;

  ifstream is;
  is.open (filename, ios::binary );

  // get length of file:
  is.seekg (0, ios::end);
  length = is.tellg();
  is.seekg (0, ios::beg);

  // allocate memory:
  buffer = new char [length+1];

  // read data as a block:
  is.read (buffer,length);

  is.close();
  //vtkDebugMacro(<<"Last 3 character read are:"<<buffer[length-3]<<buffer[length-2]<<buffer[length-1]);

  buffer[length] = '\0'; // seems to help :)
  this->SetSourceText(buffer);

  delete [] buffer;
}

bool vtkShaderObject::DeleteGlShader()
{
  if (!this->GetHandleValid())
    {
    vtkDebugMacro(<<"Calling DeleteGlProgram() without a valid handle!");
    // nothing to delete.
    return false;
    }
  glDeleteShader(this->GetHandle());
  return true;
}

//
// Print out the information log for a shader object
//
void vtkShaderObject::PrintShaderInfoLog(GLuint shader)
{
    GLint infologLength = 0;
    GLsizei charsWritten  = 0;
    GLchar *infoLog;

    //printOpenGLError();  // Check for OpenGL errors

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);

    //printOpenGLError();  // Check for OpenGL errors

    if (infologLength > 0)
    {
    infoLog = (GLchar *)malloc(infologLength);
        if (infoLog == NULL)
        {
            printf("ERROR: Could not allocate InfoLog buffer\n");
            exit(1);
        }
        glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
        printf("Shader InfoLog:\n%s\n\n", infoLog);
        free(infoLog);
    }
    //printOpenGLError();  // Check for OpenGL errors
}
