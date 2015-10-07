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
 * vtkShaderObjectCollection.h
 * by Tim Peeters
 *
 * 2005-05-04	Tim Peeters	First version
 */

#pragma once

#include <medVtkInriaExport.h>

#include <vtkCollection.h>
#include <vtkShaderObject.h> // for inline methods/static casts

class MEDVTKINRIA_EXPORT vtkShaderObjectCollection : public vtkCollection
{
public:
  static vtkShaderObjectCollection* New();
  vtkTypeMacro(vtkShaderObjectCollection, vtkCollection);
  
  /**
   * Add a ShaderObject to the list
   */
  void AddItem(vtkShaderObject* so)
    {
    this->vtkCollection::AddItem((vtkObject*)so);
    };

  /**
   * Get the next ShaderObject in the list. Return NULL when at the end of the 
   * list.
   */
  vtkShaderObject *GetNextItem()
    {
    return static_cast<vtkShaderObject *>(this->GetNextItemAsObject());
    };

  /**
   * Get the ith shader object from the list.
   */
  vtkShaderObject* GetItem(int i)
    {
    return static_cast<vtkShaderObject *>(this->GetItemAsObject(i));
    };

protected:
  vtkShaderObjectCollection() {};
  ~vtkShaderObjectCollection() {};

private:
  // hide the standard AddItem from the user and the compiler.
  void AddItem(vtkObject *o) { this->vtkCollection::AddItem(o); };

};


