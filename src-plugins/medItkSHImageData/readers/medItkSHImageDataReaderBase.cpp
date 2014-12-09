/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSHImageDataReaderBase.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkImageFileReader.h>
#include <itkObjectFactoryBase.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <itkImageRegionIteratorWithIndex.h>

// /////////////////////////////////////////////////////////////////
// itkDataSHImageReader
// /////////////////////////////////////////////////////////////////

medItkSHImageDataReaderBase::medItkSHImageDataReaderBase() : dtkAbstractDataReader()
{
    this->io = 0;
}

medItkSHImageDataReaderBase::~medItkSHImageDataReaderBase()
{
}

QStringList medItkSHImageDataReaderBase::handled() const
{
    return QStringList() << "medItkDouble3SHImageData"
                         << "medItkFloat3SHImageData";
}

QStringList medItkSHImageDataReaderBase::s_handled()
{
    return QStringList() << "medItkDouble3SHImageData"
                         << "medItkFloat3SHImageData";
}

bool medItkSHImageDataReaderBase::canRead (const QStringList &paths)
{
    if (paths.count())
        return this->canRead (paths[0]);
    return false;
}

bool medItkSHImageDataReaderBase::canRead (const QString &path)
{
    if (!this->io.IsNull()) {
        if (!this->io->CanReadFile ( path.toAscii().constData() ))
            return false;

        this->io->SetFileName (path.toAscii().constData());
        try {
            this->io->ReadImageInformation();
        }
        catch (itk::ExceptionObject &e) {
            qDebug() << e.GetDescription();
            return false;
        }
        if (this->io->GetNumberOfComponents/*PerPixel*/() != 15 && this->io->GetNumberOfComponents/*PerPixel*/() != 28)
            return false;

        return true;
    }
    return false;
}

bool medItkSHImageDataReaderBase::readInformation (const QStringList &paths)
{
    if (paths.count())
        return this->readInformation (paths[0]);

    return false;
}

bool medItkSHImageDataReaderBase::readInformation (const QString &path)
{
    if (this->io.IsNull())
        return false;
    
    this->io->SetFileName ( path.toAscii().constData() );
    try {
        this->io->ReadImageInformation();
    }
    catch (itk::ExceptionObject &e) {
        qDebug() << e.GetDescription();
        return false;
    }
    
    medAbstractData* medData = dynamic_cast<medAbstractData*>(this->data());

    if (!medData) {

        switch (this->io->GetComponentType()) {
        //            qDebug() << this->io->GetPixelTypeAsString() << this->io->GetComponentTypeAsString();

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

bool medItkSHImageDataReaderBase::read (const QStringList &paths)
{
    if (paths.count())
        return this->read (paths[0]);
    return false;
}

bool medItkSHImageDataReaderBase::read (const QString &path)
{
    if (this->io.IsNull())
        return false;
	
    this->readInformation ( path );
	
    qDebug() << "Read with: " << this->description();

    if (medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data()) ) {

        if (medData->identifier()=="medItkDouble3SHImageData") {
            typedef itk::VectorImage<double, 3> SHImageType;

            typedef itk::ImageFileReader<SHImageType> ReaderType;

            SHImageType::Pointer image = 0;

            ReaderType::Pointer reader = ReaderType::New();
            reader->SetImageIO (this->io);
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
            reader->SetImageIO (this->io);
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
