/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
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
    MED_DATA_INTERFACE("VTK Fiber Data",
                       "VTK Fiber Data")
public:
             medVtkFibersData();
    virtual ~medVtkFibersData();

    static bool registered();

public:
    void *data();

    void setData(void *data);

private:
    medVtkFibersDataPrivate *d;
};


