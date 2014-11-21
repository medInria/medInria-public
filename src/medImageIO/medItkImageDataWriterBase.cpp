/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImageDataWriterBase.h>

#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <dtkLog/dtkLog.h>

#include <medAbstractImageData.h>

#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkRGBPixel.h>
#include <itkVector.h>
#include <itkMetaDataObject.h>
#include <itkRGBAPixel.h>

medItkImageDataWriterBase::medItkImageDataWriterBase()
{
    this->io = 0;
}

medItkImageDataWriterBase::~medItkImageDataWriterBase()
{
}

bool medItkImageDataWriterBase::canWrite(const QString& path)
{
    if (this->io.IsNull())
        return false;

    return this->io->CanWriteFile ( path.toAscii().constData() );
}

template <unsigned DIM,typename T>
bool medItkImageDataWriterBase::write_image(const QString& path,const char* type) {
    medAbstractData* medData = dynamic_cast<medAbstractData*>(this->data());
    if (medData && medData->identifier()!=type)
        return false;

    typedef itk::Image<T,DIM> Image;
    typename Image::Pointer image = dynamic_cast<Image*>((itk::Object*)(this->data()->output()));
    if (image.IsNull())
        return false;
    if (medData->hasMetaData(medAbstractImageData::PixelMeaningMetaData)) {
        itk::MetaDataDictionary& dict = image->GetMetaDataDictionary();
        itk::EncapsulateMetaData(dict,"intent_name",medData->metadata(medAbstractImageData::PixelMeaningMetaData));
    }
    typename itk::ImageFileWriter<Image>::Pointer writer = itk::ImageFileWriter <Image>::New();
    writer->SetImageIO (this->io);
    writer->SetFileName(path.toAscii().constData());
    writer->SetInput(image);
    writer->Update();

    return true;
}

bool medItkImageDataWriterBase::write(const QString& path)
{
    if (!this->data())
        return false;

    if (this->io.IsNull())
        return false;

    try {

        if (!(write_image<3,unsigned char>(path,"medItkUChar3ImageData")   ||
              write_image<3,char>(path,"medItkChar3ImageData")             ||
              write_image<3,unsigned short>(path,"medItkUShort3ImageData") ||
              write_image<4,unsigned short>(path,"medItkUShort4ImageData") ||
              write_image<4,unsigned int>(path,"medItkUInt4ImageData")     ||
              write_image<4,unsigned long>(path,"medItkULong4ImageData")   ||
              write_image<4,unsigned char>(path,"medItkUChar4ImageData")   ||
              write_image<4,char>(path,"medItkChar4ImageData")             ||
              write_image<4,long>(path,"medItkLong4ImageData")             ||
              write_image<4,int>(path,"medItkInt4ImageData")               ||
              write_image<3,short>(path,"medItkShort3ImageData")           ||
              write_image<4,short>(path,"medItkShort4ImageData")           ||
              write_image<3,unsigned int>(path,"medItkUInt3ImageData")     ||
              write_image<3,int>(path,"medItkInt3ImageData")               ||
              write_image<3,unsigned long>(path,"medItkULong3ImageData")   ||
              write_image<3,long>(path,"medItkLong3ImageData")             ||
              write_image<3,float>(path,"medItkFloat3ImageData")           ||
              write_image<4,float>(path,"medItkFloat4ImageData")           ||
              write_image<3,double>(path,"medItkDouble3ImageData")         ||
              write_image<4,double>(path,"medItkDouble4ImageData")         ||
              write_image<3,itk::Vector<unsigned char,3> >(path,"medItkVectorUChar3ImageData") || //    Added by Theo.
              write_image<3,itk::Vector<float,3> >(path,"medItkVectorFloat3ImageData") ||
              write_image<3,itk::Vector<double,3> >(path,"medItkVectorDouble3ImageData") ||
              write_image<3,itk::RGBAPixel<unsigned char> >(path,"medItkRgba3ImageData") ||
              write_image<3,itk::RGBPixel<unsigned char> >(path,"medItkRgb3ImageData")))
        {
            qWarning() << "Unrecognized pixel type";
            return false;
        }

    } catch(itk::ExceptionObject &e) {
        qDebug() << e.GetDescription();
        return false;
    }


    return true;
}

QStringList medItkImageDataWriterBase::supportedFileExtensions() const
{
    QStringList ret;

    if (this->io) {
        typedef itk::ImageIOBase::ArrayOfExtensionsType ArrayOfExtensionsType;
        const ArrayOfExtensionsType & extensions = this->io->GetSupportedWriteExtensions();
        for( ArrayOfExtensionsType::const_iterator it(extensions.begin());
            it != extensions.end(); ++it )
        {
            ret << it->c_str();
        }
    }
    return ret;
}
