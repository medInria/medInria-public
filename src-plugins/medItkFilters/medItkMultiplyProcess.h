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

class medItkMultiplyProcessPrivate;
class medItkFiltersPlugin_EXPORT medItkMultiplyProcess : public medItkFiltersProcessBase
{
    Q_OBJECT

public:
    medItkMultiplyProcess(medItkFiltersProcessBase *parent = NULL);
    virtual ~medItkMultiplyProcess();

    static bool registered ();

protected:
    medItkMultiplyProcessPrivate* d;

};

dtkAbstractProcess * createmedItkMultiplyProcess();

