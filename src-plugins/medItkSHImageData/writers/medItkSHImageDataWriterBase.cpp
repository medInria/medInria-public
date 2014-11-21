/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSHImageDataWriterBase.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkImageFileWriter.h>
#include <itkImage.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkVectorImage.h>
#include <itkVector.h>


QStringList medItkSHImageDataWriterBase::s_handled()
{
    return QStringList() << "medItkDouble3SHImageData"
                         << "medItkFloat3SHImageData";
}

medItkSHImageDataWriterBase::medItkSHImageDataWriterBase(): dtkAbstractDataWriter()
{
    this->io = 0;
}

medItkSHImageDataWriterBase::~medItkSHImageDataWriterBase()
{


}

QStringList medItkSHImageDataWriterBase::handled() const
{
    return s_handled();
}

QStringList medItkSHImageDataWriterBase::supportedFileExtensions() const
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

bool medItkSHImageDataWriterBase::canWrite(const QString& path)
{
    if (this->io.IsNull())
        return false;

    return this->io->CanWriteFile ( path.toAscii().constData() );
}

bool medItkSHImageDataWriterBase::write(const QString& path)
{
	 if (!this->data())
		 return false;

	 if (this->io.IsNull())
		 return false;

    if (medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data()) ) {

                if(medData->identifier()=="medItkFloat3SHImageData") {

		    float dummy = 0;
		    write(path, dummy);
		}

                else if(medData->identifier()=="medItkDouble3SHImageData") {

		    double dummy = 0;
		    write(path, dummy);
		}

		else {
			qWarning() << "Unrecognized pixel type";
			return false;
		}
	}

	return true;
}

template <class PixelType>
bool medItkSHImageDataWriterBase::write(const QString& path, PixelType dummyArgument)
{
    typedef typename itk::VectorImage<PixelType, 3>     SHImageType;

    typedef typename SHImageType::Pointer SHImageTypePointer;
    SHImageTypePointer image = dynamic_cast< SHImageType* >( (itk::Object*)(this->data()->output()) );

    typedef typename itk::ImageFileWriter<SHImageType>::Pointer ImageFileWriterPointer;
    ImageFileWriterPointer myWriter = itk::ImageFileWriter<SHImageType>::New();
    myWriter->SetFileName(path.toAscii().constData());
    myWriter->SetInput(/*mySH*/image);
    try {
        myWriter->Write();
    }
    catch(itk::ExceptionObject &e) {
        qDebug() << e.GetDescription();
        return false;
    }

    return true;
}
