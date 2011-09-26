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

#ifndef TESSELATION_HPP
#define TESSELATION_HPP

#include "triangle.hpp"

#include "SphericalFunctionSHVisualizationExport.h"

namespace Visualization {

template<typename T>
class SPHERICALFUNCTIONSHVISUALIZATION_EXPORT tesselation {

    friend std::ostream& operator<<(std::ostream& os, const tesselation<T>& tess) {
        os << tess.ntri << " triangles" << std::endl;
        os << "counterclockwise: " << tess.cc << std::endl;
        for (int i=0; i<tess.ntri; i++)
            os << tess.tr[i] << std::endl;
        return os;
    }

public:
    tesselation();
    tesselation(const int& n);
    tesselation(const tesselation<T>& tess);

    virtual ~tesselation();

    tesselation<T>& operator= (const tesselation<T>& tess);

    int ntri;
    bool cc;
    triangle<T>* tr;
};
}
#endif
