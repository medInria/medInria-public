/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Alexandre <email>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef ITKFILTERS_H
#define ITKFILTERS_H

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractData.h>

#include "itkFiltersPluginExport.h"

class itkFiltersPrivate;
class dtkAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFilters : public dtkAbstractProcess
{
    Q_OBJECT

public:
  enum FILTER
  {
    ADD, MULTIPLY, DIVIDE, GAUSSIAN, NORMALIZE, MEDIAN, INVERT, SHRINK, INTENSITY
  };
  
    itkFilters ( void );
    virtual ~itkFilters ( void );

    virtual QString description ( void ) const;

    static bool registered ( void );

public slots:

    void setInput ( dtkAbstractData *data );
    void setParameter ( double  data, int channel );
    int update ( void );
    dtkAbstractData *output ( void );

private:
    itkFiltersPrivate *d;
};

dtkAbstractProcess *createitkFilters ( void );
#endif // ITKFILTERS_H
