#include <medITKImageDTKImageConverter.h>

#include <medAbstractImageData.h>
#include <medAbstractDataFactory.h>
#include <dtkImaging>
#include <dtkCore.h>

#include "dtkItkImageConverter.h"

class medITKImageDTKImageConverterPrivate
{
public:
    void setImage(dtkImage* img)
    {
        this->img=img;
    }

    void setId(QString id)
    {
        qDebug()<<id;
        this->id=id;
    }

    template <typename ImgT, int dim> void exec()
    {
        qDebug()<<Q_FUNC_INFO<<__LINE__;
        typedef dtkItkImageConverter<ImgT,dim> Converter;
        typedef itk::Image<typename dtkItkPixelTypeTrait<ImgT>::itkPixelType,dim> ImageType;
        typename ImageType::Pointer itkImage=ImageType::New();    qDebug()<<Q_FUNC_INFO<<__LINE__;
        Converter::convertToNative(img, itkImage);
        qDebug()<<Q_FUNC_INFO<<__LINE__;
        medImage= qobject_cast<medAbstractImageData *>(medAbstractDataFactory::instance()->create(id));

        qDebug()<<Q_FUNC_INFO<<__LINE__;
        if(medImage)
            medImage->setData(itkImage);
        qDebug()<<Q_FUNC_INFO<<__LINE__;
    }

    medAbstractImageData* image()
    {
        return medImage;
    }

private:
    dtkImage* img;
    medAbstractImageData* medImage;
    QString id;
};

dtkImage* medITKImageDTKImageConverter::toDtkImage(medAbstractImageData* medImage)
{
    if(!medImage)
        return NULL;

    QString id=medImage->identifier();

    qDebug()<<id<<medImage->xDimension()<<medImage->yDimension()<<medImage->zDimension();

    if ( id == "itkDataImageChar3" )
        return toDtkImage_helper<dtkScalarPixel<char>,3>(medImage);
    else if ( id == "itkDataImageUChar3" )
        return toDtkImage_helper<dtkScalarPixel<unsigned char>,3>(medImage);
    else if ( id == "itkDataImageShort3" )
        return toDtkImage_helper<dtkScalarPixel<short>,3>(medImage);
    else if ( id == "itkDataImageUShort3" )
        return toDtkImage_helper<dtkScalarPixel<unsigned short>,3>(medImage);
    else if ( id == "itkDataImageInt3" )
        return toDtkImage_helper<dtkScalarPixel<int>,3>(medImage);
    else if ( id == "itkDataImageUInt3" )
        return toDtkImage_helper<dtkScalarPixel<unsigned int>,3>(medImage);
    else if ( id == "itkDataImageLong3" )
        return toDtkImage_helper<dtkScalarPixel<long>,3>(medImage);
    else if ( id== "itkDataImageULong3" )
        return toDtkImage_helper<dtkScalarPixel<unsigned long>,3>(medImage);
    else if ( id == "itkDataImageFloat3" )
        return toDtkImage_helper<dtkScalarPixel<float>,3>(medImage);
    else if ( id == "itkDataImageDouble3" )
        return toDtkImage_helper<dtkScalarPixel<double>,3>(medImage);

    return 0;
}

template <typename T, int dim> dtkImage* medITKImageDTKImageConverter::toDtkImage_helper(medAbstractImageData* medImage)
{
    typedef dtkItkImageConverter<T, dim> ConverterType;
    typedef typename ConverterType::ItkImageType ItkType;

    ItkType* itkImg=static_cast<ItkType*> (medImage->image());
    dtkImage *img=new dtkImage();
    if(itkImg)
        ConverterType::convertFromNative(itkImg, img);
    else
        qWarning()<<"cast failed";
    return img;
}

medAbstractImageData* medITKImageDTKImageConverter::toMedImage(dtkImage* image)
{
    QString id="itkDataImage";
    switch(image->storageType())
    {
        case QMetaType::Char:
            id+="Char";
            break;

        case QMetaType::UChar:
            id+="UChar";
            break;

        case QMetaType::Short:
            id+="Short";
            break;

        case QMetaType::UShort:
            id+="UShort";
            break;
            
        case QMetaType::Int:
            id+="Int";
            break;
            
        case QMetaType::UInt:
            id+="UInt";
            break;

        case QMetaType::Long:
            id+="Long";
            break;
            
        case QMetaType::ULong:
            id+="ULong";
            break;

        case QMetaType::Float:
            id+="Float";
            break;
            
        case QMetaType::Double:
            id+="Double";
            break;
    }
    qDebug()<<Q_FUNC_INFO<<__LINE__;
    id+=QString::number(image->dim());
    qDebug()<<Q_FUNC_INFO<<__LINE__;
    medITKImageDTKImageConverterPrivate converter;
    converter.setImage(image);
    converter.setId(id);
    qDebug()<<Q_FUNC_INFO<<__LINE__;
    dtkFilterExecutor<medITKImageDTKImageConverterPrivate>::run(&converter, image);
    qDebug()<<Q_FUNC_INFO<<__LINE__;
    return converter.image();
}
