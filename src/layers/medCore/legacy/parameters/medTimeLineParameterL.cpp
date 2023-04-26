/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTimeLineParameterL.h>

#include <math.h>       // floor

#include <QtGui>
#include <QtWidgets>

#include <medBoolParameterL.h>
#include <medComboBox.h>
#include <medDoubleParameterL.h>
#include <medIntParameterL.h>
#include <medTriggerParameterL.h>

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

    // Time shift display
    medBoolParameterL   *displayTimeParameter;
    medDoubleParameterL *timeShiftParameter;
    medComboBox         *extensionShiftParameter;
    QString             currentDisplayedTime;

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

        if (extensionShiftParameter)
        {
            delete extensionShiftParameter;
        }
    }
};


medTimeLineParameterL::medTimeLineParameterL(QString name, QObject *parent):
    medAbstractGroupParameterL(name, parent),
    d(new medTimeLineParameterLPrivate)
{
    d->widget = nullptr;
    d->frameLabel = nullptr;
    d->numberOfFramesLabel = nullptr;

    d->speedFactorParameter = new medIntParameterL("Speed", this);
    d->speedFactorParameter->setToolTip("Speed factor of data display");
    d->speedFactorParameter->setRange(1,5000);
    d->speedFactorParameter->setValue(100);

    d->playParameter = new medBoolParameterL("Play", this);
    d->playParameter->setToolTip("Play");
    d->parametersCandidateToPool << d->playParameter;

    d->timeParameter = new medDoubleParameterL("Time", this);
    d->timeParameter->setValue(0);
    d->parametersCandidateToPool << d->timeParameter;

    d->nextFrameParameter = new medTriggerParameterL("NextFrame", this);
    d->nextFrameParameter->setToolTip("Move to the next frame");

    d->previousFrameParameter = new medTriggerParameterL("PreviousFrame", this);
    d->previousFrameParameter->setToolTip("Go back to the previous frame");

    d->timeLine = new QTimeLine(7000, this);
    d->timeLine->setCurveShape (QTimeLine::LinearCurve);

    // Choose if the time display is going to loop 
    d->loopParameter = new medBoolParameterL("Loop", this);
    d->parametersCandidateToPool << d->loopParameter;
    d->loopParameter->setText("Loop");
    d->loopParameter->setToolTip("Choose if display loops or not");

    // Time shift display
    d->displayTimeParameter = new medBoolParameterL("Display Time", this);
    d->displayTimeParameter->setText("");
    d->displayTimeParameter->setToolTip("Choose if time is displayed in the view");
    d->displayTimeParameter->setValue(true);

    d->timeShiftParameter = new medDoubleParameterL("Display time with shift:", this);
    d->timeShiftParameter->setToolTip("Add a duration in displayed time on view");
    d->timeShiftParameter->setRange(0, 500000);
    d->timeShiftParameter->setValue(0.0);

    d->extensionShiftParameter = new medComboBox;
    d->extensionShiftParameter->setToolTip("Duration extension");
    d->extensionShiftParameter->addItem(QString("µ")+"s",  0);
    d->extensionShiftParameter->addItem("ms",  1);
    d->extensionShiftParameter->addItem("s",   2);
    d->extensionShiftParameter->addItem("min", 3);
    d->extensionShiftParameter->addItem("h",   4);
    d->extensionShiftParameter->addItem("d",   5);
    d->extensionShiftParameter->setCurrentIndex(2);

    connect(d->displayTimeParameter,    SIGNAL(valueChanged(bool)),       this, SLOT(computeDisplayedTime()));
    connect(d->timeShiftParameter,      SIGNAL(valueChanged(double)),     this, SLOT(computeDisplayedTime()));
    connect(d->extensionShiftParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(computeDisplayedTime()));

    // Default values
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
    computeDisplayedTime();
}

medTimeLineParameterL::~medTimeLineParameterL()
{
    emit aboutToBeDestroyed();
    delete d;
    d = nullptr;
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

    if((d->timeLine->state() == QTimeLine::Paused || d->timeLine->state() == QTimeLine::NotRunning) && play)
    {
        d->timeLine->setCurrentTime(mapFrameToTime(d->currentFrame) * 1000);
        d->timeLine->resume();
        d->playParameter->setIcon(QIcon::fromTheme("time_pause"));
        
        emit playing(play);
    }
    else if(d->timeLine->state() == QTimeLine::Running && !play)
    {
        d->timeLine->setPaused(true);
        d->playParameter->setIcon(QIcon::fromTheme("time_play"));
    }

   this->lockTimeLine();
}

void medTimeLineParameterL::reset()
{
    d->playParameter->setValue(false);
    d->playParameter->setIcon(QIcon::fromTheme("time_play"));
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

    computeDisplayedTime();

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

void medTimeLineParameterL::computeDisplayedTime()
{
    if (d->displayTimeParameter->getCheckBox()->isChecked())
    {
        double displayedTimeValue = time()
                + d->timeShiftParameter->getSpinBox()->value();

        // Ex. "Time = 66.6 ms"
        d->currentDisplayedTime = QString("Time = ")
                + QString::number(displayedTimeValue)
                + " "
                + d->extensionShiftParameter->currentText();
    }
    else
    {
        d->currentDisplayedTime = QString("");
    }
}

QString medTimeLineParameterL::getDisplayedTime()
{
    return d->currentDisplayedTime;
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

        d->playParameter->setIcon(QIcon::fromTheme("time_play"));
        connect(d->playParameter->getPushButton(), SIGNAL(clicked()), this, SLOT(unlockTimeLine()));
        d->previousFrameParameter->getPushButton()->setIcon(QIcon::fromTheme("time_backward_step"));
        d->nextFrameParameter->getPushButton()->setIcon(QIcon::fromTheme("time_forward_step"));
        d->speedFactorParameter->getSpinBox()->setSingleStep(10);
        d->timeParameter->getSlider()->setOrientation(Qt::Horizontal);

        buttonsLayout->addWidget(d->previousFrameParameter->getPushButton());
        buttonsLayout->addWidget(d->playParameter->getPushButton());
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

        // Time Shift
        QHBoxLayout *shiftLayout = new QHBoxLayout;
        shiftLayout->setAlignment(Qt::AlignLeft);
        shiftLayout->addWidget(d->displayTimeParameter->getCheckBox());
        shiftLayout->addWidget(d->timeShiftParameter->getLabel());
        shiftLayout->addWidget(d->timeShiftParameter->getSpinBox());
        shiftLayout->addWidget(d->extensionShiftParameter);

        widgetLayout->addLayout(buttonsLayout);
        widgetLayout->addWidget(d->timeParameter->getSlider());
        widgetLayout->addLayout(indicatorLayout);
        widgetLayout->addLayout(shiftLayout);

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
    d->widget = nullptr;
}

void medTimeLineParameterL::trigger()
{
    d->timeParameter->trigger();
}
