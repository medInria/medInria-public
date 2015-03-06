/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractFilteringProcess.h>
#include <medAbstractData.h>
#include <medItkFiltersProcessBase.h>

#include <medItkFiltersPluginExport.h>

class medItkFiltersPlugin_EXPORT medItkMedianProcess : public medItkFiltersProcessBase
{
    Q_OBJECT

public:
    medItkMedianProcess(medItkFiltersProcessBase *parent = NULL);
    virtual ~medItkMedianProcess();

    static bool registered ();
};

dtkAbstractProcess * createmedItkMedianProcess();

