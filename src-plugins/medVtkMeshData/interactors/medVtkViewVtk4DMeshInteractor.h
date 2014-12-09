/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkViewVtkMeshInteractor.h>

class medVtkViewVtk4DMeshInteractorPrivate;

class medVtkViewVtk4DMeshInteractor : public medVtkViewVtkMeshInteractor
{
    Q_OBJECT

public:
    medVtkViewVtk4DMeshInteractor(medAbstractView* parent);
    ~medVtkViewVtk4DMeshInteractor();

public:
    virtual QString description() const;

    virtual QString identifier() const;
    virtual QStringList handled() const;

    static bool registered();

    virtual void setInputData(medAbstractData *data);

public slots:
    virtual void setCurrentTime (double time);

private:
    static QStringList dataHandled();

private:
    medVtkViewVtk4DMeshInteractorPrivate *d;
};
