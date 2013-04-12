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
 * vtkUniformSampler.h
 *
 * 2005-05-18	Tim Peeters
 * - First version
 */

#pragma once

#include "vtkHWShadingExport.h"

#include "vtkUniformInt.h"

/**
 * Class for representing uniform sampler variables.
 * Samplers are used to pass the name/index of a texture to use to a shader.
 *
 * Same implementation as vtkUniformInt. Here, there is no distinction between
 * sampler1D, sampler2D, sampler3D, samplerCube, sampler1DShadow or
 * sampler2DShadow. Note that the corresponding texture must be of the
 * type matching the sampler type.
 *
 * This class may be modified in the future to also handle the loading of the
 * texture and/or check for restrictions imposed on samplers.
 */
class VTK_HWSHADING_EXPORT vtkUniformSampler : public vtkUniformInt
{
public:
  static vtkUniformSampler* New();
  vtkTypeRevisionMacro(vtkUniformSampler, vtkUniformInt);

protected:
  vtkUniformSampler();
  ~vtkUniformSampler();

private:

};


