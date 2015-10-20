/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractProcess.h>

#include <QMap>
#include <QThreadPool>
#include <QPushButton>

#include <medAbstractParameter.h>

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

void medAbstractProcess::regsiterParameter(medAbstractParameter * parameter)
{
    parameter->setParent(this);
    d->parameters.insert(parameter->name(), parameter);
}

medAbstractParameter* medAbstractProcess::parameter(QString const& name) const
{
    return d->parameters[name];
}

QList<medAbstractParameter*> medAbstractProcess::parameters() const
{
    return d->parameters.values();
}
