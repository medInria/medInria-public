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


#ifndef ITKFILTERSPLUGIN_H
#define ITKFILTERSPLUGIN_H

#include <dtkCore/dtkPlugin.h>

#include "itkFiltersPluginExport.h"

class ITKFILTERSPLUGIN_EXPORT itkFiltersPluginPrivate;

class ITKFILTERSPLUGIN_EXPORT itkFiltersPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
    itkFiltersPlugin(QObject *parent = 0);
    ~itkFiltersPlugin(void);

    virtual bool initialize(void);
    virtual bool uninitialize(void);

    virtual QString contact(void) const;
    virtual QString description(void) const;
    virtual QString name(void) const;
    virtual QString version(void) const;
    virtual QStringList authors(void) const;
    virtual QStringList contributors(void) const;
    virtual QStringList dependencies(void) const;

    virtual QStringList tags(void) const;
    virtual QStringList types(void) const;

private:
    itkFiltersPluginPrivate *d;
};

#endif // ITKFILTERSPLUGIN_H
