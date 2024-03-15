/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractTractographyProcess.h>
#include <medAbstractFibersData.h>
#include <medAbstractDiffusionModelImageData.h>
#include <medMetaDataKeys.h>

class medAbstractTractographyProcessPrivate
{
public:
    medAbstractDiffusionModelImageData *input;
    medAbstractFibersData *output;
};

medAbstractTractographyProcess::medAbstractTractographyProcess(QObject *parent)
    : medAbstractProcess(parent), d(new medAbstractTractographyProcessPrivate)
{
    d->input = nullptr;
    d->output = nullptr;
}

medAbstractTractographyProcess::~medAbstractTractographyProcess()
{

}

void medAbstractTractographyProcess::setInput(medAbstractDiffusionModelImageData *data)
{
    d->input = data;
}

medAbstractDiffusionModelImageData* medAbstractTractographyProcess::input() const
{
    return d->input;
}

medAbstractFibersData* medAbstractTractographyProcess::output() const
{
    return d->output;
}

void medAbstractTractographyProcess::setOutput(medAbstractFibersData *data)
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
