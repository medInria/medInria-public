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

#include <medCoreExport.h>

class medAbstractImageData;
class medViewContainerSplitter;

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

    virtual QWidget* toolbox() const;
    virtual medViewContainerSplitter* viewContainerSplitter() const;

protected:
    void setOutput(medAbstractImageData* data);

private slots:
    void _setInput1FromContainer();
    void _setInput2FromContainer();
    void _fillOutputContainer();

private:
    medAbstractArithmeticOperationProcessPrivate* d;
};
