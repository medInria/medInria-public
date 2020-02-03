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
 * vtkShadowMappingSP.cxx
 * by Tim Peeters
 *
 * 2005-07-26	Tim Peeters
 * - First version
 *
 * 2005-09-12	Tim Peeters
 * - Included new shader that contains lighting calculation functions used
 *   in the fragment shader.
 */

#include "vtkShadowMappingSP.h"
#include "ShadowMappingVertexText.h"
#include "ShadowMappingFragmentText.h"
#include "SpotlightFunctionsText.h"

#include "vtkVertexShader.h"
#include "vtkFragmentShader.h"

#include <vtkObjectFactory.h>

vtkStandardNewMacro(vtkShadowMappingSP)

vtkShadowMappingSP::vtkShadowMappingSP()
{
  this->VertexShader = vtkVertexShader::New();
  this->FragmentShader = vtkFragmentShader::New();
  this->SpotlightFuncShader = vtkFragmentShader::New();

  this->VertexShader->SetSourceText(ShadowMappingVertexText);
  this->FragmentShader->SetSourceText(ShadowMappingFragmentText);
  this->SpotlightFuncShader->SetSourceText(SpotlightFunctionsText);

  this->AddShaderObject(this->VertexShader);
  this->AddShaderObject(this->SpotlightFuncShader);
  this->AddShaderObject(this->FragmentShader);
}

vtkShadowMappingSP::~vtkShadowMappingSP()
{
  this->VertexShader->Delete(); this->VertexShader = nullptr;
  this->FragmentShader->Delete(); this->FragmentShader = nullptr;
  this->SpotlightFuncShader->Delete(); this->SpotlightFuncShader = nullptr;
}

