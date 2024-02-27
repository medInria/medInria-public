#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameter.h>

class QWidget;

class medBoolParameter;
class medIntParameter;
class medDoubleParameter;
class medTriggerParameter;
class medStringParameter;
class medStringListParameter;

class medTimeLineParameterPrivate;
class MEDCORE_EXPORT medTimeLineParameter : public medAbstractParameter
{
    Q_OBJECT

public:
    medTimeLineParameter(QString name = "Unknown timeLine parameter", QObject* parent = nullptr);
    ~medTimeLineParameter() override;

    medParameterType type() const override { return medParameterType::MED_PARAMETER_TIMELINE; }

    void clear();

    int          speedFactor() const;
    bool         isPlaying() const;
    int          numberOfFrame() const;
    int          frame() const;
    double       time() const;
    double&      duration() const;
    int          stepFrame() const;

    unsigned int mapTimeToFrame(const double& time);
    double       mapFrameToTime(int frame);

    bool copyValueTo(medAbstractParameter &dest) override;

    QVariantMap toVariantMap() const override;
    bool fromVariantMap(QVariantMap const& pi_variantMap) override;

    void trigger() override;

    // Values change by user interaction
    medIntParameter        * speedFactorParameter();   //! Speed factor percentage
    medBoolParameter       * playParameter();          //! Playing status
    medTriggerParameter    * nextFrameParameter();     //! Trigger to increment current frame
    medTriggerParameter    * previousFrameParameter(); //! Trigger to decrement current frame
    medBoolParameter       * loopParameter();          //! Active loop play status
    // Values change by re-computation
    medDoubleParameter     * currentTimeParameter();    //! Current time.
    // Time shift display
    medBoolParameter       * displayTimeParameter();
    medDoubleParameter     * timeShiftParameter();
    medStringListParameter * extensionShiftParameter();
    medStringParameter     * currentDisplayedTime();


public slots:
    void play(bool play = true);
    void setSpeedFactor(int speedFactor);
    void setNumberOfFrame(int numberOfFrame);
    void setDuration(const double& timeDuration); //in second
    void setStepFrame(int stepFrame);
    void setFrame(int frame);
    void setTime(const double &time);
    void setLoop(bool loop);
    void reset();

    void unlockTimeLine();
    void lockTimeLine();

    void previousFrame();
    void nextFrame();

    QList<medAbstractParameter*> parametersCandidateToPool() const;

signals:
    void playing(bool isPlaying);
    void timeChanged(double time);

private slots:
    void updateTime(double time);

private:
    medTimeLineParameterPrivate* d;

};
