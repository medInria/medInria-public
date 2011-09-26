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

#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <iostream>
#include <fstream>
#include <cstdlib>

#include "SphericalFunctionSHVisualizationExport.h"

namespace Visualization {

template<typename T>
class SPHERICALFUNCTIONSHVISUALIZATION_EXPORT vertex {

    friend std::ostream& operator<<(std::ostream& os, const vertex<T>& v) {
        os << "<" << v.x << ", " << v.y << ", " << v.z << ">" << std::endl;
        return os;
    }

public:
    vertex(void);
    vertex(const T& a, const T& b, const T& c);
    vertex(const vertex<T>& v);
    virtual ~vertex();

    vertex<T>& operator= (const vertex<T>& v);
    T x,y,z;
};
}
#endif
