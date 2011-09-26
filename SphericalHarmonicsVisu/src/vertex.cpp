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

#include "vertex.hpp"

namespace Visualization {

template<typename T>
vertex<T>::vertex(void) {}

template<typename T>
vertex<T>::vertex(const T& a, const T& b, const T& c) {
    this->x = a;
    this->y = b;
    this->z = c;
}

template<typename T>
vertex<T>::vertex(const vertex<T>& v) {
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
}

template<typename T>
vertex<T>::~vertex() {}

template<typename T>
vertex<T>& vertex<T>::operator= (const vertex<T>& v) {
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
    return *this;
}

template class vertex<float>;
template class vertex<double>;
}
