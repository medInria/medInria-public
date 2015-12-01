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
class medAbstractDiffusionModelImageData;

class medAbstractDiffusionModelEstimationProcessPrivate;
class MEDCORE_EXPORT medAbstractDiffusionModelEstimationProcess : public medAbstractProcess
{
    Q_OBJECT

public:
    medAbstractDiffusionModelEstimationProcess(QObject *parent = NULL);
    virtual ~medAbstractDiffusionModelEstimationProcess();

    void setInput(medAbstractImageData* data);
    medAbstractImageData* input() const;

    void setMask(medAbstractImageData* mask);
    medAbstractImageData* mask() const;

    medAbstractDiffusionModelImageData* output() const;

protected:
    void setOutput(medAbstractDiffusionModelImageData* data);

private:
    const QScopedPointer<medAbstractDiffusionModelEstimationProcessPrivate> d;
};

DTK_DECLARE_PLUGIN        (medAbstractDiffusionModelEstimationProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractDiffusionModelEstimationProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractDiffusionModelEstimationProcess, MEDCORE_EXPORT)
