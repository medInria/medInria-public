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
 * vtkShaderUniformCollection.h
 *
 * 2005-05-17	Tim Peeters
 * - First version
 */

#include <medVtkInriaExport.h>

#include <vtkCollection.h>
#include <vtkShaderUniform.h> // for inline methods/static casts

class MEDVTKINRIA_EXPORT vtkShaderUniformCollection : public vtkCollection
{
public:
  static vtkShaderUniformCollection* New();
  vtkTypeMacro (vtkShaderUniformCollection, vtkCollection)
  
  /**
   * Add a ShaderObject to the list
   */
  void AddItem(vtkShaderUniform* su)
    {
    this->vtkCollection::AddItem((vtkObject*)su);
    }

  /**
   * Get the next ShaderUniform in the list. Return null when at the end of the
   * list.
   */
  vtkShaderUniform *GetNextItem()
  {
      return static_cast<vtkShaderUniform *>(this->GetNextItemAsObject());
  }

  /**
   * Get the ith shader object from the list.
   */
  vtkShaderUniform* GetItem(int i)
    {
    return static_cast<vtkShaderUniform *>(this->GetItemAsObject(i));
    }

protected:
  vtkShaderUniformCollection() {}
  ~vtkShaderUniformCollection() {}

private:
  // hide the standard AddItem from the user and the compiler.
  void AddItem(vtkObject *o) { this->vtkCollection::AddItem(o); }
};
