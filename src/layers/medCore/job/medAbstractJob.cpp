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
#include <medIntParameter.h>

class medAbstractJobPrivate
{
public:
    bool running;
    medIntParameter *progression;
};

medAbstractJob::medAbstractJob(QObject *parent)
    : QObject(parent), d(new medAbstractJobPrivate)
{
    d->running = false;
    d->progression = new medIntParameter("job_progression", this);
    d->progression->setRange(0, 100);
    d->progression->setValue(0);

    medJobManager::instance()->registerJob(this);

    connect(this, &medAbstractJob::running,
            this, &medAbstractJob::_setIsRunning);
    connect(this, &medAbstractJob::finished,
            this, &medAbstractJob::_resetProgression);
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

medIntParameter* medAbstractJob::progression() const
{
    return d->progression;
}

void medAbstractJob::_resetProgression()
{
    d->progression->setValue(0);
}
