#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSingleFilterOperationProcess.h>
#include <medCoreExport.h>

#include <dtkCore>

class medAbstractImageData;
class medDoubleParameter;
class medAbstractSingleFilterOperationDoubleProcessPrivate;

class MEDCORE_EXPORT  medAbstractSingleFilterOperationDoubleProcess : public medAbstractSingleFilterOperationProcess
{
    Q_OBJECT

public:
    medAbstractSingleFilterOperationDoubleProcess(QObject *parent = nullptr);
    virtual ~medAbstractSingleFilterOperationDoubleProcess();

    virtual medDoubleParameter* doubleParameter() const;

private:
    const QScopedPointer<medAbstractSingleFilterOperationDoubleProcessPrivate> d;
};
