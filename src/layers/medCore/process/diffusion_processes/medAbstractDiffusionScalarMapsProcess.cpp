/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDiffusionScalarMapsProcess.h>
#include <medAbstractImageData.h>
#include <medAbstractDiffusionModelImageData.h>
#include <medMetaDataKeys.h>

class medAbstractDiffusionScalarMapsProcessPrivate
{
public:
    medAbstractDiffusionModelImageData *input;
    medAbstractImageData *output;
};

medAbstractDiffusionScalarMapsProcess::medAbstractDiffusionScalarMapsProcess(QObject *parent)
    : medAbstractProcess(parent), d(new medAbstractDiffusionScalarMapsProcessPrivate)
{
    d->input = nullptr;
    d->output = nullptr;
}

medAbstractDiffusionScalarMapsProcess::~medAbstractDiffusionScalarMapsProcess()
{

}

void medAbstractDiffusionScalarMapsProcess::setInput(medAbstractDiffusionModelImageData *data)
{
    d->input = data;
}

medAbstractDiffusionModelImageData* medAbstractDiffusionScalarMapsProcess::input() const
{
    return d->input;
}

medAbstractImageData* medAbstractDiffusionScalarMapsProcess::output() const
{
    return d->output;
}

void medAbstractDiffusionScalarMapsProcess::setOutput(medAbstractImageData *data)
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
