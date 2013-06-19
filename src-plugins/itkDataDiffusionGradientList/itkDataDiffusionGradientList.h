/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/




#pragma once

#include <dtkCore/dtkAbstractData.h>

#include "itkDataDiffusionGradientListPluginExport.h"

class itkDataDiffusionGradientListPrivate;

class ITKDATADIFFUSIONGRADIENTLISTPLUGIN_EXPORT itkDataDiffusionGradientList : public dtkAbstractData
{
    Q_OBJECT

public:
             itkDataDiffusionGradientList();
    virtual ~itkDataDiffusionGradientList();

    virtual QString description() const;

    static bool registered();

    virtual void *data();
    virtual void *data(int channel);
    virtual void setData(void *data);

public slots:
    bool read (const QString& file);
    bool write(const QString& file);
    
private:
    itkDataDiffusionGradientListPrivate *d;
};

dtkAbstractData *createItkDataDiffusionGradientList();


