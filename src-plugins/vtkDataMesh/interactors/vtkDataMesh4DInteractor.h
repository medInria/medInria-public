/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <vtkDataMeshInteractor.h>

class vtkDataMesh4DInteractorPrivate;

class vtkDataMesh4DInteractor : public vtkDataMeshInteractor
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


public slots:
    virtual void setCurrentTime (const double &time);

private:
    static QStringList dataHandled();

private:
    vtkDataMesh4DInteractorPrivate *d;
};
