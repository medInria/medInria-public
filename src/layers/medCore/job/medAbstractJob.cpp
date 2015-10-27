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

    connect(this, &medAbstractJob::running,
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

void medAbstractJob::_setIsRunning(bool isRunning)
{
    d->running = isRunning;
}
