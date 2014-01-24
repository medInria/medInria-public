/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractDataImage.h>

#include "v3dDataPluginExport.h"

class v3dDataImagePrivate;

class V3DDATAPLUGIN_EXPORT v3dDataImage : public medAbstractDataImage
{
    Q_OBJECT

public:
             v3dDataImage();
    virtual ~v3dDataImage();

    virtual QString description() const;
    virtual QString identifier() const;
    static bool registered();

public:
    bool read(const QString& file);
    bool write(const QString& file);

    void *output();
    void *data();

    void setData(void *data);

public:
    // derived from medAbstractDataImage
    int xDimension();
    int yDimension();
    int zDimension();


private:
    v3dDataImagePrivate *d;
};

medAbstractData *createV3dDataImage();


