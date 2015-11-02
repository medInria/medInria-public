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
 * vtkShaderBase.cxx
 * by Tim Peeters
 *
 * 2005-05-03	Tim Peeters
 * - First version
 *
 * 2005-05-17	Tim Peeters
 * - Removed most implementations because they were moved to
 *   vktShaderBaseHandle subclass.
 *
 * 2005-07-14	Tim Peeters
 * - Added SupportsOpenGLVersion() function.
 */

#include "vtkShaderBase.h"
#include <vtkObjectFactory.h>


vtkStandardNewMacro(vtkShaderBase);

vtkShaderBase::vtkShaderBase()
{
  // nothing
}

vtkShaderBase::~vtkShaderBase()
{
  // nothing was created by this class, so don't destroy anything either.
  // do that in the subclasses that were creating things.
}

// from http://developer.nvidia.com/object/nv_ogl2_support.html
bool vtkShaderBase::SupportsOpenGLVersion(int atLeastMajor, int atLeastMinor)
{
  const char* version;
  int major, minor;

  //glewInit();
  version = (const char *) glGetString(GL_VERSION);
  cout<<"OpenGL version is "<<version<<endl;
  //vtkDebugMacro(<<"OpenGL version is "<<version);

  if (sscanf(version, "%d.%d", &major, &minor) == 2) {
  if (major > atLeastMajor)
    return true;
  if (major == atLeastMajor && minor >= atLeastMinor)
    return true;
  } else {
    /* OpenGL version string malformed! */
  }
  return false;
}
