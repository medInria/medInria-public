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
 * vtkLookupTableCollection.h
 *
 * 2006-04-06	Tim Peeters
 * - First version
 */

#pragma once

#include <vtkCollection.h>
class vtkLookupTable;

class VTK_HELPERS_EXPORT vtkLookupTableCollection : public vtkCollection
{
public:
  static vtkLookupTableCollection* New();

  /**
   * Add a lookup table to the list
   */
  void AddItem(vtkLookupTable* lut);

  /**
   * Get the next lookup table from the list. NULL is returned when the
   * collection is exhausted.
   */
  vtkLookupTable* GetNextItem();

  /**
   * Get the ith lookup table in the list.
   */
  vtkLookupTable *GetItem(int i);

protected:
   vtkLookupTableCollection();
   ~vtkLookupTableCollection();

private:
  // hide the standard AddItem from the user and the compiler.
  void AddItem(vtkObject *o) { this->vtkCollection::AddItem(o); };

}; // class vtkLookupTableCollection


