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

#include "v3dDataPluginExport.h"

class v3dDataFibersPrivate;

class V3DDATAPLUGIN_EXPORT v3dDataFibers : public dtkAbstractData
{
    Q_OBJECT

public:
             v3dDataFibers();
    virtual ~v3dDataFibers();

    virtual QString description() const;
    virtual QString identifier() const;
    static bool registered();

    virtual QImage        &thumbnail() const;
    virtual QList<QImage> &thumbnails() const;

public:
    void *data();

    void setData(void *data);

protected:
    void generateThumbnails() const;

private:
    v3dDataFibersPrivate *d;
};

dtkAbstractData *createV3dDataFibers();


