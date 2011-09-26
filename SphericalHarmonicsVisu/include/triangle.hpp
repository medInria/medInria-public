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

#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "vertex.hpp"

#include "SphericalFunctionSHVisualizationExport.h"

namespace Visualization {

template<typename T>
class SPHERICALFUNCTIONSHVISUALIZATION_EXPORT triangle {

    friend std::ostream& operator<<(std::ostream& os, const triangle<T>& t) {
        os << t.v[0] << t.v[1] << t.v[2] << std::endl;
        return os;
    }

public:
    triangle();
    triangle(const vertex<T>& a, const vertex<T>& b, const vertex<T>& c);
    triangle(const triangle<T>& t);

    virtual ~triangle();

    triangle<T>& operator= (const triangle<T>& t);

    vertex<T> v[3];
};
}
#endif
