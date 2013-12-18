/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once
#include "medVtkInriaExport.h"

#include <vtkPolyDataAlgorithm.h>

/** http://davidf.faricy.net/polyhedra/Platonic_Solids.html */

/** \Clas Object
* \brief Perform tesselation of the unit sphere from a given platonic solid
*/

class MEDVTKINRIA_EXPORT vtkTessellatedSphereSource : public vtkPolyDataAlgorithm {
public:
  static vtkTessellatedSphereSource *New();
  vtkTypeMacro(vtkTessellatedSphereSource,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the Resolution number of times that the selected polyhedron is going to be tesselated.
  vtkSetMacro(Resolution,unsigned int);
  vtkGetMacro(Resolution,unsigned int);

  // Description:
  // Set the type of polyhedron is going to be tesselated.
  vtkSetMacro(PolyhedraType,unsigned int);
  vtkGetMacro(PolyhedraType,unsigned int);


protected:
  vtkTessellatedSphereSource();
  vtkTessellatedSphereSource(const int ip);
  /*virtual*/ ~vtkTessellatedSphereSource(){}


  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  unsigned int Resolution;
  unsigned int PolyhedraType;
  enum {
      icosahedron,
      dodecahedron,
      octahedron,
      cube,
      tetrahedron
  };


private:

  vtkTessellatedSphereSource& operator= (const vtkTessellatedSphereSource& st); // Not implemented
  vtkTessellatedSphereSource(const vtkTessellatedSphereSource&);  // Not implemented.

  /** Initialize by using the m_PolyhedraType the polyhedron that it is gong to be tesselated  */
  void initializeTesselation(vtkPoints* vertices, vtkCellArray* triangles);
};



