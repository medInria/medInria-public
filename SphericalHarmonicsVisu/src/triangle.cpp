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

#include "triangle.hpp"

namespace Visualization {

template<typename T>
triangle<T>::triangle() {}

template<typename T>
triangle<T>::triangle(const vertex<T>& a, const vertex<T>& b, const vertex<T>& c) {
    this->v[0] = a;
    this->v[1] = b;
    this->v[2] = c;
}

template<typename T>
triangle<T>::triangle(const triangle<T>& t) {
    for (int i=0; i<3; i++)
        this->v[i] = t.v[i];
}

template<typename T>
triangle<T>::~triangle() {}

template<typename T>
triangle<T>& triangle<T>::operator= (const triangle<T>& t) {
    for (int i=0; i<3; i++)
        this->v[i] = t.v[i];
    return *this;
}

template class triangle<float>;
template class triangle<double>;
}
