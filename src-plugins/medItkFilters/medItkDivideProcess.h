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

class medItkDivideProcessPrivate;
class medItkFiltersPlugin_EXPORT medItkDivideProcess : public medItkFiltersProcessBase
{
    Q_OBJECT

public:
    medItkDivideProcess(medItkFiltersProcessBase *parent = NULL);
    virtual ~medItkDivideProcess();

    static bool registered ();

protected:
    medItkDivideProcessPrivate* d;

};

dtkAbstractProcess * createmedItkDivideProcess();

