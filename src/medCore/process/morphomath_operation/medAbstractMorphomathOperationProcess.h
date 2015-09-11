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

class medAbstractImageData;
class medViewContainerSplitter;

class medAbstractMorphomathOperationProcessPrivate;
class medAbstractMorphomathOperationProcess : public medAbstractProcess
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

protected:
    void setOutput(medAbstractImageData* data);

private slots:
    void _setInput();
    void _fillOutputContainer();

private:
    medAbstractMorphomathOperationProcessPrivate* d;
};
