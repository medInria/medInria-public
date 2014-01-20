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
 * vtkAnisoLiShadowMapSP.h
 * by Tim Peeters
 *
 * 2005-09-13	Tim Peeters
 * - First version
 *
 * 2005-10-19	Tim Peeters
 * - Made this class a subclass of vtkAnisotropicLightingSP.
 * - Remove functions already in vtkAnisotropicLightingSP.
 *
 * 2005-12-08	Tim Peeters
 * - Added support for parameters {Ambient,Diffuse,Specular}ContributionShadow.
 */

#pragma once

//#include "vtkBMIAShaderProgram.h"
#include "vtkAnisotropicLightingSP.h"

#include "medVtkInriaExport.h"

/**
 * Shader program that combines anisotropic lighting and shadow mapping.
 */
class MEDVTKINRIA_EXPORT vtkAnisoLiShadowMapSP : public vtkAnisotropicLightingSP {

public:
  static vtkAnisoLiShadowMapSP *New();
  vtkTypeRevisionMacro(vtkAnisoLiShadowMapSP, vtkAnisotropicLightingSP);

  void SetDiffuseContributionShadow(float contribution);
  float GetDiffuseContributionShadow() { return this->DiffuseContributionShadow; }
  void SetSpecularContributionShadow(float contribution);
  float GetSpecularContributionShadow() { return this->SpecularContributionShadow; }
  void SetAmbientContributionShadow(float contribution);
  float GetAmbientContributionShadow() { return this->AmbientContributionShadow; }

protected:
  vtkAnisoLiShadowMapSP();
  ~vtkAnisoLiShadowMapSP();

  float AmbientContributionShadow;
  float DiffuseContributionShadow;
  float SpecularContributionShadow;

  vtkUniform<float>* AmbientContributionShadowUniform;
  vtkUniform<float>* DiffuseContributionShadowUniform;
  vtkUniform<float>* SpecularContributionShadowUniform;

private:

}; // class vtkAnisotropicLightingSP


