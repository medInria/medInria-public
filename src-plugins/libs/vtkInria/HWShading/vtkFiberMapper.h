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
 * vtkFiberMapper.h
 * by Tim Peeters
 *
 * 2005-06-22	Tim Peeters
 * - First version
 *
 * 2005-06-29	Tim Peeters
 * - Made GetShaderProgram() public so that parameters can be set directly
 *   on the shader program.
 *
 * 2005-07-18	Tim Peeters
 * - Renamed class from vtkOpenGLFiberMapper to vtkFiberMapper.
 *
 * 2005-09-13	Tim Peeters
 * - Private variable ShaderProgram is now of type
 *   vtkAnisoLiShadowMapSP instead of vtkAnisotropicLightingSP
 * - Added private ShadowMappingHelper variable.
 * - Added RegenerateShadowMap()
 *
 * 2005-10-19	Tim Peeters
 * - Added variable bool Shadowing and functions SetShadowing(),
 *   ShadowingOn() and ShadowingOff() so that it is possible to switch off
 *   (and on) shadows.
 *
 * 2006-02-22	Tim Peeters
 * - Added Initialize() function and Initialized variable.
 * - Added GetInitialized() function.
 * - Added IsRenderSupported() function.
 *
 * 2006-03-05	Tim Peeters
 * - Removed GetShaderProgram() and GetShaderProgramShadowS() functions.
 * - Added functions for setting lighting parameters. This could first
 *   be done by getting the shader programs of this mapper and then
 *   setting the parameters of those shading programs. This is no
 *   longer supported.
 *
 * 2006-12-26	Tim Peeters
 * - Add support for tone shading.
 */

#pragma once

#include "vtkHWShadingExport.h"

#include <vtkOpenGLPolyDataMapper.h>

class vtkRenderer;
class vtkActor;
class vtkPoints;
class vtkCellArray;
class vtkRenderer;

class vtkAnisotropicLightingSP;
class vtkAnisoLiShadowMapSP;
class vtkBMIAShaderProgram;
class vtkShadowMappingHelper;

/**
 * Renders fibers in a better way.
 * Inherits from vtkOpenGLPolyDataMapper and overrides Draw() to
 * do the drawing of lines in a "better" way.
 * TODO: describe in *which* way.
 */
class VTK_HWSHADING_EXPORT vtkFiberMapper : public vtkOpenGLPolyDataMapper
{
public:
  static vtkFiberMapper *New();
  vtkTypeRevisionMacro(vtkFiberMapper, vtkOpenGLPolyDataMapper);

  // Description:
  // Draw method for OpenGL.
  //virtual int Draw(vtkRenderer *ren, vtkActor *a);
  virtual void Render(vtkRenderer *ren, vtkActor *a);

  /**
   * Enable/Disable anisotropic lighting for the lines. If anisotropic
   * lighting is disabled, all lines will be drawn without any lighting.
   * Switching lighting off also disables shadowing!
   */
  void SetLighting(bool light);
  vtkGetMacro(Lighting, bool);
  vtkBooleanMacro(Lighting, bool);

  /**
   * Enable/Disable shadowing.
   */
  void SetShadowing(bool render_shadows);
  vtkGetMacro(Shadowing, bool);
  vtkBooleanMacro(Shadowing, bool);

  /**
   * Enable/Disable tone shading.
   */
  void SetToneShading(bool tone_shading);
  vtkGetMacro(ToneShading, bool);
  vtkBooleanMacro(ToneShading, bool);

  void SetCoolColor(float r, float g, float b);
  void SetWarmColor(float r, float g, float b);

  /**
   * Set the thickness in pixels in the shadow map of a shadow
   * cast by a fiber.
   */
  vtkSetMacro(ShadowLineWidth, float);
  vtkGetMacro(ShadowLineWidth, float);

//  vtkGetObjectMacro(ShaderProgram, vtkAnisotropicLightingSP);
//  vtkGetObjectMacro(ShaderProgramShadow, vtkAnisoLiShadowMapSP);

  /**
   * Set/Get the (anisotropic) lighting parameters.
   */
  void SetAmbientContribution(float contribution);
  void SetDiffuseContribution(float contribution);
  void SetSpecularContribution(float contribution);
  void SetSpecularPower(float power);
  void SetAmbientContributionShadow(float contribution);
  void SetDiffuseContributionShadow(float contribution);
  void SetSpecularContributionShadow(float contribution);
//  void SetSpecularPowerShadow(float power);
  float GetAmbientContribution();
  float GetDiffuseContribution();
  float GetSpecularContribution();
  float GetSpecularPower();
  float GetAmbientContributionShadow();
  float GetDiffuseContributionShadow();
  float GetSpecularContributionShadow();
//  float GetSpecularPowerShadow();

  void SetRGBColoring(bool coloring);
  vtkGetMacro(RGBColoring, bool);
  vtkBooleanMacro(RGBColoring, bool);

  vtkGetMacro(Initialized, bool);

  /**
   * Returns true if the OpenGL extensions required by this mapper are
   * available, and false otherwise.
   */
  bool IsRenderSupported(vtkRenderer *r);

protected:
  vtkFiberMapper();
  ~vtkFiberMapper();

  float ShadowLineWidth;

  /**
   * Draw linestrips.
   *
   * @param points The points in the linestrips.
   * @param lineStrips The linestrips to draw.
   * @param ren The renderer to use for checking the abort status of the render window.
   * @param noAbort Abort status.
   */
  virtual void DrawLines(vtkPoints* points, vtkCellArray* lineStrips,
                         vtkRenderer* ren, int &noAbort);

  bool Initialized;
  void Initialize(vtkRenderer *r);

private:

  /**
   * Specifies whether shadowing must be enabled or disabled.
   */
  bool Shadowing;

  vtkAnisotropicLightingSP* ShaderProgram;
  vtkAnisoLiShadowMapSP* ShaderProgramShadow;

  /**
   * True if anisotropic lighting must be enabled, and false otherwise.
   */
  bool Lighting;

  /**
   * Specifies whether tone shading must be enabled or disabled.
   */
  bool ToneShading;

  bool RGBColoring;

  /**
   * Helps with the rendering of the shadow map.
   */
  vtkShadowMappingHelper* ShadowMappingHelper;

  /**
   * Render the scene to the shadow map.
   */
  //void RegenerateShadowMap();
  virtual void RegenerateShadowMap(vtkPoints* points, vtkCellArray* lineStrips,
			vtkRenderer* ren, int &noAbort);

  void DrawShadowMap(); //GLfloat windowWidth, GLfloat windowHeight);

}; // class vtkFiberMapper



