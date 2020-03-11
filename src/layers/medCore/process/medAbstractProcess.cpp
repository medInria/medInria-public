/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameter.h>
#include <medAbstractProcess.h>

#include <QMap>

class medAbstractProcessPrivate
{
public:
    QMap<QString, medAbstractParameter*> parameters;
};

medAbstractProcess::medAbstractProcess(QObject *parent)
    : medAbstractJob(parent), d(new medAbstractProcessPrivate)
{

}

medAbstractProcess::~medAbstractProcess()
{
}

void medAbstractProcess::registerParameter(medAbstractParameter * parameter)
{
    parameter->setParent(this);
    d->parameters.insert(parameter->id(), parameter);
}

medAbstractParameter* medAbstractProcess::parameter(QString const& id) const
{
    return d->parameters[id];
}

QList<medAbstractParameter*> medAbstractProcess::parameters() const
{
    return d->parameters.values();
}
