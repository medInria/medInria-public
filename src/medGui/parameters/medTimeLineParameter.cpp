/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTimeLineParameter.h>

#include <math.h>       // floor

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QSlider>
#include <QSpinBox>
#include <QTimeLine>
#include <QDebug>

#include <dtkCore/dtkSignalBlocker.h>

#include <medIntParameter.h>
#include <medBoolParameter.h>
#include <medTriggerParameter.h>
#include <medDoubleParameter.h>


class medTimeLineParameterPrivate
{
public:
    QWidget* widget;

    medIntParameter* frameLineParameter;
    medDoubleParameter* timeParameter;
    medIntParameter* speedFactorParameter;
    medBoolParameter* playParameter;
    medBoolParameter* stopParameter;
    medTriggerParameter* nextFrameParameter;
    medTriggerParameter* previousFrameParameter;

    QTimeLine *timeLine;

    unsigned int numberOfFrame;
    double duration;
    double timeBetweenFrames;
    unsigned int stepFrame;

    QList <medAbstractParameter*> parametersCandidateToPool;

    ~medTimeLineParameterPrivate()
    {
        delete widget;
        delete frameLineParameter;
        delete speedFactorParameter;
        delete playParameter;
        delete nextFrameParameter;
        delete previousFrameParameter;
        delete timeParameter;
    }
};


medTimeLineParameter::medTimeLineParameter(QString name, QObject *parent):
    medAbstractGroupParameter(name, parent),
    d(new medTimeLineParameterPrivate)
{
    d->widget = NULL;


    d->speedFactorParameter = new medIntParameter(name);
    d->parametersCandidateToPool << d->speedFactorParameter;

    d->playParameter = new medBoolParameter(name);
    d->parametersCandidateToPool << d->playParameter;

    d->stopParameter = new medBoolParameter(name);
    d->parametersCandidateToPool << d->stopParameter;

    d->timeParameter = new medDoubleParameter(name);
    d->parametersCandidateToPool << d->timeParameter;

    d->frameLineParameter = new medIntParameter(name);
    d->nextFrameParameter = new medTriggerParameter(name);
    d->previousFrameParameter = new medTriggerParameter(name);

    d->timeLine = new QTimeLine(1000, this);
    d->timeLine->setLoopCount(0);
    d->timeLine->setCurveShape (QTimeLine::LinearCurve);

    this->clear();

    connect(d->timeLine, SIGNAL(frameChanged(int)), d->frameLineParameter, SIGNAL(valueChanged(int)));

    connect(d->frameLineParameter, SIGNAL(valueChanged(int)), this, SLOT(setFrame(int)));
    connect(d->playParameter, SIGNAL(valueChanged(bool)), this, SLOT(play(bool)));
    connect(d->stopParameter, SIGNAL(valueChanged(bool)), this, SLOT(stop(bool)));
    connect(d->previousFrameParameter, SIGNAL(triggered()), this, SLOT(previousFrame()));
    connect(d->nextFrameParameter, SIGNAL(triggered()), this, SLOT(nextFrame()));
    connect(d->speedFactorParameter, SIGNAL(valueChanged(int)), this, SLOT(setSpeedFactor(int)));
    connect(d->timeParameter, SIGNAL(valueChanged(double)), this, SLOT(setFrame(double)));
}

medTimeLineParameter::~medTimeLineParameter()
{
    delete d;
}

void medTimeLineParameter::clear()
{
    this->setSpeedFactor(1);
    this->setDuration(0.0);
    this->setNumberOfFrame(1);
    this->setFrame(0);
    this->setStepFrame(1);
    this->play(false);
    d->timeBetweenFrames = 0.0;
}

int medTimeLineParameter::speedFactor() const
{
    return d->speedFactorParameter->value();
}

bool medTimeLineParameter::isPlaying() const
{
    return d->playParameter->value();
}

int medTimeLineParameter::numberOfFrame() const
{
    return d->numberOfFrame;
}

int medTimeLineParameter::frame() const
{
    return d->frameLineParameter->value();
}

double& medTimeLineParameter::duration() const
{
    return d->duration;
}

int medTimeLineParameter::stepFrame() const
{
    return d->stepFrame;
}

int medTimeLineParameter::mapTimeToFrame(const double& time)
{
    return floor(time / d->timeBetweenFrames);
}

double medTimeLineParameter::mapFrameToTime (int frame)
{
    return frame * d->timeBetweenFrames;
}

void medTimeLineParameter::setSpeedFactor(int speedFactorParameter)
{
    d->speedFactorParameter->setValue(speedFactorParameter);
}

void medTimeLineParameter::play(bool play)
{
    d->playParameter->setValue(play);
    d->stopParameter->setValue(!play);

    if(d->timeLine->state() == QTimeLine::NotRunning && play)
    {
        d->timeLine->start();
        d->playParameter->setIcon (QPixmap(":/icons/pause.png"));
        emit playing(play);
    }
    else if(d->timeLine->state() == QTimeLine::Paused && play)
    {
        d->timeLine->resume();
        d->playParameter->setIcon (QPixmap(":/icons/pause.png"));
        emit playing(play);
    }
    else if(d->timeLine->state() == QTimeLine::Running && !play)
    {
        d->timeLine->setPaused(true);
        d->playParameter->setIcon (QPixmap(":/icons/play.png"));
    }
}

void medTimeLineParameter::stop(bool stop)
{
    play(!stop);
}

void medTimeLineParameter::setNumberOfFrame(int numberOfFrame)
{
    d->numberOfFrame = numberOfFrame;
    d->frameLineParameter->setRange(0, numberOfFrame);
    if(d->duration != 0)
        d->timeBetweenFrames = d->duration / d->numberOfFrame;
    else
        d->timeBetweenFrames = 0;

    d->timeLine->setFrameRange(0, d->numberOfFrame-1 );
}

void medTimeLineParameter::setStepFrame(int stepFrame)
{
    d->stepFrame = stepFrame;
}

void medTimeLineParameter::setDuration(const double& timeDuration)
{
    d->duration = timeDuration;
    if(d->duration != 0)
        d->timeBetweenFrames = d->duration / d->numberOfFrame;
    else
        d->timeBetweenFrames = 0;

    d->timeLine->setDuration(timeDuration*1000);
}

void medTimeLineParameter::setFrame(int frame)
{
    updateTimeParameter(this->mapFrameToTime(frame));
    updateFrameParameter(frame);
}

void medTimeLineParameter::updateFrameParameter(int frame)
{
    d->frameLineParameter->blockSignals(true);
    d->frameLineParameter->setValue(frame);
    d->frameLineParameter->blockSignals(false);
}

void medTimeLineParameter::setFrame(double time)
{
    updateTimeParameter(time);
    updateFrameParameter(this->mapTimeToFrame(time));
}

void medTimeLineParameter::updateTimeParameter(double time)
{
    d->timeParameter->blockSignals(true);
    d->timeParameter->setValue(time);
    d->timeParameter->blockSignals(false);
    emit frameChanged(time);
}

double medTimeLineParameter::time() const
{
    return d->timeParameter->value();
}

void medTimeLineParameter::previousFrame()
{
    this->setFrame(static_cast<int>(d->frameLineParameter->value() - d->stepFrame));
}

void medTimeLineParameter::nextFrame()
{
    this->setFrame(static_cast<int>(d->frameLineParameter->value() + d->stepFrame));
}

QList<medAbstractParameter*> medTimeLineParameter::parametersCandidateToPool() const
{
    return d->parametersCandidateToPool;
}

QWidget* medTimeLineParameter::getWidget()
{
    if(!d->widget)
    {
        d->widget = new QWidget;
        QVBoxLayout *widgetLayout = new QVBoxLayout(d->widget);
        QHBoxLayout *buttonsLayout = new QHBoxLayout;

        d->playParameter->setIcon(QIcon(":/icons/play.png"));
        d->stopParameter->setIcon(QIcon(":/icons/stop.png"));
        d->previousFrameParameter->getPushButton()->setIcon(QIcon(":/icons/backward.png"));
        d->nextFrameParameter->getPushButton()->setIcon(QIcon(":/icons/forward.png"));

        buttonsLayout->addWidget(d->playParameter->getPushButton());
        buttonsLayout->addWidget(d->previousFrameParameter->getPushButton());
        buttonsLayout->addWidget(d->nextFrameParameter->getPushButton());
        buttonsLayout->addWidget(d->speedFactorParameter->getSpinBox());

        widgetLayout->addLayout(buttonsLayout);
        d->frameLineParameter->getSlider()->setOrientation(Qt::Horizontal);
        widgetLayout->addWidget(d->frameLineParameter->getSlider());

        connect(d->widget, SIGNAL(destroyed()), this, SLOT(removeInternWidget()));
    }

    return d->widget;
}

void medTimeLineParameter::removeInternWidget()
{
    this->removeFromInternWidgets(d->widget);
    d->widget = NULL;
}

