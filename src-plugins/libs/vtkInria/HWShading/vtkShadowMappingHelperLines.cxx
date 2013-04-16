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
 * vtkShadowMappingHelperLines.cxx
 * by Tim Peeters
 *
 * 2005-11-22	Tim Peeters
 * - First version
 */

#include "vtkShadowMappingHelperLines.h"
#include <vtkObjectFactory.h>

#include "vtkBMIAShaderProgram.h"
#include "vtkFragmentShader.h"
#include "vtkVertexShader.h"

#include "BuildShadowMapLinesVertexText.h"
#include "BuildShadowMapLinesFragmentText.h"

vtkCxxRevisionMacro(vtkShadowMappingHelperLines, "$Revision: 540 $");
vtkStandardNewMacro(vtkShadowMappingHelperLines);

vtkShadowMappingHelperLines::vtkShadowMappingHelperLines()
{
  vtkBMIAShaderProgram* prog = vtkBMIAShaderProgram::New();
  vtkVertexShader* vert = vtkVertexShader::New();
  vert->SetSourceText(BuildShadowMapLinesVertexText);
  vtkFragmentShader* frag = vtkFragmentShader::New();
  frag->SetSourceText(BuildShadowMapLinesFragmentText);
  prog->AddShaderObject(vert);
  prog->AddShaderObject(frag);
  vert->Delete(); vert = NULL;
  frag->Delete(); frag = NULL;
  this->SetShaderProgram(prog);
  prog->Delete(); prog = NULL;
}

vtkShadowMappingHelperLines::~vtkShadowMappingHelperLines()
{
  // nothing to do.
}

