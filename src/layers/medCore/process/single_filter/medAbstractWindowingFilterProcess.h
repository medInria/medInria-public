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
class medAbstractWindowingFilterProcessPrivate;

class MEDCORE_EXPORT  medAbstractWindowingFilterProcess : public medAbstractSingleFilterOperationProcess
{
    Q_OBJECT

public:
    medAbstractWindowingFilterProcess(QObject *parent = nullptr);
    virtual ~medAbstractWindowingFilterProcess();

    virtual medDoubleParameter* minimumIntensityValue();
    virtual medDoubleParameter* maximumIntensityValue();
    virtual medDoubleParameter* minimumOutputIntensityValue();
    virtual medDoubleParameter* maximumOutputIntensityValue();

protected:
    virtual QString outputNameAddon() const {return "windowing filter";}

private:
    const QScopedPointer<medAbstractWindowingFilterProcessPrivate> d;
};

DTK_DECLARE_PLUGIN_FACTORY(medAbstractWindowingFilterProcess, MEDCORE_EXPORT)
