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

class medItkSubtractImageFilterPrivate;

class medItkFiltersPlugin_EXPORT medItkSubtractImageFilter : public medItkFiltersProcessBase
{
    Q_OBJECT

public:
    medItkSubtractImageFilter(medItkFiltersProcessBase *parent = NULL);
    virtual ~medItkSubtractImageFilter();

    static bool registered ();

public:
    virtual int update ();
    QList<medAbstractParameter*> parameters();

private:
    medItkSubtractImageFilterPrivate *d;
};

dtkAbstractProcess * createimedItkSubtractImageFilter();
