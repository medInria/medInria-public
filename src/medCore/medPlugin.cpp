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

QString medPlugin::contact() const
{
    return "<music-userfeedback@inria.fr>";
}

QString medPlugin::description() const
{
    return "Copyright (c) 2015, IHU Liryc, Université de Bordeaux and Inria. All rights reserved.";
}

QStringList medPlugin::authors() const
{
    return sortByLastName(authorFullNames());
}

QStringList medPlugin::contributors() const
{
    return sortByLastName(contributorFullNames());
}

QStringList medPlugin::sortByLastName(QStringList names) const
{
    qSort(names.begin(), names.end(), nameLessThan);
    return names;
}

bool medPlugin::nameLessThan(const QString& string1, const QString& string2)
{
    return string1.split(" ").last() < string2.split(" ").last();
}
