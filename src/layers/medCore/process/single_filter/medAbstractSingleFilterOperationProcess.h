#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractProcess.h>
#include <medCoreExport.h>

#include <dtkCore>

class medAbstractImageData;
class medDoubleParameter;
class medAbstractSingleFilterOperationProcessPrivate;

class MEDCORE_EXPORT  medAbstractSingleFilterOperationProcess : public medAbstractProcess
{
    Q_OBJECT

public:
    medAbstractSingleFilterOperationProcess(QObject *parent = nullptr);
    virtual ~medAbstractSingleFilterOperationProcess();

    void setInput(medAbstractImageData* data);
    medAbstractImageData* input() const;

    medAbstractImageData* output() const;

protected:
    void setOutput(medAbstractImageData* data);
    virtual QString outputNameAddon() const {return "single filter";}

private:
    const QScopedPointer<medAbstractSingleFilterOperationProcessPrivate> d;
};

DTK_DECLARE_PLUGIN        (medAbstractSingleFilterOperationProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractSingleFilterOperationProcess, MEDCORE_EXPORT)
