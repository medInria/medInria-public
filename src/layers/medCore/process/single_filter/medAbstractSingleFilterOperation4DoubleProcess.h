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
class medDoubleParameter;

class medAbstractSingleFilterOperation4DoubleProcessPrivate;
class MEDCORE_EXPORT  medAbstractSingleFilterOperation4DoubleProcess : public medAbstractSingleFilterOperationProcess
{
    Q_OBJECT

public:
    medAbstractSingleFilterOperation4DoubleProcess(QObject *parent = NULL);
    virtual ~medAbstractSingleFilterOperation4DoubleProcess();

    virtual medDoubleParameter* w();
    virtual medDoubleParameter* x();
    virtual medDoubleParameter* y();
    virtual medDoubleParameter* z();

private:
    const QScopedPointer<medAbstractSingleFilterOperation4DoubleProcessPrivate> d;
};

DTK_DECLARE_PLUGIN        (medAbstractSingleFilterOperation4DoubleProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractSingleFilterOperation4DoubleProcess, MEDCORE_EXPORT)
