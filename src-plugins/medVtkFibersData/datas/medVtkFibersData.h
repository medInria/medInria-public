/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractFibersData.h>

#include <medVtkFibersDataPluginExport.h>

class medVtkFibersDataPrivate;

class MEDVTKFIBERSDATAPLUGIN_EXPORT medVtkFibersData : public medAbstractFibersData
{
    Q_OBJECT

public:
             medVtkFibersData();
    virtual ~medVtkFibersData();

    virtual QString description() const;
    virtual QString identifier() const;
    static bool registered();

public:
    void *data();

    void setData(void *data);

private:
    medVtkFibersDataPrivate *d;
};

medAbstractData *create_medVtkFibersData();


