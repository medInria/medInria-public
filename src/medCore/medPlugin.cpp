/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medPlugin.h>

medPlugin::medPlugin(QObject* parent) : dtkPlugin(parent)
{
}

bool medPlugin::uninitialize()
{
    return true;
}

QString medPlugin::contact() const
{
    return "For any feedback <music-userfeedback@inria.fr>";
}

QString medPlugin::description() const
{
    return "";
}

QString medPlugin::identifier() const
{
    return "";
}

QString medPlugin::version() const
{
    return "";
}

QStringList medPlugin::authors() const
{
    return QStringList() << "MUSIC team";
}

QStringList medPlugin::contributors() const
{
    return QStringList() << "MUSIC team";
}

QStringList medPlugin::dependencies() const
{
    return QStringList();
}

QStringList medPlugin::tags() const
{
    return QStringList();
}

QStringList medPlugin::types() const
{
    return QStringList();
}
