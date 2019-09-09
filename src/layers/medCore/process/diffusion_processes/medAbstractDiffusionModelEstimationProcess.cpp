/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDiffusionModelEstimationProcess.h>
#include <medAbstractImageData.h>
#include <medAbstractDiffusionModelImageData.h>
#include <medMetaDataKeys.h>
#include <medBoolParameter.h>

class medAbstractDiffusionModelEstimationProcessPrivate
{
public:
    medAbstractImageData *input;
    medAbstractDiffusionModelImageData *output;

    medAbstractDiffusionModelEstimationProcess::VectorType bvalues;
    medAbstractDiffusionModelEstimationProcess::GradientsVectorType gradients;

    QString gradientsFileName;
    medBoolParameter *gradientsInImageCoordinates;
};

medAbstractDiffusionModelEstimationProcess::medAbstractDiffusionModelEstimationProcess(QObject *parent)
    : medAbstractProcess(parent), d(new medAbstractDiffusionModelEstimationProcessPrivate)
{
    d->input = NULL;
    d->output = NULL;

    d->gradientsInImageCoordinates = new medBoolParameter("grad_coords",this);
    d->gradientsInImageCoordinates->setCaption("Gradients in image coordinates");
    d->gradientsInImageCoordinates->setDescription("Are gradients in provided file in line with image coordinates?");
    d->gradientsInImageCoordinates->setValue(true);
}

medAbstractDiffusionModelEstimationProcess::~medAbstractDiffusionModelEstimationProcess()
{
}

void medAbstractDiffusionModelEstimationProcess::setInput(medAbstractImageData *data)
{
    if (!data)
        return;

    if (data->Dimension() != 4)
        return;

    d->input = data;
}

void medAbstractDiffusionModelEstimationProcess::setGradients(QString fileName)
{
    d->gradientsFileName = fileName;
}

void medAbstractDiffusionModelEstimationProcess::extractGradientsFromInformation()
{
    // Requires input to be here
    medDiffusionGradientReader grReader;
    grReader.readGradients(d->gradientsFileName);

    d->gradients = grReader.gradients();

    if (!d->gradientsInImageCoordinates->value())
    {
        medAbstractImageData::MatrixType orientationMatrix;
        orientationMatrix = d->input->orientationMatrix();

        VectorType grad(3,0);
        for (unsigned int i = 0;i < d->gradients.size();++i)
        {
            for (unsigned int j = 0;j < 3;++j)
            {
                grad[j] = 0;
                for (unsigned int k = 0;k < 3;++k)
                    grad[j] += orientationMatrix[k][j] * d->gradients[i][k];
            }

            d->gradients[i] = grad;
        }
    }
}

void medAbstractDiffusionModelEstimationProcess::setBValues(QString fileName)
{
    medDiffusionGradientReader grReader;
    grReader.readBValues(fileName);

    d->bvalues = grReader.bvalues();
}

medAbstractImageData* medAbstractDiffusionModelEstimationProcess::input() const
{
    return d->input;
}

medAbstractDiffusionModelImageData* medAbstractDiffusionModelEstimationProcess::output() const
{
    return d->output;
}

medAbstractDiffusionModelEstimationProcess::VectorType medAbstractDiffusionModelEstimationProcess::bvalues() const
{
    return d->bvalues;
}

medAbstractDiffusionModelEstimationProcess::GradientsVectorType medAbstractDiffusionModelEstimationProcess::gradients() const
{
    return d->gradients;
}

medBoolParameter *medAbstractDiffusionModelEstimationProcess::gradientsInImageCoordinates() const
{
    return d->gradientsInImageCoordinates;
}

void medAbstractDiffusionModelEstimationProcess::setOutput(medAbstractDiffusionModelImageData *data)
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
