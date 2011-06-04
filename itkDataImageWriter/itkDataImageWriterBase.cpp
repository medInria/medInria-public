#include "itkDataImageWriterBase.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkLog.h>

#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkRGBPixel.h>
#include <itkVector.h>
#include <itkMetaDataObject.h>
#include <itkRGBAPixel.h>

itkDataImageWriterBase::itkDataImageWriterBase()
{
    this->io = 0;
}

itkDataImageWriterBase::~itkDataImageWriterBase()
{
}

bool itkDataImageWriterBase::canWrite(const QString& path)
{
    if (this->io.IsNull())
        return false;

    return this->io->CanWriteFile ( path.toAscii().constData() );
}

template <unsigned DIM,typename T>
bool itkDataImageWriterBase::write_image(const QString& path,const char* type) {
    dtkAbstractData* dtkdata = this->data();
    if (dtkdata && dtkdata->description()!=type)
        return false;

    typedef itk::Image<T,DIM> Image;
    typename Image::Pointer image = dynamic_cast<Image*>((itk::Object*)(this->data()->output()));
    if (image.IsNull())
        return false;
    if (dtkdata->hasMetaData(dtkAbstractDataImage::PixelMeaningMetaData)) {
        itk::MetaDataDictionary& dict = image->GetMetaDataDictionary();
        itk::EncapsulateMetaData(dict,"intent_name",dtkdata->metadata(dtkAbstractDataImage::PixelMeaningMetaData));
    }
    typename itk::ImageFileWriter<Image>::Pointer writer = itk::ImageFileWriter <Image>::New();
    writer->SetImageIO (this->io);
    writer->SetFileName(path.toAscii().constData());
    writer->SetInput(image);
    writer->Update();

    return true;
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
              write_image<3,itk::Vector<unsigned char,3> >(path,"itkDataImageVector3") || //    Added by Theo.
              write_image<3,itk::RGBAPixel<unsigned char> >(path,"itkDataImageRGBA3") ||
              write_image<3,itk::RGBPixel<unsigned char> >(path,"itkDataImageRGB3")))
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
