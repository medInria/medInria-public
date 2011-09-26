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

#ifndef SPHERETESSELATOR_HPP
#define SPHERETESSELATOR_HPP

#include "tesselation.hpp"
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkPointLocator.h>
#include <vtkCleanPolyData.h>
#include "defs.hpp"

#include "SphericalFunctionSHVisualizationExport.h"

namespace Visualization {

/*! http://davidf.faricy.net/polyhedra/Platonic_Solids.html */
  enum p_solid { cube,          // 0
		 dodecahedron,  // 1 
		 icosahedron,   // 2
		 octahedron,    // 3
		 tetrahedron }; // 4

/*! Perform tesselation of the unit sphere from a given platonic solid */
template<typename T>
class SPHERICALFUNCTIONSHVISUALIZATION_EXPORT sphereTesselator {
public:

    sphereTesselator();
    sphereTesselator(const p_solid& ip);
    //sphereTesselator(const sphereTesselator<T>& st);

    virtual ~sphereTesselator();

    sphereTesselator<T>& operator= (const sphereTesselator<T>& st);

    void enableCClockWise(const bool& flag);
    void enableVerbosity(const bool& flag);
    void tesselate(const int& l);
    tesselation<T> getTesselation();
    void getvtkTesselation(const bool& clean, vtkPolyData* t);

private:
    p_solid         m_initPolyhedra;
    tesselation<T>  m_tesselation;
    bool            m_cclockwise;

    vtkCleanPolyData* m_pDataCleaner;

    void m_initializeTesselation(const p_solid& ip);
    void m_flipOrientation(tesselation<T>* t);
    vertex<T> m_normalize(const vertex<T>& v);
    vertex<T> m_middle(const vertex<T>& v1, const vertex<T>& v2);
    bool m_verbose;
};
}
#endif

