/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "diffeomorphicDemons.h"

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <DiffeomorphicDemons/rpiDiffeomorphicDemons.hxx>
#include <itkResampleImageFilter.h>
#include <rpiCommonTools.hxx>

// /////////////////////////////////////////////////////////////////
// diffeomorphicDemonsDiffeomorphicDemonsPrivate
// /////////////////////////////////////////////////////////////////

class diffeomorphicDemonsPrivate
{
public:
    diffeomorphicDemons *proc;
    template <class PixelType> int update();
    template < typename TFixedImage, typename TMovingImage > bool write(const QString&);
    void *registrationMethod ;

    std::vector<unsigned int> iterations;
    unsigned char updateRule;
    unsigned char gradientType;
    float maximumUpdateStepLength;
    float updateFieldStandardDeviation;
    float displacementFieldStandardDeviation;
    bool useHistogramMatching;
};

// /////////////////////////////////////////////////////////////////
// diffeomorphicDemons
// /////////////////////////////////////////////////////////////////

diffeomorphicDemons::diffeomorphicDemons() : itkProcessRegistration(), d(new diffeomorphicDemonsPrivate)
{
    d->proc = this;
    d->registrationMethod = nullptr;
    d->updateRule = 0;
    d->gradientType = 0;
    d->maximumUpdateStepLength = 2.0;
    d->updateFieldStandardDeviation = 0.0;
    d->displacementFieldStandardDeviation = 1.5;
    d->useHistogramMatching = false;

    // Gives the exported file type for medRegistrationSelectorToolBox
    this->setProperty("outputFileType", "notText");
}

diffeomorphicDemons::~diffeomorphicDemons()
{
    d->proc = nullptr;
    typedef itk::Image< float, 3 >  RegImageType;
    delete static_cast<rpi::DiffeomorphicDemons< RegImageType, RegImageType, float > *>(d->registrationMethod);
    d->registrationMethod = nullptr;
    delete d;
    d = nullptr;
}

bool diffeomorphicDemons::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("diffeomorphicDemons",
                                                                      creatediffeomorphicDemons);
}

QString diffeomorphicDemons::description() const
{
    return "diffeomorphicDemons";
}

QString diffeomorphicDemons::identifier() const
{
    return "diffeomorphicDemons";
}

// /////////////////////////////////////////////////////////////////
// Templated Version of update
// /////////////////////////////////////////////////////////////////

template <typename PixelType>
int diffeomorphicDemonsPrivate::update()
{
    typedef itk::Image< PixelType, 3 >  FixedImageType;
    typedef itk::Image< PixelType, 3 >  MovingImageType;
    typedef itk::Image< float, 3 > RegImageType;
    typedef double TransformScalarType;

    FixedImageType  *inputFixed  = static_cast<FixedImageType*>(proc->fixedImage().GetPointer());
    MovingImageType *inputMoving = static_cast<MovingImageType*>(proc->movingImages()[0].GetPointer());

    // Cast spacing, origin, direction and size from the moving data to the fixed one
    typedef itk::ResampleImageFilter<MovingImageType, MovingImageType> ResampleFilterType;
    typename ResampleFilterType::Pointer resampleFilter = ResampleFilterType::New();
    resampleFilter->SetOutputSpacing(inputFixed->GetSpacing());
    resampleFilter->SetOutputOrigin(inputFixed->GetOrigin());
    resampleFilter->SetOutputDirection(inputFixed->GetDirection());
    resampleFilter->SetSize(inputFixed->GetLargestPossibleRegion().GetSize());
    resampleFilter->SetInput(inputMoving);
    resampleFilter->Update();
    inputMoving = resampleFilter->GetOutput();

    // Register the fixed and moving data
    typedef rpi::DiffeomorphicDemons< RegImageType, RegImageType, TransformScalarType > RegistrationType;
    RegistrationType *registration = new RegistrationType;
    registrationMethod = registration;
    registration->SetFixedImage(inputFixed);
    registration->SetMovingImage(inputMoving);
    registration->SetNumberOfIterations(iterations);
    registration->SetMaximumUpdateStepLength(maximumUpdateStepLength);
    registration->SetUpdateFieldStandardDeviation(updateFieldStandardDeviation);
    registration->SetDisplacementFieldStandardDeviation(displacementFieldStandardDeviation);
    registration->SetUseHistogramMatching(useHistogramMatching);

    // Set update rule
    switch( updateRule )
    {
        case 0:
            registration->SetUpdateRule( RegistrationType::UPDATE_DIFFEOMORPHIC ); break;
        case 1:
            registration->SetUpdateRule( RegistrationType::UPDATE_ADDITIVE );      break;
        case 2:
            registration->SetUpdateRule( RegistrationType::UPDATE_COMPOSITIVE );   break;
        default:
            throw std::runtime_error( "Update rule must fit in the range [0,2]." );
    }

    // Set gradient type
    switch( gradientType )
    {
        case 0:
            registration->SetGradientType( RegistrationType::GRADIENT_SYMMETRIZED );
            break;
        case 1:
            registration->SetGradientType( RegistrationType::GRADIENT_FIXED_IMAGE );
            break;
        case 2:
            registration->SetGradientType( RegistrationType::GRADIENT_WARPED_MOVING_IMAGE );
            break;
        case 3:
            registration->SetGradientType( RegistrationType::GRADIENT_MAPPED_MOVING_IMAGE );
            break;
        default:
            throw std::runtime_error( "Gradient type must fit in the range [0,3]." );
    }

    // Print method parameters
    QString methodParameters = proc->getTitleAndParameters();
    qDebug() << "METHOD PARAMETERS";
    qDebug() << methodParameters;

    // Run the registration
    time_t t1 = clock();
    try
    {
        registration->StartRegistration();
    }
    catch( std::exception & err )
    {
        qDebug() << "ExceptionObject caught (StartRegistration): " << err.what();
        return medAbstractProcessLegacy::FAILURE;
    }

    time_t t2 = clock();
    qDebug() << "Elasped time: " << static_cast<double>(t2-t1)/static_cast<double>(CLOCKS_PER_SEC);

    emit proc->progressed(80);

    // Cast new transformation
    typedef itk::ResampleImageFilter< MovingImageType,MovingImageType,TransformScalarType > ResampleTransformation;
    typename ResampleTransformation::Pointer resampleTransformation = ResampleTransformation::New();
    resampleTransformation->SetTransform(registration->GetTransformation());
    resampleTransformation->SetInput(inputMoving);
    resampleTransformation->SetDefaultPixelValue(0);
    try
    {
        resampleTransformation->Update();
    }
    catch (itk::ExceptionObject & err)
    {
        qDebug() << "ExceptionObject caught (resampler): " << err.GetDescription();
        return medAbstractProcessLegacy::FAILURE;
    }

    if (proc->output())
    {
        proc->output()->setData(resampleTransformation->GetOutput());
    }
    return medAbstractProcessLegacy::SUCCESS;
}

int diffeomorphicDemons::update(itkProcessRegistration::ImageType imgType)
{
    // Cast has been done in itkProcessRegistration
    if (imgType == itkProcessRegistration::FLOAT)
    {
        return d->update<float>();
    }

    return medAbstractProcessLegacy::FAILURE;
}

itk::Transform<double,3,3>::Pointer diffeomorphicDemons::getTransform()
{
    typedef float PixelType;
    typedef double TransformScalarType;
    typedef itk::Image< PixelType, 3 > RegImageType;
    // Normally should use long switch cases, but here we know we work with float3 data.
    if (rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> * registration =
            static_cast<rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> *>(d->registrationMethod))
    {
        return registration->GetTransformation();
    }

    return nullptr;
}

QString diffeomorphicDemons::getTitleAndParameters()
{
    typedef float PixelType;
    typedef double TransformScalarType;
    typedef itk::Image< PixelType, 3 > RegImageType;
    // Normally should use long switch cases, but here we know we work with float3 data.
    typedef rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> RegistrationType;
    RegistrationType *registration = static_cast<RegistrationType *>(d->registrationMethod);

    QString titleAndParameters;
    titleAndParameters += "DiffeomorphicDemons\n";
    titleAndParameters += "  Max number of iterations: " + QString::fromStdString(rpi::VectorToString(registration->GetNumberOfIterations())) + "\n";
    switch (registration->GetUpdateRule())
    {
    case 0:
        titleAndParameters += "  Update rule: DIFFEOMORPHIC\n";
        break;
    case 1:
        titleAndParameters += "  Update rule: ADDITIVE\n";
        break;
    case 2:
        titleAndParameters += "  Update rule: COMPOSITIVE\n";
        break;
    default:
        titleAndParameters += "  Update rule: Unknown\n";
    }

    switch( registration->GetGradientType() )
    {
    case 0:
        titleAndParameters += "  Gradient type: SYMMETRIZED\n";
        break;
    case 1:
        titleAndParameters += "  Gradient type: FIXED_IMAGE\n";
        break;
    case 2:
        titleAndParameters += "  Gradient type: WARPED_MOVING_IMAGE\n";
        break;
    case 3:
        titleAndParameters += "  Gradient type: MAPPED_MOVING_IMAGE\n";
        break;
    default:
        titleAndParameters += "  Gradient type: Unknown\n";
    }

    titleAndParameters += "  Maximum step length: " + QString::number(registration->GetMaximumUpdateStepLength()) + " (voxel unit)\n";
    titleAndParameters += "  Update field standard deviation: " + QString::number(registration->GetUpdateFieldStandardDeviation()) + " (voxel unit)\n";
    titleAndParameters += "  Displacement field standard deviation: " + QString::number(registration->GetDisplacementFieldStandardDeviation()) + " (voxel unit)\n";
    titleAndParameters += "  Use histogram matching: " + QString::fromStdString(rpi::BooleanToString(registration->GetUseHistogramMatching())) + "\n";

    return titleAndParameters;
}

bool diffeomorphicDemons::writeTransform(const QString& file)
{
    typedef float PixelType;
    typedef double TransformScalarType;
    typedef itk::Image< PixelType, 3 > RegImageType;
    // Normally should use long switch cases, but here we know we work with float3 data.
    if (rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> * registration =
            static_cast<rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> *>(d->registrationMethod))
    {
        try{
            rpi::writeDisplacementFieldTransformation<TransformScalarType, RegImageType::ImageDimension>(
                        registration->GetTransformation(),
                        file.toStdString());
        }
        catch (std::exception& err)
        {
            qDebug() << "ExceptionObject caught (writeTransform): " << err.what();
            return false;
        }
        return true;
    }

    return false;
}

// /////////////////////////////////////////////////////////////////
// Process parameters
// /////////////////////////////////////////////////////////////////
void diffeomorphicDemons::setUpdateRule(unsigned char updateRule)
{
    d->updateRule = updateRule;
}

void diffeomorphicDemons::setGradientType(unsigned char gradientType)
{
    d->gradientType = gradientType;
}

void diffeomorphicDemons::setMaximumUpdateLength(float maximumUpdateStepLength)
{
    d->maximumUpdateStepLength = maximumUpdateStepLength;
}

void diffeomorphicDemons::setUpdateFieldStandardDeviation(float updateFieldStandardDeviation)
{
    d->updateFieldStandardDeviation = updateFieldStandardDeviation;
}

void diffeomorphicDemons::setDisplacementFieldStandardDeviation(float displacementFieldStandardDeviation)
{
    d->displacementFieldStandardDeviation = displacementFieldStandardDeviation;
}

void diffeomorphicDemons::setUseHistogramMatching(bool useHistogramMatching)
{
    d->useHistogramMatching = useHistogramMatching;
}

void diffeomorphicDemons::setNumberOfIterations(std::vector<unsigned int> iterations)
{
    d->iterations = iterations;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *creatediffeomorphicDemons()
{
    return new diffeomorphicDemons;
}
