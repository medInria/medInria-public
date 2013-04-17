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
 * vtkFragmentShader.h
 * by Tim Peeters
 *
 * 2005-05-03	Tim Peeters
 * -First version
 *
 * 2005-06-06	Tim Peeters
 * - Renamed glCreateObject() to CreateGlShader()
 */

#pragma once

#include "vtkHWShadingExport.h"

#include "vtkShaderObject.h"

/**
 * GLSL Fragment Shader Object
 */
class VTK_HWSHADING_EXPORT vtkFragmentShader : public vtkShaderObject
{
public:
  static vtkFragmentShader* New();
  vtkTypeRevisionMacro (vtkFragmentShader, vtkShaderObject);

protected:
  vtkFragmentShader();
  ~vtkFragmentShader();

  virtual bool CreateGlShader();

private:

};


