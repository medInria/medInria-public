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

#ifndef DEFS_HPP
#define DEFS_HPP

#include <iostream>
#include <fstream>

#include <algorithm>
#include <utility>
#include <vector>
#include <cfloat>
#include <cstdlib>
#include <string>
#include <cassert>
#include <complex>
#include <cmath>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#define M_EPS 1e-9

namespace Utils {

    struct direction { 
        double x,y,z; 
        double norm2() const { return (x*x+y*y+z*z);      }
        double norm()  const { return std::sqrt(norm2()); }
    };
}

#endif //!DEFS_HPP
