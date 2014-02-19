/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractInteractor.h>

class vtkDataMesh4DInteractorPrivate;

class vtkDataMesh4DInteractor : public medAbstractInteractor
{
    Q_OBJECT

public:
    vtkDataMesh4DInteractor(medAbstractView* parent);
    ~vtkDataMesh4DInteractor();

public:
    virtual QString description() const;

    virtual QString identifier() const;
    virtual QStringList handled() const;

    static bool registered();

    virtual void setData(medAbstractData *data);
    virtual medAbstractData *data() const;

    virtual QWidget* toolBoxWidget();
    virtual QWidget* toolBarWidget();
    virtual QWidget* layerWidget();

    virtual QList<medAbstractParameter*> parameters();



    double getCurrentTime();
    /** Assigns ranfe array the minimum and maximum time of the sequence */
    void sequencesRange (double* range);
    /** Returns minimum time step for the sequence */
    double sequencesMinTimeStep();

public slots:
    void setCurrentTime (double time);

private:
    static QStringList dataHandled();

private:
    vtkDataMesh4DInteractorPrivate *d;
};

