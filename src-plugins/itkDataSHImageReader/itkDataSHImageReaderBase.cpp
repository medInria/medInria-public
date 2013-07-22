/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataSHImageReaderBase.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkImageFileReader.h>
#include <itkObjectFactoryBase.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <itkImageRegionIteratorWithIndex.h>

// /////////////////////////////////////////////////////////////////
// itkDataSHImageReader
// /////////////////////////////////////////////////////////////////

itkDataSHImageReaderBase::itkDataSHImageReaderBase() : dtkAbstractDataReader()
{
    this->io = 0;
}

itkDataSHImageReaderBase::~itkDataSHImageReaderBase()
{
}

QStringList itkDataSHImageReaderBase::handled() const
{
    return QStringList() << "itkDataSHImageDouble3"
                         << "itkDataSHImageFloat3";
}

QStringList itkDataSHImageReaderBase::s_handled()
{
    return QStringList() << "itkDataSHImageDouble3"
                         << "itkDataSHImageFloat3";
}

bool itkDataSHImageReaderBase::canRead (const QStringList &paths)
{
    if (paths.count())
        return this->canRead (paths[0]);
    return false;
}

bool itkDataSHImageReaderBase::canRead (const QString &path)
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

bool itkDataSHImageReaderBase::readInformation (const QStringList &paths)
{
    if (paths.count())
        return this->readInformation (paths[0]);

    return false;
}

bool itkDataSHImageReaderBase::readInformation (const QString &path)
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
    
    dtkAbstractData* dtkdata = this->data();

    if (!dtkdata) {

        switch (this->io->GetComponentType()) {
        //            qDebug() << this->io->GetPixelTypeAsString() << this->io->GetComponentTypeAsString();

        case itk::ImageIOBase::FLOAT:
            dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataSHImageFloat3");
            if (dtkdata)
                this->setData ( dtkdata );
            break;

	    case itk::ImageIOBase::DOUBLE:
			dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataSHImageDouble3");
			if (dtkdata)
				this->setData ( dtkdata );
			break;

	    default:
	        qDebug() << "Unsupported component type";
			return false ;
		}
    }

    if (dtkdata) {
        dtkdata->addMetaData ("FilePath", QStringList() << path);
    }
    return true;
}

bool itkDataSHImageReaderBase::read (const QStringList &paths)
{
    if (paths.count())
        return this->read (paths[0]);
    return false;
}

bool itkDataSHImageReaderBase::read (const QString &path)
{
    if (this->io.IsNull())
        return false;
	
    this->readInformation ( path );
	
    qDebug() << "Read with: " << this->description();

    if (dtkAbstractData *dtkdata = this->data() ) {

        if (dtkdata->identifier()=="itkDataSHImageDouble3") {
            typedef itk::VectorImage<double, 3> SHImageType;
            typedef SHImageType::PixelType    SHType;

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
            dtkdata->setData (image);
        }
        else if (dtkdata->identifier()=="itkDataSHImageFloat3") {
            typedef itk::VectorImage<float, 3> SHImageType;
            typedef SHImageType::PixelType    SHType;

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
            dtkdata->setData (image);

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
