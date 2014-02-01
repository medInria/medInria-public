/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractParameter.h>

#include <medGuiExport.h>

class QWidget;

class medBoolParameter;

class medTimeLineParameterPrivate;
class MEDGUI_EXPORT medTimeLineParameter : public medAbstractGroupParameter
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
    double moment() const;
    double& duration() const;
    int stepFrame() const;

    int mapMomentToFrame(const double& time);
    double mapFrameToMoment (int frame);

public slots:
    void setSpeedFactor(int speedFactor);
    void play(bool play);
    void setNumberOfFrame(int numberOfFrame);
    void setDuration(const double& timeDuration);
    void setStepFrame(int stepFrame);
    void setFrame(double moment);
    void setFrame(int frame);

    void previousFrame();
    void nextFrame();

    virtual QWidget* getWidget();
    virtual QList<medAbstractParameter*> parametersCandidateToPool() const;

signals:
    void playing(bool isPlaying);
    void frameChanged(double frameMoment);

private slots:
    void _prvt_removeInternWidget();


private:
    medTimeLineParameterPrivate* d;

};
