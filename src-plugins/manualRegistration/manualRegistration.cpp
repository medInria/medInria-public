/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <manualRegistration.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <itkImageRegistrationMethod.h>
#include <itkLandmarkBasedTransformInitializer.h>
#include <medMessageController.h>
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
    manualRegistration * proc;
    template <class PixelType> int update();
    template <typename PixelType, typename TransformType> int applyRegistration();

    QList<manualRegistrationLandmark*> * FixedLandmarks;
    QList<manualRegistrationLandmark*> * MovingLandmarks;

    TransformType_Generic::Pointer  transform;
    TransformName::TransformNameEnum transformTypeInt;
};

// /////////////////////////////////////////////////////////////////
// manualRegistration
// /////////////////////////////////////////////////////////////////

manualRegistration::manualRegistration() : itkProcessRegistration(), d(new manualRegistrationPrivate)
{
    d->proc = this;
 
    //set transform type for the exportation of the transformation to a file
    this->setProperty("transformType","Rigid");
}

manualRegistration::~manualRegistration()
{
    d->proc = NULL;
    delete d;
    d = NULL;
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
// Templated Version of update
// /////////////////////////////////////////////////////////////////


template <typename PixelType>
int manualRegistrationPrivate::update()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    int res = DTK_FAILURE;

    if (transformTypeInt == TransformName::RIGID)
    {
        res = applyRegistration<PixelType, TransformType_Rigid3D>();
    }
    else if (transformTypeInt == TransformName::AFFINE)
    {
        if ((FixedLandmarks->count() >= 4) && (MovingLandmarks->count() >= 4))
        {
            res = applyRegistration<PixelType, TransformType_Affine>();
        }
        else
        {
            proc->displayMessageError("Affine transformation needs 4 landmarks minimum by dataset");
        }
    }

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();

    return res;
}

template <typename PixelType, typename TransformType> int manualRegistrationPrivate::applyRegistration()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    typedef itk::Image< PixelType, 3 >  FixedImageType;
    typedef itk::Image< PixelType, 3 >  MovingImageType;

    typedef itk::LandmarkBasedTransformInitializer< TransformType,
            FixedImageType, MovingImageType > RegistrationType;

    typename RegistrationType::LandmarkPointContainer containerFixed;
    typename RegistrationType::LandmarkPointContainer containerMoving;

    // Fill fixed and moving containers
    for(int i = 0; i<FixedLandmarks->length(); i++)
    {
        itk::Point<double,3> point;
        point[0] = FixedLandmarks->at(i)->GetHandleRepresentation()->GetWorldPosition()[0];
        point[1] = FixedLandmarks->at(i)->GetHandleRepresentation()->GetWorldPosition()[1];
        point[2] = FixedLandmarks->at(i)->GetHandleRepresentation()->GetWorldPosition()[2];

        containerFixed.push_back(point);
    }

    for(int i = 0; i<MovingLandmarks->length(); i++)
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
    catch( std::exception & err )
    {
        qDebug(err.what());
        proc->displayMessageError("Error initializing transformation");
        QApplication::restoreOverrideCursor();
        return DTK_FAILURE;
    }

    // Save transformation for future writing
    transform = transformTemp;

    emit proc->progressed(80);

    // Apply transformation on the moving dataset
    typedef itk::ResampleImageFilter< MovingImageType,MovingImageType,TransformScalarType >    ResampleFilterType;
    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();
    resampler->SetTransform(transform);
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
    catch (itk::ExceptionObject &err)
    {
        qDebug(err.GetDescription());
        proc->displayMessageError("Error applying transformation");
        QApplication::restoreOverrideCursor();
        return DTK_FAILURE;
    }

    itk::ImageBase<3>::Pointer result = resampler->GetOutput();
    result->DisconnectPipeline();

    if (proc->output())
    {
        proc->output()->setData (result);
    }

    QApplication::restoreOverrideCursor();

    return DTK_SUCCEED;
}

int manualRegistration::update(itkProcessRegistration::ImageType imgType)
{
    if(fixedImage().IsNull() || movingImages().isEmpty()
            || movingImages()[0].IsNull())
    {
        displayMessageError("Either the fixed image or the moving image is empty");
        return DTK_FAILURE;
    }

    if (imgType != itkProcessRegistration::FLOAT)
    {
        displayMessageError("Images type should be float");
        return DTK_FAILURE;
    }

    return d->update<float>();
}

itk::Transform<double,3,3>::Pointer manualRegistration::getTransform()
{
    return NULL;
}

void manualRegistration::setParameter(int data)
{
    d->transformTypeInt = static_cast<TransformName::TransformNameEnum>(data);
}

QString manualRegistration::getTitleAndParameters()
{
    QString titleAndParameters;
    titleAndParameters += "ManualRegistration\n";
    return titleAndParameters;
}

bool manualRegistration::writeTransform(const QString& file)
{
    typedef float PixelType;

    typedef itk::Image< PixelType, 3 > RegImageType;

    try
    {
        rpi::writeLinearTransformation<TransformScalarType,
                RegImageType::ImageDimension>(
                    d->transform,
                    file.toLocal8Bit().constData());
    }
    catch (std::exception& err)
    {
        qDebug(err.what());
        displayMessageError("Error writing transformation");
        return false;
    }

    return true;
}

void manualRegistration::SetFixedLandmarks(QList<manualRegistrationLandmark*> * fixedLandmarks)
{
    d->FixedLandmarks = fixedLandmarks;
}

void manualRegistration::SetMovingLandmarks(QList<manualRegistrationLandmark*> * movingLandmarks)
{
    d->MovingLandmarks = movingLandmarks;
}

void manualRegistration::displayMessageError(QString error)
{
    qDebug() << this->description() + ": " + error;
    medMessageController::instance()->showError(error, 3000);
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createManualRegistration()
{
    return new manualRegistration;
}

