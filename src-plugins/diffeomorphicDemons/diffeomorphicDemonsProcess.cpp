/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <DiffeomorphicDemons/rpiDiffeomorphicDemons.hxx>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <diffeomorphicDemonsProcess.h>
#include <rpiCommonTools.hxx>

// /////////////////////////////////////////////////////////////////
// DiffeomorphicDemonsProcessPrivate
// /////////////////////////////////////////////////////////////////

class DiffeomorphicDemonsProcessPrivate
{
public:

    DiffeomorphicDemonsProcess * proc;
    template <class PixelType> int update();
    template < typename TFixedImage, typename TMovingImage > bool write(const QString&);

    void * registrationMethod;
    std::vector<unsigned int> iterations;
    unsigned char updateRule;
    unsigned char gradientType;
    float maximumUpdateStepLength;
    float updateFieldStandardDeviation;
    float displacementFieldStandardDeviation;
    bool useHistogramMatching;
};

// /////////////////////////////////////////////////////////////////
// DiffeomorphicDemonsProcess
// /////////////////////////////////////////////////////////////////

DiffeomorphicDemonsProcess::DiffeomorphicDemonsProcess() : itkProcessRegistration(), d(new DiffeomorphicDemonsProcessPrivate)
{
    d->proc = this;
    d->registrationMethod = NULL ;
    d->updateRule = 0;
    d->gradientType = 0;
    d->maximumUpdateStepLength = 2.0;
    d->updateFieldStandardDeviation = 0.0;
    d->displacementFieldStandardDeviation = 1.5;
    d->useHistogramMatching = false;

    // Gives the exported file type for medRegistrationSelectorToolBox
    this->setProperty("outputFileType","notText");
}

DiffeomorphicDemonsProcess::~DiffeomorphicDemonsProcess()
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

bool DiffeomorphicDemonsProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("DiffeomorphicDemonsProcess",
              createDiffeomorphicDemonsProcess);
}

QString DiffeomorphicDemonsProcess::description() const
{
    return "DiffeomorphicDemonsProcess";
}

QString DiffeomorphicDemonsProcess::identifier() const
{
    return "DiffeomorphicDemonsProcess";
}


// /////////////////////////////////////////////////////////////////
// Templated Version of update
// /////////////////////////////////////////////////////////////////


template <typename PixelType>
int DiffeomorphicDemonsProcessPrivate::update()
{
    typedef itk::Image< PixelType, 3 >  FixedImageType;
    typedef itk::Image< PixelType, 3 >  MovingImageType;

    typedef itk::Image< float, 3 > RegImageType;
    typedef double TransformScalarType;

    // Check that the inputs are the same size/origin/spacing
    if ( (proc->fixedImage()->GetLargestPossibleRegion().GetSize()
          != proc->movingImages()[0]->GetLargestPossibleRegion().GetSize())
         || (proc->fixedImage()->GetOrigin()
             != proc->movingImages()[0]->GetOrigin())
         || (proc->fixedImage()->GetSpacing()
             != proc->movingImages()[0]->GetSpacing()) )
    {
        return medAbstractProcess::MISMATCHED_DATA_SIZES_ORIGIN_SPACING;
    }

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
    try
    {
        registration->StartRegistration();
    }
    catch( std::exception & err )
    {
        qDebug() << "ExceptionObject caught (StartRegistration): " << err.what();
        return medAbstractProcess::FAILURE;
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

    try
    {
        resampler->Update();
    }
    catch (itk::ExceptionObject & err)
    {
        qDebug() << "ExceptionObject caught (resampler): " << err.GetDescription();
        return medAbstractProcess::FAILURE;
    }

    itk::ImageBase<3>::Pointer result = resampler->GetOutput();
    result->DisconnectPipeline();

    if (proc->output())
        proc->output()->setData (result);

    return medAbstractProcess::SUCCESS;
}

int DiffeomorphicDemonsProcess::update(itkProcessRegistration::ImageType imgType)
{
    // Cast has been done in itkProcessRegistration
    if (imgType == itkProcessRegistration::FLOAT)
    {
        return d->update<float>();
    }

    return medAbstractProcess::FAILURE;
}

itk::Transform<double,3,3>::Pointer DiffeomorphicDemonsProcess::getTransform(){
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

QString DiffeomorphicDemonsProcess::getTitleAndParameters(){
    typedef float PixelType;
    typedef double TransformScalarType;
    typedef itk::Image< PixelType, 3 > RegImageType;
    //normaly should use long switch cases, but here we know we work with float3 data.
    typedef rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> RegistrationType;
    RegistrationType * registration = static_cast<RegistrationType *>(d->registrationMethod);

    QString titleAndParameters;
    titleAndParameters += "DiffeomorphicDemonsProcess\n";
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

bool DiffeomorphicDemonsProcess::writeTransform(const QString& file)
{
    typedef float PixelType;
    typedef double TransformScalarType;
    typedef itk::Image< PixelType, 3 > RegImageType;
    //normaly should use long switch cases, but here we know we work with float3 data.
    if (rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> * registration =
            static_cast<rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> *>(d->registrationMethod))
    {
        try
        {
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
    else
    {
        return false;
    }
}

// /////////////////////////////////////////////////////////////////
// Process parameters
// /////////////////////////////////////////////////////////////////
void DiffeomorphicDemonsProcess::setUpdateRule(unsigned char updateRule)
{
    d->updateRule = updateRule;
}

void DiffeomorphicDemonsProcess::setGradientType(unsigned char gradientType)
{
    d->gradientType = gradientType;
}

void DiffeomorphicDemonsProcess::setMaximumUpdateLength(float maximumUpdateStepLength)
{
    d->maximumUpdateStepLength = maximumUpdateStepLength;
}

void DiffeomorphicDemonsProcess::setUpdateFieldStandardDeviation(float updateFieldStandardDeviation)
{
    d->updateFieldStandardDeviation = updateFieldStandardDeviation;
}

void DiffeomorphicDemonsProcess::setDisplacementFieldStandardDeviation(float displacementFieldStandardDeviation)
{
    d->displacementFieldStandardDeviation = displacementFieldStandardDeviation;
}

void DiffeomorphicDemonsProcess::setUseHistogramMatching(bool useHistogramMatching)
{
    d->useHistogramMatching = useHistogramMatching;
}

void DiffeomorphicDemonsProcess::setNumberOfIterations(std::vector<unsigned int> iterations)
{
    d->iterations = iterations;
}



// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createDiffeomorphicDemonsProcess()
{
    return new DiffeomorphicDemonsProcess;
}
