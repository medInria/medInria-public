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

class medAbstractWindowingProcessPrivate;
class MEDCORE_EXPORT  medAbstractWindowingProcess : public medAbstractSingleFilterOperationProcess
{
    Q_OBJECT

public:
    medAbstractWindowingProcess(QObject *parent = NULL);
    virtual ~medAbstractWindowingProcess();

    virtual medDoubleParameter* minimumIntensityValue();
    virtual medDoubleParameter* maximumIntensityValue();
    virtual medDoubleParameter* minimumOutputIntensityValue();
    virtual medDoubleParameter* maximumOutputIntensityValue();


private:
    const QScopedPointer<medAbstractWindowingProcessPrivate> d;
};

DTK_DECLARE_PLUGIN_FACTORY(medAbstractWindowingProcess, MEDCORE_EXPORT)
