/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDWIMaskingProcess.h>

#include <medAbstractImageData.h>
#include <medMetaDataKeys.h>

class medAbstractDWIMaskingProcessPrivate
{
public:
    medAbstractImageData *input;
    medAbstractImageData *output;
};

medAbstractDWIMaskingProcess::medAbstractDWIMaskingProcess(QObject *parent)
    : medAbstractProcess(parent), d(new medAbstractDWIMaskingProcessPrivate)
{
    d->input = NULL;
    d->output = NULL;
}

medAbstractDWIMaskingProcess::~medAbstractDWIMaskingProcess()
{

}

void medAbstractDWIMaskingProcess::setInput(medAbstractImageData *data)
{
    if (!data)
        return;

    if (data->Dimension() != 4)
        return;

    d->input = data;
}

medAbstractImageData* medAbstractDWIMaskingProcess::input() const
{
    return d->input;
}

medAbstractImageData* medAbstractDWIMaskingProcess::output() const
{
    return d->output;
}

void medAbstractDWIMaskingProcess::setOutput(medAbstractImageData *data)
{
    if (data->Dimension() != 3)
        return;

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
