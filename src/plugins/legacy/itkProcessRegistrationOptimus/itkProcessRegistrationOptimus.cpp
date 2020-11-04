/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include "itkProcessRegistrationOptimus.h"
#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

#include "itkImageRegistrationMethod.h"
#include "itkImage.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"

#include "time.h"

#include <rpiOptimus.h>
#include <rpiCommonTools.hxx>

// /////////////////////////////////////////////////////////////////
// itkProcessRegistrationOptimusOptimusPrivate
// /////////////////////////////////////////////////////////////////

class itkProcessRegistrationOptimusPrivate
{
public:
    itkProcessRegistrationOptimus * proc;
    template <class PixelType>
            int update(void);
    template<typename PixelType>
           bool writeTransform(const QString& file);
    void * registrationMethod ;
    void deleteRegMethod(void);
    unsigned int iterations;
    unsigned int nbOfHistogramBins;
    unsigned int nbOfSpatialSamples;
    unsigned int nbOfInterpolations;
    float        rhoStart;
    float        rhoEnd;
    float        scalingCoefficient;
};

// /////////////////////////////////////////////////////////////////
// itkProcessRegistrationOptimus
// /////////////////////////////////////////////////////////////////

itkProcessRegistrationOptimus::itkProcessRegistrationOptimus(void) : itkProcessRegistration(), d(new itkProcessRegistrationOptimusPrivate)
{
    d->proc = this;
    d->registrationMethod = NULL ;
    this->setProperty("transformType","rigid");
}

itkProcessRegistrationOptimus::~itkProcessRegistrationOptimus(void)
{
    d->proc = NULL;
    switch(fixedImageType()){
//    case itkProcessRegistration::UCHAR:
//    {
//        typedef itk::Image< float, 3 >  RegImageType;
//        delete static_cast<rpi::Optimus< RegImageType, RegImageType,
//                double > *>(d->registrationMethod);
//    }
//        break;
//    case itkProcessRegistration::CHAR:
//    {
//        typedef itk::Image< char, 3 >  RegImageType;
//        delete static_cast<rpi::Optimus< RegImageType, RegImageType,
//                double > *>(d->registrationMethod);
//    }
//        break;
//    case itkProcessRegistration::USHORT:
//    {
//        typedef itk::Image< unsigned short, 3 >  RegImageType;
//        delete static_cast<rpi::Optimus< RegImageType, RegImageType,
//                double > *>(d->registrationMethod);
//    }
//        break;
//    case itkProcessRegistration::SHORT:
//    {
//        typedef itk::Image< short, 3 >  RegImageType;
//        delete static_cast<rpi::Optimus< RegImageType, RegImageType,
//                double > *>(d->registrationMethod);
//    }
//        break;
//    case itkProcessRegistration::UINT:
//    {
//        typedef itk::Image< unsigned int, 3 >  RegImageType;
//        delete static_cast<rpi::Optimus< RegImageType, RegImageType,
//                double > *>(d->registrationMethod);
//    }
//        break;
//    case itkProcessRegistration::INT:
//    {
//        typedef itk::Image< int, 3 >  RegImageType;
//        delete static_cast<rpi::Optimus< RegImageType, RegImageType,
//                double > *>(d->registrationMethod);
//    }
//        break;
//    case itkProcessRegistration::ULONG:
//    {
//        typedef itk::Image< unsigned long, 3 >  RegImageType;
//        delete static_cast<rpi::Optimus< RegImageType, RegImageType,
//                double > *>(d->registrationMethod);
//    }
//        break;
//    case itkProcessRegistration::LONG:
//     {
//        typedef itk::Image< long, 3 >  RegImageType;
//        delete static_cast<rpi::Optimus< RegImageType, RegImageType,
//                double > *>(d->registrationMethod);
//    }
//        break;
//    case itkProcessRegistration::DOUBLE:
//     {
//        typedef itk::Image< double, 3 >  RegImageType;
//        delete static_cast<rpi::Optimus< RegImageType, RegImageType,
//                double > *>(d->registrationMethod);
//    }
//        break;
    // we only work on float at the moment.
    default:
    {
        typedef itk::Image< float, 3 >  RegImageType;
        delete static_cast<rpi::Optimus< RegImageType, RegImageType,
                double > *>(d->registrationMethod);
    }
        break;
    }
    d->registrationMethod = NULL;
    delete d;
    d = NULL;
}

bool itkProcessRegistrationOptimus::registered(void)
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkProcessRegistrationOptimus",
              createitkProcessRegistrationOptimus);
}

QString itkProcessRegistrationOptimus::description(void) const
{
    return "Optimus";
}

QString itkProcessRegistrationOptimus::identifier(void) const
{
    return "itkProcessRegistrationOptimus";
}

// /////////////////////////////////////////////////////////////////
// Templated Version of update
// /////////////////////////////////////////////////////////////////


template <typename PixelType>
        int itkProcessRegistrationOptimusPrivate::update(void)
{
    typedef itk::Image< PixelType, 3 >  FixedImageType;
    typedef itk::Image< PixelType, 3 >  MovingImageType;

    typename rpi::Optimus<FixedImageType,MovingImageType> * registration =
            new rpi::Optimus<FixedImageType,MovingImageType> ();

    registrationMethod = registration;

    registration->SetFixedImage((const FixedImageType*) proc->fixedImage().GetPointer());
    registration->SetMovingImage((const MovingImageType*) proc->movingImages()[0].GetPointer());

    registration->SetNumberOfIterations(iterations);
    registration->SetNumberOfHistogramBins(nbOfHistogramBins);
    registration->SetNumberOfInterpolations(nbOfInterpolations);
    registration->SetNumberOfSpatialSamples(nbOfSpatialSamples);
    registration->SetRhoStart(rhoStart);
    registration->SetRhoEnd(rhoEnd);
    registration->SetScalingCoefficient(scalingCoefficient);

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

    emit proc->progressed(70);

    typedef itk::ResampleImageFilter< MovingImageType,MovingImageType >    ResampleFilterType;
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
    result->DisconnectPipeline();


    if (proc->output())
      proc->output()->setData (result);
    return 0;
}

int itkProcessRegistrationOptimus::update(itkProcessRegistration::ImageType imgType)
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
//    if(fixedImage().IsNull() || movingImages().isEmpty()
//            || movingImages()[0].IsNull())
//        return 1;
//    switch (imgType){
//    case itkProcessRegistration::UCHAR:
//        return d->update<unsigned char>();
//        break;
//    case itkProcessRegistration::CHAR:
//        return d->update<char>();
//        break;
//    case itkProcessRegistration::USHORT:
//        return d->update<unsigned short>();
//        break;
//    case itkProcessRegistration::SHORT:
//        return d->update<short>();
//        break;
//    case itkProcessRegistration::UINT:
//       return d->update<unsigned int>();
//       break;
//    case itkProcessRegistration::INT:
//        return d->update<int>();
//        break;
//    case itkProcessRegistration::ULONG:
//       return d->update<unsigned long>();
//       break;
//    case itkProcessRegistration::LONG:
//        return d->update<long>();
//        break;
//    case itkProcessRegistration::DOUBLE:
//        return d->update<double>();
//        break;
//    default:
//        return d->update<float>();
//        break;
//    }
}

itk::Transform<double,3,3>::Pointer itkProcessRegistrationOptimus::getTransform()
{
    typedef float PixelType;
    typedef double TransformScalarType;
    typedef itk::Image< PixelType, 3 > RegImageType;
    
    //normaly should use long switch cases, but here we know we work with float3 data.
    if (rpi::Optimus<RegImageType,RegImageType,TransformScalarType> * registration =
            static_cast<rpi::Optimus<RegImageType,RegImageType,TransformScalarType> *>(d->registrationMethod))
    {
        return registration->GetTransformation();
    }
    
    return nullptr;
}

QString itkProcessRegistrationOptimus::getTitleAndParameters(){
    
    typedef float PixelType;
    typedef itk::Image< PixelType, 3 >  FixedImageType;
    typedef itk::Image< PixelType, 3 >  MovingImageType;
    typedef rpi::Optimus<FixedImageType,MovingImageType> RegistrationType;

    RegistrationType * registration = static_cast<RegistrationType *>(d->registrationMethod);
            
    QString titleAndParameters;
    
    titleAndParameters += "Optimus\n";
    titleAndParameters += "  Max number of iterations   : " + QString::number(registration->GetNumberOfIterations()) + "\n";
    titleAndParameters += "  Number of histogram bins   : " + QString::number(registration->GetNumberOfHistogramBins()) + "\n";
    titleAndParameters += "  Number of spatial samples  : " + QString::number(registration->GetNumberOfSpatialSamples()) + "\n";
    titleAndParameters += "  Number of interpolations   : " + QString::number(registration->GetNumberOfInterpolations()) + "\n";
    titleAndParameters += "  Rho start                  : " + QString::number(registration->GetRhoStart()) + "\n";
    titleAndParameters += "  Rho end                    : " + QString::number(registration->GetRhoEnd()) + "\n";
    titleAndParameters += "  Scaling coefficient        : " + QString::number(registration->GetScalingCoefficient()) + "\n";

    return titleAndParameters;
}

bool itkProcessRegistrationOptimus::writeTransform(const QString& file)
{
    if(!d->registrationMethod)
        return false;
    switch (this->fixedImageType()){
//    case itkProcessRegistration::UCHAR:
//        return d->writeTransform<unsigned char>(file);
//        break;
//    case itkProcessRegistration::CHAR:
//        return d->writeTransform<char>(file);
//        break;
//    case itkProcessRegistration::USHORT:
//        return d->writeTransform<unsigned short>(file);
//        break;
//    case itkProcessRegistration::SHORT:
//        return d->writeTransform<short>(file);
//        break;
//    case itkProcessRegistration::UINT:
//       return d->writeTransform<unsigned int>(file);
//       break;
//    case itkProcessRegistration::INT:
//        return d->writeTransform<int>(file);
//        break;
//    case itkProcessRegistration::ULONG:
//       return d->writeTransform<unsigned long>(file);
//       break;
//    case itkProcessRegistration::LONG:
//        return d->writeTransform<long>(file);
//        break;
//    case itkProcessRegistration::DOUBLE:
//        return d->writeTransform<double>(file);
//        break;
    default:
        return d->writeTransform<float>(file);
        break;
    }
}

template<typename PixelType>
bool itkProcessRegistrationOptimusPrivate::writeTransform(const QString& file)
{
    //typedef float PixelType;
    typedef double TransformScalarType;
    typedef itk::Image< PixelType, 3 > RegImageType;

    if (rpi::Optimus<RegImageType,RegImageType,TransformScalarType> * registration =
            static_cast<rpi::Optimus<RegImageType,RegImageType,TransformScalarType> *>(registrationMethod))
    {
        try{
            rpi::writeLinearTransformation<TransformScalarType, RegImageType::ImageDimension>(
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



void itkProcessRegistrationOptimus::setNumberOfIterations(unsigned int iterations)
{
    d->iterations = iterations;
}
void itkProcessRegistrationOptimus::setNumberOfInterpolations(unsigned int nbInterpolations)
{
    d->nbOfInterpolations = nbInterpolations;
}

void itkProcessRegistrationOptimus::setNumberOfHistogramBins(unsigned int nbOfHistBins)
{
    d->nbOfHistogramBins = nbOfHistBins;
}

void itkProcessRegistrationOptimus::setNumberOfSpatialSamples(unsigned int nbOfSpatSamp)
{
    d->nbOfSpatialSamples = nbOfSpatSamp;
}

void itkProcessRegistrationOptimus::setRhoStart(float rhoStart)
{
    d->rhoStart = rhoStart;
}

void itkProcessRegistrationOptimus::setRhoEnd(float rhoEnd)
{
    d->rhoEnd = rhoEnd;
}

void itkProcessRegistrationOptimus::setScalingCoefficient(float scalingCoeff)
{
    d->scalingCoefficient = scalingCoeff;
}


// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkProcessRegistrationOptimus(void)
{
    return new itkProcessRegistrationOptimus;
}
