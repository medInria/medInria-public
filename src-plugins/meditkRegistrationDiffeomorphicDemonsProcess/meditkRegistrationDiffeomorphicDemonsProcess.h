#pragma once

#include <dtkCore>
#include <medCore.h>

#include "legacy/itkProcessRegistrationDiffeomorphicDemons.h"

#include <medAbstractRegistrationDiffeomorphicDemonsProcess.h>


class meditkRegistrationDiffeomorphicDemonsProcess : public medAbstractRegistrationProcess
{
    Q_OBJECT

public:
     meditkRegistrationDiffeomorphicDemonsProcess(void);
    ~meditkRegistrationDiffeomorphicDemonsProcess(void);

    virtual void setUpdateRule(unsigned char updateRule);
    virtual void setGradientType(unsigned char gradientType);
    virtual void setMaximumUpdateLength(float maximumUpdateStepLength);
    virtual void setUpdateFieldStandardDeviation(float updateFieldStandardDeviation);
    virtual void setDisplacementFieldStandardDeviation(float displacementFieldStandardDeviation);
    virtual void setUseHistogramMatching(bool useHistogramMatching);
    virtual void setNumberOfIterations(std::vector<unsigned int> iterations);
    
    virtual void setImage(medAbstractImageData* image) = 0;

public: 
    virtual medAbstractImageData* image(void) const = 0;
    virtual QTransform transform(void) const = 0;

public:
    void run();

private:
    itkProcessRegistrationDiffeomorphicDemons process;
    medAbstractImageData* transformedImage;
    QTransform resultingTransfo;
    

};


inline medAbstractRegistrationDiffeomorphicDemonsProcess *meditkRegistrationDiffeomorphicDemonsProcessCreator(void)
{
    return new meditkRegistrationDiffeomorphicDemonsProcess();
}
