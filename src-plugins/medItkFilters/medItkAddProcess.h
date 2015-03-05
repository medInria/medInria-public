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

class medItkAddProcessPrivate;
class medItkFiltersPlugin_EXPORT medItkAddProcess : public medItkFiltersProcessBase
{
    Q_OBJECT

public:
    medItkAddProcess(medItkFiltersProcessBase *parent = NULL);
    virtual ~medItkAddProcess();

    static bool registered ();

protected:
    medItkAddProcessPrivate* d;

};

dtkAbstractProcess * createmedItkAddProcess();

