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
#include <BuildShadowMapVertexText.h>
#include <BuildShadowMapFragmentText.h>

#include "vtkShaderObjectCollection.h"

#define CHECK_FRAMEBUFFER_STATUS() \
{ \
    GLenum status; \
    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); \
    switch(status) { \
    case GL_FRAMEBUFFER_COMPLETE_EXT: \
    vtkDebugMacro(<<"Framebuffer objects supported."); \
    break; \
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT: \
    /* choose different formats */ \
    vtkErrorMacro(<<"Framebuffer objects not supported!"); \
    break; \
    default: \
    /* programming error; will fail on all hardware */ \
    vtkErrorMacro(<<"FBO programming error; will fail on all hardware!"); \
    } \
    }


vtkStandardNewMacro(vtkShadowMappingHelper);

vtkShadowMappingHelper::vtkShadowMappingHelper()
{
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
    VertexShader->Delete();
    VertexShader = nullptr;
    FragmentShader->Delete();
    FragmentShader = nullptr;

    this->ShadowMapWidth = 1024;
    this->ShadowMapHeight = 1024;
}

vtkShadowMappingHelper::~vtkShadowMappingHelper()
{
    this->ShadowMapSampler->Delete();
    this->ShadowMapSampler = nullptr;
    this->ShaderProgram->Delete();
}

void vtkShadowMappingHelper::InitializeShadowMap()
{
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
    // Image data pointer: nullptr
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->ShadowMapWidth, this->ShadowMapHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    // Linear interpolation for pixel values when pixel is > or <= one
    // texture element:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Clamp (and not repeat) parameters for texture coordinates:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    // create FBO
    glGenFramebuffersEXT(1, &(this->ShadowFBO));
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->ShadowFBO);

    // bind texture to FBO
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->ShadowTexture, 0);

    // the render buffer for storing intermediate depth values.
    glGenRenderbuffersEXT(1, &(this->DepthRBO));
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->DepthRBO);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, this->ShadowMapWidth, this->ShadowMapHeight);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->DepthRBO);

    GLenum status;
    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            vtkDebugMacro(<<"Framebuffer objects supported.");
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            // choose different formats
            vtkErrorMacro(<<"Framebuffer objects not supported!");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            vtkErrorMacro(<<"FBO: Incomplete attachment!");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            vtkErrorMacro(<<"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            vtkErrorMacro(<<"FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            vtkErrorMacro(<<"FRAMEBUFFER_INCOMPLETE_FORMATS_EXT");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            vtkErrorMacro(<<"FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            vtkErrorMacro(<<"FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT");
            break;
        default:
            // programming error; will fail on all hardware
            vtkErrorMacro(<<"FBO programming error; will fail on all hardware!");
    }
    vtkDebugMacro(<<"Shadowmap texture initialized.");

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    this->ShadowTextureInitialized = true;
}

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

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->ShadowFBO);

    glGetIntegerv(GL_VIEWPORT, this->WindowViewport);
    glViewport(0, 0, this->ShadowMapWidth, this->ShadowMapHeight);
    glScissor(0, 0, this->ShadowMapWidth, this->ShadowMapHeight);

    vtkMatrix4x4* matrix = vtkMatrix4x4::New();

    vtkDebugMacro(<<"Clear depth buffer");
    GLbitfield  clear_mask = 0;

    // store the clear color to set it back later.
    glGetFloatv(GL_COLOR_CLEAR_VALUE, this->ColorClearValue);

    // set the new clear color.
    glClearColor(0, 1, 1, 1);

    clear_mask |= GL_COLOR_BUFFER_BIT;

    glClearDepth( (GLclampd)( 1.0 ) );
    clear_mask |= GL_DEPTH_BUFFER_BIT;
    glClear(clear_mask);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // Set up projection parameters:
    glMatrixMode(GL_PROJECTION);

    matrix->DeepCopy(lightCamera->GetProjectionTransformMatrix(1, 0, 1)); //TODO: replace first 1 by aspect ratio
    matrix->Transpose();
    glLoadMatrixd(matrix->Element[0]);

    // Also add this to the texture matrix.
    if( glActiveTexture )
    {
        glActiveTexture(GL_TEXTURE0);
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

    glMatrixMode(GL_MODELVIEW);

    matrix->Delete();
    matrix = nullptr;

    this->ShaderProgram->Activate();
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

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    // XXX: is glActivateTexture(GL_TEXTURE0); needed?
    // appears not.. what is glActivateTexture useful for then?
    if( glActiveTexture )
    {
        glActiveTexture(GL_TEXTURE0);
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

void vtkShadowMappingHelper::SetupTextureMatrix(vtkCamera* cam)
{
    // first, store the old matrix so that it can be restored in RestoreTextureMatrix().
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadMatrixd(this->StoredTextureMatrix);

    // use the texture matrix for conversion between camera and light coordinates
    vtkMatrix4x4* viewTransformMatrix = cam->GetViewTransformMatrix();
    vtkMatrix4x4* inverseViewTransformMatrix = vtkMatrix4x4::New();
    inverseViewTransformMatrix->DeepCopy(viewTransformMatrix);
    inverseViewTransformMatrix->Invert();
    inverseViewTransformMatrix->Transpose();

    glMultMatrixd(inverseViewTransformMatrix->Element[0]);
    inverseViewTransformMatrix->Delete();
    inverseViewTransformMatrix = nullptr;
    viewTransformMatrix = nullptr;
}

void vtkShadowMappingHelper::RestoreTextureMatrix()
{
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
}

