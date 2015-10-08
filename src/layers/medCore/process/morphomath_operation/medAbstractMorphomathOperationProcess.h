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
class medDoubleParameter;

class medAbstractMorphomathOperationProcessPrivate;
class MEDCORE_EXPORT medAbstractMorphomathOperationProcess : public medAbstractProcess
{
    Q_OBJECT

public:
    medAbstractMorphomathOperationProcess(QObject *parent = NULL);
    virtual ~medAbstractMorphomathOperationProcess();

    void setInput(medAbstractImageData* data);
    medAbstractImageData* input() const;

    medAbstractImageData* output() const;

    virtual QWidget* toolbox() const;
    virtual medViewContainerSplitter* viewContainerSplitter() const;

    virtual medDoubleParameter* kernelRadius() const;
    virtual void setKernelRadius(double const& radius);

protected:
    void setOutput(medAbstractImageData* data);

private slots:
    void _setInputFromContainer();
    void _fillOutputContainer();

private:
    medAbstractMorphomathOperationProcessPrivate* d;
};
