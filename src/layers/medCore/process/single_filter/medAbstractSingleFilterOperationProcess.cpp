/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
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
};

medAbstractSingleFilterOperationProcess::medAbstractSingleFilterOperationProcess(QObject *parent): medAbstractProcess(parent),
    d(new medAbstractSingleFilterOperationProcessPrivate)
{
    d->input = NULL;
    d->output = NULL;
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

medAbstractImageData* medAbstractSingleFilterOperationProcess::output() const
{
    return d->output;
}

