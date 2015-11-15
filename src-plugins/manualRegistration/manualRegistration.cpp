/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <manualRegistration.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

#include <itkImageRegistrationMethod.h>

#include <itkImage.h>
#include <itkResampleImageFilter.h>
#include <itkLandmarkBasedTransformInitializer.h>
#include <time.h>
#include <itkPoint.h>
#include <vector>
#include <iostream>
#include <vtkPointHandleRepresentation2D.h>

// /////////////////////////////////////////////////////////////////
// manualRegistrationDiffeomorphicDemonsPrivate
// /////////////////////////////////////////////////////////////////

class manualRegistrationPrivate
{
public:
    manualRegistration * proc;
    template <class PixelType>
    int update();
    template < typename TFixedImage, typename TMovingImage >
    bool write(const QString&);
    void * registrationMethod ;
    QList<manualRegistrationLandmark*> * FixedLandmarks;
    QList<manualRegistrationLandmark*> * MovingLandmarks;
};

// /////////////////////////////////////////////////////////////////
// manualRegistration
// /////////////////////////////////////////////////////////////////

manualRegistration::manualRegistration() : itkProcessRegistration(), d(new manualRegistrationPrivate)
{
    d->proc = this;
    d->registrationMethod = NULL ;

    //set transform type for the exportation of the transformation to a file
    this->setProperty("transformType","Rigid");
}

manualRegistration::~manualRegistration()
{
    d->proc = NULL;
    d->registrationMethod = NULL;
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
    QApplication::processEvents();

    typedef itk::Image< PixelType, 3 >  FixedImageType;
    typedef itk::Image< PixelType, 3 >  MovingImageType;

    typedef double TransformScalarType;
    typedef itk::VersorRigid3DTransform<TransformScalarType> TransformType;

    typedef itk::LandmarkBasedTransformInitializer< TransformType, FixedImageType, MovingImageType > RegistrationType;

    typename RegistrationType::Pointer registration = RegistrationType::New();
    
    registrationMethod = registration;

    typename RegistrationType::LandmarkPointContainer containerFixed;
    typename RegistrationType::LandmarkPointContainer containerMoving;
    
    for(int i = 0;i<FixedLandmarks->length();i++)
    {
        itk::Point<double,3> point;
        point[0] = FixedLandmarks->at(i)->GetHandleRepresentation()->GetWorldPosition()[0];
        point[1] = FixedLandmarks->at(i)->GetHandleRepresentation()->GetWorldPosition()[1];
        point[2] = FixedLandmarks->at(i)->GetHandleRepresentation()->GetWorldPosition()[2];

        containerFixed.push_back(point);
    }

    for(int i = 0;i<MovingLandmarks->length();i++)
    {
        itk::Point<double,3> point;
        point[0] = MovingLandmarks->at(i)->GetHandleRepresentation()->GetWorldPosition()[0];
        point[1] = MovingLandmarks->at(i)->GetHandleRepresentation()->GetWorldPosition()[1];
        point[2] = MovingLandmarks->at(i)->GetHandleRepresentation()->GetWorldPosition()[2];

        containerMoving.push_back(point);
    }

    registration->SetFixedLandmarks(containerFixed);
    registration->SetMovingLandmarks(containerMoving);

    TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();
    registration->SetTransform(transform);

    // Print method parameters
    //QString methodParameters = proc->getTitleAndParameters();

    // Run the registration
    //time_t t1 = clock();
    try {
        registration->InitializeTransform();
    }
    catch( std::exception & err )
    {
        qDebug() << "ExceptionObject caught ! (startRegistration)" << err.what();
        QApplication::restoreOverrideCursor();
        QApplication::processEvents();
        return 1;
    }
    //time_t t2 = clock();

    emit proc->progressed(80);
    
    typedef itk::ResampleImageFilter< MovingImageType,MovingImageType,TransformScalarType >    ResampleFilterType;
    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();
    resampler->SetTransform(transform);
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
        QApplication::restoreOverrideCursor();
        QApplication::processEvents();
        return 1;
    }

    itk::ImageBase<3>::Pointer result = resampler->GetOutput();
    result->DisconnectPipeline();
    
    if (proc->output())
        proc->output()->setData (result);

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();

    return 0;
}

int manualRegistration::update(itkProcessRegistration::ImageType imgType)
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

itk::Transform<double,3,3>::Pointer manualRegistration::getTransform(){
    //typedef float PixelType;
    //typedef double TransformScalarType;
    //typedef itk::Image< PixelType, 3 > RegImageType;
    ////normaly should use long switch cases, but here we know we work with float3 data.
    //if (rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> * registration =
    //        static_cast<rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> *>(d->registrationMethod))
    //{
    //    return registration->GetTransformation();
    //}
    //else
    return NULL;
}

QString manualRegistration::getTitleAndParameters()
{
    QString titleAndParameters;
    titleAndParameters += "ManualRegistration\n";
    return titleAndParameters;
}

bool manualRegistration::writeTransform(const QString& file)
{
    //typedef float PixelType;
    //typedef double TransformScalarType;
    //typedef itk::Image< PixelType, 3 > RegImageType;
    ////normaly should use long switch cases, but here we know we work with float3 data.
    //if (rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> * registration =
    //        static_cast<rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> *>(d->registrationMethod))
    //{
    //    try{
    //        rpi::writeDisplacementFieldTransformation<TransformScalarType, RegImageType::ImageDimension>(
    //                    registration->GetTransformation(),
    //                    file.toStdString());
    //    }
    //    catch (std::exception)
    //    {
    //        return false;
    //    }
    //    return true;
    //}
    //else
    //{
    //    return false;
    //}
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

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createManualRegistration()
{
    return new manualRegistration;
}

