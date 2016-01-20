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

class medAbstractSingleFilterOperationDoubleProcessPrivate;
class MEDCORE_EXPORT  medAbstractSingleFilterOperationDoubleProcess : public medAbstractSingleFilterOperationProcess
{
    Q_OBJECT

public:
    medAbstractSingleFilterOperationDoubleProcess(QObject *parent = NULL);
    virtual ~medAbstractSingleFilterOperationDoubleProcess();

    virtual medDoubleParameter* sigma() const;

private:
    const QScopedPointer<medAbstractSingleFilterOperationDoubleProcessPrivate> d;
};

DTK_DECLARE_PLUGIN        (medAbstractSingleFilterOperationDoubleProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractSingleFilterOperationDoubleProcess, MEDCORE_EXPORT)
