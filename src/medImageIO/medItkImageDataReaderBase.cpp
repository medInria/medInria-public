/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageDataReaderBase.h>

#include <medAbstractImageData.h>
#include <medMetaDataKeys.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <itkImageFileReader.h>
#include <itkRGBPixel.h>
#include <itkVector.h>
#include <itkRGBAPixel.h>
#include <itkObjectFactoryBase.h>
#include <itkMetaDataObject.h>

medItkImageDataReaderBase::medItkImageDataReaderBase() : dtkAbstractDataReader()
{
    this->io = 0;
}


medItkImageDataReaderBase::~medItkImageDataReaderBase()
{
}

bool medItkImageDataReaderBase::canRead (const QString& path)
{
    if (this->io.IsNull())
        return false;

    if (!this->io->CanReadFile( path.toAscii().constData() ))
    {
        return false;
    }
    else
    {
        // regular image readers can only read images with 4 or less
        // components if the pixel type is vector. images with more
        // will be handled by more specific image readers
        // (e.g. tensors if 6 or 9 components)

        this->io->SetFileName( path.toAscii().constData() );
        try {
           this->io->ReadImageInformation();
        }
        catch (itk::ExceptionObject &e) {
           qDebug() << e.GetDescription();
           return false;
        }

        if (this->io->GetPixelType() == itk::ImageIOBase::VECTOR)
        {
            return this->io->GetNumberOfComponents() <= 4 ;
        }
        else
        {
            return true;
        }
    }
}

bool medItkImageDataReaderBase::canRead (const QStringList& paths)
{
    if (!paths.count())
        return false;
    return this->canRead ( paths[0].toAscii().constData() );
}

bool medItkImageDataReaderBase::readInformation (const QString& path)
{
    if (this->io.IsNull())
        return false;

    this->io->SetFileName(path.toAscii().constData());
    try
    {
        this->io->ReadImageInformation();
    }
    catch (itk::ExceptionObject &e)
    {
        qDebug() << e.GetDescription();
        return false;
    }

    //TODO why all of this is in a methode named readInformation ? - RDE
    medAbstractData *medData = NULL;
    if (this->io->GetPixelType()==itk::ImageIOBase::SCALAR )
    {

        const int  dim  = this->io->GetNumberOfDimensions();
        if (!(dim>0 && dim<=4))
        {
            qDebug() << "Unrecognized component type";
            return false;
        }
        const char cdim = '0'+((dim<=3) ? 3 : 4);

        switch (this->io->GetComponentType())
        {

        case itk::ImageIOBase::UCHAR:
            medData = medAbstractDataFactory::instance()->create(QString("medItkUChar").append(cdim) + "ImageData");
            break;

        case itk::ImageIOBase::CHAR:
            medData = medAbstractDataFactory::instance()->create (QString("medItkChar").append(cdim) + "ImageData");
            break;

        case itk::ImageIOBase::USHORT:
            medData = medAbstractDataFactory::instance()->create (QString("medItkUShort").append(cdim) + "ImageData");
            break;

        case itk::ImageIOBase::SHORT:
            medData = medAbstractDataFactory::instance()->create (QString("medItkShort").append(cdim) + "ImageData");
            break;

        case itk::ImageIOBase::UINT:
            medData = medAbstractDataFactory::instance()->create (QString("medItkUInt").append(cdim) + "ImageData");
            break;

        case itk::ImageIOBase::INT:
            medData = medAbstractDataFactory::instance()->create (QString("medItkInt").append(cdim) + "ImageData");
            break;

        case itk::ImageIOBase::ULONG:
            medData = medAbstractDataFactory::instance()->create (QString("medItkULong").append(cdim) + "ImageData");
            break;

        case itk::ImageIOBase::LONG:
            medData = medAbstractDataFactory::instance()->create (QString("medItkLong").append(cdim) + "ImageData");
            break;

        case itk::ImageIOBase::FLOAT:
            medData = medAbstractDataFactory::instance()->create (QString("medItkFloat").append(cdim) + "ImageData");  // Bug ???
            break;

        case itk::ImageIOBase::DOUBLE:
            medData = medAbstractDataFactory::instance()->create (QString("medItkDouble").append(cdim) + "ImageData");  // Bug (added 4 which was not existing) ??
            break;

        default:
            qDebug() << "Unrecognized component type";
            return false;
        }
    }
    else if (this->io->GetPixelType()==itk::ImageIOBase::RGB)
    {

        switch (this->io->GetComponentType())
        {

            case itk::ImageIOBase::UCHAR:
                medData = medAbstractDataFactory::instance()->create ("medItkRgb3ImageData");
                break;

            default:
                qDebug() << "Unrecognized component type";
                return false;
        }
    }
    else if (this->io->GetPixelType()==itk::ImageIOBase::VECTOR)
    { //   Added by Theo.
        switch (this->io->GetComponentType())
        {

            case itk::ImageIOBase::UCHAR:
                medData = medAbstractDataFactory::instance()->create ("medItkVectorUChar3ImageData");
                break;
            case itk::ImageIOBase::FLOAT:
                medData = medAbstractDataFactory::instance()->create ("medItkVectorFloat3ImageData");
                break;
            case itk::ImageIOBase::DOUBLE:
                medData = medAbstractDataFactory::instance()->create ("medItkVectorDouble3ImageData");
                break;
            default:
                qDebug() << "Unrecognized component type";
                return false;
        }
    }
    else if ( this->io->GetPixelType()==itk::ImageIOBase::RGBA )
    {

        switch (this->io->GetComponentType())
        {

        case itk::ImageIOBase::UCHAR:
            medData = medAbstractDataFactory::instance()->create ("medItkRgba3ImageData");
            break;

        default:
            qDebug() << "Unrecognized component type";
            return false;
        }
    }
    else
    {
        qDebug() << "Unsupported pixel type";
        return false;
    }

    if (medData)
    {
        this->setData(medData);
        medData->addMetaData ("FilePath", QStringList() << path);
        return true;
    }
    else
        return false;
}


bool medItkImageDataReaderBase::readInformation (const QStringList& paths)
{
    if (!paths.count())
        return false;
    return this->readInformation ( paths[0].toAscii().constData() );
}

template <unsigned DIM,typename T>
bool medItkImageDataReaderBase::read_image(const QString& path,const char* type)
{
    medAbstractData* medData = dynamic_cast<medAbstractData*>(this->data());
    if (medData && medData->identifier()!=type)
        return false;

    typedef itk::Image<T,DIM> Image;
    typename itk::ImageFileReader<Image>::Pointer TReader = itk::ImageFileReader<Image>::New();
    TReader->SetImageIO(this->io);
    TReader->SetFileName(path.toAscii().constData());
    TReader->SetUseStreaming(true);
    typename Image::Pointer im = TReader->GetOutput();
    medData->setData(im);

    //  Recover the pixel meaning from the intent_name.

    const itk::MetaDataDictionary dict = im->GetMetaDataDictionary();
    std::string PixMeaning;
    if (itk::ExposeMetaData(dict,"intent_name",PixMeaning))
        medData->addMetaData(medAbstractImageData::PixelMeaningMetaData,QString(PixMeaning.c_str()));

    TReader->Update();

    return true;
}

bool medItkImageDataReaderBase::read(const QString& path)
{
    if (this->io.IsNull())
        return false;

    this->setProgress(0);

    if ( ! this->readInformation(path) )
        return false;

    this->setProgress(50);

    /*
    itk::DataImageReaderCommand::Pointer command = itk::DataImageReaderCommand::New();
    command->SetDCMTKDataImageReader ( this );
    this->io->AddObserver ( itk::ProgressEvent(), command);
    */

    try
    {
        if (!(read_image<3,unsigned char>(path,"medItkUChar3ImageData")   ||
              read_image<3,char>(path,"medItkChar3ImageData")             ||
              read_image<3,unsigned short>(path,"medItkUShort3ImageData") ||
              read_image<4,unsigned short>(path,"medItkUShort4ImageData") ||
              read_image<4,unsigned int>(path,"medItkUInt4ImageData")     ||
              read_image<4,unsigned long>(path,"medItkULong4ImageData")   ||
              read_image<4,unsigned char>(path,"medItkUChar4ImageData")   ||
              read_image<4,char>(path,"medItkChar4ImageData")             ||
              read_image<4,long>(path,"medItkLong4ImageData")             ||
              read_image<4,int>(path,"medItkInt4ImageData")               ||
              read_image<3,short>(path,"medItkShort3ImageData")           ||
              read_image<4,short>(path,"medItkShort4ImageData")           ||
              read_image<3,unsigned int>(path,"medItkUInt3ImageData")     ||
              read_image<3,int>(path,"medItkInt3ImageData")               ||
              read_image<3,unsigned long>(path,"medItkULong3ImageData")   ||
              read_image<3,long>(path,"medItkLong3ImageData")             ||
              read_image<3,float>(path,"medItkFloat3ImageData")           ||
              read_image<4,float>(path,"medItkFloat4ImageData")           ||
              read_image<3,double>(path,"medItkDouble3ImageData")         ||
              read_image<4,double>(path,"medItkDouble4ImageData")         ||
              read_image<3,itk::Vector<unsigned char,3> >(path,"medItkVectorUChar3ImageData") ||  //  Added by Theo.
              read_image<3,itk::Vector<float,3> >(path,"medItkVectorFloat3ImageData") ||
              read_image<3,itk::Vector<double,3> >(path,"medItkVectorDouble3ImageData") ||
              read_image<3,itk::RGBAPixel<unsigned char> >(path,"medItkRgba3ImageData") ||
              read_image<3,itk::RGBPixel<unsigned char> >(path,"medItkRgb3ImageData")))
        {
            qWarning() << "Unrecognized pixel type";
            return false;
        }
    }
    catch (itk::ExceptionObject &e)
    {
        qWarning() << e.GetDescription();
        return false;
    }

    this->setProgress (100);

    return true;
}

bool medItkImageDataReaderBase::read (const QStringList& paths)
{
    if (paths.count() < 1)
        return false;
    return this->read(paths[0].toAscii().constData());
}

void medItkImageDataReaderBase::setProgress (int value)
{
    emit progressed (value);
}
