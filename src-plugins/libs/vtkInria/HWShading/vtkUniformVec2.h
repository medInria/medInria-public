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
 * vtkUniformVec2.h
 *
 * 2005-05-17	Tim Peeters
 * - First version
 */

#pragma once

#include "vtkHWShadingExport.h"

#include "vtkShaderUniform.h"

/**
 * Class for representing uniform vec2 variables.
 */
class VTK_HWSHADING_EXPORT vtkUniformVec2: public vtkShaderUniform
{
public:
  static vtkUniformVec2* New();
  vtkTypeRevisionMacro(vtkUniformVec2, vtkShaderUniform);

  vtkSetVector2Macro(Value, float);
  vtkGetVector2Macro(Value, float);

protected:
  vtkUniformVec2();
  ~vtkUniformVec2();

  virtual void SetGlUniformSpecific();

private:
  float Value[2];
};


