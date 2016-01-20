/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractProcess.h>

#include <dtkCore>

#include <medCoreExport.h>

class medAbstractImageData;

class medAbstractArithmeticOperationProcessPrivate;
class MEDCORE_EXPORT  medAbstractArithmeticOperationProcess : public medAbstractProcess
{
    Q_OBJECT

public:
    medAbstractArithmeticOperationProcess(QObject *parent = NULL);
    virtual ~medAbstractArithmeticOperationProcess();

    void setInput1(medAbstractImageData* data);
    medAbstractImageData* input1() const;

    void setInput2(medAbstractImageData* data);
    medAbstractImageData* input2() const;

    medAbstractImageData* output() const;

protected:
    void setOutput(medAbstractImageData* data);

private:
    const QScopedPointer<medAbstractArithmeticOperationProcessPrivate> d;
};

DTK_DECLARE_PLUGIN        (medAbstractArithmeticOperationProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractArithmeticOperationProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractArithmeticOperationProcess, MEDCORE_EXPORT)
