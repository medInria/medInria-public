/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
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
class medAbstractDiffusionModelImageData;

class medAbstractDiffusionScalarMapsProcessPrivate;
class MEDCORE_EXPORT medAbstractDiffusionScalarMapsProcess : public medAbstractProcess
{
    Q_OBJECT

public:
    medAbstractDiffusionScalarMapsProcess(QObject *parent = NULL);
    virtual ~medAbstractDiffusionScalarMapsProcess();

    void setInput(medAbstractDiffusionModelImageData* data);
    medAbstractDiffusionModelImageData* input() const;

    medAbstractImageData* output() const;

protected:
    void setOutput(medAbstractImageData* data);
    virtual QString outputNameAddon() const {return "scalar map";}

private:
    const QScopedPointer<medAbstractDiffusionScalarMapsProcessPrivate> d;
};

DTK_DECLARE_PLUGIN        (medAbstractDiffusionScalarMapsProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractDiffusionScalarMapsProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractDiffusionScalarMapsProcess, MEDCORE_EXPORT)
