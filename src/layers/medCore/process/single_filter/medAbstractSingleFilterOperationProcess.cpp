/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSingleFilterOperationProcess.h>
#include <medAbstractImageData.h>
#include <medDoubleParameter.h>
#include <medMetaDataKeys.h>

class medAbstractSingleFilterOperationProcessPrivate
{
public:
    medAbstractImageData *input;
    medAbstractImageData *output;
};

medAbstractSingleFilterOperationProcess::medAbstractSingleFilterOperationProcess(QObject *parent): medAbstractProcess(parent),
    d(new medAbstractSingleFilterOperationProcessPrivate)
{
    d->input = nullptr;
    d->output = nullptr;
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

    for( QString metaData : d->input->metaDataList() )
    {
        d->output->setMetaData ( metaData, d->input->metaDataValues ( metaData ) );
    }

    for( QString property : d->input->propertyList() )
    {
        d->output->addProperty ( property,d->input->propertyValues ( property ) );
    }
}

medAbstractImageData* medAbstractSingleFilterOperationProcess::output() const
{
    return d->output;
}

