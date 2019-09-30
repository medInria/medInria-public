/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkVector.h>
#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkRGBAPixel.h>

#include <medAbstractDataFactory.h>
#include <medAbstractTypedImageData.h>
#include <itkDataImagePluginExport.h>
#include <itkDataPrivateTypes.h>

template <unsigned DIM,typename T>
struct ImagePrivateType: public itkDataScalarImagePrivateType<DIM,T>
{
    ImagePrivateType(): itkDataScalarImagePrivateType<DIM,T>() { }
    ImagePrivateType(const ImagePrivateType& other): itkDataScalarImagePrivateType<DIM,T>(other) { }
};

template <unsigned DIM,typename T,unsigned N>
struct ImagePrivateType<DIM,itk::Vector<T,N> >: public itkDataVectorImagePrivateType<DIM,itk::Vector<T,N> >
{
    ImagePrivateType(): itkDataVectorImagePrivateType<DIM,itk::Vector<T,N> > () { }
    ImagePrivateType(const ImagePrivateType& other): itkDataVectorImagePrivateType<DIM,itk::Vector<T,N> >(other) { }
};

template <unsigned DIM,typename T>
struct ImagePrivateType<DIM,itk::RGBPixel<T> >: public itkDataVectorImagePrivateType<DIM,itk::RGBPixel<T> >
{
    ImagePrivateType(): itkDataVectorImagePrivateType<DIM,itk::RGBPixel<T> >() { }
    ImagePrivateType(const ImagePrivateType& other): itkDataVectorImagePrivateType<DIM,itk::RGBPixel<T> >(other) { }
};

template <unsigned DIM,typename T>
struct ImagePrivateType<DIM,itk::RGBAPixel<T> >: public itkDataVectorImagePrivateType<DIM,itk::RGBAPixel<T> >
{
    ImagePrivateType(): itkDataVectorImagePrivateType<DIM,itk::RGBAPixel<T> >() { }
    ImagePrivateType(const ImagePrivateType& other): itkDataVectorImagePrivateType<DIM,itk::RGBAPixel<T> >(other) { }
};

template <unsigned DIM,typename T,const char* ID>
class ITKDATAIMAGEPLUGIN_EXPORT itkDataImage: public medAbstractTypedImageData<DIM,T>
{

    typedef ImagePrivateType<DIM,T> PrivateMember;

    // Special macro because this class is templated, so moc can't run on it
    // so we don;t have a staticMetaObject attribute, hence we need to define our
    // own staticIdentifier() method, as done below.
    MED_DATA_INTERFACE_NO_MOC("Itk Data Image", "Itk Data Image")

public:

    typedef T PixelType;
    enum { Dimension=DIM };

    itkDataImage(): medAbstractTypedImageData<DIM,T>(),d(new PrivateMember) { }
    itkDataImage(const itkDataImage& other): medAbstractTypedImageData<DIM,T>(), d(new PrivateMember(*(other.d))) { }
    ~itkDataImage()
    {
        delete d;
        d = nullptr;
    }

    static QString staticIdentifier() { return ID; }
    static bool registered() {
        return medAbstractDataFactory::instance()->registerDataType<itkDataImage<DIM,T,ID> >();
    }

    virtual itkDataImage* clone()
    {
        return new itkDataImage(*this);
    }

    // Inherited slots (through virtual member functions).
    void* output() { return d->image.GetPointer(); }
    void* data() { return d->image.GetPointer(); }

    void setData(void* data) {
        typedef typename PrivateMember::ImageType ImageType;
        typename ImageType::Pointer image = dynamic_cast<ImageType*>(static_cast<itk::Object*>(data));
        if (image.IsNull())
        {
            qDebug() << "Cannot cast data to correct data type";
            return;
        }
        d->image = image;
        d->reset();
    }

    void update() { }

    void onMetaDataSet(const QString& key,const QString& value) {
        Q_UNUSED(key);
        Q_UNUSED(value);
    }

    void onPropertySet(const QString& key,const QString& value) {
        Q_UNUSED(key);
        Q_UNUSED(value);
    }

    // derived from medAbstractImageData

    medAbstractImageData::MatrixType orientationMatrix()
    {
       medAbstractImageData::MatrixType orientationMatrix;

       if (!d->image)
           return orientationMatrix;

       for (unsigned int i = 0;i < DIM;++i) {
           std::vector <double> orientationVector(DIM,0);
           for(unsigned int j = 0;j < DIM;++j)
               orientationVector[j] = d->image->GetDirection()(i,j);

           orientationMatrix.push_back(orientationVector);
       }

       return orientationMatrix;
    }

    int xDimension() {
        if (d->image.IsNull())
            return -1;
        return d->image->GetLargestPossibleRegion().GetSize()[0];
    }

    int yDimension() {
        if (d->image.IsNull())
            return -1;
        return d->image->GetLargestPossibleRegion().GetSize()[1];
    }

    int zDimension() {
        if (d->image.IsNull())
            return -1;
        return d->image->GetLargestPossibleRegion().GetSize()[2];
    }

    int tDimension() {
        if (d->image.IsNull())
            return -1;
        if (DIM<4)
            return 1;
        else
            return d->image->GetLargestPossibleRegion().GetSize()[3];
    }

    int minRangeValue() { return d->minRangeValue(); }
    int maxRangeValue() { return d->maxRangeValue(); }

    int scalarValueCount(int value) { return d->scalarValueCount(value); }
    int scalarValueMinCount() { return d->scalarValueMinCount(); }
    int scalarValueMaxCount() { return d->scalarValueMaxCount(); }

private:

    PrivateMember* d;
};
