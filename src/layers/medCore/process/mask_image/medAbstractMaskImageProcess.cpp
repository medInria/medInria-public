/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractMaskImageProcess.h>

#include <medAbstractImageData.h>
#include <medMetaDataKeys.h>

class medAbstractMaskImageProcessPrivate
{
public:
    medAbstractImageData *input;
    medAbstractImageData *mask;
    medAbstractImageData *output;
};

medAbstractMaskImageProcess::medAbstractMaskImageProcess(QObject *parent)
    : medAbstractProcess(parent), d(new medAbstractMaskImageProcessPrivate)
{
    d->input = NULL;
    d->output = NULL;
    d->mask = NULL;
}

medAbstractMaskImageProcess::~medAbstractMaskImageProcess()
{

}

void medAbstractMaskImageProcess::setInput(medAbstractImageData *data)
{
    d->input = data;
}

medAbstractImageData* medAbstractMaskImageProcess::input() const
{
    return d->input;
}

void medAbstractMaskImageProcess::setMask(medAbstractImageData *data)
{
    d->mask = data;
}

medAbstractImageData* medAbstractMaskImageProcess::mask() const
{
    return d->mask;
}

medAbstractImageData* medAbstractMaskImageProcess::output() const
{
    return d->output;
}

void medAbstractMaskImageProcess::setOutput(medAbstractImageData *data)
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
