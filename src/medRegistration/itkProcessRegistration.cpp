/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkProcessRegistration.h>

#include <dtkCore/dtkSmartPointer.h>
#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractDataWriter.h>

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

#include <itkCastImageFilter.h>

#include <itkImageRegistrationMethod.h>
#include <itkMattesMutualInformationImageToImageMetric.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkEuler3DTransform.h>
#include <itkCenteredTransformInitializer.h>

#include <itkImageLinearConstIteratorWithIndex.h>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkMetaImageIO.h>
#include <itkResampleImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkExtractImageFilter.h>

#include <itkCommand.h>

#include <time.h>

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
    dtkSmartPointer<medAbstractData> output;

    template <class PixelType>
            void setInput(medAbstractData * data,int channel);
    mutable QMutex mutex;
};

// /////////////////////////////////////////////////////////////////
// itkProcessRegistration
// /////////////////////////////////////////////////////////////////

itkProcessRegistration::itkProcessRegistration() : medAbstractRegistrationProcess(), d(new itkProcessRegistrationPrivate)
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
        void itkProcessRegistrationPrivate::setInput(medAbstractData * data,int channel)
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
           return;
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
            extractFilter->SetDirectionCollapseToGuess();
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
            extractFilter->SetDirectionCollapseToGuess();
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

class CastFilterAdapter
{
public:
    virtual ~CastFilterAdapter() { }
    virtual int SetInput(void* iData) = 0;
    virtual int Update() = 0;
    virtual void* GetOutput() = 0;

};

template <class T, unsigned int S = 3>
class CastFilterTemplateAdapter : public CastFilterAdapter
{
public:
    CastFilterTemplateAdapter() : CastFilterAdapter() { m_CasterPointer = CastFilterType::New(); }

    typedef typename itk::Image< float, 3 > RegImageType; // We always convert to float,3
    typedef typename itk::Image<T, S> ImageType; // input data type
    typedef typename itk::CastImageFilter<ImageType, RegImageType> CastFilterType;
    typedef typename CastFilterType::Pointer TYPEPTR;

    int SetInput(void* iData)
    {
        m_CasterPointer->SetInput((const ImageType*)iData);
        return 0;
    }

    int Update()
    {
        m_CasterPointer->Update();
        return 0;
    }

    void* GetOutput()
    {
        return m_CasterPointer->GetOutput();
    }

private:

    TYPEPTR m_CasterPointer;


};


bool itkProcessRegistration::setInputData(medAbstractData *data, int channel)
{
    bool res = true; // default behaviour is to always pass, except in exceptional cases
    if (!data)
        return res;

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

    dtkSmartPointer <medAbstractData> convertedData = medAbstractDataFactory::instance()->create ("itkDataImageFloat3");
    foreach ( QString metaData, data->metaDataList() )
        if (!convertedData->hasMetaData(metaData))
            convertedData->addMetaData ( metaData, data->metaDataValues ( metaData ) );

    foreach ( QString property, data->propertyList() )
        convertedData->addProperty ( property,data->propertyValues ( property ) );

    if (channel==0)
        d->output = medAbstractDataFactory::instance()->create ("itkDataImageFloat3");

    QScopedPointer<CastFilterAdapter> castFilterAdapterPtr;
    if (id =="itkDataImageChar3")
    {
        castFilterAdapterPtr.reset(new CastFilterTemplateAdapter<char>);
    }
    else if (id =="itkDataImageUChar3")
    {
        castFilterAdapterPtr.reset(new CastFilterTemplateAdapter<unsigned char>);
    }
    else if (id == "itkDataImageShort3")
    {
        castFilterAdapterPtr.reset(new CastFilterTemplateAdapter<short>);
    }
    else if (id == "itkDataImageUShort3")
    {
        castFilterAdapterPtr.reset(new CastFilterTemplateAdapter<unsigned short>);
    }
    else if(id == "itkDataImageInt3")
    {
        castFilterAdapterPtr.reset(new CastFilterTemplateAdapter<int>);
    }
    else if(id == "itkDataImageUInt3")
    {
        castFilterAdapterPtr.reset(new CastFilterTemplateAdapter<unsigned int>);
    }
    else if(id == "itkDataImageLong3")
    {
        castFilterAdapterPtr.reset(new CastFilterTemplateAdapter<long>);
    }
    else if(id == "itkDataImageULong3")
    {
        castFilterAdapterPtr.reset(new CastFilterTemplateAdapter<unsigned long>);
    }
    else if(id == "itkDataImageFloat3")
    {
        d->setInput<float>(data,channel);
    }
    else if(id == "itkDataImageDouble3")
    {
        castFilterAdapterPtr.reset(new CastFilterTemplateAdapter<double>);
    }

    try
    {
        if (castFilterAdapterPtr)
        {
            castFilterAdapterPtr->SetInput(data->data());
            castFilterAdapterPtr->Update();
            convertedData->setData(castFilterAdapterPtr->GetOutput());
            d->setInput<float>(convertedData,channel);
        }
    }
    catch(itk::ExceptionObject& e)
    {
        qDebug() << e.what();
        res = false; // In this case, we've failed
    }

    return res;
}

bool itkProcessRegistration::setFixedInput(medAbstractData *data)
{
    return this->setInputData(data, 0);
}

bool itkProcessRegistration::setMovingInput(medAbstractData *data)
{
    return this->setInputData(data, 1);
}

/**
 * @brief Apply the update on a specified fixed image type.
 *
 * Called by update(). The image type is the fixed image type.
 *
 * @param ImageType: the fixed image type
 * @return int
*/
int itkProcessRegistration::update(itkProcessRegistration::ImageType)
{
    DTK_DEFAULT_IMPLEMENTATION;
    return 1;
}

/**
 * @brief Runs the process.
 *
 * @param void
 * @return int 0 if it succeeded, any other value is an error. (could be used as error code)
*/
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

/**
 * @brief Gets the registered image.
 *
 * @param void
 * @return medAbstractData *: itkDataImageXXY, same type as the moving input image.
*/
medAbstractData *itkProcessRegistration::output()
{
    return d->output;
}
void itkProcessRegistration::setOutput(medAbstractData * output)
{
    d->output = output;
}

/**
 * @brief Gets an itk smart pointer to the fixed image.
 *
 * It uses the itkImageBase class to avoid a templated method.
 * Using the fixedImageType() method will give the type necessary for a down cast.
 *
 * @return itk::ImageBase<int> NULL if none is set yet.
*/
itk::ImageBase<3>::Pointer itkProcessRegistration::fixedImage()
{
    return d->fixedImage;
}

/**
 * @brief Gets an itk smart pointer to the moving image.
 *
 * it uses the itkImageBase class to avoid a templated method.
 * Using the movingImageType() method will give the type necessary for a down cast.
 *
 * @return itk::ImageBase<int> NULL if none is set yet.
*/
QVector<itk::ImageBase<3>::Pointer> itkProcessRegistration::movingImages()
{
    return d->movingImages;
}

/**
 * @brief Gets the fixed image ImageType.
 *
 * @return itkProcessRegistration::ImageType
*/
itkProcessRegistration::ImageType itkProcessRegistration::fixedImageType()
{
    return d->fixedImageType;
}

/**
 * @brief Gets the moving image ImageType.
 *
 * @return itkProcessRegistration::ImageType
*/
itkProcessRegistration::ImageType itkProcessRegistration::movingImageType()
{
    return d->movingImageType;
}

/**
 * @brief Writes to a file an image or a transformation.
 *
 * This function is inherited from dtk.
 * @warning This function writes the image in the first file,
 * and the transformation in the second. Otheritems in the list are ignored.
 * An empty string as a first element with a path as the second only writes the transformation.
 * A single element in the list means only the image will be written.
 *
 * This function is usualy called from the generic registration toolbox.
 *
 * @param files: list of File path. Here files[0] is a path to the image,
 * and files[1] a path to a transformation.
 * @return bool: true on successful operation, false otherwise.
*/
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

/**
 * @brief Writes a transformation to a file.
 *
 * Given the registration (rigid, non-rigid),
 * this could be a simple matrix or a displacement field.
 *
 * @param file: path to the file.
 * @return bool: true on successful operation, false otherwise.
*/
bool itkProcessRegistration::writeTransform(const QString& file)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(file);
    return false;
}

/**
 * @brief Writes the resulting image to a file.
 *
 * Uses a dtkAbstractDataWriter to export it the file.
 * If no suitable writer is found by the factory, the method returns false.
 * @param file: path to the file.
 * @return bool: true on successful operation, false otherwise.
*/
bool itkProcessRegistration::write(const QString& file)
{
    if (output() == NULL)
    {
        qDebug() << "the registration method hasn't been run yet.";
        return false;
    }

    bool writeSuccess = false;
    medAbstractData * out = output();
    QList<QString> writers = medAbstractDataFactory::instance()->writers();
    for (int i=0; i<writers.size(); i++)
    {
        dtkAbstractDataWriter *dataWriter = medAbstractDataFactory::instance()->writer(writers[i]);
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
                //medDataList.push_back (output);
                writeSuccess = true;
                delete dataWriter;
                break;
            }
        }
        delete dataWriter;
    }
    return writeSuccess;
}
