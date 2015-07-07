/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractProcessPlugin.h>

class medAsbtractProcessPluginPrivate
{
public:
    QStringList tags;
};

medAsbtractProcessPlugin::medAsbtractProcessPlugin(QObject *parent): medAbstractPlugin(parent),
    d(new medAsbtractProcessPluginPrivate)
{

}

medAsbtractProcessPlugin::~medAsbtractProcessPlugin()
{
    delete d;
}

void medAsbtractProcessPlugin::addTags(QStringList tags)
{
    d->tags << tags;
}

QStringList medAsbtractProcessPlugin::tags() const
{
    return d->tags;
}
