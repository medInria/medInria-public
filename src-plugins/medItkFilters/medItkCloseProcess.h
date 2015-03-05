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

class medItkCloseProcessPrivate;
class medItkFiltersPlugin_EXPORT medItkCloseProcess : public medItkFiltersProcessBase
{
    Q_OBJECT

public:
    medItkCloseProcess(medItkFiltersProcessBase *parent = NULL);
    virtual ~medItkCloseProcess();

    static bool registered ();

protected:
    medItkCloseProcessPrivate* d;

};

dtkAbstractProcess * createmedItkCloseProcess();

