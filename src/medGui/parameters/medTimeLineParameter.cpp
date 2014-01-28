/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medTimeLineParameter.h"

#include <math.h>       // floor

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QSlider>
#include <QSpinBox>

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
    medDoubleParameter* momentParameter;
    medIntParameter* speedFactorParameter;
    medBoolParameter* playParameter;
    medTriggerParameter* nextFrameParameter;
    medTriggerParameter* previousFrameParameter;


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
        delete momentParameter;
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

    d->momentParameter = new medDoubleParameter(name);
    d->parametersCandidateToPool << d->momentParameter;

    d->frameLineParameter = new medIntParameter(name);
    d->nextFrameParameter = new medTriggerParameter(name);
    d->previousFrameParameter = new medTriggerParameter(name);

    this->clear();

    connect(d->frameLineParameter, SIGNAL(valueChanged(int)), this, SLOT(setFrame(int)));
    connect(d->playParameter, SIGNAL(valueChanged(bool)), this, SLOT(play(bool)));
    connect(d->previousFrameParameter, SIGNAL(triggered()), this, SLOT(previousFrame()));
    connect(d->nextFrameParameter, SIGNAL(triggered()), this, SLOT(nextFrame()));
    connect(d->speedFactorParameter, SIGNAL(valueChanged(int)), this, SLOT(setSpeedFactor(int)));
    connect(d->momentParameter, SIGNAL(valueChanged(double)), this, SLOT(setFrame(double)));
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

int medTimeLineParameter::mapMomentToFrame(const double& time)
{
    return floor(time / d->timeBetweenFrames);
}

double medTimeLineParameter::mapFrameToMoment (int frame)
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
    emit playing(play);
}

void medTimeLineParameter::setNumberOfFrame(int numberOfFrame)
{
    d->numberOfFrame = numberOfFrame;
    d->frameLineParameter->setRange(0, numberOfFrame);
    if(d->duration != 0)
        d->timeBetweenFrames = d->numberOfFrame / d->duration;
    else
        d->timeBetweenFrames = 0;
}

void medTimeLineParameter::setStepFrame(int stepFrame)
{
    d->stepFrame = stepFrame;
}

void medTimeLineParameter::setDuration(const double& timeDuration)
{
    d->duration = timeDuration;
    if(d->duration != 0)
        d->timeBetweenFrames = d->numberOfFrame / d->duration;
    else
        d->timeBetweenFrames = 0;
}

void medTimeLineParameter::setFrame(int frame)
{
    this->setFrame(this->mapFrameToMoment(frame));
}

void medTimeLineParameter::setFrame(double moment)
{
    d->momentParameter->setValue(moment);
    d->frameLineParameter->setValue(this->mapMomentToFrame(moment));

    emit frameChanged(moment);
}

double medTimeLineParameter::moment() const
{
    return d->momentParameter->value();
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
    if(d->widget)
    {
        d->widget = new QWidget;
        QVBoxLayout *widgetLayout = new QVBoxLayout(d->widget);
        QHBoxLayout *buttonsLayout = new QHBoxLayout;

        buttonsLayout->addWidget(d->playParameter->getPushButton());
        buttonsLayout->addWidget(d->previousFrameParameter->getPushButton());
        buttonsLayout->addWidget(d->nextFrameParameter->getPushButton());
        buttonsLayout->addWidget(d->speedFactorParameter->getSpinBox());

        widgetLayout->addLayout(buttonsLayout);
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

