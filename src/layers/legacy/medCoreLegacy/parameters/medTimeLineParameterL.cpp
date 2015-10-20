/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTimeLineParameterL.h>

#include <math.h>       // floor

#include <QtGui>
#include <QtWidgets>
#include <dtkCoreSupport/dtkSignalBlocker.h>

#include <medIntParameterL.h>
#include <medBoolParameterL.h>
#include <medTriggerParameterL.h>
#include <medDoubleParameterL.h>



class medTimeLineParameterLPrivate
{
public:
    QWidget* widget;

    medDoubleParameterL* timeParameter;
    medIntParameterL* speedFactorParameter;
    medBoolParameterL* playParameter;
    medTriggerParameterL* nextFrameParameter;
    medTriggerParameterL* previousFrameParameter;
    medBoolParameterL *loopParameter;

    QTimeLine *timeLine;
    QPointer<QLabel> frameLabel;
    QPointer<QLabel> numberOfFramesLabel;

    unsigned int numberOfFrames;
    int currentFrame;
    double duration;
    double timeBetweenFrames;
    unsigned int stepFrame;

    bool timeLineLocked;

    QList <medAbstractParameterL*> parametersCandidateToPool;

    ~medTimeLineParameterLPrivate()
    {
        if(widget)
            delete widget;

        if(frameLabel)
            delete frameLabel;

        if(numberOfFramesLabel)
            delete numberOfFramesLabel;
    }
};


medTimeLineParameterL::medTimeLineParameterL(QString name, QObject *parent):
    medAbstractGroupParameterL(name, parent),
    d(new medTimeLineParameterLPrivate)
{
    d->widget = NULL;
    d->frameLabel = NULL;
    d->numberOfFramesLabel = NULL;

    d->speedFactorParameter = new medIntParameterL("Speed", this);
    d->speedFactorParameter->setRange(1,5000);
    d->speedFactorParameter->setValue(100);

    d->playParameter = new medBoolParameterL("Play", this);
    d->parametersCandidateToPool << d->playParameter;

    d->timeParameter = new medDoubleParameterL("Time", this);
    d->timeParameter->setValue(0);
    d->parametersCandidateToPool << d->timeParameter;

    d->nextFrameParameter = new medTriggerParameterL("NextFrame", this);
    d->previousFrameParameter = new medTriggerParameterL("PreviousFrame", this);

    d->timeLine = new QTimeLine(7000, this);
    d->timeLine->setCurveShape (QTimeLine::LinearCurve);

    d->loopParameter = new medBoolParameterL("Loop", this);
    d->parametersCandidateToPool << d->loopParameter;
    d->loopParameter->setText("Loop");

    d->duration = 1;
    d->currentFrame = 0;
    d->timeBetweenFrames = 0;
    d->numberOfFrames = 0;
    d->timeLineLocked = false;

    this->clear();

    connect(d->timeLine, SIGNAL(frameChanged(int)), this, SLOT(setFrame(int)));
    connect(d->timeLine, SIGNAL(finished()), this, SLOT(reset()));

    connect(d->playParameter, SIGNAL(valueChanged(bool)), this, SLOT(play(bool)));
    connect(d->previousFrameParameter, SIGNAL(triggered()), this, SLOT(previousFrame()));
    connect(d->nextFrameParameter, SIGNAL(triggered()), this, SLOT(nextFrame()));
    connect(d->speedFactorParameter, SIGNAL(valueChanged(int)), this, SLOT(setSpeedFactor(int)));
    connect(d->timeParameter, SIGNAL(valueChanged(double)), this, SLOT(updateTime(double)));
    connect(d->loopParameter, SIGNAL(valueChanged(bool)), this, SLOT(setLoop(bool)));
    connect(d->timeParameter, SIGNAL(valueChanged(double)), this, SLOT(updateFrameLabel()));

    d->loopParameter->setValue(true);
}

medTimeLineParameterL::~medTimeLineParameterL()
{
    delete d;
}

void medTimeLineParameterL::clear()
{
    this->setSpeedFactor(1);
    this->setDuration(1);
    this->setNumberOfFrame(1);
    this->setFrame(0);
    this->setStepFrame(1);
    this->play(false);
    d->timeBetweenFrames = 0.0;
}

int medTimeLineParameterL::speedFactor() const
{
    return d->speedFactorParameter->value();
}

bool medTimeLineParameterL::isPlaying() const
{
    return d->playParameter->value();
}

int medTimeLineParameterL::numberOfFrame() const
{
    return d->numberOfFrames;
}

int medTimeLineParameterL::frame() const
{
    return d->currentFrame;
}

double& medTimeLineParameterL::duration() const
{
    return d->duration;
}

int medTimeLineParameterL::stepFrame() const
{
    return d->stepFrame;
}

unsigned int medTimeLineParameterL::mapTimeToFrame(const double &time)
{
    return floor( (double)(time / d->timeBetweenFrames) + 0.5 );
}

double medTimeLineParameterL::mapFrameToTime (int frame)
{
    return frame * d->timeBetweenFrames;
}

void medTimeLineParameterL::setSpeedFactor(int speedFactor)
{
    d->timeLine->setDuration((d->duration)*(1000/(speedFactor/100.0)));
}

void medTimeLineParameterL::unlockTimeLine()
{
    d->timeLineLocked = false;
}

void medTimeLineParameterL::lockTimeLine()
{
     d->timeLineLocked = true;
}

void medTimeLineParameterL::play(bool play)
{
    d->playParameter->setValue(play);

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

void medTimeLineParameterL::reset()
{
    d->playParameter->setValue(false);
    d->playParameter->setIcon (QPixmap(":/icons/play.png"));
}

void medTimeLineParameterL::setNumberOfFrame(int numberOfFrame)
{
    d->numberOfFrames = numberOfFrame;
    if((d->duration != 0)&&(d->numberOfFrames > 1))
    {
        d->timeBetweenFrames = d->duration / (d->numberOfFrames - 1);
        d->timeParameter->setSingleStep(d->timeBetweenFrames);
    }
    else
        d->timeBetweenFrames = 0;

    d->timeLine->setFrameRange(0, d->numberOfFrames);

    updateNumberOfFrameLabel();
}

void medTimeLineParameterL::setStepFrame(int stepFrame)
{
    d->stepFrame = stepFrame;
}

void medTimeLineParameterL::setDuration(const double& timeDuration)
{
    d->duration = timeDuration;
    d->timeParameter->setRange(0, d->duration);
    if((d->duration != 0)&&(d->numberOfFrames > 1))
    {
        d->timeBetweenFrames = d->duration / (d->numberOfFrames - 1);
        d->timeParameter->setSingleStep(d->timeBetweenFrames);
    }
    else
        d->timeBetweenFrames = 0;

    d->timeLine->setDuration(timeDuration*1000);

}

void medTimeLineParameterL::setFrame(int frame)
{
    double time = this->mapFrameToTime(frame);
    if(!d->timeLineLocked)
        d->timeParameter->setValue(time);
}

void medTimeLineParameterL::setTime(const double &time)
{
    d->timeParameter->setValue(time);
}

void medTimeLineParameterL::updateTime(double time)
{
    unsigned int frame = this->mapTimeToFrame(time);

    if(frame > d->numberOfFrames)
        d->currentFrame = d->numberOfFrames;
    else d->currentFrame = frame;

    emit timeChanged(time);
}

double medTimeLineParameterL::time() const
{
    return d->timeParameter->value();
}

void medTimeLineParameterL::previousFrame()
{
    this->unlockTimeLine();
    int tempFrame = this->frame();
    this->setFrame( tempFrame - d->stepFrame );
}

void medTimeLineParameterL::nextFrame()
{
    this->unlockTimeLine();
    int tempFrame = this->frame();
    this->setFrame( tempFrame + d->stepFrame );
}

void medTimeLineParameterL::setLoop(bool loop)
{
    int loopCount = (loop == true)?0:1;
    d->timeLine->setLoopCount(loopCount);
}

QList<medAbstractParameterL*> medTimeLineParameterL::parametersCandidateToPool() const
{
    return d->parametersCandidateToPool;
}

QWidget* medTimeLineParameterL::getWidget()
{
    if(!d->widget)
    {
        d->widget = new QWidget;
        QVBoxLayout *widgetLayout = new QVBoxLayout(d->widget);
        QHBoxLayout *buttonsLayout = new QHBoxLayout;
        QHBoxLayout *indicatorLayout = new QHBoxLayout;

        d->playParameter->setIcon(QIcon(":/icons/play.png"));
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

        if(d->frameLabel.isNull())
        {
            d->frameLabel = new QLabel;
            this->updateFrameLabel();
            connect(d->timeParameter, SIGNAL(valueChanged(double)), this, SLOT(updateFrameLabel()));
        }

        if(d->numberOfFramesLabel.isNull())
        {
            d->numberOfFramesLabel = new QLabel;
            this->updateNumberOfFrameLabel();
        }
        indicatorLayout->addWidget(d->timeParameter->getValueLabel(), 0, Qt::AlignLeft);
        indicatorLayout->addWidget(new QLabel("/ " + QString::number(d->duration,'g',2) + " s"), 0, Qt::AlignLeft);
        indicatorLayout->addStretch(1);
        indicatorLayout->addWidget(d->frameLabel, 0, Qt::AlignRight);
        indicatorLayout->addWidget(d->numberOfFramesLabel, 0, Qt::AlignRight);

        widgetLayout->addLayout(buttonsLayout);
        widgetLayout->addWidget(d->timeParameter->getSlider());
        widgetLayout->addLayout(indicatorLayout);

        this->addToInternWidgets(d->widget);
        connect(d->widget, SIGNAL(destroyed()), this, SLOT(removeInternWidget()));
    }

    return d->widget;
}

void medTimeLineParameterL::updateFrameLabel()
{
    if(d->frameLabel.isNull())
        return;

    d->frameLabel->setText(QString::number(d->currentFrame + 1));
}

void medTimeLineParameterL::updateNumberOfFrameLabel()
{
    if(d->numberOfFramesLabel.isNull())
        return;

    d->numberOfFramesLabel->setText("/ " + QString::number(d->numberOfFrames) + " frames");
}

void medTimeLineParameterL::removeInternWidget()
{
    this->removeFromInternWidgets(d->widget);
    d->widget = NULL;
}

void medTimeLineParameterL::trigger()
{
    d->timeParameter->trigger();
}
