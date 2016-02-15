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
class medIntParameter;

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

    virtual medIntParameter* kernelRadius() const;

protected:
    void setOutput(medAbstractImageData* data);

private:
    const QScopedPointer<medAbstractMorphomathOperationProcessPrivate> d;
};

DTK_DECLARE_PLUGIN        (medAbstractMorphomathOperationProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractMorphomathOperationProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractMorphomathOperationProcess, MEDCORE_EXPORT)
