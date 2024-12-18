/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTimeLineParameter.h>

#include <math.h>       // floor

#include <QTimeLine>
#include <QEasingCurve>

#include <medBoolParameter.h>
#include <medComboBox.h>
#include <medDoubleParameter.h>
#include <medIntParameter.h>
#include <medTriggerParameter.h>
#include <medStringParameter.h>
#include <medStringListParameter.h>

class medTimeLineParameterPrivate
{
public:
    medTimeLineParameterPrivate() = default;
    ~medTimeLineParameterPrivate() = default;

    QTimeLine           *timeLine;               //! Periodic tool to generate tick playing signal.

    // Values change by context change (like "3D data + time" change)
    double               duration;               //! Total time.
    unsigned int         numberOfFrames;         //! Timeline frame division count

    // Values change by user interaction
    medIntParameter     *speedFactorParameter;   //! Speed factor percentage
    medBoolParameter    *playParameter;          //! Playing status
    medTriggerParameter *nextFrameParameter;     //! Trigger to increment current frame
    medTriggerParameter *previousFrameParameter; //! Trigger to decrement current frame
    medBoolParameter    *loopParameter;          //! Active loop play status


    // Values change by re-computation
    medDoubleParameter  *currentTimeParameter;   //! Current time.
    int                  currentFrame;           //! Current frame index.

    // Time shift display
    medBoolParameter       *displayTimeParameter;
    medDoubleParameter     *timeShiftParameter;
    medStringListParameter *extensionShiftParameter;
    medStringParameter     *currentDisplayedTime;

    double timeBetweenFrames;
    unsigned int stepFrame;

    bool timeLineLocked;

    QList <medAbstractParameter*> parametersCandidateToPool;

    inline void computeTimeBetweenFrames();

};

inline void medTimeLineParameterPrivate::computeTimeBetweenFrames()
{
    if ((duration != 0) && (numberOfFrames > 1))
    {
        timeBetweenFrames = duration / (numberOfFrames - 1);
        //currentTimeParameter->setSingleStep(timeBetweenFrames);//TODO FLO
    }
    else
    {
        timeBetweenFrames = 0;
    }
}

medTimeLineParameter::medTimeLineParameter(QString name, QObject *parent):
    medAbstractParameter(name, parent),
    d(new medTimeLineParameterPrivate)
{
    d->speedFactorParameter = new medIntParameter("Speed", this);
    d->speedFactorParameter->setRange(1,5000);
    d->speedFactorParameter->setValue(100);

    d->playParameter = new medBoolParameter("Play", this);
    d->parametersCandidateToPool << d->playParameter;

    d->currentTimeParameter = new medDoubleParameter("Time", this);
    d->currentTimeParameter->setValue(0);
    d->parametersCandidateToPool << d->currentTimeParameter;

    d->nextFrameParameter = new medTriggerParameter("NextFrame", this);
    d->previousFrameParameter = new medTriggerParameter("PreviousFrame", this);

    d->timeLine = new QTimeLine(7000, this);
    d->timeLine->setEasingCurve(QEasingCurve());

    d->loopParameter = new medBoolParameter("Loop", this);
    d->parametersCandidateToPool << d->loopParameter;
    d->loopParameter->setCaption("Loop");
    
    // Default values
    d->duration = 1;
    d->currentFrame = 0;
    d->timeBetweenFrames = 0.0;
    d->numberOfFrames = 0;
    d->timeLineLocked = false;

    this->clear();

    connect(d->timeLine, SIGNAL(frameChanged(int)), this, SLOT(setFrame(int)));
    connect(d->timeLine, SIGNAL(finished()), this, SLOT(reset()));

    connect(d->playParameter, SIGNAL(valueChanged(bool)), this, SLOT(play(bool)));
    connect(d->playParameter, &medBoolParameter::valueChanged, this, &medTimeLineParameter::playing); //rebound playing information from playParameter to public playing signal
    connect(d->previousFrameParameter, SIGNAL(triggered()), this, SLOT(previousFrame()));
    connect(d->nextFrameParameter, SIGNAL(triggered()), this, SLOT(nextFrame()));
    connect(d->speedFactorParameter, SIGNAL(valueChanged(int)), this, SLOT(setSpeedFactor(int)));
    connect(d->currentTimeParameter, SIGNAL(valueChanged(double)), this, SLOT(updateTime(double)));
    connect(d->loopParameter, SIGNAL(valueChanged(bool)), this, SLOT(setLoop(bool)));
    connect(this, &medTimeLineParameter::timeChanged, this, &medTimeLineParameter::triggered);


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
    return d->numberOfFrames;
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

unsigned int medTimeLineParameter::mapTimeToFrame(const double &time)
{
    return floor( (double)(time / d->timeBetweenFrames) + 0.5 );
}

double medTimeLineParameter::mapFrameToTime (int frame)
{
    return frame * d->timeBetweenFrames;
}

bool medTimeLineParameter::copyValueTo(medAbstractParameter & dest)
{
    bool bRes = typeid(dest) == typeid(*this);

    if (bRes)
    {
        dynamic_cast<medTimeLineParameter*>(&dest)->setFrame(frame());
    }

    return bRes;
}

QVariantMap medTimeLineParameter::toVariantMap() const
{
    return QVariantMap(); //TODO
}

bool medTimeLineParameter::fromVariantMap(QVariantMap const & pi_variantMap)
{
    return false; //TODO
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
    d->playParameter->setValue(play); //will emit playing signal by rebound connection line 113

    if((d->timeLine->state() == QTimeLine::Paused || d->timeLine->state() == QTimeLine::NotRunning) && play)
    {
        d->timeLine->setCurrentTime(mapFrameToTime(d->currentFrame) * 1000);
        d->timeLine->resume();
    }
    else if(d->timeLine->state() == QTimeLine::Running && !play)
    {
        d->timeLine->setPaused(true);
    }

    this->lockTimeLine();
}

void medTimeLineParameter::reset()
{
    d->playParameter->setValue(false);
}

void medTimeLineParameter::setNumberOfFrame(int numberOfFrame)
{
    d->numberOfFrames = numberOfFrame;

    d->computeTimeBetweenFrames();
    d->timeLine->setFrameRange(0, d->numberOfFrames);
}

void medTimeLineParameter::setDuration(const double& timeDuration)
{
    d->duration = timeDuration;

    d->currentTimeParameter->setRange(0, d->duration);
    d->computeTimeBetweenFrames();
    d->timeLine->setDuration(timeDuration*1000); // convert to milliseconds
}

void medTimeLineParameter::setStepFrame(int stepFrame)
{
    d->stepFrame = stepFrame;
}

void medTimeLineParameter::setFrame(int frame)
{
    double time = this->mapFrameToTime(frame);
    if(!d->timeLineLocked)
        d->currentTimeParameter->setValue(time);
}

void medTimeLineParameter::setTime(const double &time)
{
    d->currentTimeParameter->setValue(time);
}

void medTimeLineParameter::updateTime(double time)
{
    unsigned int frame = this->mapTimeToFrame(time);

    if(frame > d->numberOfFrames)
        d->currentFrame = d->numberOfFrames;
    else d->currentFrame = frame;

    emit timeChanged(time);
}

double medTimeLineParameter::time() const
{
    return d->currentTimeParameter->value();
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

void medTimeLineParameter::trigger()
{
    d->currentTimeParameter->trigger();
}


medIntParameter * medTimeLineParameter::speedFactorParameter()
{
    return d->speedFactorParameter;
}

medBoolParameter * medTimeLineParameter::playParameter()
{
    return d->playParameter;
}

medTriggerParameter * medTimeLineParameter::nextFrameParameter()
{
    return d->nextFrameParameter;
}

medTriggerParameter * medTimeLineParameter::previousFrameParameter()
{
    return d->previousFrameParameter;
}

medBoolParameter * medTimeLineParameter::loopParameter()
{
    return d->loopParameter;
}

medDoubleParameter * medTimeLineParameter::currentTimeParameter()
{
    return d->currentTimeParameter;
}

medBoolParameter * medTimeLineParameter::displayTimeParameter()
{
    return d->displayTimeParameter;
}

medDoubleParameter * medTimeLineParameter::timeShiftParameter()
{
    return d->timeShiftParameter;
}

medStringListParameter * medTimeLineParameter::extensionShiftParameter()
{
    return d->extensionShiftParameter;
}

medStringParameter * medTimeLineParameter::currentDisplayedTime()
{
    return d->currentDisplayedTime;
}