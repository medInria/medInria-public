/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkDataMeshPluginExport.h"

#include <vtkDataMeshInteractor.h>

class vtkDataMesh4DInteractorPrivate;

class VTKDATAMESHPLUGIN_EXPORT vtkDataMesh4DInteractor : public vtkDataMeshInteractor
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

    virtual void setInputData(medAbstractData *data);

public slots:
    virtual void setCurrentTime (double time);

private:
    static QStringList dataHandled();

private:
    vtkDataMesh4DInteractorPrivate *d;
};
