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
    QStringList tags;
};

medAbstractProcess::medAbstractProcess(QObject *parent): QObject(parent),
    d(new medAbstractProcessPrivate)
{
    // Mandatory to be runned by a QThreadPool, since this inherits from QObject
    // It can recieve signals trigerring slots after having be deleted by the QThreadPool
    // if setAutoDelete is set to true (default value).
    this->setAutoDelete(false);
}

medAbstractProcess::~medAbstractProcess()
{
    delete d;
}

void medAbstractProcess::addParameter(medAbstractParameter *parameter)
{
    d->parameters.insert(parameter->name(), parameter);
}

medAbstractParameter* medAbstractProcess::parameter(QString name) const
{
    return d->parameters[name];
}

QList<medAbstractParameter*> medAbstractProcess::parameters() const
{
    return d->parameters.values();
}

void medAbstractProcess::addTags(QStringList tags)
{
    d->tags << tags;
}

QStringList medAbstractProcess::tags() const
{
    return d->tags;
}

QPushButton* medAbstractProcess::runButton() const
{
    QPushButton *runButton = new QPushButton(tr("Run"));
    connect(runButton, &QPushButton::clicked,
            this, &medAbstractProcess::runInThread);

    return runButton;
}

void medAbstractProcess::runInThread()
{
    QThreadPool::globalInstance()->start(this);
}
