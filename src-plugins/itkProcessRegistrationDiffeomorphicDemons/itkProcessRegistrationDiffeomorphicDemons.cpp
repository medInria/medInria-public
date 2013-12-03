/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkProcessRegistrationDiffeomorphicDemons.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

#include "itkImageRegistrationMethod.h"

#include "itkImage.h"
#include "itkResampleImageFilter.h"

#include "time.h"

#include <DiffeomorphicDemons/rpiDiffeomorphicDemons.hxx>
#include <rpiCommonTools.hxx>
#include <registrationFactory/registrationFactory.h>


// /////////////////////////////////////////////////////////////////
// itkProcessRegistrationDiffeomorphicDemonsDiffeomorphicDemonsPrivate
// /////////////////////////////////////////////////////////////////

class itkProcessRegistrationDiffeomorphicDemonsPrivate
{
public:
    itkProcessRegistrationDiffeomorphicDemons * proc;
    template <class PixelType>
            int update();
    template < typename TFixedImage, typename TMovingImage >
           bool write(const QString&);
    void * registrationMethod ;

    std::vector<unsigned int> iterations;
    unsigned char updateRule;
    unsigned char gradientType;
    float maximumUpdateStepLength;
    float updateFieldStandardDeviation;
    float displacementFieldStandardDeviation;
    bool useHistogramMatching;
};

// /////////////////////////////////////////////////////////////////
// itkProcessRegistrationDiffeomorphicDemons
// /////////////////////////////////////////////////////////////////

itkProcessRegistrationDiffeomorphicDemons::itkProcessRegistrationDiffeomorphicDemons() : itkProcessRegistration(), d(new itkProcessRegistrationDiffeomorphicDemonsPrivate)
{
    d->proc = this;
    d->registrationMethod = NULL ;
    d->updateRule = 0;
    d->gradientType = 0;
    d->maximumUpdateStepLength = 2.0;
    d->updateFieldStandardDeviation = 0.0;
    d->displacementFieldStandardDeviation = 1.5;
    d->useHistogramMatching = false;
    //set transform type for the exportation of the transformation to a file
    this->setProperty("transformType","nonRigid");
}

itkProcessRegistrationDiffeomorphicDemons::~itkProcessRegistrationDiffeomorphicDemons()
{
    d->proc = NULL;
    switch(fixedImageType()){
    //only float will be used here, diffeoMorphic demons only work on float and double.

    default:
    {
        typedef itk::Image< float, 3 >  RegImageType;
        delete static_cast<rpi::DiffeomorphicDemons< RegImageType, RegImageType,
                float > *>(d->registrationMethod);
    }
        break;
    }
    d->registrationMethod = NULL;
    delete d;
    d = NULL;
}

bool itkProcessRegistrationDiffeomorphicDemons::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkProcessRegistrationDiffeomorphicDemons",
              createitkProcessRegistrationDiffeomorphicDemons);
}

QString itkProcessRegistrationDiffeomorphicDemons::description() const
{
    return "itkProcessRegistrationDiffeomorphicDemons";
}

QString itkProcessRegistrationDiffeomorphicDemons::identifier() const
{
    return "itkProcessRegistrationDiffeomorphicDemons";
}


// /////////////////////////////////////////////////////////////////
// Templated Version of update
// /////////////////////////////////////////////////////////////////


template <typename PixelType>
        int itkProcessRegistrationDiffeomorphicDemonsPrivate::update()
{
    typedef itk::Image< PixelType, 3 >  FixedImageType;
    typedef itk::Image< PixelType, 3 >  MovingImageType;

    //unfortunately diffeomorphic demons only work with double or float types...
    // so we need to use a cast filter.
    typedef itk::Image< float, 3 > RegImageType;
    typedef double TransformScalarType;
    typedef rpi::DiffeomorphicDemons< RegImageType, RegImageType,
                    TransformScalarType > RegistrationType;
    RegistrationType * registration = new RegistrationType;

    registrationMethod = registration;

    registration->SetFixedImage((FixedImageType*)proc->fixedImage().GetPointer());
    registration->SetMovingImage((MovingImageType*)proc->movingImages()[0].GetPointer());

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
    try {
        registration->StartRegistration();
    }
    catch( std::exception & err )
    {
        qDebug() << "ExceptionObject caught ! (startRegistration)" << err.what();
        return 1;
    }
    time_t t2 = clock();

    qDebug() << "Elasped time: " << (double)(t2-t1)/(double)CLOCKS_PER_SEC;

    emit proc->progressed(80);
    
    typedef itk::ResampleImageFilter< MovingImageType,MovingImageType,TransformScalarType >    ResampleFilterType;
    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();
    resampler->SetTransform(registration->GetTransformation());
    resampler->SetInput((const MovingImageType*)proc->movingImages()[0].GetPointer());
    resampler->SetSize( proc->fixedImage()->GetLargestPossibleRegion().GetSize() );
    resampler->SetOutputOrigin( proc->fixedImage()->GetOrigin() );
    resampler->SetOutputSpacing( proc->fixedImage()->GetSpacing() );
    resampler->SetOutputDirection( proc->fixedImage()->GetDirection() );
    resampler->SetDefaultPixelValue( 0 );

    try {
        resampler->Update();
    }
    catch (itk::ExceptionObject &e) {
        qDebug() << e.GetDescription();
        return 1;
    }

    itk::ImageBase<3>::Pointer result = resampler->GetOutput();
    qDebug() << "Resampled? ";
    result->DisconnectPipeline();
    
    if (proc->output())
        proc->output()->setData (result);
        
    return 0;
}

int itkProcessRegistrationDiffeomorphicDemons::update(itkProcessRegistration::ImageType imgType)
{
    if(fixedImage().IsNull() || movingImages().isEmpty()
            || movingImages()[0].IsNull())
    {
        qWarning() << "Either the fixed image or the moving image is Null";
        return 1;
    }

    if (imgType != itkProcessRegistration::FLOAT)
    {
        qWarning() << "the imageType should be float, and it's :"<<imgType;
        return 1;
    }

    return d->update<float>();
}

itk::Transform<double,3,3>::Pointer itkProcessRegistrationDiffeomorphicDemons::getTransform(){
    typedef float PixelType;
    typedef double TransformScalarType;
    typedef itk::Image< PixelType, 3 > RegImageType;
    //normaly should use long switch cases, but here we know we work with float3 data.
    if (rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> * registration =
            static_cast<rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> *>(d->registrationMethod))
    {
        return registration->GetTransformation();
    }
    else
        return NULL;
}

QString itkProcessRegistrationDiffeomorphicDemons::getTitleAndParameters(){
    typedef float PixelType;
    typedef double TransformScalarType;
    typedef itk::Image< PixelType, 3 > RegImageType;
    //normaly should use long switch cases, but here we know we work with float3 data.
    typedef rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> RegistrationType;
    RegistrationType * registration = static_cast<RegistrationType *>(d->registrationMethod);

    QString titleAndParameters;
    titleAndParameters += "DiffeomorphicDemons\n";
    titleAndParameters += "  Max number of iterations   : " + QString::fromStdString(rpi::VectorToString(registration->GetNumberOfIterations())) + "\n";
    switch (registration->GetUpdateRule())
    {
        case 0:
            titleAndParameters += "  Update rule   : DIFFEOMORPHIC\n";
            break;
        case 1:
            titleAndParameters += "  Update rule   : ADDITIVE\n";
            break;
        case 2:
            titleAndParameters += "  Update rule   : COMPOSITIVE\n";
            break;
        default:
            titleAndParameters += "  Update rule   : Unknown\n";
    }

    switch( registration->GetGradientType() )
    {
        case 0:
            titleAndParameters += "  Gradient type   : SYMMETRIZED\n";
            break;
        case 1:
            titleAndParameters += "  Gradient type   : FIXED_IMAGE\n";
            break;
        case 2:
            titleAndParameters += "  Gradient type   : WARPED_MOVING_IMAGE\n";
            break;
        case 3:
            titleAndParameters += "  Gradient type   : MAPPED_MOVING_IMAGE\n";
            break;
        default:
            titleAndParameters += "  Gradient type   : Unknown\n";
    }

    titleAndParameters += "  Maximum step length   : " + QString::number(registration->GetMaximumUpdateStepLength()) + " (voxel unit)\n";
    titleAndParameters += "  Update field standard deviation   : " + QString::number(registration->GetUpdateFieldStandardDeviation()) + " (voxel unit)\n";
    titleAndParameters += "  Displacement field standard deviation   : " + QString::number(registration->GetDisplacementFieldStandardDeviation()) + " (voxel unit)\n";
    titleAndParameters += "  Use histogram matching   : " + QString::fromStdString(rpi::BooleanToString(registration->GetUseHistogramMatching())) + "\n";

    return titleAndParameters;
}

bool itkProcessRegistrationDiffeomorphicDemons::writeTransform(const QString& file)
{
    typedef float PixelType;
    typedef double TransformScalarType;
    typedef itk::Image< PixelType, 3 > RegImageType;
    //normaly should use long switch cases, but here we know we work with float3 data.
    if (rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> * registration =
            static_cast<rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> *>(d->registrationMethod))
    {
        try{
            rpi::writeDisplacementFieldTransformation<TransformScalarType, RegImageType::ImageDimension>(
                        registration->GetTransformation(),
                        file.toStdString());
        }
        catch (std::exception)
        {
            return false;
        }
        return true;
    }
    else
    {
        return false;
    }
}

// /////////////////////////////////////////////////////////////////
// Process parameters
// /////////////////////////////////////////////////////////////////
void itkProcessRegistrationDiffeomorphicDemons::setUpdateRule(unsigned char updateRule)
{
    d->updateRule = updateRule;
}

void itkProcessRegistrationDiffeomorphicDemons::setGradientType(unsigned char gradientType)
{
    d->gradientType = gradientType;
}

void itkProcessRegistrationDiffeomorphicDemons::setMaximumUpdateLength(float maximumUpdateStepLength)
{
    d->maximumUpdateStepLength = maximumUpdateStepLength;
}

void itkProcessRegistrationDiffeomorphicDemons::setUpdateFieldStandardDeviation(float updateFieldStandardDeviation)
{
    d->updateFieldStandardDeviation = updateFieldStandardDeviation;
}

void itkProcessRegistrationDiffeomorphicDemons::setDisplacementFieldStandardDeviation(float displacementFieldStandardDeviation)
{
    d->displacementFieldStandardDeviation = displacementFieldStandardDeviation;
}

void itkProcessRegistrationDiffeomorphicDemons::setUseHistogramMatching(bool useHistogramMatching)
{
    d->useHistogramMatching = useHistogramMatching;
}

void itkProcessRegistrationDiffeomorphicDemons::setNumberOfIterations(std::vector<unsigned int> iterations)
{
    d->iterations = iterations;
}



// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkProcessRegistrationDiffeomorphicDemons()
{
    return new itkProcessRegistrationDiffeomorphicDemons;
}

