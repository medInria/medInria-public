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
 * vtkUniformBool.h
 *
 * 2005-05-17	Tim Peeters
 * - First version
 *
 * 2005-06-06	Tim Peeters
 * - Renamed glSpecificUniform() to SetGlUniformSpecific().
 */

#pragma once

#include <medVtkInriaExport.h>

#include <vtkShaderUniform.h>

/**
 * Class for representing uniform bool variables.
 */
class MEDVTKINRIA_EXPORT vtkUniformBool: public vtkShaderUniform
{
public:
  static vtkUniformBool* New();
  vtkTypeMacro(vtkUniformBool, vtkShaderUniform);

  vtkSetMacro(Value, bool);
  vtkGetMacro(Value, bool);
  vtkBooleanMacro(Value, bool);

protected:
  vtkUniformBool();
  ~vtkUniformBool();

  virtual void SetGlUniformSpecific();

private:
  bool Value;
};


