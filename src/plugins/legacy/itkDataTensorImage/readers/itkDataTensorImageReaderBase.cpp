/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataTensorImageReaderBase.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkTensor.h>
#include <itkImageFileReader.h>
#include <itkObjectFactoryBase.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <itkImageRegionIteratorWithIndex.h>

// /////////////////////////////////////////////////////////////////
// itkDataTensorImageReader
// /////////////////////////////////////////////////////////////////

itkDataTensorImageReaderBase::itkDataTensorImageReaderBase() : dtkAbstractDataReader()
{
    this->io = nullptr;
}

itkDataTensorImageReaderBase::~itkDataTensorImageReaderBase()
{
}

QStringList itkDataTensorImageReaderBase::handled() const
{
    return QStringList() << "itkDataTensorImageDouble3"
                         << "itkDataTensorImageFloat3";
}

QStringList itkDataTensorImageReaderBase::s_handled()
{
    return QStringList() << "itkDataTensorImageDouble3"
                         << "itkDataTensorImageFloat3";
}

bool itkDataTensorImageReaderBase::canRead (const QStringList &paths)
{
    if (paths.count())
        return this->canRead (paths[0]);
    return false;
}

bool itkDataTensorImageReaderBase::canRead (const QString &path)
{
    if (!this->io.IsNull()) {
        if (!this->io->CanReadFile ( path.toUtf8().constData() ))
            return false;

        this->io->SetFileName (path.toUtf8().constData());
        try
        {
            this->io->ReadImageInformation();
        }
        catch (itk::ExceptionObject &e)
        {
            qDebug() << e.GetDescription();
            return false;
        }

        if (this->io->GetNumberOfComponents()!=6 && this->io->GetNumberOfComponents()!=9)
            return false;

        return true;
    }
    return false;
}

bool itkDataTensorImageReaderBase::readInformation (const QStringList &paths)
{
    if (paths.count())
        return this->readInformation (paths[0]);
    
    return false;
}

bool itkDataTensorImageReaderBase::readInformation (const QString &path)
{
    if (this->io.IsNull())
        return false;
    
    this->io->SetFileName ( path.toUtf8().constData() );
    try
    {
        this->io->ReadImageInformation();
    }
    catch (itk::ExceptionObject &e)
    {
        qDebug() << e.GetDescription();
        return false;
    }
    
    medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data());

    if (!medData) {

        switch (this->io->GetComponentType()) {

            case itk::IOComponentEnum::FLOAT:
                medData = medAbstractDataFactory::instance()->create ("itkDataTensorImageFloat3");
                if (medData)
                    this->setData ( medData );
                break;

            case itk::IOComponentEnum::DOUBLE:
                medData = medAbstractDataFactory::instance()->create ("itkDataTensorImageDouble3");
                if (medData)
                    this->setData ( medData );
                break;

            default:
                qDebug() << "Unsupported component type";
                return false;
        }
    }

    if (medData) {
        medData->addMetaData ("FilePath", QStringList() << path);
    }
    
    return true;
}

bool itkDataTensorImageReaderBase::read (const QStringList &paths)
{
    if (paths.count())
        return this->read (paths[0]);
    return false;
}

bool itkDataTensorImageReaderBase::read (const QString &path)
{
    if (this->io.IsNull())
        return false;

    this->readInformation ( path );

    qDebug() << "Read with: " << this->identifier();

    if (medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data()) ) {

        if (medData->identifier()=="itkDataTensorImageDouble3") {

            if (this->io->GetNumberOfComponents()==6) {

                typedef itk::Tensor<double, 3>    TensorType;
                typedef itk::Image<TensorType, 3> TensorImageType;

                typedef itk::Vector<double, 6>    VectorType;
                typedef itk::Image<VectorType, 3> VectorImageType;

                typedef itk::ImageFileReader<VectorImageType> ReaderType;

                VectorImageType::Pointer image = nullptr;
                {
                    ReaderType::Pointer reader = ReaderType::New();
                    reader->SetImageIO (this->io);
                    reader->SetFileName ( path.toUtf8().constData() );
                    try {
                        reader->Update();
                    }
                    catch (itk::ExceptionObject &e) {
                        qDebug() << e.GetDescription();
                        return false;
                    }
                    image = reader->GetOutput();
                }

                TensorImageType::Pointer tensors = TensorImageType::New();
                TensorImageType::RegionType region = image->GetLargestPossibleRegion();
                tensors->SetRegions   (region);
                tensors->SetSpacing   (image->GetSpacing());
                tensors->SetOrigin    (image->GetOrigin());
                tensors->SetDirection (image->GetDirection());

                try {
                    tensors->Allocate();
                }
                catch (itk::ExceptionObject &e) {
                    qDebug() << e.GetDescription();
                    return false;
                }

                itk::ImageRegionConstIteratorWithIndex<VectorImageType>  itIn (image,
                                                                               image->GetLargestPossibleRegion());
                itk::ImageRegionIteratorWithIndex<TensorImageType> itOut(tensors,
                                                                         tensors->GetLargestPossibleRegion());

                while(!itOut.IsAtEnd()) {

                    VectorType vec = itIn.Get();
                    TensorType tensor;

                    for( unsigned int j=0; j<6; j++) {
                        tensor[j] = vec[j];
                    }

                    itOut.Set (tensor);

                    ++itOut;
                    ++itIn;
                }

                medData->setData (tensors);

            }
            else if (this->io->GetNumberOfComponents()==9) {

                typedef itk::Tensor<double, 3>    TensorType;
                typedef itk::Image<TensorType, 3> TensorImageType;

                typedef itk::Vector<double, 9>    VectorType;
                typedef itk::Image<VectorType, 3> VectorImageType;

                typedef itk::ImageFileReader<VectorImageType> ReaderType;

                VectorImageType::Pointer image = nullptr;
                {
                    ReaderType::Pointer reader = ReaderType::New();
                    reader->SetImageIO (this->io);
                    reader->SetFileName ( path.toUtf8().constData() );
                    try {
                        reader->Update();
                    }
                    catch (itk::ExceptionObject &e) {
                        qDebug() << e.GetDescription();
                        return false;
                    }
                    image = reader->GetOutput();
                }

                TensorImageType::Pointer tensors = TensorImageType::New();
                TensorImageType::RegionType region = image->GetLargestPossibleRegion();
                tensors->SetRegions   (region);
                tensors->SetSpacing   (image->GetSpacing());
                tensors->SetOrigin    (image->GetOrigin());
                tensors->SetDirection (image->GetDirection());

                try {
                    tensors->Allocate();
                }
                catch (itk::ExceptionObject &e) {
                    qDebug() << e.GetDescription();
                    return false;
                }

                itk::ImageRegionConstIteratorWithIndex<VectorImageType>  itIn (image,
                                                                               image->GetLargestPossibleRegion());
                itk::ImageRegionIteratorWithIndex<TensorImageType> itOut(tensors,
                                                                         tensors->GetLargestPossibleRegion());

                while(!itOut.IsAtEnd()) {

                    VectorType vec = itIn.Get();
                    TensorType tensor;

                    for (unsigned int i=0; i<3; i++)
                        for (unsigned int j=0; j<3; j++)
                            tensor.SetComponent (i, j, vec[i*3+j]);

                    itOut.Set (tensor);

                    ++itOut;
                    ++itIn;
                }

                medData->setData (tensors);
            }
            else {
                qDebug() << "Unsupported number of components";
                return false;
            }
        }

        else if (medData->identifier()=="itkDataTensorImageFloat3") {

            if (this->io->GetNumberOfComponents()==6) {

                typedef itk::Tensor<float, 3>     TensorType;
                typedef itk::Image<TensorType, 3> TensorImageType;

                typedef itk::Vector<float, 6>     VectorType;
                typedef itk::Image<VectorType, 3> VectorImageType;

                typedef itk::ImageFileReader<VectorImageType> ReaderType;

                VectorImageType::Pointer image = nullptr;
                {
                    ReaderType::Pointer reader = ReaderType::New();
                    reader->SetImageIO (this->io);
                    reader->SetFileName ( path.toUtf8().constData() );
                    try {
                        reader->Update();
                    }
                    catch (itk::ExceptionObject &e) {
                        qDebug() << e.GetDescription();
                        return false;
                    }
                    image = reader->GetOutput();
                }

                TensorImageType::Pointer tensors = TensorImageType::New();
                TensorImageType::RegionType region = image->GetLargestPossibleRegion();
                tensors->SetRegions   (region);
                tensors->SetSpacing   (image->GetSpacing());
                tensors->SetOrigin    (image->GetOrigin());
                tensors->SetDirection (image->GetDirection());

                try {
                    tensors->Allocate();
                }
                catch (itk::ExceptionObject &e) {
                    qDebug() << e.GetDescription();
                    return false;
                }

                itk::ImageRegionConstIteratorWithIndex<VectorImageType>  itIn (image,
                                                                               image->GetLargestPossibleRegion());
                itk::ImageRegionIteratorWithIndex<TensorImageType> itOut(tensors,
                                                                         tensors->GetLargestPossibleRegion());

                while(!itOut.IsAtEnd()) {

                    VectorType vec = itIn.Get();
                    TensorType tensor;

                    for( unsigned int j=0; j<6; j++) {
                        tensor[j] = vec[j];
                    }

                    itOut.Set (tensor);

                    ++itOut;
                    ++itIn;
                }

                medData->setData (tensors);
            }
            else if (this->io->GetNumberOfComponents()==9) {

                typedef itk::Tensor<float, 3>     TensorType;
                typedef itk::Image<TensorType, 3> TensorImageType;

                typedef itk::Vector<float, 9>     VectorType;
                typedef itk::Image<VectorType, 3> VectorImageType;

                typedef itk::ImageFileReader<VectorImageType> ReaderType;

                VectorImageType::Pointer image = nullptr;
                {
                    ReaderType::Pointer reader = ReaderType::New();
                    reader->SetImageIO (this->io);
                    reader->SetFileName ( path.toUtf8().constData() );
                    try {
                        reader->Update();
                    }
                    catch (itk::ExceptionObject &e) {
                        qDebug() << e.GetDescription();
                        return false;
                    }
                    image = reader->GetOutput();
                }

                TensorImageType::Pointer tensors = TensorImageType::New();
                TensorImageType::RegionType region = image->GetLargestPossibleRegion();
                tensors->SetRegions   (region);
                tensors->SetSpacing   (image->GetSpacing());
                tensors->SetOrigin    (image->GetOrigin());
                tensors->SetDirection (image->GetDirection());

                try {
                    tensors->Allocate();
                }
                catch (itk::ExceptionObject &e) {
                    qDebug() << e.GetDescription();
                    return false;
                }

                itk::ImageRegionConstIteratorWithIndex<VectorImageType>  itIn (image,
                                                                               image->GetLargestPossibleRegion());
                itk::ImageRegionIteratorWithIndex<TensorImageType> itOut(tensors,
                                                                         tensors->GetLargestPossibleRegion());

                while(!itOut.IsAtEnd()) {

                    VectorType vec = itIn.Get();
                    TensorType tensor;

                    for (unsigned int i=0; i<3; i++)
                        for (unsigned int j=0; j<3; j++)
                            tensor.SetComponent (i, j, vec[i*3+j]);

                    itOut.Set (tensor);

                    ++itOut;
                    ++itIn;
                }

                medData->setData (tensors);
            }
            else {
                qDebug() << "Unsupported number of components";
                return false;
            }
        }
        else {
            qDebug() << "Unsupported data type";
            return false;
        }
    }
    else {
        qDebug() << "No data set or could not create one";
        return false;
    }

    return true;
}
