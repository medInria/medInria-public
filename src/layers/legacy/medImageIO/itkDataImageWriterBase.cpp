/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataImageWriterBase.h>

#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <dtkLog/dtkLog.h>

#include <medAbstractImageData.h>
#include <medMetaDataKeys.h>

#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkRGBPixel.h>
#include <itkVector.h>
#include <itkMetaDataObject.h>
#include <itkRGBAPixel.h>

itkDataImageWriterBase::itkDataImageWriterBase()
{
    this->io = 0;

    this->medKeyToItkKey[medAbstractImageData::PixelMeaningMetaData] = "intent_name";
    this->medKeyToItkKey[medMetaDataKeys::Modality.key()] = "MED_MODALITY";
    this->medKeyToItkKey[medMetaDataKeys::Orientation.key()] = "MED_ORIENTATION";
}

itkDataImageWriterBase::~itkDataImageWriterBase()
{
}

bool itkDataImageWriterBase::canWrite(const QString& path)
{
    if (this->io.IsNull())
        return false;

    return this->io->CanWriteFile ( path.toUtf8().constData() );
}

template <unsigned DIM,typename T>
bool itkDataImageWriterBase::write_image(const QString& path,const char* type){
    medAbstractData* medData = dynamic_cast<medAbstractData*>(this->data());
    if (medData && medData->identifier() != type)
    {
        return false;
    }

    typedef itk::Image<T,DIM> Image;
    typename Image::Pointer image = dynamic_cast<Image*>((itk::Object*)(this->data()->output()));
    if (image.IsNull())
    {
        return false;
    }

    itk::MetaDataDictionary& dict = image->GetMetaDataDictionary();
    if (medData->hasMetaData(medAbstractImageData::PixelMeaningMetaData))
    {
        itk::EncapsulateMetaData(dict, "intent_name", medData->metadata(medAbstractImageData::PixelMeaningMetaData));
    }

    // save relevant metaDataKeys
    encapsulateSharedMetaData(dict);

    typename itk::ImageFileWriter<Image>::Pointer writer = itk::ImageFileWriter <Image>::New();
    writer->SetImageIO (this->io);
    writer->UseCompressionOn();
    writer->SetFileName(path.toUtf8().constData());
    writer->SetInput(image);
    writer->Update();

    return true;
}

std::string itkDataImageWriterBase::convertMedKeyToItkKey(QString medKey)
{
    std::string itkKey;
    if (this->medKeyToItkKey.contains(medKey))
    {
        itkKey = this->medKeyToItkKey[medKey];
    }
    else
    {
        itkKey = medKey.toStdString().c_str();
    }

    return itkKey;
}

void itkDataImageWriterBase::encapsulateSharedMetaData(itk::MetaDataDictionary& dict)
{
    itk::Object* itkImage = static_cast<itk::Object*>(data()->data());
    itk::MetaDataDictionary& metaDataDictionary = itkImage->GetMetaDataDictionary();

    foreach (QString metaDataKey, data()->metaDataList())
    {
        if (medMetaDataKeys::Key::fromKeyName(metaDataKey))
        {
            std::string key = convertMedKeyToItkKey(metaDataKey);
            itk::EncapsulateMetaData(metaDataDictionary, key, data()->metadata(metaDataKey).toStdString());
        }
    }
}

bool itkDataImageWriterBase::write(const QString& path)
{
    if (!this->data())
        return false;

    if (this->io.IsNull())
        return false;

    try {

        if (!(write_image<3,unsigned char>(path,"itkDataImageUChar3")   ||
              write_image<3,char>(path,"itkDataImageChar3")             ||
              write_image<3,unsigned short>(path,"itkDataImageUShort3") ||
              write_image<4,unsigned short>(path,"itkDataImageUShort4") ||
              write_image<4,unsigned int>(path,"itkDataImageUInt4")     ||
              write_image<4,unsigned long>(path,"itkDataImageULong4")   ||
              write_image<4,unsigned char>(path,"itkDataImageUChar4")   ||
              write_image<4,char>(path,"itkDataImageChar4")             ||
              write_image<4,long>(path,"itkDataImageLong4")             ||
              write_image<4,int>(path,"itkDataImageInt4")               ||
              write_image<3,short>(path,"itkDataImageShort3")           ||
              write_image<4,short>(path,"itkDataImageShort4")           ||
              write_image<3,unsigned int>(path,"itkDataImageUInt3")     ||
              write_image<3,int>(path,"itkDataImageInt3")               ||
              write_image<3,unsigned long>(path,"itkDataImageULong3")   ||
              write_image<3,long>(path,"itkDataImageLong3")             ||
              write_image<3,float>(path,"itkDataImageFloat3")           ||
              write_image<4,float>(path,"itkDataImageFloat4")           ||
              write_image<3,double>(path,"itkDataImageDouble3")         ||
              write_image<4,double>(path,"itkDataImageDouble4")         ||
              write_image<3,itk::Vector<unsigned char,3> >(path,"itkDataImageVectorUChar3") || //    Added by Theo.
              write_image<3,itk::Vector<float,3> >(path,"itkDataImageVectorFloat3") ||
              write_image<3,itk::Vector<double,3> >(path,"itkDataImageVectorDouble3") ||
              write_image<3,itk::RGBAPixel<unsigned char> >(path,"itkDataImageRGBA3") ||
              write_image<3,itk::RGBPixel<unsigned char> >(path,"itkDataImageRGB3")))
        {
            dtkWarn() << "Unrecognized pixel type";
            return false;
        }

    } catch(itk::ExceptionObject &e) {
        dtkDebug() << e.GetDescription();
        return false;
    }


    return true;
}

QStringList itkDataImageWriterBase::supportedFileExtensions() const
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
