/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <%1.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

#include <itkImageRegistrationMethod.h>

#include <itkImage.h>
#include <itkResampleImageFilter.h>
#include <itkCastImageFilter.h>


#include <time.h>

// Include specific RPI implementation of the registration method
#include <rpi%4.h>
#include <rpiCommonTools.hxx>

// /////////////////////////////////////////////////////////////////
// %1Private
// /////////////////////////////////////////////////////////////////

class %1Private
{
public:
    %1 * proc;
    template <class PixelType>
    int update();
    template <typename PixelType>
    bool writeTransform(const QString& file);
    
    void * registrationMethod;
    
};

// /////////////////////////////////////////////////////////////////
// %1
// /////////////////////////////////////////////////////////////////

%1::%1() : itkProcessRegistration(), d(new %1Private)
{
    d->proc = this;
    d->registrationMethod = NULL;
}

%1::~%1()
{
    d->proc = NULL;

    typedef itk::Image< float, 3 >  RegImageType;
    
    if (d->registrationMethod)
        delete static_cast<rpi::%4< RegImageType, RegImageType,float > *>(d->registrationMethod);

    d->registrationMethod = NULL;
    
    delete d;
    d = 0;
}

bool %1::registered()
{
    return dtkAbstractProcessFactory::instance()->register%2Type("%1",
                                                                 create%3);
}

QString %1::description() const
{
    return "%1";
}



// /////////////////////////////////////////////////////////////////
// Templated Version of update
// /////////////////////////////////////////////////////////////////


template <typename PixelType>
int %1Private::update()
{
    typedef itk::Image< PixelType, 3 >  FixedImageType;
    typedef itk::Image< PixelType, 3 >  MovingImageType;
    
    
    typename rpi::%4<FixedImageType,MovingImageType> * registration =
    new rpi::%4<FixedImageType,MovingImageType> ();
    
    registrationMethod = registration;
    
    registration->SetFixedImage((const FixedImageType*) proc->fixedImage().GetPointer());
    registration->SetMovingImage((const MovingImageType*) proc->movingImages()[0].GetPointer());
    
    
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

int %1::update(itkProcessRegistration::ImageType imgType)
{
    if(fixedImage().IsNull() || movingImages()[0].IsNull())
        return 1;

    return d->update<float>();
}


template <typename PixelType>
bool %1Private::writeTransform(const QString& file)
{
    typedef double TransformScalarType;
    typedef itk::Image< PixelType, 3 > RegImageType;
    
    if (rpi::%4<RegImageType,RegImageType,TransformScalarType> * registration =
        static_cast<rpi::%4<RegImageType,RegImageType,TransformScalarType> *>(registrationMethod))
    {
        try{
            rpi::writeDisplacementFieldTransformation<TransformScalarType, 3>(registration->GetTransformation(),
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

bool %1::writeTransform(const QString& file)
{
    if(d->registrationMethod == NULL)
        return 1;
    
    return d->writeTransform<float>(file);
}

itk::Transform<double,3,3>::Pointer %1::getTransform()
{
    return NULL;
}

QString %1::getTitleAndParameters()
{
    return QString();
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstract%2 *create%3()
{
    return new %1;
}
