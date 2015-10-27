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
#include <medDoubleParameter.h>

class medAbstractJobPrivate
{
public:
    bool running;
    medDoubleParameter *progression;
};

medAbstractJob::medAbstractJob(QObject *parent)
    : QObject(parent), d(new medAbstractJobPrivate)
{
    d->running = false;
    d->progression = new medDoubleParameter("job_progression", this);
    d->progression->setRange(0, 100);
    d->progression->setValue(0);

//    d->progression->setCaption(this->caption());
//    d->progression->setDescription(this->caption() + " progression...");

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

medDoubleParameter* medAbstractJob::progression() const
{
    return d->progression;
}
