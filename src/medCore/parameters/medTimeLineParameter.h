/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractParameter.h>

#include <medCoreExport.h>

class QWidget;

class medBoolParameter;

class medTimeLineParameterPrivate;
class MEDCORE_EXPORT medTimeLineParameter : public medAbstractGroupParameter
{
    Q_OBJECT

public:
    medTimeLineParameter(QString name = "Unknow timeLine parameter", QObject* parent = 0);
    virtual ~medTimeLineParameter();

    void clear();

    int speedFactor() const;
    bool isPlaying() const;
    int numberOfFrame() const;
    int frame() const;
    double time() const;
    double& duration() const;
    int stepFrame() const;

    unsigned int mapTimeToFrame(const double& time);
    double mapFrameToTime(int frame);

    virtual void trigger();

public slots:
    void setSpeedFactor(int speedFactor);
    void play(bool play = true);
    void setNumberOfFrame(int numberOfFrame);
    void setDuration(const double& timeDuration);
    void setStepFrame(int stepFrame);
    void setFrame(int frame);
    void setTime(const double &time);
    void setLoop(bool loop);
    void reset();

    void previousFrame();
    void nextFrame();

    virtual QWidget* getWidget();
    virtual QList<medAbstractParameter*> parametersCandidateToPool() const;

signals:
    void playing(bool isPlaying);
    void timeChanged(double time);

private slots:
    void unlockTimeLine();
    void lockTimeLine();

    void updateTime(double time);
    void updateFrameLabel();
    void updateNumberOfFrameLabel();
    void removeInternWidget();

private:
    medTimeLineParameterPrivate* d;

};
