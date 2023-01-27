/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataSHImageWriterBase.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkImageFileWriter.h>
#include <itkImage.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkVectorImage.h>
#include <itkVector.h>


QStringList itkDataSHImageWriterBase::s_handled()
{
    return QStringList() << "itkDataSHImageDouble3"
                         << "itkDataSHImageFloat3";
}

itkDataSHImageWriterBase::itkDataSHImageWriterBase(): dtkAbstractDataWriter()
{
    this->io = nullptr;
}

itkDataSHImageWriterBase::~itkDataSHImageWriterBase()
{

}

QStringList itkDataSHImageWriterBase::handled() const
{
    return s_handled();
}

QStringList itkDataSHImageWriterBase::supportedFileExtensions() const
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

bool itkDataSHImageWriterBase::canWrite(const QString& path)
{
    if (this->io.IsNull())
        return false;

    return this->io->CanWriteFile ( path.toUtf8().constData() );
}

bool itkDataSHImageWriterBase::write(const QString& path)
{
    if (!this->data())
    {
        return false;
    }

    if (this->io.IsNull())
    {
        return false;
    }

    if (medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data()) )
    {
        if(medData->identifier()=="itkDataSHImageFloat3")
        {
            float dummy = 0;
            write(path, dummy);
        }

        else if(medData->identifier()=="itkDataSHImageDouble3")
        {
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
bool itkDataSHImageWriterBase::write(const QString& path, PixelType dummyArgument)
{
    Q_UNUSED(dummyArgument)

    typedef typename itk::VectorImage<PixelType, 3>     SHImageType;

    typedef typename SHImageType::Pointer SHImageTypePointer;
    SHImageTypePointer image = dynamic_cast< SHImageType* >( (itk::Object*)(this->data()->output()) );

    typedef typename itk::ImageFileWriter<SHImageType>::Pointer ImageFileWriterPointer;
    ImageFileWriterPointer myWriter = itk::ImageFileWriter<SHImageType>::New();
    myWriter->SetFileName(path.toUtf8().constData());
    myWriter->SetInput(/*mySH*/image);
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
