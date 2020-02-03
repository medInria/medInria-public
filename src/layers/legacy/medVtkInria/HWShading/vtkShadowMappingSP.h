#pragma once
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
 * vtkShadowMappingSP.h
 * by Tim Peeters
 *
 * 2005-07-26	Tim Peeters
 * - First version
 */

#include <medVtkInriaExport.h>

#include <vtkBMIAShaderProgram.h>

class vtkVertexShader;
class vtkFragmentShader;

/**
 * Shader program that renders a scene with shadows. A shadow map must
 * have been generated and supplied to this shader program.
 */
class MEDVTKINRIA_EXPORT vtkShadowMappingSP : public vtkBMIAShaderProgram
{
public:
  static vtkShadowMappingSP* New();
  vtkTypeMacro(vtkShadowMappingSP, vtkBMIAShaderProgram)

protected:
  vtkShadowMappingSP();
  ~vtkShadowMappingSP();

private:
  vtkVertexShader* VertexShader;
  vtkFragmentShader* FragmentShader;
  vtkFragmentShader* SpotlightFuncShader;

}; // class vtkShadowMappingSP
