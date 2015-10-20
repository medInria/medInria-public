/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractJob.h>

#include <medJobManager.h>

class medAbstractJobPrivate
{
public:
    bool running;
};

medAbstractJob::medAbstractJob(QObject *parent)
    : QObject(parent), d(new medAbstractJobPrivate)
{
    d->running = false;
    medJobManager::instance()->registerJob(this);

    // Mandatory to be runned by a QThreadPool, since this inherits from QObject
    // It can recieve signals trigerring slots after having be deleted by the QThreadPool
    // if setAutoDelete is set to true (default value).
    this->setAutoDelete(false);

    connect(this, &medAbstractJob::success,
            this, &medAbstractJob::_emitNotRunning);
    connect(this, &medAbstractJob::failure,
            this, &medAbstractJob::_emitNotRunning);
    connect(this, &medAbstractJob::_running,
            this, &medAbstractJob::_setIsRunning);
}

medAbstractJob::~medAbstractJob()
{
    medJobManager::instance()->unregisterJob(this);
}

bool medAbstractJob::isRunning() const
{
    return d->running;
}

void medAbstractJob::_emitNotRunning()
{
    emit _running(false);
}

void medAbstractJob::_setIsRunning(bool isRunning)
{
    d->running = isRunning;
}
