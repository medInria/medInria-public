/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkProcessRegistration.h"

#include <dtkCore/dtkSmartPointer.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractDataWriter.h>

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

#include "itkCastImageFilter.h"

#include "itkImageRegistrationMethod.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkEuler3DTransform.h"
#include "itkCenteredTransformInitializer.h"

#include "itkImageLinearConstIteratorWithIndex.h"

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkMetaImageIO.h>
#include <itkResampleImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkExtractImageFilter.h>

#include <itkCommand.h>

#include "time.h"

// /////////////////////////////////////////////////////////////////
// itkProcessRegistrationPrivate
// /////////////////////////////////////////////////////////////////

class itkProcessRegistrationPrivate
{
public:

    unsigned int dimensions;
    itk::ImageBase<3>::Pointer fixedImage;
    QVector<itk::ImageBase<3>::Pointer> movingImages;
    //itk::ImageBase<3>::Pointer movingImage;
    itkProcessRegistration::ImageType fixedImageType;
    itkProcessRegistration::ImageType movingImageType;
    dtkSmartPointer<dtkAbstractData> output;

    template <class PixelType>
            void setInput(dtkAbstractData * data,int channel);
    mutable QMutex mutex;
};

// /////////////////////////////////////////////////////////////////
// itkProcessRegistration
// /////////////////////////////////////////////////////////////////

itkProcessRegistration::itkProcessRegistration() : dtkAbstractProcess(), d(new itkProcessRegistrationPrivate)
{
    d->fixedImage = NULL;
    d->output = NULL;
    d->dimensions=3;
    d->fixedImageType = itkProcessRegistration::FLOAT;
    d->movingImageType = itkProcessRegistration::FLOAT;
    QStringList types;
    types << "rigid" << "nonRigid";
    this->addProperty("transformType",types);
    this->addMetaData("category","registration");
}

itkProcessRegistration::~itkProcessRegistration()
{
    delete d;
    d = 0;
}


// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////
template <typename PixelType>
        void itkProcessRegistrationPrivate::setInput(dtkAbstractData * data,int channel)
{
    //typedef itk::Image<PixelType, 3> OutputImageType;
    itkProcessRegistration::ImageType inputType;
    if ( typeid(PixelType) == typeid(unsigned char) )
        inputType = itkProcessRegistration::UCHAR;
       else if ( typeid(PixelType) == typeid(char) )
           inputType = itkProcessRegistration::CHAR;
       else if ( typeid(PixelType) == typeid(unsigned short) )
           inputType = itkProcessRegistration::USHORT;
       else if ( typeid(PixelType) == typeid(short) )
           inputType = itkProcessRegistration::SHORT;
       else if ( typeid(PixelType) == typeid(unsigned int) )
           inputType = itkProcessRegistration::UINT;
       else if ( typeid(PixelType) == typeid(int) )
           inputType = itkProcessRegistration::INT;
       else if ( typeid(PixelType) == typeid(unsigned long int) )
           inputType = itkProcessRegistration::ULONG;
       else if ( typeid(PixelType) == typeid(long int) )
           inputType = itkProcessRegistration::LONG;
       else if ( typeid(PixelType) == typeid(float) )
           inputType = itkProcessRegistration::FLOAT;
       else if ( typeid(PixelType) == typeid(double) )
           inputType = itkProcessRegistration::DOUBLE;
       else
       {
           QString pixel_type =  typeid(PixelType).name();
           qDebug()<< "Error: The pixel type " + pixel_type + " is not supported yet."  ;
       }
    if ( dimensions == 3 ){

        typedef itk::Image <PixelType, 3> InputImageType;


        if (channel==0)
        {
            fixedImageType = inputType;
            fixedImage = dynamic_cast<InputImageType *>((itk::Object*)(data->data()));
        }
        if (channel==1)
        {
            movingImageType = inputType;
            movingImages = QVector<itk::ImageBase<3>::Pointer>(1);
            movingImages[0] =  dynamic_cast<InputImageType *>((itk::Object*)(data->data()));


        }
    }
    else if ( dimensions == 4 ){

        typedef itk::Image <PixelType, 4> Image4DType;
        typedef itk::Image <PixelType, 3> InputImageType;
        typename Image4DType::Pointer image4d = dynamic_cast<Image4DType *>((itk::Object*)(data->data()));
        typename Image4DType::RegionType region = image4d->GetLargestPossibleRegion();
        typename Image4DType::SizeType size = image4d->GetLargestPossibleRegion().GetSize();
        const unsigned int frameNumber =  region.GetSize()[3];
        if (image4d.IsNull())
          return;

        if(channel == 0)
        {
            typename itk::ExtractImageFilter<Image4DType, InputImageType>::Pointer extractFilter = itk::ExtractImageFilter<Image4DType, InputImageType>::New();

            typename Image4DType::IndexType index = {{0,0,0,0}};
            size[3] = 0;
            index[3] = 0;
            region.SetSize(size);
            region.SetIndex(index);
            extractFilter->SetExtractionRegion(region);
            extractFilter->SetInput( image4d );

            try
            {
                extractFilter->Update();
            }
            catch(itk::ExceptionObject &ex)
            {
                qDebug() << "Extraction failed";
                return ;
            }
            fixedImage = extractFilter->GetOutput();
        }
        if(channel == 1)
        {
        //may work on dim > 3
        movingImages = QVector<itk::ImageBase<3>::Pointer>(frameNumber);

        for(unsigned int i = 0 ; i < frameNumber ; i++)
        {
            typename itk::ExtractImageFilter<Image4DType, InputImageType>::Pointer extractFilter = itk::ExtractImageFilter<Image4DType, InputImageType>::New();

            typename Image4DType::IndexType index = {{0,0,0,0}};
            size[3] = 0;
            index[3] = i;
            region.SetSize(size);
            region.SetIndex(index);
            extractFilter->SetExtractionRegion(region);
            extractFilter->SetInput( image4d );

            try
            {
                extractFilter->Update();
            }
            catch(itk::ExceptionObject &ex)
            {
                qDebug() << "Extraction failed";
                return ;
            }
            movingImages[i] = extractFilter->GetOutput();

        }
        }
    }
}


void itkProcessRegistration::setInput(dtkAbstractData *data, int channel)
{
    if (!data)
        return;

    QString id = QString (data->identifier());
    QString::iterator last_charac = id.end() - 1;
    if (*last_charac == '3'){
        d->dimensions = 3;
    }
    else if (*last_charac == '4'){
        d->dimensions = 4;
    }
    else{
        qDebug() << "Unable to handle the number of dimensions " \
                << "for an image of description: "<< data->identifier();
    }

    *last_charac = '3';
    typedef itk::Image< float, 3 > RegImageType;

    dtkSmartPointer <dtkAbstractData> convertedData = dtkAbstractDataFactory::instance()->create ("itkDataImageFloat3");
    foreach ( QString metaData, data->metaDataList() )
        if (!convertedData->hasMetaData(metaData))
            convertedData->addMetaData ( metaData, data->metaDataValues ( metaData ) );

    foreach ( QString property, data->propertyList() )
        convertedData->addProperty ( property,data->propertyValues ( property ) );

    if (channel==0)
        d->output = dtkAbstractDataFactory::instance()->create ("itkDataImageFloat3");
    if (id =="itkDataImageChar3") {
        typedef itk::Image< char, 3 > InputImageType;
        typedef itk::CastImageFilter< InputImageType, RegImageType > CastFilterType;

        CastFilterType::Pointer  caster = CastFilterType::New();
        caster->SetInput((const InputImageType*)data->data());
        caster->Update();
        convertedData->setData(caster->GetOutput());

        d->setInput<float>(convertedData,channel);
    }
    else if (id =="itkDataImageUChar3") {
        typedef itk::Image< unsigned char, 3 > InputImageType;
        typedef itk::CastImageFilter< InputImageType, RegImageType > CastFilterType;

        CastFilterType::Pointer  caster = CastFilterType::New();
        caster->SetInput((const InputImageType*)data->data());
        caster->Update();
        convertedData->setData(caster->GetOutput());

        d->setInput<float>(convertedData,channel);
    }
    else if (id =="itkDataImageShort3") {
        typedef itk::Image< short, 3 > InputImageType;
        typedef itk::CastImageFilter< InputImageType, RegImageType > CastFilterType;

        CastFilterType::Pointer  caster = CastFilterType::New();
        caster->SetInput((const InputImageType*)data->data());
        caster->Update();
        convertedData->setData(caster->GetOutput());

        d->setInput<float>(convertedData,channel);
    }
    else if (id =="itkDataImageUShort3") {
        typedef itk::Image< unsigned short, 3 > InputImageType;
        typedef itk::CastImageFilter< InputImageType, RegImageType > CastFilterType;

        CastFilterType::Pointer  caster = CastFilterType::New();
        caster->SetInput((const InputImageType*)data->data());
        caster->Update();
        convertedData->setData(caster->GetOutput());

        d->setInput<float>(convertedData,channel);
    }

    else if (id =="itkDataImageInt3") {
        typedef itk::Image< int, 3 > InputImageType;
        typedef itk::CastImageFilter< InputImageType, RegImageType > CastFilterType;

        CastFilterType::Pointer  caster = CastFilterType::New();
        caster->SetInput((const InputImageType*)data->data());
        caster->Update();
        convertedData->setData(caster->GetOutput());

        d->setInput<float>(convertedData,channel);
    }
    else if (id =="itkDataImageUInt3") {
        typedef itk::Image< unsigned int, 3 > InputImageType;
        typedef itk::CastImageFilter< InputImageType, RegImageType > CastFilterType;

        CastFilterType::Pointer  caster = CastFilterType::New();
        caster->SetInput((const InputImageType*)data->data());
        caster->Update();
        convertedData->setData(caster->GetOutput());

        d->setInput<float>(convertedData,channel);
    }

    else if (id =="itkDataImageLong3") {
        typedef itk::Image< long, 3 > InputImageType;
        typedef itk::CastImageFilter< InputImageType, RegImageType > CastFilterType;

        CastFilterType::Pointer  caster = CastFilterType::New();
        caster->SetInput((const InputImageType*)data->data());
        caster->Update();
        convertedData->setData(caster->GetOutput());

        d->setInput<float>(convertedData,channel);
    }
    else if (id=="itkDataImageULong3") {
        typedef itk::Image< unsigned long, 3 > InputImageType;
        typedef itk::CastImageFilter< InputImageType, RegImageType > CastFilterType;

        CastFilterType::Pointer  caster = CastFilterType::New();
        caster->SetInput((const InputImageType*)data->data());
        caster->Update();
        convertedData->setData(caster->GetOutput());

        d->setInput<float>(convertedData,channel);
    }
    else if (id =="itkDataImageFloat3") {
        d->setInput<float>(data,channel);
    }
    else if (id =="itkDataImageDouble3") {
        typedef itk::Image< double, 3 > InputImageType;
        typedef itk::CastImageFilter< InputImageType, RegImageType > CastFilterType;

        CastFilterType::Pointer  caster = CastFilterType::New();
        caster->SetInput((const InputImageType*)data->data());
        caster->Update();
        convertedData->setData(caster->GetOutput());

        d->setInput<float>(convertedData,channel);
    }

}

int itkProcessRegistration::update(itkProcessRegistration::ImageType)
{
    DTK_DEFAULT_IMPLEMENTATION;
    return 1;
}


int itkProcessRegistration::update()
{
    if (!d->mutex.tryLock())
    {
        return -1;
    }

    if(d->fixedImage.IsNull() || d->movingImages.empty())
        return 1;

    int retval =  update(d->fixedImageType);
    d->mutex.unlock();
    return retval;
}

dtkAbstractData *itkProcessRegistration::output()
{
    return d->output;
}
void itkProcessRegistration::setOutput(dtkAbstractData * output)
{
    d->output = output;
}

itk::ImageBase<3>::Pointer itkProcessRegistration::fixedImage()
{
    return d->fixedImage;
}

QVector<itk::ImageBase<3>::Pointer> itkProcessRegistration::movingImages()
{
    return d->movingImages;
}
itkProcessRegistration::ImageType itkProcessRegistration::fixedImageType()
{
    return d->fixedImageType;
}

itkProcessRegistration::ImageType itkProcessRegistration::movingImageType()
{
    return d->movingImageType;
}

bool itkProcessRegistration::write(const QStringList& files)
{
    if (files.count()!=2)
    {
        qDebug() << "can't write, the list doesn't have 2 items";
        return false;
    }

    if (!files.at(0).isEmpty())
    {
        return write(files.at(0));
    }

    if(!files.at(1).isEmpty())
    {
        return writeTransform(files.at(1));
    }
    return false;
}

bool itkProcessRegistration::writeTransform(const QString& file)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(file);
    return false;
}


bool itkProcessRegistration::write(const QString& file)
{
    if (output() == NULL)
    {
        qDebug() << "the registration method hasn't been run yet.";
        return false;
    }

    bool writeSuccess = false;
    dtkAbstractData * out = output();
    QList<QString> writers = dtkAbstractDataFactory::instance()->writers();
    for (int i=0; i<writers.size(); i++)
    {
        dtkAbstractDataWriter *dataWriter = dtkAbstractDataFactory::instance()->writer(writers[i]);
        qDebug() << "trying " << dataWriter->identifier();

        if (! dataWriter->handled().contains(out->identifier()))
        {
          qDebug() << "failed with " << dataWriter->identifier();
          continue;
        }

        qDebug() << "success with " << dataWriter->identifier();
        dataWriter->setData (out);

        qDebug() << "trying to write in file : "<<file;

        if (dataWriter->canWrite( file )) {
            if (dataWriter->write( file )) {
                //dtkDataList.push_back (output);
                writeSuccess = true;
                delete dataWriter;
                break;
            }
        }
        delete dataWriter;
    }
    return writeSuccess;
}
