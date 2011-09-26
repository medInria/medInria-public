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

#include "sphereTesselator.hpp"

#ifdef WIN32
#pragma warning(disable:4661)
#endif

namespace Visualization {

template<typename T>
sphereTesselator<T>::sphereTesselator() {
    m_initPolyhedra = icosahedron;
    m_cclockwise = false;
    m_verbose = false;
    m_pDataCleaner = vtkCleanPolyData::New();
    m_initializeTesselation(m_initPolyhedra);
}

template<typename T>
sphereTesselator<T>::sphereTesselator(const p_solid& ip) {
  m_initPolyhedra = ip;
  m_cclockwise = false;
  m_verbose = false;
  m_pDataCleaner = vtkCleanPolyData::New();
  m_initializeTesselation(m_initPolyhedra);
}

//  template<typename T>
//  sphereTesselator<T>::sphereTesselator(const sphereTesselator<T>& st) {
//      m_initPolyhedra = st.m_initPolyhedra;
//      m_cclockwise    = st.m_cclockwise;
//      m_verbose = st.m_verbose;
//      m_tesselation   = st.m_tesselation;
//      m_pDataCleaner = vtkCleanPolyData::New();
//      m_pDataCleaner->ShallowCopy(st.m_pDataCleaner);
//  }

template<typename T>
sphereTesselator<T>::~sphereTesselator() {
    m_pDataCleaner->Delete();
}

template<typename T>
void sphereTesselator<T>::enableCClockWise(const bool& flag) {
    m_cclockwise = flag;
}

template<typename T>
void sphereTesselator<T>::enableVerbosity(const bool& flag) {
    m_verbose = flag;
}

template<typename T>
void sphereTesselator<T>::m_initializeTesselation(const p_solid& ip) {
    switch( ip ) {
    case icosahedron: {
            T tau = 0.850650808352039932;
            T one = 0.525731112119133606;

            /* 12 vertices */
            vertex<T> za( tau,  one,  0.0);
            vertex<T> zb(-tau,  one,  0.0);
            vertex<T> zc(-tau, -one,  0.0);
            vertex<T> zd( tau, -one,  0.0);
            vertex<T> ya( one,  0.0,  tau);
            vertex<T> yb( one,  0.0, -tau);
            vertex<T> yc(-one,  0.0, -tau);
            vertex<T> yd(-one,  0.0,  tau);
            vertex<T> xa( 0.0,  tau,  one);
            vertex<T> xb( 0.0, -tau,  one);
            vertex<T> xc( 0.0, -tau, -one);
            vertex<T> xd( 0.0,  tau, -one);

            /* Create Icosahedron */
            m_tesselation.ntri = 20;
            m_tesselation.cc   = m_cclockwise;
            m_tesselation.tr   = new triangle<T>[m_tesselation.ntri];

            m_tesselation.tr[0] = triangle<T>(ya, xa, yd);
            m_tesselation.tr[1] = triangle<T>(ya, yd, xb);
            m_tesselation.tr[2] = triangle<T>(yb, yc, xd);
            m_tesselation.tr[3] = triangle<T>(yb, xc, yc);
            m_tesselation.tr[4] = triangle<T>(za, ya, zd);
            m_tesselation.tr[5] = triangle<T>(za, zd, yb);
            m_tesselation.tr[6] = triangle<T>(zc, yd, zb);
            m_tesselation.tr[7] = triangle<T>(zc, zb, yc);
            m_tesselation.tr[8] = triangle<T>(xa, za, xd);
            m_tesselation.tr[9] = triangle<T>(xa, xd, zb);
            m_tesselation.tr[10] = triangle<T>(xb, xc, zd);
            m_tesselation.tr[11] = triangle<T>(xb, zc, xc);
            m_tesselation.tr[12] = triangle<T>(xa, ya, za);
            m_tesselation.tr[13] = triangle<T>(xd, za, yb);
            m_tesselation.tr[14] = triangle<T>(ya, xb, zd);
            m_tesselation.tr[15] = triangle<T>(yb, zd, xc);
            m_tesselation.tr[16] = triangle<T>(yd, xa, zb);
            m_tesselation.tr[17] = triangle<T>(yc, zb, xd);
            m_tesselation.tr[18] = triangle<T>(yd, zc, xb);
            m_tesselation.tr[19] = triangle<T>(yc, xc, zc);
            break;
        }
    case octahedron: {
            /* 6 vertices */
            vertex<T> xplus(1.0, 0.0, 0.0);
            vertex<T> xmin(-1.0, 0.0, 0.0);
            vertex<T> yplus(0.0, 1.0, 0.0);
            vertex<T> ymin( 0.0,-1.0, 0.0);
            vertex<T> zplus(0.0, 0.0, 1.0);
            vertex<T> zmin( 0.0, 0.0, -1.0);

            /* Create Octahedron */
            m_tesselation.ntri = 8;
            m_tesselation.cc   = m_cclockwise;
            m_tesselation.tr   = new triangle<T>[m_tesselation.ntri];

            m_tesselation.tr[0] = triangle<T>(xplus, zplus, yplus);
            m_tesselation.tr[1] = triangle<T>(yplus, zplus, xmin );
            m_tesselation.tr[2] = triangle<T>(xmin , zplus, ymin );
            m_tesselation.tr[3] = triangle<T>(ymin , zplus, xplus);
            m_tesselation.tr[4] = triangle<T>(xplus, yplus, zmin );
            m_tesselation.tr[5] = triangle<T>(yplus, xmin , zmin );
            m_tesselation.tr[6] = triangle<T>(xmin , ymin , zmin );
            m_tesselation.tr[7] = triangle<T>(ymin , xplus, zmin );
            break;
        }
    case tetrahedron: {
            T sqrt_3 = 0.5773502692;

            /* 4 vertices */
            vertex<T> ppp( sqrt_3,  sqrt_3,  sqrt_3);
            vertex<T> mmp(-sqrt_3, -sqrt_3,  sqrt_3);
            vertex<T> mpm(-sqrt_3,  sqrt_3, -sqrt_3);
            vertex<T> pmm( sqrt_3, -sqrt_3, -sqrt_3);

            /* Create Tetrahedron */
            m_tesselation.ntri = 4;
            m_tesselation.cc   = m_cclockwise;
            m_tesselation.tr   = new triangle<T>[m_tesselation.ntri];

            m_tesselation.tr[0] = triangle<T>(ppp, mmp, mpm);
            m_tesselation.tr[1] = triangle<T>(ppp, pmm, mmp);
            m_tesselation.tr[2] = triangle<T>(mpm, mmp, pmm);
            m_tesselation.tr[3] = triangle<T>(ppp, mpm, pmm);
            break;
        }
    default: {
            std::cerr << "Unsupported Platonic Solid (switching to icosahedron)" << std::endl;
            m_initPolyhedra = icosahedron;
            T tau = 0.850650808352039932;
            T one = 0.525731112119133606;

            /* 12 vertices */
            vertex<T> za( tau,  one,  0.0);
            vertex<T> zb(-tau,  one,  0.0);
            vertex<T> zc(-tau, -one,  0.0);
            vertex<T> zd( tau, -one,  0.0);
            vertex<T> ya( one,  0.0,  tau);
            vertex<T> yb( one,  0.0, -tau);
            vertex<T> yc(-one,  0.0, -tau);
            vertex<T> yd(-one,  0.0,  tau);
            vertex<T> xa( 0.0,  tau,  one);
            vertex<T> xb( 0.0, -tau,  one);
            vertex<T> xc( 0.0, -tau, -one);
            vertex<T> xd( 0.0,  tau, -one);

            /* Create Icosahedron */
            m_tesselation.ntri = 20;
            m_tesselation.cc   = m_cclockwise;
            m_tesselation.tr   = new triangle<T>[m_tesselation.ntri];

            m_tesselation.tr[0] = triangle<T>(ya, xa, yd);
            m_tesselation.tr[1] = triangle<T>(ya, yd, xb);
            m_tesselation.tr[2] = triangle<T>(yb, yc, xd);
            m_tesselation.tr[3] = triangle<T>(yb, xc, yc);
            m_tesselation.tr[4] = triangle<T>(za, ya, zd);
            m_tesselation.tr[5] = triangle<T>(za, zd, yb);
            m_tesselation.tr[6] = triangle<T>(zc, yd, zb);
            m_tesselation.tr[7] = triangle<T>(zc, zb, yc);
            m_tesselation.tr[8] = triangle<T>(xa, za, xd);
            m_tesselation.tr[9] = triangle<T>(xa, xd, zb);
            m_tesselation.tr[10] = triangle<T>(xb, xc, zd);
            m_tesselation.tr[11] = triangle<T>(xb, zc, xc);
            m_tesselation.tr[12] = triangle<T>(xa, ya, za);
            m_tesselation.tr[13] = triangle<T>(xd, za, yb);
            m_tesselation.tr[14] = triangle<T>(ya, xb, zd);
            m_tesselation.tr[15] = triangle<T>(yb, zd, xc);
            m_tesselation.tr[16] = triangle<T>(yd, xa, zb);
            m_tesselation.tr[17] = triangle<T>(yc, zb, xd);
            m_tesselation.tr[18] = triangle<T>(yd, zc, xb);
            m_tesselation.tr[19] = triangle<T>(yc, xc, zc);
            break;
        }
    }
}

template<typename T>
vertex<T> sphereTesselator<T>::m_middle(const vertex<T>& v1, const vertex<T>& v2) {
    vertex<T> r;
    r.x = 0.5*(v1.x + v2.x);
    r.y = 0.5*(v1.y + v2.y);
    r.z = 0.5*(v1.z + v2.z);
    return r;
}

template<typename T>
vertex<T> sphereTesselator<T>::m_normalize(const vertex<T>& v) {
    vertex<T> r;
    r = v;
    T norm = r.x * r.x + r.y * r.y + r.z * r.z;
    if (norm != 0.0) {
        norm = 1.0/sqrt(norm);
        r.x *= norm;
        r.y *= norm;
        r.z *= norm;
    }
    return r;
}

template<typename T>
void sphereTesselator<T>::m_flipOrientation(tesselation<T>* t) {
    vertex<T> tmp;
    for(int i=0; i<t->ntri; i++) {
        tmp = t->tr[i].v[0];
        t->tr[i].v[0] = t->tr[i].v[2];
        t->tr[i].v[2] = tmp;
    }
}

template<typename T>
void sphereTesselator<T>::tesselate(const int& l) {
    tesselation<T>* newTess = NULL;
	
    if (m_cclockwise) {
        m_flipOrientation(&m_tesselation);
        m_tesselation.cc = true;
    }

    for (int level = 2; level <= l; level++) {
        /* Allocate a new tesselation */
        newTess = new tesselation<T>;
        if (newTess == NULL) {
            std::cerr << "sphereTesselator<T>::m_tesselate(): Out of memory on subdivision level " << level << std::endl;
            exit(1);
        }
        newTess->ntri = m_tesselation.ntri * 4;
        /* Allocate 4* the number of points in the current tesselation */
        newTess->tr = new triangle<T>[newTess->ntri];
        if (newTess->tr == NULL) {
            std::cerr << "sphereTesselator<T>::m_tesselate(): Out of memory on subdivision level " << level << std::endl;
            exit(1);
        }

        /* Subdivide each triangle in the old approximation and normalize
         * the new points thus generated to lie on the surface of the unit
         * sphere.
         * Each input triangle with vertices labelled [0,1,2] as shown
         * below will be turned into four new triangles:
         *              * Make new points
         *              a = (0+2)/2
         *              b = (0+1)/2
         *              c = (1+2)/2
         *      1
         *       /\         Normalize a, b, c
         *     /   \
         *  b/ __\ c    Construct new triangles
         *   /\    /\       [0,b,a]
         *  /  \  /  \      [b,1,c]
         * /___\/__\    [a,b,c]
         *   0  a   2   [a,c,2]
        */

        for (int i = 0; i < m_tesselation.ntri; i++) {
            triangle<T> oldt = m_tesselation.tr[i];
            triangle<T>* newt = &(newTess->tr[i*4]);
            vertex<T> a, b, c;

            a = m_normalize(m_middle(oldt.v[0], oldt.v[2]));
            b = m_normalize(m_middle(oldt.v[0], oldt.v[1]));
            c = m_normalize(m_middle(oldt.v[1], oldt.v[2]));
            newt->v[0] = oldt.v[0];
            newt->v[1] = b;
            newt->v[2] = a;
            newt++;
            newt->v[0] = b;
            newt->v[1] = oldt.v[1];
            newt->v[2] = c;
            newt++;
            newt->v[0] = a;
            newt->v[1] = b;
            newt->v[2] = c;
            newt++;
            newt->v[0] = a;
            newt->v[1] = c;
            newt->v[2] = oldt.v[2];
        }

        m_tesselation = (tesselation<T>)(*newTess);
        delete newTess;
    }
}

template<typename T>
tesselation<T> sphereTesselator<T>::getTesselation() {
    return m_tesselation;
}

template<typename T>
void sphereTesselator<T>::getvtkTesselation(const bool& clean, vtkPolyData* t) {
    /* Create a vtkPolyData from the raw tesselation (ie. w. many duplicated vertices) */
    /* and clean it to generate minimal number of vertices */
    vertex<T> v;
    triangle<T> tr;
    int vidx = 0;
    vtkIdType* vi = new vtkIdType[3];
    vtkPolyData* sphere = vtkPolyData::New();
    vtkPoints* vertices = vtkPoints::New();
    vtkCellArray* poly  = vtkCellArray::New();

    if (m_verbose)
        std::cout << "*** Vertices ***" << std::endl;

    for (int i=0; i<m_tesselation.ntri; i++) { // each triangle
        for(int j=0; j<3; j++) {               // each vertex
            v = m_tesselation.tr[i].v[j];
            if (v.x == 0 && v.y == 0) {
                vertices->InsertPoint(vidx,v.x,v.y - M_EPS,v.z);
            }
			else {
				vertices->InsertPoint(vidx,v.x,v.y,v.z);
			}
            vi[j] = vidx++;
        }

		if (m_verbose)
			std::cout << v.x << " " << v.y << " " << v.z << std::endl;

		poly->InsertNextCell((vtkIdType)3,vi);
    }

    if (m_verbose) {
        std::cout << std::endl << "*** Triangles ***" << std::endl;

        for (int i=0; i<m_tesselation.ntri; i++) {
            tr = m_tesselation.tr[i];
            std::cout << tr.v[0].x << " " << tr.v[0].y << " " << tr.v[0].z << std::endl;
            std::cout << tr.v[1].x << " " << tr.v[1].y << " " << tr.v[1].z << std::endl;
            std::cout << tr.v[2].x << " " << tr.v[2].y << " " << tr.v[2].z << std::endl << std::endl;
        }
    }

    delete [] vi;
    sphere->SetPoints(vertices);
	vertices->Delete();
    sphere->SetPolys(poly);
    poly->Delete();

    // Now clean the polydata
    if (clean) {
        m_pDataCleaner->SetInput(sphere);
        m_pDataCleaner->PointMergingOn();
        m_pDataCleaner->ConvertStripsToPolysOn();
        m_pDataCleaner->ConvertPolysToLinesOn();
        m_pDataCleaner->ConvertLinesToPointsOn();
        m_pDataCleaner->SetAbsoluteTolerance(0.0);
        m_pDataCleaner->Update();
        t->ShallowCopy(m_pDataCleaner->GetOutput());
        sphere->Delete();
    } else {
        t->ShallowCopy(sphere);
        sphere->Delete();
    }
}

template class sphereTesselator<float>;
template class sphereTesselator<double>;
}
