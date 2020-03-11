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
#include <medDiffusionGradientReader.h>

#include <dtkCore>

class medAbstractImageData;
class medAbstractDiffusionModelImageData;
class medAbstractDiffusionModelEstimationProcess;
class medAbstractDWIMaskingProcess;
class medDiffusionModelEstimationMetaProcessPrivate;

class MEDCORE_EXPORT medDiffusionModelEstimationMetaProcess : public medAbstractProcess
{
    Q_OBJECT

public:
    medDiffusionModelEstimationMetaProcess(QObject *parent = nullptr);
    virtual ~medDiffusionModelEstimationMetaProcess();

    void setInput(medAbstractImageData* data);
    medAbstractImageData* input() const;

    medAbstractDiffusionModelImageData* output() const;

    virtual medAbstractJob::medJobExitStatus run();
    virtual void cancel();

    virtual QString caption() const;
    virtual QString description() const;

    QStringList dwiMaskingPlugins() const;
    QStringList modelEstimationPlugins() const;

    bool chooseDWIMaskingPlugin(QString processName);
    bool chooseModelEstimationPlugin(QString processName);

    medAbstractDWIMaskingProcess *dwiMaskCalculatorProcess();
    medAbstractDiffusionModelEstimationProcess *modelEstimationProcess();

public slots:
    void updateMaskCalculatorProgression(int value);
    void updateModelEstimationProgression(int value);

protected:
    void setOutput(medAbstractDiffusionModelImageData* data);
    virtual QString outputNameAddon() const {return "models";}

private:
    const QScopedPointer<medDiffusionModelEstimationMetaProcessPrivate> d;
};
