/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataSHImageReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkImageFileReader.h>
#include <itkObjectFactoryBase.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <itkImageRegionIteratorWithIndex.h>

// /////////////////////////////////////////////////////////////////
// itkDataSHImageReader
// /////////////////////////////////////////////////////////////////

itkDataSHImageReader::itkDataSHImageReader() : dtkAbstractDataReader()
{
}

itkDataSHImageReader::~itkDataSHImageReader()
{
}

QStringList itkDataSHImageReader::handled() const
{
    return QStringList() << "itkDataSHImageDouble3"
                         << "itkDataSHImageFloat3";
}

QStringList itkDataSHImageReader::s_handled()
{
    return QStringList() << "itkDataSHImageDouble3"
                         << "itkDataSHImageFloat3";
}

bool itkDataSHImageReader::registered()
{
  return medAbstractDataFactory::instance()->registerDataReaderType("itkDataSHImageReader", s_handled(),
                                                                    createItkDataSHImageReader);
}

QString itkDataSHImageReader::identifier() const
{
    return "itkDataSHImageReader";
}

QString itkDataSHImageReader::description() const
{
    return "Reader for spherical harmonics images";
}

bool itkDataSHImageReader::canRead (const QStringList &paths)
{
    if (paths.count())
        return this->canRead (paths[0]);
    return false;
}

bool itkDataSHImageReader::canRead (const QString &path)
{
    itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(path.toLatin1().constData(),
                                                                           itk::ImageIOFactory::ReadMode);

    if (!imageIO.IsNull()) {
        if (!imageIO->CanReadFile ( path.toLatin1().constData() ))
            return false;

        imageIO->SetFileName (path.toLatin1().constData());
        try {
            imageIO->ReadImageInformation();
        }
        catch (itk::ExceptionObject &e) {
            qDebug() << e.GetDescription();
            return false;
        }
        if (imageIO->GetNumberOfComponents/*PerPixel*/() != 15 && imageIO->GetNumberOfComponents/*PerPixel*/() != 28)
            return false;

        return true;
    }
    return false;
}

bool itkDataSHImageReader::readInformation (const QStringList &paths)
{
    if (paths.count())
        return this->readInformation (paths[0]);

    return false;
}

bool itkDataSHImageReader::readInformation (const QString &path)
{
    itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(path.toLatin1().constData(),
                                                                           itk::ImageIOFactory::ReadMode);
    
    imageIO->SetFileName ( path.toLatin1().constData() );
    try {
        imageIO->ReadImageInformation();
    }
    catch (itk::ExceptionObject &e) {
        qDebug() << e.GetDescription();
        return false;
    }
    
    medAbstractData* medData = dynamic_cast<medAbstractData*>(this->data());

    if (!medData) {

        switch (imageIO->GetComponentType()) {
        //            qDebug() << imageIO->GetPixelTypeAsString() << imageIO->GetComponentTypeAsString();

        case itk::ImageIOBase::FLOAT:
            medData = medAbstractDataFactory::instance()->create ("itkDataSHImageFloat3");
            if (medData)
                this->setData ( medData );
            break;

	    case itk::ImageIOBase::DOUBLE:
            medData = medAbstractDataFactory::instance()->create ("itkDataSHImageDouble3");
            if (medData)
                this->setData ( medData );
			break;

	    default:
	        qDebug() << "Unsupported component type";
			return false ;
		}
    }

    if (medData) {
        medData->addMetaData ("FilePath", QStringList() << path);
    }
    return true;
}

bool itkDataSHImageReader::read (const QStringList &paths)
{
    if (paths.count())
        return this->read (paths[0]);
    return false;
}

bool itkDataSHImageReader::read (const QString &path)
{
    this->readInformation ( path );
	
    qDebug() << "Read with: " << this->description();

    if (medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data()) ) {

        if (medData->identifier()=="itkDataSHImageDouble3") {
            typedef itk::VectorImage<double, 3> SHImageType;

            typedef itk::ImageFileReader<SHImageType> ReaderType;

            SHImageType::Pointer image = 0;

            ReaderType::Pointer reader = ReaderType::New();
            reader->SetFileName ( path.toLatin1().constData() );
            try {
                reader->Update();
            }
            catch (itk::ExceptionObject &e) {
                qDebug() << e.GetDescription();
                return false;
            }
            image = reader->GetOutput();
            medData->setData (image);
        }
        else if (medData->identifier()=="itkDataSHImageFloat3") {
            typedef itk::VectorImage<float, 3> SHImageType;

            typedef itk::ImageFileReader<SHImageType> ReaderType;

            SHImageType::Pointer image = 0;

            ReaderType::Pointer reader = ReaderType::New();
            reader->SetFileName ( path.toLatin1().constData() );
            try {
                reader->Update();
            }
            catch (itk::ExceptionObject &e) {
                qDebug() << e.GetDescription();
                return false;
            }
            image = reader->GetOutput();
            medData->setData (image);

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

dtkAbstractDataReader *createItkDataSHImageReader()
{
    return new itkDataSHImageReader;
}
