/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTimeLineParameter.h>

#include <math.h>       // floor

#include <QtGui>

#include <dtkCore/dtkSignalBlocker.h>

#include <medIntParameter.h>
#include <medBoolParameter.h>
#include <medTriggerParameter.h>
#include <medDoubleParameter.h>


class medTimeLineParameterPrivate
{
public:
    QWidget* widget;

    medDoubleParameter* timeParameter;
    medIntParameter* speedFactorParameter;
    medBoolParameter* playParameter;
    medBoolParameter* stopParameter;
    medTriggerParameter* nextFrameParameter;
    medTriggerParameter* previousFrameParameter;
    medBoolParameter *loopParameter;

    QTimeLine *timeLine;
    QLabel *frameLabel;

    unsigned int numberOfFrame;
    int currentFrame;
    double duration;
    double timeBetweenFrames;
    unsigned int stepFrame;

    bool timeLineLocked;

    QList <medAbstractParameter*> parametersCandidateToPool;

    ~medTimeLineParameterPrivate()
    {
        if(widget)
            delete widget;

        if(frameLabel)
            delete frameLabel;
    }
};


medTimeLineParameter::medTimeLineParameter(QString name, QObject *parent):
    medAbstractGroupParameter(name, parent),
    d(new medTimeLineParameterPrivate)
{
    d->widget = NULL;
    d->frameLabel = NULL;

    d->speedFactorParameter = new medIntParameter("Speed", this);
    d->speedFactorParameter->setRange(1,5000);
    d->speedFactorParameter->setValue(100);

    d->playParameter = new medBoolParameter("Play", this);
    d->parametersCandidateToPool << d->playParameter;

    d->stopParameter = new medBoolParameter("Stop", this);
    d->parametersCandidateToPool << d->stopParameter;

    d->timeParameter = new medDoubleParameter("Time", this);
    d->timeParameter->setValue(0);
    d->parametersCandidateToPool << d->timeParameter;

    d->nextFrameParameter = new medTriggerParameter("NextFrame", this);
    d->previousFrameParameter = new medTriggerParameter("PreviousFrame", this);

    d->timeLine = new QTimeLine(7000, this);
    d->timeLine->setCurveShape (QTimeLine::LinearCurve);

    d->loopParameter = new medBoolParameter("Loop", this);
    d->parametersCandidateToPool << d->loopParameter;
    d->loopParameter->setText("Loop");

    d->duration = 1;
    d->currentFrame = 0;

    this->clear();

    connect(d->timeLine, SIGNAL(frameChanged(int)), SLOT(setFrame(int)));

    connect(d->playParameter, SIGNAL(valueChanged(bool)), this, SLOT(play(bool)));
    connect(d->stopParameter, SIGNAL(valueChanged(bool)), this, SLOT(stop(bool)));
    connect(d->previousFrameParameter, SIGNAL(triggered()), this, SLOT(previousFrame()));
    connect(d->nextFrameParameter, SIGNAL(triggered()), this, SLOT(nextFrame()));
    connect(d->speedFactorParameter, SIGNAL(valueChanged(int)), this, SLOT(setSpeedFactor(int)));
    connect(d->timeParameter, SIGNAL(valueChanged(double)), this, SLOT(updateTime(double)));
    connect(d->loopParameter, SIGNAL(valueChanged(bool)), this, SLOT(setLoop(bool)));
    connect(d->timeParameter, SIGNAL(valueChanged(double)), this, SLOT(updateFrameLabel()));

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
    return d->currentFrame;
}

double& medTimeLineParameter::duration() const
{
    return d->duration;
}

int medTimeLineParameter::stepFrame() const
{
    return d->stepFrame;
}

unsigned int medTimeLineParameter::mapTimeToFrame(const double& time)
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

void medTimeLineParameter::unlockTimeLine()
{
    d->timeLineLocked = false;
}

void medTimeLineParameter::lockTimeLine()
{
     d->timeLineLocked = true;
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

   this->lockTimeLine();
}

void medTimeLineParameter::stop(bool stop)
{
    play(!stop);
}

void medTimeLineParameter::setNumberOfFrame(int numberOfFrame)
{
    d->numberOfFrame = numberOfFrame;
    if(d->duration != 0)
    {
        d->timeBetweenFrames = d->duration / (d->numberOfFrame - 1);
        d->timeParameter->setSingleStep(d->timeBetweenFrames);
    }
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
    {
        d->timeBetweenFrames = d->duration / (d->numberOfFrame - 1);
        d->timeParameter->setSingleStep(d->timeBetweenFrames);
    }
    else
        d->timeBetweenFrames = 0;

    d->timeLine->setDuration(timeDuration*1000);

}

void medTimeLineParameter::setFrame(int frame)
{
    double time = this->mapFrameToTime(frame);
    if(!d->timeLineLocked)
        d->timeParameter->setValue(time);
}

void medTimeLineParameter::setTime(const double &time)
{
    d->timeParameter->setValue(time);
}

void medTimeLineParameter::updateTime(double time)
{
    unsigned int frame = this->mapTimeToFrame(time);

    if(frame > d->numberOfFrame)
        d->currentFrame = d->numberOfFrame;
    else d->currentFrame = frame;

    emit timeChanged(time);
}

double medTimeLineParameter::time() const
{
    return d->timeParameter->value();
}

void medTimeLineParameter::previousFrame()
{
    this->unlockTimeLine();
    int tempFrame = this->frame();
    this->setFrame( tempFrame - d->stepFrame );
}

void medTimeLineParameter::nextFrame()
{
    this->unlockTimeLine();
    int tempFrame = this->frame();
    this->setFrame( tempFrame + d->stepFrame );
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
        QHBoxLayout *indiactorLayout = new QHBoxLayout;

        d->playParameter->setIcon(QIcon(":/icons/play.png"));
        d->stopParameter->setIcon(QIcon(":/icons/stop.png"));
        connect(d->playParameter->getPushButton(), SIGNAL(clicked()), this, SLOT(unlockTimeLine()));
        d->previousFrameParameter->getPushButton()->setIcon(QIcon(":/icons/backward.png"));
        d->nextFrameParameter->getPushButton()->setIcon(QIcon(":/icons/forward.png"));
        d->speedFactorParameter->getSpinBox()->setSingleStep(10);
        d->timeParameter->getSlider()->setOrientation(Qt::Horizontal);

        buttonsLayout->addWidget(d->playParameter->getPushButton());
        buttonsLayout->addWidget(d->previousFrameParameter->getPushButton());
        buttonsLayout->addWidget(d->nextFrameParameter->getPushButton());
        buttonsLayout->addWidget(d->speedFactorParameter->getLabel());
        buttonsLayout->addWidget(d->speedFactorParameter->getSpinBox());
        buttonsLayout->addWidget(d->loopParameter->getCheckBox());
        buttonsLayout->addWidget(d->frameLabel);


        if(!d->frameLabel)
        {
            d->frameLabel = new QLabel;
            this->updateFrameLabel();
            connect(d->timeParameter, SIGNAL(valueChanged(double)), this, SLOT(updateFrameLabel()));
            connect(d->frameLabel, SIGNAL(destroyed()), this, SLOT(removeInternFrameLabel()));
        }
        indiactorLayout->addWidget(d->timeParameter->getValueLabel(), 0, Qt::AlignLeft);
        indiactorLayout->addWidget(new QLabel("/ " + QString::number(d->duration) + " second"), 0, Qt::AlignLeft);
        indiactorLayout->addStretch(1);
        indiactorLayout->addWidget(d->frameLabel, 0, Qt::AlignRight);
        indiactorLayout->addWidget(new QLabel("/ " + QString::number(d->numberOfFrame) + " frame"), 0, Qt::AlignRight);


        widgetLayout->addLayout(buttonsLayout);
        widgetLayout->addWidget(d->timeParameter->getSlider());
        widgetLayout->addLayout(indiactorLayout);

        connect(d->widget, SIGNAL(destroyed()), this, SLOT(removeInternWidget()));
    }

    return d->widget;
}

void medTimeLineParameter::updateFrameLabel()
{
    if(!d->frameLabel)
        return;

    d->frameLabel->setText(QString::number(d->currentFrame + 1));
}

void medTimeLineParameter::removeInternWidget()
{
    this->removeFromInternWidgets(d->widget);
    d->widget = NULL;
}

void medTimeLineParameter::removeInternFrameLabel()
{
    d->frameLabel = NULL;
}
