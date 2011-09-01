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

#include "itkFilters.h"
#include "itkFiltersPlugin.h"
#include "itkFiltersToolBox.h"

#include <dtkCore/dtkLog.h>

class itkFiltersPluginPrivate
{
public:
};

itkFiltersPlugin::itkFiltersPlugin ( QObject* parent ) : dtkPlugin ( parent ), d ( new itkFiltersPluginPrivate )
{

}

itkFiltersPlugin::~itkFiltersPlugin ( void )
{
    delete d;

    d = NULL;
}

bool itkFiltersPlugin::initialize ( void )
{
    if ( !itkFilters::registered() )
        dtkWarning() << "Unable to register itkFilters type";

    if ( !itkFiltersToolBox::registered() )
        dtkWarning() << "Unable to register itkFilters toolbox";

    return true;
}

bool itkFiltersPlugin::uninitialize ( void )
{
    return true;
}

QString itkFiltersPlugin::description ( void ) const
{
    return "ITK Filters";
}

QString itkFiltersPlugin::name ( void ) const
{
    return "itkFiltersPlugin";
}

QStringList itkFiltersPlugin::tags ( void ) const
{
    return QStringList() << "ITK" << "process" << "preprocessing";
}

QStringList itkFiltersPlugin::types ( void ) const
{
    return QStringList() << "itkFilters";
}

Q_EXPORT_PLUGIN2 ( itkFiltersPlugin, itkFiltersPlugin )
