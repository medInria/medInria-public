/***************************************************************************
 *   Copyright (C) 2004 by Christophe Lenglet                              *
 *   clenglet@sophia.inria.fr                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef vtkSphereTesselator_HPP
#define vtkSphereTesselator_HPP

#include <defs.hpp>
#include <SphericalFunctionSHVisualizationExport.h>

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
class SPHERICALFUNCTIONSHVISUALIZATION_EXPORT vtkSphereTesselator {
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

