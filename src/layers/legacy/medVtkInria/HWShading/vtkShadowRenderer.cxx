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
 * vtkShadowRenderer.cxx
 * by Tim Peeters
 *
 * 2005-07-19	Tim Peeters
 * - First version
 *
 * 2005-07-27	Tim Peeters
 * - Completely new version which uses GLSL instead of NVidia extensions.
 *   This is better because it also runs on non-NVidia graphics cards,
 *   and it can be combined with e.g. anisotropic lighting. But it is not
 *   possible to run this on graphics cards that do not support GLSL.
 *
 * 2005-08-08	Tim Peeters
 * - Renamed ShowShadowMap() to DrawShadowMap().
 * - Added bool ShowShadowMap variable which is checked at render-time to
 *   determine whether the scene or the shadow map must be drawn on the
 *   screen.
 *
 * 2005-09-12	Tim Peeters
 * - Make use of bmia::vtkShadowMappingHelper.
 *
 * 2005-11-18	Tim Peeters
 * - Better DrawShadowMap()
 * - Some other small improvements.
 *
 * 2006-01-30	Tim Peeters
 * - Make use of  vtkgl.h
 */

#include "vtkShadowRenderer.h"
#include "vtkShadowMappingSP.h"
#include "vtkUniformSampler.h"
#include "vtkShadowMappingHelper.h"

#include <vtkObjectFactory.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkMatrix4x4.h>
#include <vtkLight.h>
#include <vtkLightCollection.h>

#include <assert.h>
#include <vtk_glew.h>

#include "vtkAnisoLiShadowMapSP.h"

vtkStandardNewMacro(vtkShadowRenderer);

vtkShadowRenderer::vtkShadowRenderer()
{
  this->RenderShadows = true;
  this->ShowShadowMap = false;
  this->ShadowMappingHelper = vtkShadowMappingHelper::New();
  this->ShadowMappingHelper->DebugOn();
  this->ShadowMappingSP = vtkShadowMappingSP::New();
//  this->ShadowMappingSP = vtkAnisoLiShadowMapSP::New();
  this->ShadowMappingSP->AddShaderUniform(this->ShadowMappingHelper->GetShadowMapSampler());
}

vtkShadowRenderer::~vtkShadowRenderer()
{
    this->ShadowMappingHelper->Delete();
    this->ShadowMappingHelper = nullptr;

    this->ShadowMappingSP->Delete();
    this->ShadowMappingSP = nullptr;
}

void vtkShadowRenderer::DeviceRender()
{
  if (!this->RenderShadows)
    {
    this->vtkOpenGLRenderer::DeviceRender();
    return;
    }

  // glClearColor(0.0, 0.0, 0.0, 0.0); // this->Clear()

  vtkDebugMacro(<<"Enable OpenGL lighting");
  glEnable(GL_LIGHTING);
  //glEnable(GL_LIGHT0);
  //glShadeModel(GL_SMOOTH);

  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_TEXTURE_2D);
  //glFrontFace(GL_CCW);
  //glEnable(GL_CULL_FACE);

  this->RenderWindow->MakeCurrent();

  if (!this->ShadowMappingHelper->GetShadowTextureInitialized())
    {
    vtkDebugMacro(<<"Initialize shadow map");
    this->InitializeShadowMap();
    }
  //glEnable(GL_LIGHT0); // Makes no difference whether this is here.

  // Regenerate the shadow map if the first light was changed.
  // This is also done if the shadow map was not yet initialized because
  // then GetShadowMapMTime() returns 0.
  // XXX: At initialization, we assume that GetMTime() of the first light does not return 0.
  // XXX: It is also assumed that the first light was not replaced by a different light.
//  if (this->GetFirstLight()->GetMTime() > this->GetShadowMapMTime())
//    {

    vtkDebugMacro(<<"First render pass: Render from light camera");
    this->RegenerateShadowMap();
//    }

    vtkDebugMacro(<<"Second render pass: Render from eye with shadows from shadowmap");
    this->RenderWithShadows();
//    }

  if (this->ShowShadowMap)
    {  // show the shadow map for testing
    vtkDebugMacro(<<"Renderig the shadow map to be shown on-screen");
    this->DrawShadowMap();
    }

  //this->vtkOpenGLRenderer::DeviceRender();
}

void vtkShadowRenderer::InitializeShadowMap()
{
  this->ShadowMappingHelper->InitializeShadowMap();
}

void vtkShadowRenderer::RegenerateShadowMap()
{
  vtkDebugMacro(<<"Create camera from light");

  vtkCamera* light_cam = vtkCamera::New();
  vtkCamera* view_cam = this->GetActiveCamera();

  // Light camera follows the regular camera
  double vec[3];
  view_cam->GetPosition(vec);
  light_cam->SetPosition(vec);
  view_cam->GetFocalPoint(vec);
  light_cam->SetFocalPoint(vec);
  view_cam->GetViewUp(vec);
  light_cam->SetViewUp(vec);
  light_cam->SetViewAngle(view_cam->GetViewAngle());
  light_cam->SetClippingRange(view_cam->GetClippingRange());

  // Off to one side
  light_cam->Roll(-10);
  // Elevate
  light_cam->Elevation(20);
  // Zoom out
  light_cam->Zoom(.8);

  this->ShadowMappingHelper->PreShadowMapRender(light_cam);
  light_cam->Delete();
  light_cam = nullptr;
  this->UpdateGeometry();
  this->ShadowMappingHelper->PostShadowMapRender();

  this->ShadowMapMTime.Modified();
}

void vtkShadowRenderer::DrawShadowMap()
{
  // Display shadow map for testing purposes
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, 1, 0, 1, -1, 1);
  glDisable(GL_DEPTH_TEST);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  glLoadIdentity();
  glDisable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
    glColor3f(1, 0, 1);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(0.3f, 0.0f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(0.3f, 0.3f);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0.0f, 0.3f);
  glEnd(); // GL_QUADS

  glEnable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // int* windowDimensions = this->RenderWindow->GetSize();
  // GLfloat windowWidth = (GLfloat)windowDimensions[0];
  // GLfloat windowHeight = (GLfloat)windowDimensions[1];

  glBegin(GL_QUADS);
  glColor3f(1, 1, 1);
  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(0.0f, 0.0f);

  glTexCoord2f(1.0f, 0.0f);
  glVertex2f(0.3f, 0.0f);

  glTexCoord2f(1.0f, 1.0f);
  glVertex2f(0.3f, 0.3f);

  glTexCoord2f(0.0f, 1.0f);
  glVertex2f(0.0f, 0.3f);
  glEnd(); // GL_QUADS
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void vtkShadowRenderer::RenderWithShadows()
{
  // In the Sandia renderer, the code between the ===='s
  // is commented out because it "messes everything up".
  // For now, it seems to me as if these functions set up everything
  // in a nice way, so I leave it there.
  // As long as it leaves the shadow map texture in place I guess its ok.

  // =========================================================
  // standard render method
  this->ClearLights();

  this->UpdateCamera();
  this->UpdateLightGeometry();
  this->UpdateLights();
  // =========================================================

  vtkCamera* cam = this->GetActiveCamera();
  if (!cam)
    {
    vtkErrorMacro(<<"No active camera!");
    }

  vtkMatrix4x4* viewTransformMatrix = cam->GetViewTransformMatrix();
  vtkMatrix4x4* inverseViewTransformMatrix = vtkMatrix4x4::New();
  inverseViewTransformMatrix->DeepCopy(viewTransformMatrix);
  inverseViewTransformMatrix->Invert();
  inverseViewTransformMatrix->Transpose();
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  glMultMatrixd(inverseViewTransformMatrix->Element[0]);

  inverseViewTransformMatrix->Delete();
  inverseViewTransformMatrix = nullptr;
  viewTransformMatrix = nullptr;
  cam = nullptr;

  // set matrix mode for actors
  glMatrixMode(GL_MODELVIEW);

  this->ShadowMappingSP->Activate();
  this->UpdateGeometry();
  this->ShadowMappingSP->Deactivate();

  // clean up the model view matrix set up by the camera
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  // clean up the view/camera component of the texture matrix
  glMatrixMode(GL_TEXTURE);
  glPopMatrix();
}

unsigned long vtkShadowRenderer::GetShadowMapMTime()
{
  return this->ShadowMapMTime.GetMTime();
}


