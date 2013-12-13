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
 * vtkBMIAShaderProgramReader.h
 *
 * 2005-05-17	Tim Peeters
 * - First version
 */

#pragma once

#include "vtkHWShadingExport.h"

#include "vtkBetterDataReader.h"

#include <iostream>
using namespace std;

#include "vtkBMIAShaderProgram.h"


  //class vtkBMIAShaderProgram;

/**
 * Class shader programs.
 */
class VTK_HWSHADING_EXPORT vtkBMIAShaderProgramReader: public vtkBetterDataReader
{
public:
  static vtkBMIAShaderProgramReader *New();
  vtkTypeRevisionMacro (vtkBMIAShaderProgramReader, vtkBetterDataReader);

  /**
   * Set/Get the output of this reader.
   */
  void SetOutput(vtkBMIAShaderProgram* output);
  vtkGetObjectMacro(Output, vtkBMIAShaderProgram);

  /**
   * Read the shader from file. Always call this function before using the
   * output. This is not done automatically as in VTK readers that output
   * vtkDataObjects!
   */
  void Execute();

protected:
  vtkBMIAShaderProgramReader();
  ~vtkBMIAShaderProgramReader();

  void ReadUniformFromLine(string line, unsigned int linepos);

private:
  vtkBMIAShaderProgram* Output;

};


