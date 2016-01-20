/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSingleFilterOperationProcess.h>

#include <dtkCore>

#include <medCoreExport.h>

class medAbstractImageData;
class medIntParameter;

class medAbstractSingleFilterOperation3IntProcessPrivate;
class MEDCORE_EXPORT  medAbstractSingleFilterOperation3IntProcess : public medAbstractSingleFilterOperationProcess
{
    Q_OBJECT

public:
    medAbstractSingleFilterOperation3IntProcess(QObject *parent = NULL);
    virtual ~medAbstractSingleFilterOperation3IntProcess();

    virtual medIntParameter* x();
    virtual medIntParameter* y();
    virtual medIntParameter* z();

private:
    const QScopedPointer<medAbstractSingleFilterOperation3IntProcessPrivate> d;
};

DTK_DECLARE_PLUGIN        (medAbstractSingleFilterOperation3IntProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractSingleFilterOperation3IntProcess, MEDCORE_EXPORT)
