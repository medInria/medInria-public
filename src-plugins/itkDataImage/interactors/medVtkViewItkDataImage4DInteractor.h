/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkViewItkDataImageInteractor.h>

class medVtkViewItkDataImage4DInteractorPrivate;

class medVtkViewItkDataImage4DInteractor : public medVtkViewItkDataImageInteractor
{
    Q_OBJECT

public:
    medVtkViewItkDataImage4DInteractor(medAbstractView* parent);
    ~medVtkViewItkDataImage4DInteractor();

public:
    virtual QString description() const;

    virtual QString identifier() const;
    virtual QStringList handled() const;

    static bool registered();

    virtual void setData(medAbstractData *data);
    virtual medAbstractData *data() const;

    virtual QWidget* buildToolBoxWidget();
    virtual QWidget* buildToolBarWidget();
    virtual QWidget* buildLayerWidget();

    virtual QList<medAbstractParameter*> linkableParameters();

    /** Assigns ranfe array the minimum and maximum time of the sequence */
    void timeRange (double* range);
    /** Returns minimum time step for the sequence */
    double frameRate();


public slots:
    void setCurrentTime (double time);

     virtual void updateWidgets();

private:
    static QStringList dataHandled();

private:
    medVtkViewItkDataImage4DInteractorPrivate *d;
};


