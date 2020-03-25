/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkProcessTensorEstimation.h"

#include <dtkCore/dtkSmartPointer.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractData.h>

#include <itkProcessTensorLogEuclideanEstimation.h>
#include <itkProcessTensorDTIBrainMaskCalculator.h>
#include <itkProcessTensorLogEuclideanRemoveNonPositive.h>
#include <itkProcessTensorLogEuclideanAnisotropicSmoothing.h>

#include <medMetaDataKeys.h>

// /////////////////////////////////////////////////////////////////
// itkProcessTensorEstimationPrivate
// /////////////////////////////////////////////////////////////////

class itkProcessTensorEstimationPrivate
{
public:
    dtkSmartPointer <medAbstractData> input;
    dtkSmartPointer <medAbstractData> output;
    
    dtkSmartPointer <itkProcessTensorLogEuclideanEstimation> tensorEstimator;
    dtkSmartPointer <itkProcessTensorDTIBrainMaskCalculator> maskCalculator;
    dtkSmartPointer <itkProcessTensorLogEuclideanRemoveNonPositive> tensorCleaner;
    dtkSmartPointer <itkProcessTensorLogEuclideanAnisotropicSmoothing> tensorSmoother;
    
    // Parameters
    bool maskBrain;
    bool tensorSmoothing;
    
    QString smoothingLevel;
    double b0Threshold;
};

// /////////////////////////////////////////////////////////////////
// itkProcessTensorEstimation
// /////////////////////////////////////////////////////////////////

itkProcessTensorEstimation::itkProcessTensorEstimation() : medAbstractDiffusionProcess(), d(new itkProcessTensorEstimationPrivate)
{
    d->maskBrain = true;
    d->tensorSmoothing = false;
    d->smoothingLevel = "low";
    d->b0Threshold = 0;
}

itkProcessTensorEstimation::~itkProcessTensorEstimation()
{
    delete d;
    d = 0;
}

bool itkProcessTensorEstimation::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkProcessTensorEstimation", createItkProcessTensorEstimation);
}

QString itkProcessTensorEstimation::identifier() const
{
    return "itkProcessTensorEstimation";
}

QString itkProcessTensorEstimation::description() const
{
    return "itkProcessTensorEstimation";
}

void itkProcessTensorEstimation::setInputImage(medAbstractData *input)
{
    if (!input)
        return;
    
    d->input = input;
}

void itkProcessTensorEstimation::setAutomaticBrainMasking(bool value)
{
    d->maskBrain = value;
}

void itkProcessTensorEstimation::setTensorSmoothing(bool value)
{
    d->tensorSmoothing = value;
}

void itkProcessTensorEstimation::setSmoothingLevel(QString value)
{
    d->smoothingLevel = value;
}

void itkProcessTensorEstimation::setB0Threshold(double value)
{
    d->b0Threshold = value;
}

int itkProcessTensorEstimation::update()
{
    // Here call sub-processes
    d->tensorEstimator = new itkProcessTensorLogEuclideanEstimation;
    
    int retval = 0;
    
    if (d->maskBrain)
    {
        d->maskCalculator = new itkProcessTensorDTIBrainMaskCalculator;
        
        d->maskCalculator->setInputImage(d->input);
        // Offset is 1 to remove first B0
        d->maskCalculator->setParameter(1,0);
        retval = d->maskCalculator->update();
        
        if (retval != 0)
            return retval;

        if (d->input->hasMetaData("DiffusionGradientList"))
            d->maskCalculator->output()->setMetaData("DiffusionGradientList",
                                                     d->input->metaDataValues("DiffusionGradientList"));
        
        d->tensorEstimator->setInputImage (d->maskCalculator->output());
        d->tensorEstimator->setParameter(0.0);
    }
    else
    {
        d->tensorEstimator->setInputImage (d->input);
        d->tensorEstimator->setParameter(d->b0Threshold);
    }
    
    retval = d->tensorEstimator->update();
    
    if (retval != 0)
        return retval;
    
    d->tensorCleaner = new itkProcessTensorLogEuclideanRemoveNonPositive;
    
    d->tensorCleaner->setInputImage(d->tensorEstimator->output());
    
    retval = d->tensorCleaner->update();
    
    if (retval != 0)
        return retval;
    
    d->output = d->tensorCleaner->output();
    if (d->tensorSmoothing)
    {
        d->tensorSmoother = new itkProcessTensorLogEuclideanAnisotropicSmoothing;
        
        connect (d->tensorSmoother, SIGNAL (progressed (int)), this, SIGNAL (progressed (int)));
        
        d->tensorSmoother->setInputImage(d->tensorCleaner->output());
        d->tensorSmoother->setProperty("SmoothingLevel", d->smoothingLevel);
        
        retval = d->tensorSmoother->update();
        
        if (retval != 0)
            return retval;

        d->output = d->tensorSmoother->output();
    }
    
    QString newSeriesDescription = d->input->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " tensors";
    d->output->setMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
    
    foreach ( QString metaData, d->input->metaDataList() )
    {
        if (metaData == "DiffusionGradientList")
            continue;
        
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

void itkProcessTensorEstimation::onCanceled()
{
    if (d->tensorEstimator)
        d->tensorEstimator->cancel();
    if (d->maskCalculator)
        d->maskCalculator->cancel();
    if (d->tensorCleaner)
        d->tensorCleaner->cancel();
    if (d->tensorSmoother)
        d->tensorSmoother->cancel();
}

medAbstractData *itkProcessTensorEstimation::output()
{
    return d->output;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createItkProcessTensorEstimation()
{
    return new itkProcessTensorEstimation;
}
