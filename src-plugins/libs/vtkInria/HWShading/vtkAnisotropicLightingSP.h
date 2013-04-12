/*============================================================================

Program:   vtkINRIA3D
Module:    $Id: HWShading 480 2007-11-21 08:48:21Z $
Language:  C++
Author:    $Author: Tim Peeters $
Date:      $Date: 2007-11-21 09:48:21 +0100 (Wed, 21 Nov 2007) $
Version:   $Revision: 480 $

The Hardware Shading (HWShading) module of vtkINRIA3D is protected by the
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
 * vtkAnisotropicLightingSP.h
 * by Tim Peeters
 *
 * 2005-06-28	Tim Peeters
 * - First version
 *
 * 2006-12-26	Tim Peeters
 * - Add support for tone shading
 */

#pragma once

#include "vtkHWShadingExport.h"

#include "vtkBMIAShaderProgram.h"

class vtkUniformFloat;
class vtkUniformBool;
class vtkUniformVec3;
class vtkVertexShader;
class vtkFragmentShader;

/**
 * Shader program that applies anisotropic lighting realistic rendering
 * of lines. To be used by special OpenGL mappers that render lines.
 * Instead of passing a normal to OpenGL using glNormal(), glNormal() must
 * be used to pass the tangent direction of the line for each vertex.
 * The shader program will then itself compute the appropriate normal for
 * the given light(s) and eye positions and locations.
 * Currently only one light source is supported.
 */
class VTK_HWSHADING_EXPORT vtkAnisotropicLightingSP : public vtkBMIAShaderProgram {

public:
  static vtkAnisotropicLightingSP *New();
  vtkTypeRevisionMacro(vtkAnisotropicLightingSP, vtkBMIAShaderProgram);
  /**
   * Get/Set the diffuse and specular contributions to the final lighting of
   * the line. Values must be between 0 and 1.
   * Note: VTK usually uses doubles, but in OpenGL 2.0 the uniforms for
   * shaders are floats, so floats are also used here.
   */
  void SetDiffuseContribution(float contribution);
  float GetDiffuseContribution() { return this->DiffuseContribution; }
  void SetSpecularContribution(float contribution);
  float GetSpecularContribution() { return this->SpecularContribution; }
  void SetAmbientContribution(float contribution);
  float GetAmbientContribution() { return this->AmbientContribution; }

  /**
   * Get/Set the specular power component. Can be used to generate sharper
   * or less-sharp highlights.
   * Note: VTK usually uses doubles, but in OpenGL 2.0 the uniforms for
   * shaders are floats, so floats are also used here.
   */
  void SetSpecularPower(float power);
  float GetSpecularPower() { return this->SpecularPower; }

  void SetRGBColoring(bool coloring);
  bool GetRGBColoring() { return this->RGBColoring; }

  void SetToneShading(bool tone);
  bool GetToneShading() { return this->ToneShading; }
  void SetWarmColor(double* rgb);
  void SetWarmColor(double red, double green, double blue);
  void SetCoolColor(double* rgb);
  void SetCoolColor(double red, double green, double blue);
  void GetWarmColor(double rgb[3]);
  void GetCoolColor(double rgb[3]);

protected:
  vtkAnisotropicLightingSP();
  ~vtkAnisotropicLightingSP();

  float SpecularPower;
  float DiffuseContribution;
  float SpecularContribution;
  float AmbientContribution;

  vtkUniformFloat* SpecularPowerUniform;
  vtkUniformFloat* DiffuseContributionUniform;
  vtkUniformFloat* SpecularContributionUniform;
  vtkUniformFloat* AmbientContributionUniform;

  /**
   * RGB encoding of local direction.
   */
  bool RGBColoring;
  vtkUniformBool* RGBColoringUniform;

  bool ToneShading;
  vtkUniformBool* ToneShadingUniform;
  vtkUniformVec3* WarmColorUniform;
  vtkUniformVec3* CoolColorUniform;

  vtkVertexShader* VertexShader;
  vtkFragmentShader* FragmentShader;
  vtkFragmentShader* ShaderFunctions;

private:

}; // class vtkAnisotropicLightingSP


