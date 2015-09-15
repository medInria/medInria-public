#include "meditkRegistrationDiffeomorphicDemonsProcess.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkRegistrationDiffeomorphicDemonsProcess::meditkRegistrationDiffeomorphicDemonsProcess(void)
{

}

meditkRegistrationDiffeomorphicDemonsProcess::~meditkRegistrationDiffeomorphicDemonsProcess(void)
{

}

void meditkRegistrationDiffeomorphicDemonsProcess::setUpdateRule(unsigned char updateRule)
{
    process.setUpdateRule(updateRule);
}

void meditkRegistrationDiffeomorphicDemonsProcess::setGradientType(unsigned char gradientType)
{
    process.setGradientType(gradientType);
}

void meditkRegistrationDiffeomorphicDemonsProcess::setMaximumUpdateLength(float maximumUpdateStepLength)
{
    process.setMaximumUpdateLength(maximumUpdateStepLength);
}

void meditkRegistrationDiffeomorphicDemonsProcess::setUpdateFieldStandardDeviation(float updateFieldStandardDeviation)
{
    process.setUpdateFieldStandardDeviation(updateFieldStandardDeviation);
}

void meditkRegistrationDiffeomorphicDemonsProcess::setDisplacementFieldStandardDeviation(float displacementFieldStandardDeviation)
{
    process.setDisplacementFieldStandardDeviation(displacementFieldStandardDeviation);
}

void meditkRegistrationDiffeomorphicDemonsProcess::setUseHistogramMatching(bool useHistogramMatching)
{
    process.setUseHistogramMatching(useHistogramMatching);
}

void meditkRegistrationDiffeomorphicDemonsProcess::setNumberOfIterations(std::vector<unsigned int> iterations)
{
    process.setNumberOfIterations(iterations);
}
    
void meditkRegistrationDiffeomorphicDemonsProcess::setImage(medAbstractImageData* image)
{
    process.setImage(image);
}

medAbstractImageData* meditkRegistrationDiffeomorphicDemonsProcess::medAbstractImageData* image(void) const
{
    return transformedImage;
}

QTransform meditkRegistrationDiffeomorphicDemonsProcess::transform(void) const 
{
    return resultingTransfo;
}


medAbstractImageData* meditkRegistrationDiffeomorphicDemonsProcess::filteredImage(void) const
{
    return res;
}

void meditkRegistrationDiffeomorphicDemonsProcess::run(void)
{
    process.update();

    transformedImage=dynamic_cast<medAbstractImageData*>(process.output());
    itk::Transform<double,3,3>::Pointer transfo=process.getTransform();

}
    
    std::cerr<<res;
}
