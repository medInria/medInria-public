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
#include <QCheckBox>
#include <QLabel>

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
    medBoolParameter *loopParameter;

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
        delete loopParameter;
    }
};


medTimeLineParameter::medTimeLineParameter(QString name, QObject *parent):
    medAbstractGroupParameter(name, parent),
    d(new medTimeLineParameterPrivate)
{
    d->widget = NULL;


    d->speedFactorParameter = new medIntParameter("Speed", this);
    d->speedFactorParameter->setRange(1,5000);
    d->speedFactorParameter->setValue(100);

    d->playParameter = new medBoolParameter("Play", this);
    d->parametersCandidateToPool << d->playParameter;

    d->stopParameter = new medBoolParameter("Stop", this);

    d->timeParameter = new medDoubleParameter("Time", this);
    d->parametersCandidateToPool << d->timeParameter;

    d->frameLineParameter = new medIntParameter("Frame", this);
    d->nextFrameParameter = new medTriggerParameter("NextFrame", this);
    d->previousFrameParameter = new medTriggerParameter("PreviousFrame", this);

    d->timeLine = new QTimeLine(1000, this);
    d->timeLine->setCurveShape (QTimeLine::LinearCurve);

    d->loopParameter = new medBoolParameter("Loop", this);
    d->parametersCandidateToPool << d->loopParameter;
    d->loopParameter->setText("Loop");

    d->duration = 1;

    this->clear();

    connect(d->timeLine, SIGNAL(frameChanged(int)), d->frameLineParameter, SIGNAL(valueChanged(int)));

    connect(d->frameLineParameter, SIGNAL(valueChanged(int)), this, SLOT(setFrame(int)));
    connect(d->playParameter, SIGNAL(valueChanged(bool)), this, SLOT(play(bool)));
    connect(d->stopParameter, SIGNAL(valueChanged(bool)), this, SLOT(stop(bool)));
    connect(d->previousFrameParameter, SIGNAL(triggered()), this, SLOT(previousFrame()));
    connect(d->nextFrameParameter, SIGNAL(triggered()), this, SLOT(nextFrame()));
    connect(d->speedFactorParameter, SIGNAL(valueChanged(int)), this, SLOT(setSpeedFactor(int)));
    connect(d->timeParameter, SIGNAL(valueChanged(double)), this, SLOT(setFrame(double)));
    connect(d->loopParameter, SIGNAL(valueChanged(bool)), this, SLOT(setLoop(bool)));

    d->loopParameter->setValue(true);
}

medTimeLineParameter::~medTimeLineParameter()
{
    delete d;
}

void medTimeLineParameter::clear()
{
    this->setSpeedFactor(1);
    this->setDuration(1);
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

void medTimeLineParameter::setSpeedFactor(int speedFactor)
{
    d->timeLine->setDuration((d->duration)*(1000/(speedFactor/100.0)));
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
    d->frameLineParameter->setRange(0, numberOfFrame-1);
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
    d->timeParameter->setRange(0, d->duration);
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
    //d->timeParameter->blockSignals(true);
    d->timeParameter->setValue(time);
    //d->timeParameter->blockSignals(false);
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

void medTimeLineParameter::setLoop(bool loop)
{
    int loopCount = (loop == true)?0:1;
    d->timeLine->setLoopCount(loopCount);
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
        d->speedFactorParameter->getSpinBox()->setSingleStep(10);
        d->frameLineParameter->getSlider()->setOrientation(Qt::Horizontal);

        buttonsLayout->addWidget(d->playParameter->getPushButton());
        buttonsLayout->addWidget(d->previousFrameParameter->getPushButton());
        buttonsLayout->addWidget(d->nextFrameParameter->getPushButton());
        buttonsLayout->addWidget(d->speedFactorParameter->getLabel());
        buttonsLayout->addWidget(d->speedFactorParameter->getSpinBox());
        buttonsLayout->addWidget(d->loopParameter->getCheckBox());

        QHBoxLayout *sliderLayout = new QHBoxLayout;
        sliderLayout->addWidget(d->timeParameter->getValueLabel());
        sliderLayout->addWidget(d->frameLineParameter->getSlider());
        sliderLayout->addWidget(new QLabel(QString::number(d->duration - 1, 'f', 2))); // duration - 1 beacause the first moment is 0s

        widgetLayout->addLayout(buttonsLayout);
        widgetLayout->addLayout(sliderLayout);

        connect(d->widget, SIGNAL(destroyed()), this, SLOT(removeInternWidget()));
    }

    return d->widget;
}

void medTimeLineParameter::removeInternWidget()
{
    this->removeFromInternWidgets(d->widget);
    d->widget = NULL;
}

