/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkProcessTensorTractography.h"

#include <dtkCore/dtkSmartPointer.h>
#include <medAbstractData.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkProcessTensorLogEuclideanTractography.h>

#include <medMetaDataKeys.h>

// /////////////////////////////////////////////////////////////////
// itkProcessTensorTractographyPrivate
// /////////////////////////////////////////////////////////////////

class itkProcessTensorTractographyPrivate
{
public:
    dtkSmartPointer <medAbstractData> input;
    dtkSmartPointer <medAbstractData> output;
    
    dtkSmartPointer <itkProcessTensorLogEuclideanTractography> tractographyFilter;

    // Parameters
    double FA1Value;
    double FA2Value;
    double smoothnessValue;
    double samplingValue;
    double minLengthValue;
};

// /////////////////////////////////////////////////////////////////
// itkProcessTensorTractography
// /////////////////////////////////////////////////////////////////

itkProcessTensorTractography::itkProcessTensorTractography() : medAbstractDiffusionProcess(), d(new itkProcessTensorTractographyPrivate)
{
    d->FA1Value = 0.3;
    d->FA2Value = 0.2;
    d->smoothnessValue = 0;
    d->samplingValue = 1;
    d->minLengthValue = 10;
}

itkProcessTensorTractography::~itkProcessTensorTractography()
{
    delete d;
    d = 0;
}

bool itkProcessTensorTractography::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkProcessTensorTractography", createItkProcessTensorTractography);
}

QString itkProcessTensorTractography::identifier() const
{
    return "itkProcessTensorTractography";
}

QString itkProcessTensorTractography::description() const
{
    return "itkProcessTensorTractography";
}

void itkProcessTensorTractography::setInputImage(medAbstractData *input)
{
    if (!input)
        return;
    
    d->input = input;
}

void itkProcessTensorTractography::setFA1Value (double value)
{
    d->FA1Value = value;
}

void itkProcessTensorTractography::setFA2Value (double value)
{
    d->FA2Value = value;
}

void itkProcessTensorTractography::setSmoothnessValue (double value)
{
    d->smoothnessValue = value;
}

void itkProcessTensorTractography::setSamplingValue (int value)
{
    d->samplingValue = value;
}

void itkProcessTensorTractography::setMinLengthValue (int value)
{
    d->minLengthValue = value;
}

int itkProcessTensorTractography::update()
{
    // Here call sub-processes
    d->tractographyFilter = new itkProcessTensorLogEuclideanTractography;
    
    // Set parameters
    
    d->tractographyFilter->setParameter(d->FA1Value,4);
    d->tractographyFilter->setParameter(d->FA2Value,5);
    d->tractographyFilter->setParameter(d->smoothnessValue,6);
    d->tractographyFilter->setParameter(d->samplingValue,7);
    d->tractographyFilter->setParameter(d->minLengthValue,9);
    
    d->tractographyFilter->setInputImage(d->input);
    
    connect (d->tractographyFilter, SIGNAL (progressed (int)), this, SIGNAL (progressed (int)));

    int retval = d->tractographyFilter->update();
    if (retval != 0)
        return retval;
    
    d->output = d->tractographyFilter->output();

    QString newSeriesDescription = d->input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " fibers";
    d->output->setMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    
    foreach ( QString metaData, d->input->metaDataList() )
    {
        if (!d->output->hasMetaData(metaData))
            d->output->addMetaData ( metaData, d->input->metaDataValues ( metaData ) );
    }
    
    foreach ( QString property, d->input->propertyList() )
    d->output->addProperty ( property,d->input->propertyValues ( property ) );
    
    QString generatedID = QUuid::createUuid().toString().replace("{","").replace("}","");
    d->output->setMetaData ( medMetaDataKeys::SeriesID.key(), generatedID );
    
    emit progressed(100);
    
    return EXIT_SUCCESS;
}

void itkProcessTensorTractography::onCanceled()
{
    if (d->tractographyFilter)
        d->tractographyFilter->cancel();
}

medAbstractData *itkProcessTensorTractography::output()
{
    return d->output;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createItkProcessTensorTractography()
{
    return new itkProcessTensorTractography;
}
