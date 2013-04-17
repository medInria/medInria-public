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
 * vtkShadowRenderer.h
 * by Tim Peeters
 *
 * 2005-07-19	Tim Peeters
 * - First version
 *
 * 2005-08-08	Tim Peeters
 * - Renamed ShowShadowMap() to DrawShadowMap().
 * - Added bool ShowShadowMap variable and functions to get/set it.
 *
 * 2006-01-30	Tim Peeters
 * - Removed #include <GL/glew.h>
 */

#pragma once

#include "vtkHWShadingExport.h"

#include <vtkOpenGLRenderer.h>
//#include <GL/glew.h>

#include <vtkTimeStamp.h>

class vtkCamera;

//class vtkShadowMappingSP;
class vtkBMIAShaderProgram;
class vtkUniformSampler;
class vtkShadowMappingHelper;

/**
 * Class for rendering scenes with shadowing.
 * WARNING: glewInit() must be called after the render window was
 * initialized and before this renderer renders.
 */
class VTK_HWSHADING_EXPORT vtkShadowRenderer : public vtkOpenGLRenderer {

public:
  static vtkShadowRenderer* New();
  vtkTypeRevisionMacro(vtkShadowRenderer, vtkOpenGLRenderer);

  /**
   * Get/Set the rendering of shadows by this renderer.
   */
  vtkSetMacro(RenderShadows, bool);
  vtkGetMacro(RenderShadows, bool);
  vtkBooleanMacro(RenderShadows, bool);
  void SetShadows(bool shadows) { this->SetRenderShadows(shadows); };
  bool GetShadows() { return this->GetRenderShadows(); };
  vtkBooleanMacro(Shadows, bool);

  /**
   * Specifies whether the shadow map must be drawn on the screen or
   * the scene itself. This option only has an effect if RenderShadows
   * is true. If RenderShadows is false, the scene is always drawn with
   * no shadows, and no shadowmap is generated or displayed.
   */
  vtkSetMacro(ShowShadowMap, bool);
  vtkGetMacro(ShowShadowMap, bool);
  vtkBooleanMacro(ShowShadowMap, bool);

  // TODO: make this protected? it's protected in vtkShadowRenderer
  // from Sandia.. why?
  virtual void DeviceRender();

protected:
  vtkShadowRenderer();
  ~vtkShadowRenderer();

  
private:
  /**
   * Helper for rendering to the shadow map and creating shadow mapping
   * texture.
   */
  vtkShadowMappingHelper* ShadowMappingHelper;

  /**
   * Specifies whether shadows must be rendered or not.
   */
  bool RenderShadows;

  /**
   * Initialize texture for storing&reading shadow map.
   */
  void InitializeShadowMap();

  /**
   * Render to the shadow map from the light.
   */
  void RegenerateShadowMap();

  /**
   * Renders the scene using the shadow map for shadows.
   */
  void RenderWithShadows();

  /**
   * Display the shadow map.
   */
  void DrawShadowMap();

  /**
   * Shader program used for the shadow mapping.
   */
//  vtkShadowMappingSP* ShadowMappingSP;
  vtkBMIAShaderProgram* ShadowMappingSP;

  /**
   * The last time the shadow map was (re)generated.
   */
  vtkTimeStamp ShadowMapMTime;

  virtual unsigned long GetShadowMapMTime();

  /**
   * When true, the shadowmap is shown by the DeviceRender() function
   * instead of the scene itself.
   */
  bool ShowShadowMap;

};


