/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataImageReaderBase.h>

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

itkDataImageReaderBase::itkDataImageReaderBase() : dtkAbstractDataReader()
{
    this->io = 0;
}


itkDataImageReaderBase::~itkDataImageReaderBase()
{
}

bool itkDataImageReaderBase::canRead (const QString& path)
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

bool itkDataImageReaderBase::canRead (const QStringList& paths)
{
    if (!paths.count())
        return false;
    return this->canRead ( paths[0].toAscii().constData() );
}

bool itkDataImageReaderBase::readInformation (const QString& path)
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

    const int  dim  = this->io->GetNumberOfDimensions();
    if (!(dim>0 && dim<=4))
    {
        qDebug() << "Unrecognized component type";
        return false;
    }
    const char cdim = '0'+((dim<=3) ? 3 : 4);

    //TODO why all of this is in a methode named readInformation ? - RDE
    medAbstractData *medData = NULL;
    if (this->io->GetPixelType()==itk::ImageIOBase::SCALAR )
    {
        switch (this->io->GetComponentType())
        {

            case itk::ImageIOBase::UCHAR:
                medData = medAbstractDataFactory::instance()->create(QString("itkDataImageUChar").append(cdim));
                break;

            case itk::ImageIOBase::CHAR:
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageChar").append(cdim));
                break;

            case itk::ImageIOBase::USHORT:
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageUShort").append(cdim));
                break;

            case itk::ImageIOBase::SHORT:
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageShort").append(cdim));
                break;

            case itk::ImageIOBase::UINT:
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageUInt").append(cdim));
                break;

            case itk::ImageIOBase::INT:
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageInt").append(cdim));
                break;

            case itk::ImageIOBase::ULONG:
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageULong").append(cdim));
                break;

            case itk::ImageIOBase::LONG:
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageLong").append(cdim));
                break;

            case itk::ImageIOBase::FLOAT:
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageDouble").append(cdim));  // Bug ???
                break;

            case itk::ImageIOBase::DOUBLE:
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageDouble").append(cdim));  // Bug (added 4 which was not existing) ??
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
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageRGB").append(cdim)); 
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
                medData = medAbstractDataFactory::instance()->create ("itkDataImageVectorUChar3");
                break;
            case itk::ImageIOBase::FLOAT:
                medData = medAbstractDataFactory::instance()->create ("itkDataImageVectorFloat3");
                break;
            case itk::ImageIOBase::DOUBLE:
                medData = medAbstractDataFactory::instance()->create ("itkDataImageVectorDouble3");
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
            medData = medAbstractDataFactory::instance()->create ("itkDataImageRGBA3");
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


bool itkDataImageReaderBase::readInformation (const QStringList& paths)
{
    if (!paths.count())
        return false;
    return this->readInformation ( paths[0].toAscii().constData() );
}

template <unsigned DIM,typename T>
bool itkDataImageReaderBase::read_image(const QString& path,const char* type)
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

bool itkDataImageReaderBase::read(const QString& path)
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
        if (!(read_image<3,unsigned char>(path,"itkDataImageUChar3")   ||
              read_image<3,char>(path,"itkDataImageChar3")             ||
              read_image<3,unsigned short>(path,"itkDataImageUShort3") ||
              read_image<4,unsigned short>(path,"itkDataImageUShort4") ||
              read_image<4,unsigned int>(path,"itkDataImageUInt4")     ||
              read_image<4,unsigned long>(path,"itkDataImageULong4")   ||
              read_image<4,unsigned char>(path,"itkDataImageUChar4")   ||
              read_image<4,char>(path,"itkDataImageChar4")             ||
              read_image<4,long>(path,"itkDataImageLong4")             ||
              read_image<4,int>(path,"itkDataImageInt4")               ||
              read_image<3,short>(path,"itkDataImageShort3")           ||
              read_image<4,short>(path,"itkDataImageShort4")           ||
              read_image<3,unsigned int>(path,"itkDataImageUInt3")     ||
              read_image<3,int>(path,"itkDataImageInt3")               ||
              read_image<3,unsigned long>(path,"itkDataImageULong3")   ||
              read_image<3,long>(path,"itkDataImageLong3")             ||
              read_image<3,float>(path,"itkDataImageFloat3")           ||
              read_image<4,float>(path,"itkDataImageFloat4")           ||
              read_image<3,double>(path,"itkDataImageDouble3")         ||
              read_image<4,double>(path,"itkDataImageDouble4")         ||
              read_image<3,itk::Vector<unsigned char,3> >(path,"itkDataImageVectorUChar3") ||  //  Added by Theo.
              read_image<3,itk::Vector<float,3> >(path,"itkDataImageVectorFloat3") ||
              read_image<3,itk::Vector<double,3> >(path,"itkDataImageVectorDouble3") ||
              read_image<3,itk::RGBAPixel<unsigned char> >(path,"itkDataImageRGBA3") ||
              read_image<3,itk::RGBPixel<unsigned char> >(path,"itkDataImageRGB3")   ||
              read_image<4,itk::RGBPixel<unsigned char> >(path,"itkDataImageRGB4"))) 
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

bool itkDataImageReaderBase::read (const QStringList& paths)
{
    if (paths.count() < 1)
        return false;
    return this->read(paths[0].toAscii().constData());
}

void itkDataImageReaderBase::setProgress (int value)
{
    emit progressed (value);
}
