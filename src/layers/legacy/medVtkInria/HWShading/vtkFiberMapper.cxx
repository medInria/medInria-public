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
 * vtkFiberMapper.cxx
 * by Tim Peeters
 *
 * 2005-06-22	Tim Peeters
 * - First version
 *
 * 2005-06-28	Tim Peeters
 * - Instead of reading a shader program from a specified file in the
 *   constructor, use an instance vtkAnisotropicLightingSP to do the lighting.
 *
 * 2005-07-18	Tim Peeters
 * - Renamed vtkOpenGLFiberMapper to vtkFiberMapper
 *
 * 2005-09-07	Tim Peeters
 * - Deactivate shader program after rendering so that it is not applied to
 *   things rendered outside this mapper.
 *
 * 2005-09-13	Tim Peeters
 * - Render a shadow map using bmia::vtkShadowMappingHelper
 * - Use bmia::vtkAnisoLiShadowMapSP instead of bmia::vtkAnisotropicLightingSP
 *   so that shadows can be rendered.
 *
 * 2005-09-14	Tim Peeters
 * - Added DrawShadowMap() which can be used for testing. This function was
 *   copied from bmia::vtkShadowRenderer.
 *
 * 2005-11-21	Tim Peeters
 * - Copied the new DrawShadowMap() from vtkShadowRenderer.
 *
 * 2005-11-21	Tim Peeters
 * - Initialize this->ShadowMappingHelper as a ShadowMappingHelperLines.
 *
 * 2005-12-07	Tim Peeters
 * - It is now possible to enable and disable shadowing and lighting.
 *   Shadowing only works if lighting is also enabled.
 *
 * 2006-01-30	Tim Peeters
 * - Use VTK OpenGL extension manager and vtkgl.h instead of GLEW.
 *
 * 2006-02-22	Tim Peeters
 * - Handle initialization of OpenGL extensions if needed.
 * - Include vtkOpenGLExtensionManager.h
 * - Added Initialize() and IsRenderSupported() functions.
 *
 * 2006-03-05	Tim Peeters
 * - Added {Get,Set}{Ambient,Diffuse,Specular}Contribution()
 *   and {Get,Set}SpecularPower() functions.
 *
 * 2006-03-06	Tim Peeters
 * - Add support for vertex coloring.
 *
 * 2006-12-26	Tim Peeters
 * - Add support for tone shading.
 */

#include "vtkFiberMapper.h"
#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkActor.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkOpenGLRenderer.h>
#include <vtkCellArray.h>
#include <vtkSetGet.h>
#include <vtkCamera.h>
#include <vtkMatrix4x4.h>

#include <assert.h>

#include "vtkAnisotropicLightingSP.h"
#include "vtkAnisoLiShadowMapSP.h"
#include "vtkShadowMappingHelperLines.h"

#include <vtkOpenGLRenderWindow.h>
#include <vtkProperty.h>

vtkStandardNewMacro(vtkFiberMapper);

vtkFiberMapper::vtkFiberMapper()
{
  this->ShaderProgram = vtkAnisotropicLightingSP::New();

  this->ShaderProgramShadow = vtkAnisoLiShadowMapSP::New();

  this->ShadowMappingHelper = vtkShadowMappingHelperLines::New();

  this->Lighting = true;
  this->Shadowing = true;
  this->ToneShading = false;

  this->ShadowLineWidth = 4.0;

  this->Initialized = false;

}

vtkFiberMapper::~vtkFiberMapper()
{
    this->ShaderProgram->Delete();
    this->ShaderProgram       = nullptr;
    this->ShaderProgramShadow->Delete();
    this->ShaderProgramShadow = nullptr;
    this->ShadowMappingHelper->Delete();
    this->ShadowMappingHelper = nullptr;
}

void vtkFiberMapper::Render(vtkRenderer* aren, vtkActor* act)
{
    if (!this->Lighting)
    {
        return this->vtkOpenGLPolyDataMapper::Render(aren, act);
    }

    aren->GetRenderWindow()->MakeCurrent();
    if ( !this->Initialized )
    {
        this->Initialize(aren);
    }

    if ( !this->Initialized )
    {
        vtkErrorMacro(<<"Required extensions not supported. Not rendering!");
        return;
    }

    vtkDebugMacro(<<"Drawing actor "<<act<<" with renderer "<<aren);
    vtkOpenGLRenderer *ren = (vtkOpenGLRenderer *)aren;
    vtkPolyData* input = this->GetInput();

    if (ren == nullptr)
    {
        vtkErrorMacro(<<"Trying to render with renderer null. Aborting.");
        return;
    }

    if (input == nullptr)
    {
        vtkWarningMacro(<<"Input is null. Nothing to render.");
        return;
    }

    // map the scalars to colors:
    this->MapScalars(act->GetProperty()->GetOpacity());

    // this->Colors should be set now.
    if (!this->Colors) glColor4d(act->GetProperty()->GetColor()[0],
            act->GetProperty()->GetColor()[1],
            act->GetProperty()->GetColor()[2],
            act->GetProperty()->GetOpacity());

    int noAbort = 1;

    vtkDebugMacro(<<"Getting input data.");
    vtkPoints* points = input->GetPoints();
    vtkCellArray* lineStrips = input->GetLines();

    vtkCamera* cam = aren->GetActiveCamera();
    if (!cam)
    {
        vtkErrorMacro(<<"No camera!");
        return;
    }

    if (this->Shadowing)
    {
        glDisable(GL_LIGHTING);

        if (!this->ShadowMappingHelper->GetShadowTextureInitialized())
        {
            vtkDebugMacro(<<"Initialize shadow map");
            this->ShadowMappingHelper->InitializeShadowMap();
        }

        vtkDebugMacro(<<"First render pass: Render from light camera");
        this->RegenerateShadowMap(points, lineStrips, aren, noAbort);

        // use the texture matrix for conversion between camera and light coordinates.
        vtkDebugMacro(<<"Setting up texture matrix...");
        this->ShadowMappingHelper->SetupTextureMatrix(cam);
    }

    if (this->Lighting)
    {
        vtkDebugMacro(<<"Enabling anisotropic lighting shader "
                      <<this->ShaderProgram<<".");
        if (this->Shadowing)
        {
            this->ShaderProgramShadow->Activate();
            vtkDebugMacro(<<"Anisotropic lighting shader with shadows enabled.");
        }
        else
        {
            this->ShaderProgram->Activate();
        }
    }
    else
    {
        vtkDebugMacro(<<"Disabling anisotropic lighting shader.");
        vtkBMIAShaderProgram::Deactivate();
        glDisable(GL_LIGHTING);
    }

    // set matrix mode for actors
    glMatrixMode(GL_MODELVIEW);

    this->DrawLines(points, lineStrips, aren, noAbort);

    // make sure other mappers do not use this shader.
    vtkBMIAShaderProgram::Deactivate();

    // clean up the view/camera component of the texture matrix
    if (this->Shadowing)
    {
        this->ShadowMappingHelper->RestoreTextureMatrix();
    }

    vtkDebugMacro(<<"Rendering finished.");

    return;
}

void vtkFiberMapper::DrawLines(vtkPoints* points, vtkCellArray* lineStrips,
                vtkRenderer* ren, int &noAbort)
{
    vtkDebugMacro(<<"Drawing lines...");

    int i; int j;
    vtkIdType *pts = 0;
    vtkIdType npts = 0;
    unsigned short count = 0;
    vtkIdType cellNum = 0;
    vtkIdType totalCells = lineStrips->GetNumberOfCells();
    (void)totalCells; // to avoid unused warning when not in debug

    vtkDebugMacro(<<"Drawing "<<totalCells<<" line strips.");

    double* previousPoint = new double[3];
    double* currentPoint = nullptr;
    double* tangent = new double[3];
    short lineID[2];

    for(lineStrips->InitTraversal(); noAbort  && lineStrips->GetNextCell(npts, pts);
        count++)
    {
        lineID[0] = count;

        glBegin(GL_LINES);

        // compute the tangent for the first part and initialize
        // currentPoint as the first point.
        currentPoint = points->GetPoint(pts[0]);

        for (i = 1; i < npts; i++)
        {
            lineID[1] = i;

            for (j=0; j < 3; j++)
            {
                previousPoint[j] = currentPoint[j];
            }
            currentPoint = points->GetPoint(pts[i]);

            if (this->Lighting)
            {
                for (int k=0; k < 3; k++)
                {
                    tangent[k] = currentPoint[k] - previousPoint[k];
                }

                glVertexAttrib3dv(1, tangent);
                if (this->Shadowing)
                {
                    glVertexAttrib2sv(2, lineID); // pass fibernr, linenr (in fiber)
                }
            }

            // Color the vertices.
            if (this->Colors) glColor4ubv(this->Colors->GetPointer(pts[i] << 2));

            if ((this->RGBColoring) && (!this->Lighting))
            {
                double sqsum = 0.0;
                for (int k=0; k < 3; k++)
                {
                    tangent[k] = currentPoint[k] - previousPoint[k];
                    sqsum += tangent[k]*tangent[k];
                }
                sqsum = sqrt(sqsum);

                // normalize the tangent
                for (int k=0; k< 3; k++)
                {
                    tangent[k] = fabs(tangent[k])/sqsum;
                }

                // use the normalized tangent as the color.
                glColor3dv(tangent);
            }

            glVertex3dv(previousPoint);

            if (this->Shadowing)
            {
                glVertexAttrib2sv(2, lineID);
            }
            glVertex3dv(currentPoint);
        }
        glEnd(); // GL_LINES // GL_LINE_STRIP

        ++cellNum;
    }
    delete[] tangent;
    tangent = nullptr;
}

void vtkFiberMapper::RegenerateShadowMap(vtkPoints* points, vtkCellArray* lineStrips,
                vtkRenderer* ren, int &noAbort)
{
  vtkDebugMacro(<<"Create camera from light");

  vtkCamera* light_cam = vtkCamera::New();
  assert(ren != nullptr);
  vtkCamera* view_cam = ren->GetActiveCamera();
  assert(view_cam != nullptr);

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

  glLineWidth(this->ShadowLineWidth);
  this->DrawLines(points, lineStrips, ren, noAbort);
  glLineWidth(1.0);
  this->ShadowMappingHelper->PostShadowMapRender();
}

void vtkFiberMapper::DrawShadowMap()
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
    glColor4f(1, 1, 0, 1);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(0.1f, 0.0f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(0.1f, 0.1f);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0.0f, 0.1f);
  glEnd(); // GL_QUADS

  glEnable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBegin(GL_QUADS);
    glColor4f(1, 0, 0, 1);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(0.2f, 0.0f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(0.2f, 0.2f);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0.0f, 0.2f);
  glEnd(); // GL_QUADS

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
  glMatrixMode(GL_TEXTURE);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glEnable(GL_DEPTH_TEST);
}

void vtkFiberMapper::SetShadowing(bool render_shadows)
{
  if (this->Shadowing == render_shadows)
    {
    return;
    }

  this->Shadowing = render_shadows;
}

void vtkFiberMapper::SetLighting(bool light)
{
  if (this->Lighting == light)
    {
    return;
    }

  this->Lighting = light;
}

void vtkFiberMapper::SetRGBColoring(bool coloring)
{
  if (this->RGBColoring == coloring)
    {
    return;
    }

  this->RGBColoring = coloring;
  this->ShaderProgram->SetRGBColoring(this->RGBColoring);
  this->ShaderProgramShadow->SetRGBColoring(this->RGBColoring);
}

void vtkFiberMapper::SetToneShading(bool tone_shading)
{
  if (this->ToneShading == tone_shading)
    {
    return;
    }

  this->ToneShading = tone_shading;
  this->ShaderProgram->SetToneShading(this->ToneShading);
  this->ShaderProgramShadow->SetToneShading(this->ToneShading);
}

void vtkFiberMapper::SetCoolColor(float r, float g, float b)
{
    this->ShaderProgram->SetCoolColor(r, g, b);
    this->ShaderProgramShadow->SetCoolColor(r, g, b);
}

void vtkFiberMapper::SetWarmColor(float r, float g, float b)
{
    this->ShaderProgram->SetWarmColor(r, g, b);
    this->ShaderProgramShadow->SetWarmColor(r, g, b);
}

void vtkFiberMapper::Initialize(vtkRenderer *r)
{
  this->Initialized = false;

    // Not in opengl2 vbackend anymore, but since we are sure we use opengl2, probably useless to do this
//  vtkOpenGLExtensionManager *extensions=static_cast<vtkOpenGLRenderWindow *>(
//    r->GetRenderWindow())->GetExtensionManager();

//  int supports_GL_VERSION_2_0			= extensions->ExtensionSupported("GL_VERSION_2_0");
//  int supports_GL_EXT_framebuffer_object	= extensions->ExtensionSupported("GL_EXT_framebuffer_object");

//  if (supports_GL_VERSION_2_0)
//    {
//    extensions->LoadExtension("GL_VERSION_2_0");
//    }
//  else
//    {
//    vtkWarningMacro(<<"GL_VERSION_2_0 not supported!");
//    return;
//    }

//  if (supports_GL_EXT_framebuffer_object)
//    {
//    extensions->LoadExtension("GL_EXT_framebuffer_object");
//    }
//  else
//    {
//    vtkWarningMacro(<<"GL_EXT_framebuffer_object not supported!");
//    return;
//    }

  this->Initialized = true;
}

bool vtkFiberMapper::IsRenderSupported(vtkRenderer *r)
{
  if (!this->Initialized)
    {
    this->Initialize(r);
    }

  return this->Initialized;
}

void vtkFiberMapper::SetAmbientContribution(float contribution)
{
  this->ShaderProgram->SetAmbientContribution(contribution);
  this->ShaderProgramShadow->SetAmbientContribution(contribution);
}

void vtkFiberMapper::SetDiffuseContribution(float contribution)
{
  this->ShaderProgram->SetDiffuseContribution(contribution);
  this->ShaderProgramShadow->SetDiffuseContribution(contribution);
}

void vtkFiberMapper::SetSpecularContribution(float contribution)
{
  this->ShaderProgram->SetSpecularContribution(contribution);
  this->ShaderProgramShadow->SetSpecularContribution(contribution);
}

void vtkFiberMapper::SetSpecularPower(float power)
{
  this->ShaderProgram->SetSpecularPower(power);
  this->ShaderProgramShadow->SetSpecularPower(power);
}

void vtkFiberMapper::SetAmbientContributionShadow(float contribution)
{
  this->ShaderProgramShadow->SetAmbientContributionShadow(contribution);
}

void vtkFiberMapper::SetDiffuseContributionShadow(float contribution)
{
  this->ShaderProgramShadow->SetDiffuseContributionShadow(contribution);
}

void vtkFiberMapper::SetSpecularContributionShadow(float contribution)
{
  this->ShaderProgramShadow->SetSpecularContributionShadow(contribution);
}

//void vtkFiberMapper::SetSpecularPowerShadow(float power)
//{
//  this->ShaderProgramShadow->SetSpecularPowerShadow(power);
//}

float vtkFiberMapper::GetAmbientContribution()
{
  return this->ShaderProgram->GetAmbientContribution();
}

float vtkFiberMapper::GetDiffuseContribution()
{
  return this->ShaderProgram->GetDiffuseContribution();
}

float vtkFiberMapper::GetSpecularContribution()
{
  return this->ShaderProgram->GetSpecularContribution();
}

float vtkFiberMapper::GetSpecularPower()
{
  return this->ShaderProgram->GetSpecularPower();
}

float vtkFiberMapper::GetAmbientContributionShadow()
{
  return this->ShaderProgramShadow->GetAmbientContributionShadow();
}

float vtkFiberMapper::GetDiffuseContributionShadow()
{
  return this->ShaderProgramShadow->GetDiffuseContributionShadow();
}

float vtkFiberMapper::GetSpecularContributionShadow()
{
  return this->ShaderProgramShadow->GetSpecularContributionShadow();
}

//float vtkFiberMapper::GetSpecularPowerShadow()
//{
//  return this->ShaderProgramShadow->GetSpecularPowerShadow();
//}

