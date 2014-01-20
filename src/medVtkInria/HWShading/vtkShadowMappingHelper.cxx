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
 * vtkShadowMappingHelper.cxx
 * by Tim Peeters
 *
 * 2005-09-12	Tim Peeters
 * - First version
 *
 * 2005-11-18	 Tim Peeters
 * - Use FrameBuffer Objects
 * - Lots of changes. ;)
 *
 * 2005-11-22	Tim Peeters
 * - Make this->ShaderProgram replacable.
 *
 * 2005-12-08	Tim Peeters
 * - Yesterday I updated the NVIDIA drivers from 1.0-7676 to 1.0-8174 and now
 *   it seems I can use shadow maps with a resolution of up to 4096^2 instead
 *   2048^2 :)
 * - Changed the default resolution to 4096x3072 (4:3). I must make this an
 *   option that can be changed at run-time some time.
 *
 * 2006-01-30	Tim Peeters
 * - Updated to use vtkOpenGLExtensionManager and vtkgl.h instead of GLEW.
 *
 * 2006-03-06	Tim Peeters
 * - Cleaned up some texture matrix stuff such that actors in a scene that do not
 *   use the shadow map work correctly.
 * - Added SetupTextureMatrix() and RestoreTextureMatrix() functions.
 */

#include "vtkShadowMappingHelper.h"
#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>    // for light camera
#include <vtkMatrix4x4.h> // for light camera

#include "vtkUniformSampler.h"
#include "vtkBMIAShaderProgram.h"
#include "vtkVertexShader.h"
#include "vtkFragmentShader.h"
//#include <assert.h>
#include <BuildShadowMapVertexText.h>
#include <BuildShadowMapFragmentText.h>

#include "vtkShaderObjectCollection.h"

#define CHECK_FRAMEBUFFER_STATUS() \
{ \
GLenum status; \
status = glCheckFramebufferStatusEXT(vtkgl::FRAMEBUFFER_EXT); \
switch(status) { \
case vtkgl::FRAMEBUFFER_COMPLETE_EXT: \
vtkDebugMacro(<<"Framebuffer objects supported."); \
break; \
case vtkgl::FRAMEBUFFER_UNSUPPORTED_EXT: \
/* choose different formats */ \
vtkErrorMacro(<<"Framebuffer objects not supported!"); \
break; \
default: \
/* programming error; will fail on all hardware */ \
vtkErrorMacro(<<"FBO programming error; will fail on all hardware!"); \
} \
}

vtkCxxRevisionMacro(vtkShadowMappingHelper, "$Revision: 540 $");
vtkStandardNewMacro(vtkShadowMappingHelper);

vtkShadowMappingHelper::vtkShadowMappingHelper() {
  this->ShadowTextureInitialized = false;
  this->ShadowMapSampler = vtkUniformSampler::New();
  this->ShadowMapSampler->SetName("ShadowMap");

  this->ShaderProgram = vtkBMIAShaderProgram::New();
  vtkVertexShader* VertexShader = vtkVertexShader::New();
  vtkFragmentShader* FragmentShader = vtkFragmentShader::New();
  VertexShader->SetSourceText(BuildShadowMapVertexText);
  FragmentShader->SetSourceText(BuildShadowMapFragmentText);
  this->ShaderProgram->AddShaderObject(VertexShader);
  this->ShaderProgram->AddShaderObject(FragmentShader);
  VertexShader->Delete(); VertexShader = NULL;
  FragmentShader->Delete(); FragmentShader = NULL;

  //this->ShadowMapWidth = 4000;
  //this->ShadowMapHeight = 3072;
  //this->ShadowMapHeight = 4000;

//  this->ShadowMapWidth = 4096;
//  this->ShadowMapHeight = 3072;
  this->ShadowMapWidth = 1024;
  this->ShadowMapHeight = 1024;
}

vtkShadowMappingHelper::~vtkShadowMappingHelper() {
  this->ShadowMapSampler->Delete();
  this->ShadowMapSampler = NULL;

  this->ShaderProgram->Delete();

  // TODO: if (this->ShadowTextureInitialized), destroy shadow texture?!
}

void vtkShadowMappingHelper::InitializeShadowMap()
{
  // TODO: check whether needed extensions are supported

  // =========================
  // Create the shadow texture
  // =========================

  // Generate one texture name:
  glGenTextures(1, &(this->ShadowTexture));

  // Bind texture to texturing target: 
  glBindTexture(GL_TEXTURE_2D, this->ShadowTexture);

  // Specify a 2D texture
  // Levels of detail: 0 (no mipmap)
  // Internal components: Depth component
  // Width, Height: 512, 512
  // Border: 0
  // Format: Depth component
  // Type: unsigned byte
  // Image data pointer: NULL
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->ShadowMapWidth, this->ShadowMapHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
//  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->ShadowMapWidth, this->ShadowMapHeight, 0, GL_RGB, GL_SHORT, NULL);

  // Linear interpolation for pixel values when pixel is > or <= one
  // texture element:
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Clamp (and not repeat) parameters for texture coordinates:
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  // create FBO
  //glGenFramebuffersEXT(1, &(this->ShadowFBO));
  vtkgl::GenFramebuffersEXT(1, &(this->ShadowFBO));
  vtkgl::BindFramebufferEXT(vtkgl::FRAMEBUFFER_EXT, this->ShadowFBO);

//  glDrawBuffer(GL_NONE);
//  glReadBuffer(GL_NONE);

  // bind texture to FBO
  vtkgl::FramebufferTexture2DEXT(vtkgl::FRAMEBUFFER_EXT, vtkgl::COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->ShadowTexture, 0);

  // the render buffer for storing intermediate depth values.
  vtkgl::GenRenderbuffersEXT(1, &(this->DepthRBO));
  vtkgl::BindRenderbufferEXT(vtkgl::RENDERBUFFER_EXT, this->DepthRBO);
  vtkgl::RenderbufferStorageEXT(vtkgl::RENDERBUFFER_EXT, vtkgl::DEPTH_COMPONENT24, this->ShadowMapWidth, this->ShadowMapHeight);
  vtkgl::FramebufferRenderbufferEXT(vtkgl::FRAMEBUFFER_EXT, vtkgl::DEPTH_ATTACHMENT_EXT, vtkgl::RENDERBUFFER_EXT, this->DepthRBO);

 // glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, this->ShadowTexture, 0);
//  CHECK_FRAMEBUFFER_STATUS();

  GLenum status;
  status = vtkgl::CheckFramebufferStatusEXT(vtkgl::FRAMEBUFFER_EXT);

  switch(status) {
  case vtkgl::FRAMEBUFFER_COMPLETE_EXT:
    vtkDebugMacro(<<"Framebuffer objects supported.");
    break;
  case vtkgl::FRAMEBUFFER_UNSUPPORTED_EXT:
    // choose different formats
    vtkErrorMacro(<<"Framebuffer objects not supported!");
    break;
  case vtkgl::FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
    vtkErrorMacro(<<"FBO: Incomplete attachment!");
    break;
  case vtkgl::FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
    vtkErrorMacro(<<"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT");
    break;
    //  case vtkgl::FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
    //vtkErrorMacro(<<"FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT");
    //break;
  case vtkgl::FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
    vtkErrorMacro(<<"FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT");
    break;
  case vtkgl::FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
    vtkErrorMacro(<<"FRAMEBUFFER_INCOMPLETE_FORMATS_EXT");
    break;
  case vtkgl::FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
    vtkErrorMacro(<<"FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT");
    break;
  case vtkgl::FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
    vtkErrorMacro(<<"FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT");
    break;
//  case vtkgl::FRAMEBUFFER_STATUS_ERROR_EXT:
//    vtkErrorMacro(<<"FBO: status error!");
//    break;
  default:
    // programming error; will fail on all hardware
    vtkErrorMacro(<<"FBO programming error; will fail on all hardware!");
    //assert(0);
  }
  vtkDebugMacro(<<"Shadowmap texture initialized.");

  vtkgl::BindFramebufferEXT(vtkgl::FRAMEBUFFER_EXT, 0);
  this->ShadowTextureInitialized = true;
}

// TODO: speed this up. Generation of shadow map is too slow. This can
// be tested by making the call to RegenerateShadowMap in DeviceRender()
// unconditional.
void vtkShadowMappingHelper::PreShadowMapRender(vtkCamera* lightCamera)
{
  // first, store the matrices that I am going to change temporarily
  // for rendering the shadow map. These will be restored in
  // PostRenderShadowMap().
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();

  vtkgl::BindFramebufferEXT(vtkgl::FRAMEBUFFER_EXT, this->ShadowFBO);

    
  glGetIntegerv(GL_VIEWPORT, this->WindowViewport);
  glViewport(0, 0, this->ShadowMapWidth, this->ShadowMapHeight);
  glScissor(0, 0, this->ShadowMapWidth, this->ShadowMapHeight);

  vtkMatrix4x4* matrix = vtkMatrix4x4::New();

  vtkDebugMacro(<<"Clear depth buffer");
  GLbitfield  clear_mask = 0;

  
  
  // TODO: remove clearing of color buffer..
  //glClearColor( ((GLclampf)(this->Background[0])),
  //              ((GLclampf)(this->Background[1])),
  //              ((GLclampf)(this->Background[2])),
  //              ((GLclampf)(0.0)) );
  //glClearColor(0,0,1,1);

  // store the clear color to set it back later.
  glGetFloatv(GL_COLOR_CLEAR_VALUE, this->ColorClearValue);

  // set the new clear color.
  glClearColor(0, 1, 1, 1);
//  glClearColor(
//	((double)rand())/(double)RAND_MAX,
//	((double)rand())/(double)RAND_MAX,
//	((double)rand())/(double)RAND_MAX,
//	1);

  

  clear_mask |= GL_COLOR_BUFFER_BIT;

  glClearDepth( (GLclampd)( 1.0 ) );
  clear_mask |= GL_DEPTH_BUFFER_BIT;

  //vtkDebugMacro(<< "glClear\n");
  glClear(clear_mask);
  //glClear(GL_DEPTH_BUFFER_BIT);

  // TODO: disable lighting etc (e.g. by using a shader that
  //       does nothing but ft_transform() to improve performance

  // Set the viewport size to the shadow map size:
  // XXX: COMMENTED OUT BECAUSE IT MESSES UP THE VIEW IN vtkFiberMapper, but not in vtkShadowRenderer.
  //glViewport(0, 0, 1024, 1024);	//TODO: find out how I can use this without messing up the view.
//  glViewport(0, 0, 800, 550);

  // Deactivate writing in the color buffer (for better performance):
  //glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

  
  
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

  // Set up projection parameters:
  glMatrixMode(GL_PROJECTION);
#if VTK_MAJOR_VERSION>5 || (VTK_MAJOR_VERSION == 5 && VTK_MINOR_VERSION >=4)
  matrix->DeepCopy(lightCamera->GetProjectionTransformMatrix(1, 0, 1)); //TODO: replace first 1 by aspect ratio
#else
  matrix->DeepCopy(lightCamera->GetPerspectiveTransformMatrix(1, 0, 1)); //TODO: replace first 1 by aspect ratio
#endif
  matrix->Transpose();
  glLoadMatrixd(matrix->Element[0]);


  
  
  // Also add this to the texture matrix.
  if( vtkgl::ActiveTexture )
  {
    vtkgl::ActiveTexture(vtkgl::TEXTURE0);
  }  
  glMatrixMode(GL_TEXTURE);
  glLoadMatrixd(matrix->Element[0]);


  
    
  // Set up modelview parameters
  glMatrixMode(GL_MODELVIEW);
  matrix->DeepCopy(lightCamera->GetViewTransformMatrix());
  matrix->Transpose();
  glLoadMatrixd(matrix->Element[0]);

  // Also add this to the texture matrix
  glMatrixMode(GL_TEXTURE);
  glMultMatrixd(matrix->Element[0]);

  // store the texture matrix because it will be used later in SetupTextureMatrix.
  glGetDoublev(GL_TEXTURE_MATRIX, this->StoredTextureMatrix);


  
  
// van hier
/*
  vtkMatrix4x4* viewTransformMatrix = lightCamera->GetViewTransformMatrix();
  //vtkTransform* viewTransform = cam->GetViewTransformObject();
  vtkMatrix4x4* inverseViewTransformMatrix = vtkMatrix4x4::New();
  inverseViewTransformMatrix->DeepCopy(viewTransformMatrix);
  inverseViewTransformMatrix->Invert();
  inverseViewTransformMatrix->Transpose();
//  glMatrixMode(GL_TEXTURE);
//  glPushMatrix();
  glMultMatrixd(inverseViewTransformMatrix->Element[0]);  

  inverseViewTransformMatrix->Delete();
  inverseViewTransformMatrix = NULL;
  viewTransformMatrix = NULL;
  //cam = NULL;
*/
// tot hier










  
  glMatrixMode(GL_MODELVIEW);

  matrix->Delete(); matrix = NULL;

  //glPolygonOffset(po_scale, po_bias);
  //glEnable(GL_POLYGON_OFFSET_FILL);
  //glPolygonOffset(5.0, 2.0);

  this->ShaderProgram->Activate();
  
  /*
  cout<<"-- Activating shader program with vertex shader:"<<endl;
  cout<<this->ShaderProgram->GetShaderObjects()->GetItem(0)->GetSourceText();
  cout<<"-- and fragment shader:"<<endl;
  cout<<this->ShaderProgram->GetShaderObjects()->GetItem(1)->GetSourceText();
  cout<<"============================================================"<<endl;
  */
}

// Draw geometry:
// ren->UpdateGeometry(); // TODO: check if this does nothing with the matrices and/or
			  // switch from modelview to other matrices?

void vtkShadowMappingHelper::PostShadowMapRender()
{
  
  this->ShaderProgram->Deactivate();

//  glViewport(0, 0, this->ShadowMapWidth, this->ShadowMapHeight);

  // restore the projection matrix:
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  // restore the viewport of the window.
  glViewport(this->WindowViewport[0], this->WindowViewport[1], this->WindowViewport[2], this->WindowViewport[3]);
  glScissor(this->WindowViewport[0], this->WindowViewport[1], this->WindowViewport[2], this->WindowViewport[3]);

  // restore the texture and modelview matrices:
  glMatrixMode(GL_TEXTURE);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  // restore the original clear color.
  glClearColor(this->ColorClearValue[0], this->ColorClearValue[1], this->ColorClearValue[2], this->ColorClearValue[3]);

  vtkgl::BindFramebufferEXT(vtkgl::FRAMEBUFFER_EXT, 0);
  // XXX: is glActivateTexture(GL_TEXTURE0); needed?
  // appears not.. what is glActivateTexture useful for then?
  if( vtkgl::ActiveTexture )
  {
    vtkgl::ActiveTexture(vtkgl::TEXTURE0);
  }
  
  this->ShadowMapSampler->SetValue(0);
  // XXX: make sure actors w/ textures etc don't mess things up.
  // see how different textures for different actors are handled by VTK.

  // Store the depth buffer in the shadow texture:
  glBindTexture(GL_TEXTURE_2D, this->ShadowTexture);
  //glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, 512, 512);
  //glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, 1024, 1024);
//  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, 800, 550);
  // why not use glCopyTexImage2D instead of glCopyTexSubImage2D?

  //glClearColor(1, 0, 0, 1);

  /**
     Are these 2 glClear useful? When they are present, it seems
     impossible to render 2 set of fibers with shadowing activated
     in the same renderwindow...
  */
  //glClear(GL_DEPTH_BUFFER_BIT);
  //glClear(GL_COLOR_BUFFER_BIT);

  // disable the offset again
  //glDisable(GL_POLYGON_OFFSET_FILL);

  // Activate writing in the color buffer
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  
}

vtkCxxSetObjectMacro(vtkShadowMappingHelper, ShaderProgram, vtkBMIAShaderProgram);

//void vtkShadowMappingHelper::PrintFBOInfo()
//{
//  
//}

void vtkShadowMappingHelper::SetupTextureMatrix(vtkCamera* cam)
{
  // first, store the old matrix so that it can be restored in RestoreTextureMatrix().
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  // GLdouble* m = this->StoredTextureMatrix;
  //cout<<"Setting texture matrix to "<<m[0]<<", "<<m[1]<<", "<<m[2]<<", "<<m[3]<<", "<<m[4]<<", "<<m[5]<<", "<<m[6]<<", "<<m[7]<<", "<<m[8]<<", "<<m[9]<<", "<<m[10]
	//	<<", "<<m[11]<<", "<<m[12]<<", "<<m[13]<<", "<<m[14]<<", "<<m[15]<<"."<<endl;
  glLoadMatrixd(this->StoredTextureMatrix);

  // use the texture matrix for conversion between camera and light coordinates
  vtkMatrix4x4* viewTransformMatrix = cam->GetViewTransformMatrix();
  vtkMatrix4x4* inverseViewTransformMatrix = vtkMatrix4x4::New();
  inverseViewTransformMatrix->DeepCopy(viewTransformMatrix);
  inverseViewTransformMatrix->Invert();
  inverseViewTransformMatrix->Transpose();
  //glMatrixMode(GL_TEXTURE);
  //glPushMatrix();

  glMultMatrixd(inverseViewTransformMatrix->Element[0]);  
  inverseViewTransformMatrix->Delete();
  inverseViewTransformMatrix = NULL;
  viewTransformMatrix = NULL;
}

void vtkShadowMappingHelper::RestoreTextureMatrix()
{
  glMatrixMode(GL_TEXTURE);
  glPopMatrix();
}

