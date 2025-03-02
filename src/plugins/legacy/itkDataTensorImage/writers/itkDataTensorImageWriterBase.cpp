/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataTensorImageWriterBase.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkTensor.h>
#include <itkImageFileWriter.h>
#include <itkImage.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkVectorImage.h>
#include <itkVector.h>

itkDataTensorImageWriterBase::itkDataTensorImageWriterBase() : medAbstractDataWriter()
{
    this->io = nullptr;
}

itkDataTensorImageWriterBase::~itkDataTensorImageWriterBase()
{
}

QStringList itkDataTensorImageWriterBase::handled() const
{
    return s_handled();
}

QStringList itkDataTensorImageWriterBase::supportedFileExtensions() const
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

QStringList itkDataTensorImageWriterBase::s_handled()
{
    return QStringList() << "itkDataTensorImageDouble3"
                         << "itkDataTensorImageFloat3";
}

bool itkDataTensorImageWriterBase::canWrite(const QString& path)
{
    if (this->io.IsNull())
        return false;

    return this->io->CanWriteFile ( path.toUtf8().constData() );
}

bool itkDataTensorImageWriterBase::write(const QString& path)
{
    if (!this->data())
        return false;

    if (this->io.IsNull())
        return false;

    if (medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data()) ) {

        if(medData->identifier()=="itkDataTensorImageFloat3") {

            float dummy = 0;
            write(path, dummy);
        }
        else if(medData->identifier()=="itkDataTensorImageDouble3") {

            double dummy = 0;
            write(path, dummy);
        }
        else
        {
            qWarning() << "Unrecognized pixel type";
            return false;
        }
    }

    return true;
}

template <class PixelType>
bool itkDataTensorImageWriterBase::write(const QString& path, PixelType dummyArgument)
{
    typedef typename itk::Vector<PixelType, 6>     VectorType;
    typedef typename itk::Image<VectorType, 3>     VectorImageType;
    typedef typename itk::Tensor<PixelType, 3>     TensorType;
    typedef typename itk::Image<TensorType, 3>     TensorImageType;

    typedef typename VectorImageType::Pointer VectorImageTypePointer;
    VectorImageTypePointer myTensorImage = VectorImageType::New();

    typedef typename TensorImageType::Pointer TensorImageTypePointer;
    TensorImageTypePointer image = dynamic_cast< TensorImageType* >( (itk::Object*)(this->data()->output()) );

    typedef typename TensorImageType::RegionType TensorImageTypeRegionType;
    TensorImageTypeRegionType region = image->GetLargestPossibleRegion();

    myTensorImage->SetRegions (region);
    myTensorImage->SetSpacing (image->GetSpacing());
    myTensorImage->SetOrigin (image->GetOrigin());
    myTensorImage->SetDirection (image->GetDirection());
    try {
        myTensorImage->Allocate();
    }
    catch (itk::ExceptionObject &e) {
        std::cerr << e;
        throw itk::ExceptionObject (__FILE__,__LINE__,"Error during memory allocation.");
    }

    typedef itk::ImageRegionConstIterator<TensorImageType> IteratorType;
    IteratorType it (image, image->GetLargestPossibleRegion());

    itk::ImageRegionIteratorWithIndex<VectorImageType> itOut(myTensorImage, myTensorImage->GetLargestPossibleRegion());

    while( !it.IsAtEnd() )
    {
        TensorType tensor = it.Get();
        VectorType vec;

        for( unsigned int i=0; i<6; i++) {
            vec[i] = static_cast<float>(tensor[i]);
        }
        itOut.Set (vec);

        ++it;
        ++itOut;
    }

    typedef typename itk::ImageFileWriter<VectorImageType>::Pointer ImageFileWriterPointer;
    ImageFileWriterPointer myWriter = itk::ImageFileWriter<VectorImageType>::New();
    myWriter->SetFileName(path.toUtf8().constData());
    myWriter->SetInput(myTensorImage);
    try
    {
        myWriter->Write();
    }
    catch(itk::ExceptionObject &e)
    {
        qDebug() << e.GetDescription();
        return false;
    }

    return true;
}
