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

#include "tesselation.hpp"

namespace Visualization {

template<typename T>
tesselation<T>::tesselation() {
    this->ntri = 0;
    this->cc   = false;
    this->tr   = NULL;
}

template<typename T>
tesselation<T>::tesselation(const int& n) {
    this->ntri = n;
    this->cc   = false;
    this->tr   = new triangle<T>[n];
}

template<typename T>
tesselation<T>::tesselation(const tesselation<T>& tess) {
    this->ntri = tess.ntri;
    this->cc   = tess.cc;
    this->tr   = new triangle<T>[this->ntri];
    for(int i=0; i<this->ntri; i++)
        this->tr[i] = tess.tr[i];
}

template<typename T>
tesselation<T>::~tesselation() {
    if (this->tr != NULL)
        delete [] this->tr;
}

template<typename T>
tesselation<T>& tesselation<T>::operator= (const tesselation<T>& tess) {
    this->ntri = tess.ntri;
    this->cc   = tess.cc;
    if (this->tr != NULL)
        delete [] this->tr;
    this->tr   = new triangle<T>[this->ntri];
    for(int i=0; i<this->ntri; i++)
        this->tr[i] = tess.tr[i];
    return *this;
}

template class tesselation<float>;
template class tesselation<double>;
}
