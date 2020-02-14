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
 * vtkAnisoLiShadowMap.cxx
 * by Tim Peeters
 *
 * 2005-09-13	Tim Peeters
 * - First version
 *
 * 2005-10-19	Tim Peeters
 * - Because this class is now a subclass of vtkAnisotropicLightingSP,
 *   functions and variables already in that class were removed here.
 *   It is almost empty now. Except for the two lines in the constructor.
 */

#include "vtkAnisoLiShadowMapSP.h"
#include "AnisotropicLightingShadowMappingVertexText.h"
#include "AnisotropicLightingShadowMappingFragmentText.h"

#include "vtkVertexShader.h"
#include "vtkFragmentShader.h"

#include "vtkUniform.h"

#include <vtkObjectFactory.h>

vtkStandardNewMacro(vtkAnisoLiShadowMapSP);

vtkAnisoLiShadowMapSP::vtkAnisoLiShadowMapSP()
{
  this->VertexShader->SetSourceText(AnisotropicLightingShadowMappingVertexText);
  this->FragmentShader->SetSourceText(AnisotropicLightingShadowMappingFragmentText);

  this->AmbientContributionShadow = 0.5*this->AmbientContribution;
  this->DiffuseContributionShadow = 0.5*this->DiffuseContribution;
  this->SpecularContributionShadow = 0.0;

  this->AmbientContributionShadowUniform = vtkUniform<float>::New();
  this->AmbientContributionShadowUniform->SetName("AmbientContributionShadow");
  this->AmbientContributionShadowUniform->SetValue(this->AmbientContributionShadow);
  this->AddShaderUniform(this->AmbientContributionShadowUniform);

  this->DiffuseContributionShadowUniform = vtkUniform<float>::New();
  this->DiffuseContributionShadowUniform->SetName("DiffuseContributionShadow");
  this->DiffuseContributionShadowUniform->SetValue(this->DiffuseContributionShadow);
  this->AddShaderUniform(this->DiffuseContributionShadowUniform);

  this->SpecularContributionShadowUniform = vtkUniform<float>::New();
  this->SpecularContributionShadowUniform->SetName("SpecularContributionShadow");
  this->SpecularContributionShadowUniform->SetValue(this->SpecularContributionShadow);
  this->AddShaderUniform(this->SpecularContributionShadowUniform);
}

vtkAnisoLiShadowMapSP::~vtkAnisoLiShadowMapSP()
{
    this->AmbientContributionShadowUniform->Delete();
    this->AmbientContributionShadowUniform = nullptr;
    this->DiffuseContributionShadowUniform->Delete();
    this->DiffuseContributionShadowUniform = nullptr;
    this->SpecularContributionShadowUniform->Delete();
    this->SpecularContributionShadowUniform = nullptr;
}

void vtkAnisoLiShadowMapSP::SetAmbientContributionShadow(float contribution)
{
  vtkDebugMacro(<<"Setting ambient contribution shadow to "<<contribution);
  if (contribution != this->AmbientContributionShadow)
    {
    this->AmbientContributionShadow = contribution;
    this->AmbientContributionShadowUniform->SetValue(this->AmbientContributionShadow);
    if (this->IsLinked())
      {
//      this->SetGlUniform(this->AmbientContributionShadowUniform);
      }
    // this->Modified();
  } // if
}

void vtkAnisoLiShadowMapSP::SetDiffuseContributionShadow(float contribution)
{
  vtkDebugMacro(<<"Setting diffuse contribution shadow to "<<contribution);
  if (contribution != this->DiffuseContributionShadow)
    {
    this->DiffuseContributionShadow = contribution;
    this->DiffuseContributionShadowUniform->SetValue(this->DiffuseContributionShadow);
    if (this->IsLinked())
      {
//      this->SetGlUniform(this->DiffuseContributionShadowUniform);
      }
    //this->Modified();
    }
}

void vtkAnisoLiShadowMapSP::SetSpecularContributionShadow(float contribution)
{
  vtkDebugMacro(<<"Setting specular contribution shadow to "<<contribution);
  if (contribution != this->SpecularContributionShadow)
    {
    this->SpecularContributionShadow = contribution;
    this->SpecularContributionShadowUniform->SetValue(this->SpecularContributionShadow);
    if (this->IsLinked())
      {
//      this->SetGlUniform(this->SpecularContributionShadowUniform);
      }
    //this->Modified();
    }
}

