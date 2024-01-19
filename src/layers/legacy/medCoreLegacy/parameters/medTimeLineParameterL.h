#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameterL.h>
#include <medCoreLegacyExport.h>

class QWidget;

class medBoolParameterL;

class medTimeLineParameterLPrivate;
class MEDCORELEGACY_EXPORT medTimeLineParameterL : public medAbstractGroupParameterL
{
    Q_OBJECT

public:
    medTimeLineParameterL(QString name = "Unknown timeLine parameter", QObject* parent = nullptr);
    virtual ~medTimeLineParameterL();

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

    //! Get the visible time (+shift) which could be added to the view
    QString getDisplayedTime();

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

    void unlockTimeLine();
    void lockTimeLine();

    void previousFrame();
    void nextFrame();

    //! Compute the visible time (+shift) which could be added to the view
    void computeDisplayedTime();

    virtual QWidget* getWidget();
    virtual QList<medAbstractParameterL*> parametersCandidateToPool() const;

signals:
    void playing(bool isPlaying);
    void timeChanged(double time);

    void aboutToBeDestroyed();

private slots:
    void updateTime(double time);
    void updateFrameLabel();
    void updateNumberOfFrameLabel();
    void removeInternWidget();

private:
    medTimeLineParameterLPrivate* d;

};
