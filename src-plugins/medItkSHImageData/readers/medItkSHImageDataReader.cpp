/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSHImageDataReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkImageFileReader.h>
#include <itkObjectFactoryBase.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <itkImageRegionIteratorWithIndex.h>

// /////////////////////////////////////////////////////////////////
// medItkSHImageDataReader
// /////////////////////////////////////////////////////////////////

medItkSHImageDataReader::medItkSHImageDataReader() : dtkAbstractDataReader()
{
}

medItkSHImageDataReader::~medItkSHImageDataReader()
{
}

QStringList medItkSHImageDataReader::handled() const
{
    return QStringList() << "medItkDouble3SHImageData"
                         << "medItkFloat3SHImageData";
}

QStringList medItkSHImageDataReader::s_handled()
{
    return QStringList() << "medItkDouble3SHImageData"
                         << "medItkFloat3SHImageData";
}

bool medItkSHImageDataReader::registered()
{
  return medAbstractDataFactory::instance()->registerDataReaderType("medItkSHImageDataReader", s_handled(),
                                                                    createmedItkSHImageDataReader);
}

QString medItkSHImageDataReader::identifier() const
{
    return "medItkSHImageDataReader";
}

QString medItkSHImageDataReader::description() const
{
    return "Reader for spherical harmonics images";
}

bool medItkSHImageDataReader::canRead (const QStringList &paths)
{
    if (paths.count())
        return this->canRead (paths[0]);
    return false;
}

bool medItkSHImageDataReader::canRead (const QString &path)
{
    itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(path.toAscii().constData(),
                                                                           itk::ImageIOFactory::ReadMode);

    if (!imageIO.IsNull()) {
        if (!imageIO->CanReadFile ( path.toAscii().constData() ))
            return false;

        imageIO->SetFileName (path.toAscii().constData());
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

bool medItkSHImageDataReader::readInformation (const QStringList &paths)
{
    if (paths.count())
        return this->readInformation (paths[0]);

    return false;
}

bool medItkSHImageDataReader::readInformation (const QString &path)
{
    itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(path.toAscii().constData(),
                                                                           itk::ImageIOFactory::ReadMode);
    
    imageIO->SetFileName ( path.toAscii().constData() );
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
            medData = medAbstractDataFactory::instance()->create ("medItkFloat3SHImageData");
            if (medData)
                this->setData ( medData );
            break;

	    case itk::ImageIOBase::DOUBLE:
            medData = medAbstractDataFactory::instance()->create ("medItkDouble3SHImageData");
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

bool medItkSHImageDataReader::read (const QStringList &paths)
{
    if (paths.count())
        return this->read (paths[0]);
    return false;
}

bool medItkSHImageDataReader::read (const QString &path)
{
    this->readInformation ( path );
	
    qDebug() << "Read with: " << this->description();

    if (medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data()) ) {

        if (medData->identifier()=="medItkDouble3SHImageData") {
            typedef itk::VectorImage<double, 3> SHImageType;

            typedef itk::ImageFileReader<SHImageType> ReaderType;

            SHImageType::Pointer image = 0;

            ReaderType::Pointer reader = ReaderType::New();
            reader->SetFileName ( path.toAscii().constData() );
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
        else if (medData->identifier()=="medItkFloat3SHImageData") {
            typedef itk::VectorImage<float, 3> SHImageType;

            typedef itk::ImageFileReader<SHImageType> ReaderType;

            SHImageType::Pointer image = 0;

            ReaderType::Pointer reader = ReaderType::New();
            reader->SetFileName ( path.toAscii().constData() );
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

dtkAbstractDataReader *createmedItkSHImageDataReader()
{
    return new medItkSHImageDataReader;
}
