#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractProcess.h>
#include <medCoreExport.h>

#include <dtkCore>

class medAbstractImageData;
class medIntParameter;

class medAbstractMorphomathOperationProcessPrivate;
class MEDCORE_EXPORT medAbstractMorphomathOperationProcess : public medAbstractProcess
{
    Q_OBJECT

public:
    medAbstractMorphomathOperationProcess(QObject *parent = nullptr);
    virtual ~medAbstractMorphomathOperationProcess();

    void setInput(medAbstractImageData* data);
    medAbstractImageData* input() const;
    medAbstractImageData* output() const;

    virtual medIntParameter* kernelRadius() const;

protected:
    void setOutput(medAbstractImageData* data);
    virtual QString outputNameAddon() const {return "morpho";}

private:
    const QScopedPointer<medAbstractMorphomathOperationProcessPrivate> d;
};

DTK_DECLARE_PLUGIN        (medAbstractMorphomathOperationProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractMorphomathOperationProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractMorphomathOperationProcess, MEDCORE_EXPORT)
