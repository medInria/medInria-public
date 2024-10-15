/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "LCCLogDemons.h"
#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

#include <itkImageRegistrationMethod.h>
#include <itkImage.h>
#include <itkResampleImageFilter.h>
#include <itkCastImageFilter.h>

#include <time.h>

#include <rpiLCClogDemons.hxx>
#include <rpiCommonTools.hxx>

// /////////////////////////////////////////////////////////////////
// LCCLogDemonsPrivate
// /////////////////////////////////////////////////////////////////
typedef itk::Image< float, 3 >  RegImageType;
typedef double TransformScalarType;

class LCCLogDemonsPrivate
{
public:
    LCCLogDemons * proc;
    template <class PixelType> int update();
    
    rpi::LCClogDemons< RegImageType, RegImageType, double > * registrationMethod;
    rpi::LCClogDemons< RegImageType, RegImageType, double >::UpdateRule updateRule;
    rpi::LCClogDemons< RegImageType, RegImageType, double >::GradientType gradientType;
    std::vector<unsigned int> iterations;
    double maxStepLength, similarityCriteriaSigma, sigmaI, updateFieldSigma, velocityFieldSigma;
    unsigned int BCHExpansion;
    bool verbose, boundaryCheck, useHistogramMatching, useMask;
    int interpolatorType;
};

// /////////////////////////////////////////////////////////////////
// LCCLogDemons
// /////////////////////////////////////////////////////////////////

LCCLogDemons::LCCLogDemons() : itkProcessRegistration(), d(new LCCLogDemonsPrivate)
{
    d->proc = this;
    d->registrationMethod = nullptr;

    // Gives the exported file type for medRegistrationSelectorToolBox
    this->setProperty("outputFileType", "notText");
}

LCCLogDemons::~LCCLogDemons()
{
    d->proc = nullptr;
    
    if (d->registrationMethod)
    {
        delete d->registrationMethod;
    }
    d->registrationMethod = nullptr;
    
    delete d;
    d = nullptr;
}

bool LCCLogDemons::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("LCCLogDemons",
                                                                      createLCCLogDemons);
}

void LCCLogDemons::setUpdateRule(int rule)
{
    d->updateRule = (rpi::LCClogDemons< RegImageType, RegImageType, double >::UpdateRule)rule;
}

void LCCLogDemons::setVerbosity(bool verbose)
{
    d->verbose = verbose;
}

void LCCLogDemons::setNumberOfIterations(std::vector<unsigned int> iterations)
{
    d->iterations = iterations;
}

void LCCLogDemons::setMaximumUpdateStepLength(double value)
{
    d->maxStepLength = value;
}

void LCCLogDemons::setGradientType(int value)
{
    d->gradientType = (rpi::LCClogDemons< RegImageType, RegImageType, double >::GradientType)value;
}

void LCCLogDemons::setUseHistogramMatching(bool flag)
{
    d->useHistogramMatching = flag;
}

void LCCLogDemons::setBoundaryCheck(bool flag)
{
    d->boundaryCheck = flag;
}

void LCCLogDemons::setSigmaI(double sigmaI)
{
    d->sigmaI = sigmaI;
}

void LCCLogDemons::setSimilarityCriteriaSigma(double std)
{
    d->similarityCriteriaSigma = std;
}

void LCCLogDemons::setUpdateFieldSigma(double sigma)
{
    d->updateFieldSigma = sigma;
}

void LCCLogDemons::setVelocityFieldSigma(double sigma)
{
    d->velocityFieldSigma = sigma;
}

void LCCLogDemons::setNumberOfTermsBCHExpansion(unsigned int number)
{
    d->BCHExpansion = number;
}

void LCCLogDemons::useMask (bool flag)
{
    d->useMask = flag;
}

void LCCLogDemons::setInterpolatorType(int value)
{
    d->interpolatorType = value;;
}

QString LCCLogDemons::description() const
{
    return "LCCLogDemons";
}

// /////////////////////////////////////////////////////////////////
// Templated Version of update
// /////////////////////////////////////////////////////////////////


template <typename PixelType>
int LCCLogDemonsPrivate::update()
{
    RegImageType *inputFixed  = (RegImageType*) proc->fixedImage().GetPointer();
    RegImageType *inputMoving = (RegImageType*) proc->movingImages()[0].GetPointer();

    // Cast spacing, origin, direction and size from the moving data to the fixed one
    typedef itk::ResampleImageFilter<RegImageType, RegImageType> ResampleFilterType;
    typename ResampleFilterType::Pointer resampleFilter = ResampleFilterType::New();
    resampleFilter->SetOutputSpacing(inputFixed->GetSpacing());
    resampleFilter->SetOutputOrigin(inputFixed->GetOrigin());
    resampleFilter->SetOutputDirection(inputFixed->GetDirection());
    resampleFilter->SetSize(inputFixed->GetLargestPossibleRegion().GetSize());
    resampleFilter->SetInput(inputMoving);
    resampleFilter->Update();
    inputMoving = resampleFilter->GetOutput();

    // Register the fixed and moving data
    registrationMethod = new rpi::LCClogDemons<RegImageType,RegImageType,double> ();
    registrationMethod->SetFixedImage(inputFixed);
    registrationMethod->SetMovingImage(inputMoving);
    registrationMethod->SetUpdateRule(updateRule);
    registrationMethod->SetVerbosity(verbose);

    //Log
    registrationMethod->SetMaximumUpdateStepLength(maxStepLength);
    registrationMethod->SetGradientType(gradientType);
    registrationMethod->SetUseHistogramMatching(useHistogramMatching);
    
    registrationMethod->SetNumberOfIterations(iterations);
    registrationMethod->SetBoundaryCheck(boundaryCheck);
    registrationMethod->SetSimilarityCriteriaStandardDeviation(similarityCriteriaSigma);
    registrationMethod->SetSigmaI(sigmaI);
    registrationMethod->SetUpdateFieldStandardDeviation(updateFieldSigma);
    registrationMethod->SetStationaryVelocityFieldStandardDeviation(velocityFieldSigma);
    registrationMethod->SetNumberOfTermsBCHExpansion(BCHExpansion);
    registrationMethod->UseMask(useMask);
    
    // Print method parameters
    QString methodParameters = proc->getTitleAndParameters();

    qDebug() << "METHOD PARAMETERS";
    qDebug() << methodParameters;

    // Run the registration
    time_t t1 = clock();
    try
    {
        registrationMethod->StartRegistration();
    }
    catch( std::exception & err )
    {
        qDebug() << "ExceptionObject caught (startRegistration): " << err.what();
        return medAbstractProcessLegacy::FAILURE;
    }
    
    time_t t2 = clock();
    qDebug() << "Elasped time: " << static_cast<double>(t2-t1)/static_cast<double>(CLOCKS_PER_SEC);

    typedef itk::ResampleImageFilter< RegImageType,RegImageType, double> ResampleFilterType;
    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();
    resampler->SetTransform(registrationMethod->GetDisplacementFieldTransformation());
    resampler->SetInput(inputMoving);
    resampler->SetDefaultPixelValue(0);
    
    // Set the image interpolator
    switch(interpolatorType)
    {
        case rpi::INTERPOLATOR_NEAREST_NEIGHBOR:
        {
            resampler->SetInterpolator(itk::NearestNeighborInterpolateImageFunction<RegImageType, double>::New());
            break;
        }
        case rpi::INTERPOLATOR_LINEAR:
        {
            // Nothing to do ; linear interpolator by default
            break;
        }
        case rpi::INTERPOLATOR_BSLPINE:
        {
            resampler->SetInterpolator(itk::BSplineInterpolateImageFunction<RegImageType, double>::New());
            break;
        }
        case rpi::INTERPOLATOR_SINUS_CARDINAL:
        {
            resampler->SetInterpolator(itk::WindowedSincInterpolateImageFunction<
                                       RegImageType,
                                       RegImageType::ImageDimension,
                                       itk::Function::HammingWindowFunction<RegImageType::ImageDimension>,
                                       itk::ConstantBoundaryCondition<RegImageType>,
                                       double
                                       >::New());
            break;
        }
    }
    
    try
    {
        resampler->Update();
    }
    catch (itk::ExceptionObject & err)
    {
        qDebug() << "ExceptionObject caught (resampler): " << err.GetDescription();
        return medAbstractProcessLegacy::FAILURE;
    }

    if (proc->output())
    {
        proc->output()->setData(resampler->GetOutput());
    }
    return medAbstractProcessLegacy::SUCCESS;
}

int LCCLogDemons::update(itkProcessRegistration::ImageType imgType)
{
    // Cast has been done in itkProcessRegistration
    if (imgType == itkProcessRegistration::FLOAT)
    {
        return d->update<float>();
    }

    return medAbstractProcessLegacy::FAILURE;
}

bool LCCLogDemons::writeTransform(const QString& file)
{
    try
    {
        if (auto transform = getTransform())
        {
            rpi::writeDisplacementFieldTransformation<double, 3>(transform, file.toStdString());
        }
    }
    catch (std::exception& err)
    {
        qDebug() << "ExceptionObject caught (writeTransform): " << err.what();
        return false;
    }
    return true;
}

itk::Transform<double,3,3>::Pointer LCCLogDemons::getTransform()
{
    if (d->registrationMethod)
    {
        return d->registrationMethod->GetDisplacementFieldTransformation().GetPointer();
    }
    return nullptr;
}

QString LCCLogDemons::getTitleAndParameters()
{
    auto registration = d->registrationMethod;

    QString titleAndParameters;
    titleAndParameters += "LCC Log Demons\n";

    titleAndParameters += "  Max number of iterations: " +
            QString::fromStdString(rpi::VectorToString(registration->GetNumberOfIterations())) + "\n";

    switch (registration->GetUpdateRule())
    {
        case 0:
        {
            titleAndParameters += "  Update rule: SSD Non Symmetric Log Domain\n";
            break;
        }
        case 1:
        {
            titleAndParameters += "  Update rule: SSD Symmetric Log Domain\n";
            break;
        }
        case 2:
        {
            titleAndParameters += "  Update rule: LCC\n";
            break;
        }
        default:
        {
            titleAndParameters += "  Update rule: Unknown\n";
        }
    }

    titleAndParameters += "  Update Field Sigma: "   + QString::number(d->updateFieldSigma) + "\n";
    titleAndParameters += "  Velocity Field Sigma: " + QString::number(d->velocityFieldSigma) + "\n";
    titleAndParameters += "  BCH Expansion: "        + QString::number(d->BCHExpansion) + "\n";

    switch(d->interpolatorType)
    {
        case rpi::INTERPOLATOR_NEAREST_NEIGHBOR:
        {
            titleAndParameters += "  Interpolator type: Nearest Neighbor\n";
            break;
        }
        case rpi::INTERPOLATOR_LINEAR:
        {
            titleAndParameters += "  Interpolator type: Linear\n";
            break;
        }
        case rpi::INTERPOLATOR_BSLPINE:
        {
            titleAndParameters += "  Interpolator type: B-Spline\n";
            break;
        }
        case rpi::INTERPOLATOR_SINUS_CARDINAL:
        {
            titleAndParameters += "  Interpolator type: Sinus Cardinal\n";
            break;
        }
        default:
        {
            titleAndParameters += "  Interpolator type: Unknown\n";
        }
    }

    return titleAndParameters;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createLCCLogDemons()
{
    return new LCCLogDemons;
}
