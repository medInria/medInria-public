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

#include <cstdlib>
#include <cmath>

#include "vtkINRIA3DConfigure.h"

#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkPointLocator.h>

namespace Visualization {

/*! http://davidf.faricy.net/polyhedra/Platonic_Solids.html */
enum p_solid { cube,          // 0
               dodecahedron,  // 1
               icosahedron,   // 2
               octahedron,    // 3
               tetrahedron }; // 4

/*! Perform tesselation of the unit sphere from a given platonic solid */
template<typename T>
class VTK_VISUMANAGEMENT_EXPORT vtkSphereTesselator {
public:

    vtkSphereTesselator();
    vtkSphereTesselator(const p_solid& ip);
    //vtkSphereTesselator(const vtkSphereTesselator<T>& st);

    virtual ~vtkSphereTesselator();

    vtkSphereTesselator<T>& operator= (const vtkSphereTesselator<T>& st);

    void enableCClockWise(const bool& flag);
    void enableVerbosity(const bool& flag);

    /** Specify the R number of times that the selected polyhedron is going to be tesselated  */
    void tesselate(const int& R);

    /** Set in the vtkPolyData the vtkPoints and vtkCellArray that define the tesellated polyhedron   */
    void getvtkTesselation(vtkPolyData* t);

private:
    p_solid         m_initPolyhedra;
    bool            m_cclockwise;
    vtkPoints* m_vertices;
    vtkCellArray* m_triangles;

    /** Initialize by using the m_initPolyhedra the polyhedron that it is gong to be tesselated  */
    void m_initializeTesselation();

    //    void m_flipOrientation(tesselation<T>* t);
    bool m_verbose;
};
}
#endif

