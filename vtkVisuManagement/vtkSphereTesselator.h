//  Authors:	   Maxime Descoteaux, Jaime Garcia Guevara, Theodore Papadopoulo.
//
//  Description:  source of one spherical mesh.
//  Define the mesh and associated value for each direction
//  Deform the mesh after
//
//   Copyright (c) 2007-2011, INRIA Sophia Antipolis, France, groups Odyssee, Athena.
//   Please see the copyright notice included in this distribution for full details.

#ifndef vtkSphereTesselator_H
#define vtkSphereTesselator_H
#include "vtkINRIA3DConfigure.h"

#include <cmath>

#include <vtkPolyDataAlgorithm.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPointLocator.h>

/** http://davidf.faricy.net/polyhedra/Platonic_Solids.html */


/** \Clas Object
* \brief Perform tesselation of the unit sphere from a given platonic solid
*/
template<typename T>
class VTK_VISUMANAGEMENT_EXPORT vtkSphereTesselator : public vtkPolyDataAlgorithm {
public:
    enum {
        cube=0,
        dodecahedron = 1,
        icosahedron = 2,
        octahedron = 3,
        tetrahedron = 4
    };

    vtkSphereTesselator();
    vtkSphereTesselator(const int ip);

    virtual ~vtkSphereTesselator();

    vtkSphereTesselator<T>& operator= (const vtkSphereTesselator<T>& st);

    /** Specify the R number of times that the selected polyhedron is going to be tesselated  */
    void tesselate(const int& R);

    /** Set in the vtkPolyData the vtkPoints and vtkCellArray that define the tesellated polyhedron  */
    void getvtkTesselation(vtkPolyData* t);



private:
    int         m_initPolyhedra;
    vtkPoints* m_vertices;
    vtkCellArray* m_triangles;

    /** Initialize by using the m_initPolyhedra the polyhedron that it is gong to be tesselated  */
    void m_initializeTesselation();

};

#endif

