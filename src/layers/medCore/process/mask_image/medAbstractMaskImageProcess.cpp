/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
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
    d->input = nullptr;
    d->output = nullptr;
    d->mask = nullptr;
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

    QString newSeriesDescription = d->input->metadata ( medMetaDataKeys::key("SeriesDescription") );
    newSeriesDescription += " " + this->outputNameAddon();

    if (!d->output->hasMetaData(medMetaDataKeys::key("SeriesDescription")))
        d->output->setMetaData ( medMetaDataKeys::key("SeriesDescription"), newSeriesDescription );

    for( QString metaData : d->input->metaDataList() )
    {
        d->output->setMetaData ( metaData, d->input->metaDataValues ( metaData ) );
    }

    for( QString property : d->input->propertyList() )
    {
        d->output->addProperty ( property,d->input->propertyValues ( property ) );
    }
}
