/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDiffusionModelEstimationMetaProcess.h>
#include <medAbstractImageData.h>
#include <medAbstractDiffusionModelImageData.h>
#include <medMetaDataKeys.h>

#include <medAbstractDiffusionModelEstimationProcess.h>
#include <medAbstractDWIMaskingProcess.h>
#include <medAbstractMaskImageProcess.h>
#include <medIntParameter.h>
#include <medCore.h>

class medDiffusionModelEstimationMetaProcessPrivate
{
public:
    medAbstractImageData *input;
    medAbstractDiffusionModelImageData *output;

    medAbstractDWIMaskingProcess *dwiMaskCalculator;
    medAbstractMaskImageProcess *dwiMaskApplyer;
    medAbstractDiffusionModelEstimationProcess *modelEstimator;
};

medDiffusionModelEstimationMetaProcess::medDiffusionModelEstimationMetaProcess(QObject *parent)
    : medAbstractProcess(parent), d(new medDiffusionModelEstimationMetaProcessPrivate)
{
    d->input = nullptr;
    d->output = nullptr;

    d->dwiMaskCalculator = nullptr;
    d->dwiMaskApplyer = medCore::maskImage::pluginFactory().create("medItkMaskImageProcess");
    d->modelEstimator = nullptr;
}

medDiffusionModelEstimationMetaProcess::~medDiffusionModelEstimationMetaProcess()
{
}

void medDiffusionModelEstimationMetaProcess::setInput(medAbstractImageData *data)
{
    if (!data)
        return;

    if (data->Dimension() != 4)
        return;

    d->input = data;
}

medAbstractImageData* medDiffusionModelEstimationMetaProcess::input() const
{
    return d->input;
}

medAbstractDiffusionModelImageData* medDiffusionModelEstimationMetaProcess::output() const
{
    return d->output;
}

void medDiffusionModelEstimationMetaProcess::setOutput(medAbstractDiffusionModelImageData *data)
{
    d->output = data;

    QString newSeriesDescription = d->input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " " + this->outputNameAddon();

    if (!d->output->hasMetaData(medMetaDataKeys::SeriesDescription.key()))
        d->output->setMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

    foreach ( QString metaData, d->input->metaDataList() )
    {
        d->output->setMetaData ( metaData, d->input->metaDataValues ( metaData ) );
    }

    foreach ( QString property, d->input->propertyList() )
        d->output->addProperty ( property,d->input->propertyValues ( property ) );
}

QStringList medDiffusionModelEstimationMetaProcess::dwiMaskingPlugins() const
{
    QStringList pluginsList;

    foreach(QString pluginKey, medCore::dwiMasking::pluginFactory().keys())
        pluginsList << pluginKey;

    return pluginsList;
}

QStringList medDiffusionModelEstimationMetaProcess::modelEstimationPlugins() const
{
    QStringList pluginsList;

    foreach(QString pluginKey, medCore::diffusionModelEstimation::pluginFactory().keys())
        pluginsList << pluginKey;

    return pluginsList;
}

bool medDiffusionModelEstimationMetaProcess::chooseDWIMaskingPlugin(QString processName)
{
    if (d->dwiMaskCalculator)
    {
        if (d->dwiMaskCalculator->caption() == processName)
            return false;
    }

    d->dwiMaskCalculator = medCore::dwiMasking::pluginFactory().create(processName);

    return (d->dwiMaskCalculator != 0);
}

bool medDiffusionModelEstimationMetaProcess::chooseModelEstimationPlugin(QString processName)
{
    if (d->modelEstimator)
    {
        if (d->modelEstimator->caption() == processName)
            return false;
    }

    d->modelEstimator = medCore::diffusionModelEstimation::pluginFactory().create(processName);

    return (d->modelEstimator != 0);
}

medAbstractDWIMaskingProcess *medDiffusionModelEstimationMetaProcess::dwiMaskCalculatorProcess()
{
    return d->dwiMaskCalculator;
}

medAbstractDiffusionModelEstimationProcess *medDiffusionModelEstimationMetaProcess::modelEstimationProcess()
{
    return d->modelEstimator;
}

medAbstractJob::medJobExitStatus medDiffusionModelEstimationMetaProcess::run()
{
    if (!d->modelEstimator)
    {
        qDebug() << "Diffusion error: no model estimator";
        return medAbstractJob::MED_JOB_EXIT_FAILURE;
    }

    if (d->dwiMaskCalculator && !d->dwiMaskApplyer)
    {
        qDebug() << "Diffusion error: no mask application process";
        return medAbstractJob::MED_JOB_EXIT_FAILURE;
    }

    d->modelEstimator->setInput(d->input);
    medAbstractJob::medJobExitStatus returnValue;

    if (d->dwiMaskCalculator)
    {
        connect(d->dwiMaskCalculator->progression(),SIGNAL(valueChanged(int)),
                this,SLOT(updateMaskCalculatorProgression(int)));

        d->dwiMaskCalculator->setInput(d->input);

        returnValue = d->dwiMaskCalculator->run();
        if (returnValue != medAbstractJob::MED_JOB_EXIT_SUCCESS)
            return returnValue;

        this->progression()->setValue(50);

        d->dwiMaskApplyer->setInput(d->input);
        d->dwiMaskApplyer->setMask(d->dwiMaskCalculator->output());

        returnValue = d->dwiMaskApplyer->run();
        if (returnValue != medAbstractJob::MED_JOB_EXIT_SUCCESS)
            return returnValue;

        d->modelEstimator->setInput(d->dwiMaskApplyer->output());
    }

    connect(d->modelEstimator->progression(),SIGNAL(valueChanged(int)),
            this,SLOT(updateModelEstimationProgression(int)));
    returnValue = d->modelEstimator->run();
    if (returnValue != medAbstractJob::MED_JOB_EXIT_SUCCESS)
    {
        qDebug() << "Diffusion error: model estimation fail";
        return returnValue;
    }

    this->progression()->setValue(100);
    this->setOutput(d->modelEstimator->output());

    return medAbstractJob::MED_JOB_EXIT_SUCCESS;
}

QString medDiffusionModelEstimationMetaProcess::caption() const
{
    return "DWI masking and model estimation";
}

QString medDiffusionModelEstimationMetaProcess::description() const
{
    return "Combine several plugins to perform diffusion model estimation on masked images.";
}

void medDiffusionModelEstimationMetaProcess::updateMaskCalculatorProgression(int value)
{
    this->progression()->setValue(value / 2);
}

void medDiffusionModelEstimationMetaProcess::updateModelEstimationProgression(int value)
{
    if (d->dwiMaskCalculator)
        this->progression()->setValue(50 + value / 2);
    else
        this->progression()->setValue(value);
}

void medDiffusionModelEstimationMetaProcess::cancel()
{
    if(d->dwiMaskCalculator)
    {
        if (d->dwiMaskCalculator->isRunning())
            d->dwiMaskCalculator->cancel();
    }

    if(d->dwiMaskApplyer)
    {
        if (d->dwiMaskApplyer->isRunning())
            d->dwiMaskApplyer->cancel();
    }

    if(d->modelEstimator)
    {
        if (d->modelEstimator->isRunning())
            d->modelEstimator->cancel();
    }
}
