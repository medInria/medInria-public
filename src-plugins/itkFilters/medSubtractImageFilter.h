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
#include <itkFiltersProcessBase.h>

#include <itkFiltersPluginExport.h>

class medSubtractImageFilterPrivate;

class ITKFILTERSPLUGIN_EXPORT medSubtractImageFilter : public itkFiltersProcessBase
{
    Q_OBJECT

public:
    medSubtractImageFilter(itkFiltersProcessBase *parent = NULL);
    virtual ~medSubtractImageFilter();

    static bool registered ();

public:
    virtual int update ();
    QList<medAbstractParameter*> parameters();

private:
    medSubtractImageFilterPrivate *d;
};

dtkAbstractProcess * createimedSubtractImageFilter();
