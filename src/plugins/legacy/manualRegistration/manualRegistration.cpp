/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "manualRegistration.h"

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkImageRegistrationMethod.h>
#include <itkLandmarkBasedTransformInitializer.h>

#include <rpiCommonTools.hxx>

#include <vtkPointHandleRepresentation2D.h>

typedef double TransformScalarType;
typedef itk::VersorRigid3DTransform<TransformScalarType>    TransformType_Rigid3D;
typedef itk::AffineTransform<TransformScalarType>           TransformType_Affine;
typedef itk::MatrixOffsetTransformBase<TransformScalarType> TransformType_Generic;

// /////////////////////////////////////////////////////////////////
// manualRegistrationDiffeomorphicDemonsPrivate
// /////////////////////////////////////////////////////////////////

class manualRegistrationPrivate
{
public:
    manualRegistration* proc;
    template <class PixelType> int update();
    template <typename PixelType, typename TransformType> int applyRegistration();

    QList<manualRegistrationLandmark*>* FixedLandmarks;
    QList<manualRegistrationLandmark*>* MovingLandmarks;

    TransformType_Generic::Pointer transform;
    manualRegistration::TransformName transformTypeInt;
};

// /////////////////////////////////////////////////////////////////
// manualRegistration
// /////////////////////////////////////////////////////////////////

manualRegistration::manualRegistration() : itkProcessRegistration(), d(new manualRegistrationPrivate)
{
    d->proc = this;

    // Gives the exported file type for medRegistrationSelectorToolBox
    this->setProperty("outputFileType", "text");
}

manualRegistration::~manualRegistration()
{
    d->proc = nullptr;
    delete d;
    d = nullptr;
}

bool manualRegistration::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("manualRegistration",
                                                                      createManualRegistration);
}

QString manualRegistration::description() const
{
    return "manualRegistration";
}

QString manualRegistration::identifier() const
{
    return "manualRegistration";
}

// /////////////////////////////////////////////////////////////////

template <class PixelType> int manualRegistrationPrivate::update()
{
    int res = medAbstractProcessLegacy::FAILURE;

    if (transformTypeInt == manualRegistration::RIGID)
    {
        res = applyRegistration<PixelType, TransformType_Rigid3D>();
    }
    else if (transformTypeInt == manualRegistration::AFFINE)
    {
        res = applyRegistration<PixelType, TransformType_Affine>();

    }

    return res;
}

template <typename PixelType, typename TransformType> int manualRegistrationPrivate::applyRegistration()
{
    typedef itk::Image< PixelType, 3 >  FixedImageType;
    typedef itk::Image< PixelType, 3 >  MovingImageType;

    typedef itk::LandmarkBasedTransformInitializer< TransformType,
            FixedImageType, MovingImageType > RegistrationType;

    typename RegistrationType::LandmarkPointContainer containerFixed;
    typename RegistrationType::LandmarkPointContainer containerMoving;

    // Fill fixed and moving containers
    for(int i=0; i<FixedLandmarks->length(); i++)
    {
        itk::Point<double,3> point;
        point[0] = FixedLandmarks->at(i)->GetHandleRepresentation()->GetWorldPosition()[0];
        point[1] = FixedLandmarks->at(i)->GetHandleRepresentation()->GetWorldPosition()[1];
        point[2] = FixedLandmarks->at(i)->GetHandleRepresentation()->GetWorldPosition()[2];

        containerFixed.push_back(point);
    }

    for(int i=0; i<MovingLandmarks->length(); i++)
    {
        itk::Point<double,3> point;
        point[0] = MovingLandmarks->at(i)->GetHandleRepresentation()->GetWorldPosition()[0];
        point[1] = MovingLandmarks->at(i)->GetHandleRepresentation()->GetWorldPosition()[1];
        point[2] = MovingLandmarks->at(i)->GetHandleRepresentation()->GetWorldPosition()[2];

        containerMoving.push_back(point);
    }

    // Set transformation parameters to registration
    typename RegistrationType::Pointer registration = RegistrationType::New();
    registration->SetFixedLandmarks(containerFixed);
    registration->SetMovingLandmarks(containerMoving);

    typename TransformType::Pointer transformTemp = TransformType::New();
    transformTemp->SetIdentity();
    registration->SetTransform(transformTemp);

    // Run registration to compute transformation
    try
    {
        registration->InitializeTransform();
    }
    catch(std::exception & err)
    {
        qDebug() << "ExceptionObject caught (InitializeTransform): " << err.what();
        return medAbstractProcessLegacy::FAILURE;
    }

    // Save transformation for future writing
    transform = transformTemp;

    emit proc->progressed(80);

    // Apply transformation on the moving dataset
    typedef itk::ResampleImageFilter< MovingImageType,MovingImageType,TransformScalarType >    ResampleFilterType;
    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();
    resampler->SetTransform(transform);
    resampler->SetInput(static_cast<const MovingImageType*>(proc->movingImages()[0].GetPointer()));
    resampler->SetSize(proc->fixedImage()->GetLargestPossibleRegion().GetSize());
    resampler->SetOutputOrigin( proc->fixedImage()->GetOrigin() );
    resampler->SetOutputSpacing( proc->fixedImage()->GetSpacing() );
    resampler->SetOutputDirection( proc->fixedImage()->GetDirection() );
    resampler->SetDefaultPixelValue(0);

    try
    {
        resampler->Update();
    }
    catch (itk::ExceptionObject &err)
    {
        qDebug() << "ExceptionObject caught (resampler): " << err.GetDescription();
        return medAbstractProcessLegacy::FAILURE;
    }

    itk::ImageBase<3>::Pointer result = resampler->GetOutput();
    result->DisconnectPipeline();

    if (proc->output())
    {
        proc->output()->setData (result);
    }

    return medAbstractProcessLegacy::SUCCESS;
}

int manualRegistration::update(itkProcessRegistration::ImageType imgType)
{
    // Cast has been done in itkProcessRegistration
    if (imgType == itkProcessRegistration::FLOAT)
    {
        return d->update<float>();
    }

    return medAbstractProcessLegacy::FAILURE;
}

itk::Transform<double,3,3>::Pointer manualRegistration::getTransform()
{
    return d->transform.GetPointer();
}

void manualRegistration::setParameter(int data)
{
    d->transformTypeInt = static_cast<TransformName>(data);
}

QString manualRegistration::getTitleAndParameters()
{
    QString titleAndParameters;
    titleAndParameters += "ManualRegistration\n  ";

    if (d->transformTypeInt == RIGID)
    {
        titleAndParameters += "Rigid method";
    }
    else if (d->transformTypeInt == AFFINE)
    {
        titleAndParameters += "Affine method";
    }
    return titleAndParameters;
}

bool manualRegistration::writeTransform(const QString& file)
{
    typedef itk::Image<TransformScalarType, 3> RegImageType;

    try
    {
        rpi::writeLinearTransformation<TransformScalarType,
                RegImageType::ImageDimension>(
                    d->transform,
                    file.toLocal8Bit().constData());
    }
    catch (std::exception& err)
    {
        qDebug() << "ExceptionObject caught (writeTransform): " << err.what();
        return false;
    }

    return true;
}

void manualRegistration::SetFixedLandmarks(QList<manualRegistrationLandmark*>* fixedLandmarks)
{
    d->FixedLandmarks = fixedLandmarks;
}

void manualRegistration::SetMovingLandmarks(QList<manualRegistrationLandmark*>* movingLandmarks)
{
    d->MovingLandmarks = movingLandmarks;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess* createManualRegistration()
{
    return new manualRegistration;
}

