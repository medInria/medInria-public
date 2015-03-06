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

class medItkInvertProcessPrivate;
class medItkFiltersPlugin_EXPORT medItkInvertProcess : public medItkFiltersProcessBase
{
    Q_OBJECT

public:
    medItkInvertProcess(medItkFiltersProcessBase *parent = NULL);
    virtual ~medItkInvertProcess();

    static bool registered ();
};

dtkAbstractProcess * createmedItkInvertProcess();

