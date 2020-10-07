/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medPluginLegacy.h>

medPluginLegacy::medPluginLegacy(QObject* parent) : dtkPlugin(parent)
{
}

bool medPluginLegacy::uninitialize()
{
    return true;
}

QString medPluginLegacy::contact() const
{
    return "";
}

QString medPluginLegacy::description() const
{
    return "";
}

QString medPluginLegacy::identifier() const
{
    return metaObject()->className();
}

QString medPluginLegacy::version() const
{
    return "";
}

QStringList medPluginLegacy::authors() const
{
    return QStringList();
}

QStringList medPluginLegacy::contributors() const
{
    return QStringList();
}

QStringList medPluginLegacy::dependencies() const
{
    return QStringList();
}

QStringList medPluginLegacy::tags() const
{
    return QStringList();
}

QStringList medPluginLegacy::types() const
{
    return QStringList();
}
