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
    virtual QString name() const;
    virtual QString identifier() const;
    virtual QStringList handled() const;

    static bool registered();

    virtual void setInputData(medAbstractData *data);
    virtual medAbstractData *inputData() const;

    virtual QWidget* buildToolBoxWidget();
    virtual QWidget* buildToolBarWidget();
    virtual QWidget* buildLayerWidget();

    virtual QList<medAbstractParameter*> linkableParameters();

public slots:
    virtual void setCurrentTime (double time);
    virtual void updateWidgets();

private:
    static QStringList dataHandled();

private:
    medVtkViewItkDataImage4DInteractorPrivate *d;
};


