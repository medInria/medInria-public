/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSingleFilterOperationProcess.h>

#include <medAbstractImageData.h>
#include <medMetaDataKeys.h>

#include <medDoubleParameter.h>

class medAbstractSingleFilterOperationProcessPrivate
{
public:
    medAbstractImageData *input;
    medAbstractImageData *output;
    medDoubleParameter* sigma;
};

medAbstractSingleFilterOperationProcess::medAbstractSingleFilterOperationProcess(QObject *parent): medAbstractProcess(parent),
    d(new medAbstractSingleFilterOperationProcessPrivate)
{
    d->input = NULL;
    d->output = NULL;

    d->sigma = new medDoubleParameter("sigma", this);
    d->sigma->setCaption("Kernel radius");
    d->sigma->setDescription("Set the radius in all directions of the kernel used as structuring element");
    d->sigma->setValue(1);
}

medAbstractSingleFilterOperationProcess::~medAbstractSingleFilterOperationProcess()
{

}

void medAbstractSingleFilterOperationProcess::setInput(medAbstractImageData *data)
{
    d->input = data;
}

medAbstractImageData* medAbstractSingleFilterOperationProcess::input() const
{
    return d->input;
}

void medAbstractSingleFilterOperationProcess::setOutput(medAbstractImageData *data)
{
    d->output = data;

    QString newSeriesDescription = d->input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " single filter";

    if (!d->output->hasMetaData(medMetaDataKeys::SeriesDescription.key()))
        d->output->setMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

    foreach ( QString metaData, d->input->metaDataList() )
    {
        if (!d->output->hasMetaData(metaData))
            d->output->addMetaData ( metaData, d->input->metaDataValues ( metaData ) );
    }

    foreach ( QString property, d->input->propertyList() )
        d->output->addProperty ( property,d->input->propertyValues ( property ) );

}

medDoubleParameter* medAbstractSingleFilterOperationProcess::sigma() const
{
    return d->sigma;
}

medAbstractImageData* medAbstractSingleFilterOperationProcess::output() const
{
    return d->output;
}

