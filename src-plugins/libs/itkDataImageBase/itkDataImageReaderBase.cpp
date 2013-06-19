/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataImageReaderBase.h>

#include <medAbstractDataImage.h>
#include <medMetaDataKeys.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <itkImageFileReader.h>
#include <itkRGBPixel.h>
#include <itkVector.h>
#include <itkRGBAPixel.h>
#include <itkObjectFactoryBase.h>
#include <itkMetaDataObject.h>

itkDataImageReaderBase::itkDataImageReaderBase() : dtkAbstractDataReader()
{
    this->io = 0;
}


itkDataImageReaderBase::~itkDataImageReaderBase()
{
}

bool itkDataImageReaderBase::canRead (const QString& path)
{
    if (this->io.IsNull())
        return false;

    if (!this->io->CanReadFile( path.toAscii().constData() ))
    {
        return false;
    }
    else
    {
        // regular image readers can only read images with 4 or less
        // components if the pixel type is vector. images with more
        // will be handled by more specific image readers
        // (e.g. tensors if 6 or 9 components)

        this->io->SetFileName( path.toAscii().constData() );
        try {
           this->io->ReadImageInformation();
        }
        catch (itk::ExceptionObject &e) {
           qDebug() << e.GetDescription();
           return false;
        }

        if (this->io->GetPixelType() == itk::ImageIOBase::VECTOR)
        {
            return this->io->GetNumberOfComponents() <= 4 ;
        }
        else
        {
            return true;
        }
    }
}

bool itkDataImageReaderBase::canRead (const QStringList& paths)
{
    if (!paths.count())
        return false;
    return this->canRead ( paths[0].toAscii().constData() );
}

bool itkDataImageReaderBase::readInformation (const QString& path)
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

    dtkSmartPointer<dtkAbstractData> dtkdata = this->data();

    if (!dtkdata) {

        if (this->io->GetPixelType()==itk::ImageIOBase::SCALAR ) {

            const int  dim  = this->io->GetNumberOfDimensions();
            if (!(dim>0 && dim<=4)) {
                qDebug() << "Unrecognized component type";
                return false;
            }

            const char cdim = '0'+((dim<=3) ? 3 : 4);

            switch (this->io->GetComponentType()) {

                case itk::ImageIOBase::UCHAR:
                    dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer (QString("itkDataImageUChar").append(cdim));
                    break;

                case itk::ImageIOBase::CHAR:
                    dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer (QString("itkDataImageChar").append(cdim));
                    break;

                case itk::ImageIOBase::USHORT:
                    dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer (QString("itkDataImageUShort").append(cdim));
                    break;

                case itk::ImageIOBase::SHORT:
                    dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer (QString("itkDataImageShort").append(cdim));
                    break;

                case itk::ImageIOBase::UINT:
                    dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer (QString("itkDataImageUInt").append(cdim));
                    break;

                case itk::ImageIOBase::INT:
                    dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer (QString("itkDataImageInt").append(cdim));
                    break;

                case itk::ImageIOBase::ULONG:
                    dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer (QString("itkDataImageULong").append(cdim));
                    break;

                case itk::ImageIOBase::LONG:
                    dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer (QString("itkDataImageLong").append(cdim));
                    break;

                case itk::ImageIOBase::FLOAT:
                    dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer (QString("itkDataImageDouble").append(cdim));  // Bug ???
                    break;

                case itk::ImageIOBase::DOUBLE:
                    dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer (QString("itkDataImageDouble").append(cdim));  // Bug (added 4 which was not existing) ??
                    break;

                default:
                    qDebug() << "Unrecognized component type";
                    return false;
            }
        }
        else if (this->io->GetPixelType()==itk::ImageIOBase::RGB) {

            switch (this->io->GetComponentType()) {

                case itk::ImageIOBase::UCHAR:
                    dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer ("itkDataImageRGB3");
                    break;

                default:
                    qDebug() << "Unrecognized component type";
                    return false;
            }
        }
        else if (this->io->GetPixelType()==itk::ImageIOBase::VECTOR) { //   Added by Theo.
            qDebug() << "this->io->GetPixelType()" << this->io->GetComponentType();
            switch (this->io->GetComponentType()) {

                case itk::ImageIOBase::UCHAR:
                    dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer ("itkDataImageVectorUChar3");
                    break;
                case itk::ImageIOBase::FLOAT:
                    dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer ("itkDataImageVectorFloat3");
                    break;
                default:
                    qDebug() << "Unrecognized component type";
                    return false;
            }
        }
        else if ( this->io->GetPixelType()==itk::ImageIOBase::RGBA ) {

            switch (this->io->GetComponentType()) {

            case itk::ImageIOBase::UCHAR:
                dtkdata = dtkAbstractDataFactory::instance()->createSmartPointer ("itkDataImageRGBA3");
                break;

            default:
                qDebug() << "Unrecognized component type";
                return false;
            }
        }
        else {
            qDebug() << "Unsupported pixel type";
            return false;
        }

        if (dtkdata)
            this->setData(dtkdata);
    }


    if (dtkdata) {
        /*
      QStringList patientName;
      QStringList studyName;
      QStringList seriesName;
      QStringList filePaths;

      patientName << this->io->GetPatientName().c_str();
      studyName << this->io->GetStudyDescription().c_str();
      seriesName << this->io->GetSeriesDescription().c_str();

      for (unsigned int i=0; i<this->io->GetOrderedFileNames().size(); i++ )
      filePaths << this->io->GetOrderedFileNames()[i].c_str();

      if (!dtkdata->hasMetaData ( medMetaDataKeys::PatientName.key()) ))
      dtkdata->addMetaData ( medMetaDataKeys::PatientName.key(), patientName );
      else
      dtkdata->setMetaData ( medMetaDataKeys::PatientName.key(), patientName );

      if (!dtkdata->hasMetaData (medMetaDataKeys::StudyDescription.key()))
      dtkdata->addMetaData ( medMetaDataKeys::StudyDescription.key(), studyName );
      else
      dtkdata->setMetaData ( medMetaDataKeys::StudyDescription.key(), studyName );

      if (!dtkdata->hasMetaData (medMetaDataKeys::SeriesDescription.key() ))
      dtkdata->addMetaData ( medMetaDataKeys::SeriesDescription.key(), seriesName );
      else
      dtkdata->setMetaData ( medMetaDataKeys::SeriesDescription.key(), seriesName );
    */
        dtkdata->addMetaData ("FilePath", QStringList() << path);

    }
    
    return true;
}


bool itkDataImageReaderBase::readInformation (const QStringList& paths)
{
    if (!paths.count())
        return false;
    return this->readInformation ( paths[0].toAscii().constData() );
}

template <unsigned DIM,typename T>
bool itkDataImageReaderBase::read_image(const QString& path,const char* type) {
    dtkAbstractData* dtkdata = this->data();
    if (dtkdata && dtkdata->identifier()!=type)
        return false;

    typedef itk::Image<T,DIM> Image;
    typename itk::ImageFileReader<Image>::Pointer TReader = itk::ImageFileReader<Image>::New();
    TReader->SetImageIO(this->io);
    TReader->SetFileName(path.toAscii().constData());
    TReader->SetUseStreaming(true);
    typename Image::Pointer im = TReader->GetOutput();
    dtkdata->setData(im);

    //  Recover the pixel meaning from the intent_name.

    const itk::MetaDataDictionary dict = im->GetMetaDataDictionary();
    std::string PixMeaning;
    if (itk::ExposeMetaData(dict,"intent_name",PixMeaning))
        dtkdata->addMetaData(medAbstractDataImage::PixelMeaningMetaData,QString(PixMeaning.c_str()));

    TReader->Update();

    return true;
}

bool itkDataImageReaderBase::read(const QString& path)
{
    if (this->io.IsNull())
        return false;

    this->setProgress(0);

    this->readInformation(path);

    this->setProgress(50);

    /*
    itk::DataImageReaderCommand::Pointer command = itk::DataImageReaderCommand::New();
    command->SetDCMTKDataImageReader ( this );
    this->io->AddObserver ( itk::ProgressEvent(), command);
    */

    try {
        if (!(read_image<3,unsigned char>(path,"itkDataImageUChar3")   ||
              read_image<3,char>(path,"itkDataImageChar3")             ||
              read_image<3,unsigned short>(path,"itkDataImageUShort3") ||
              read_image<4,unsigned short>(path,"itkDataImageUShort4") ||
              read_image<4,unsigned int>(path,"itkDataImageUInt4")     ||
              read_image<4,unsigned long>(path,"itkDataImageULong4")   ||
              read_image<4,unsigned char>(path,"itkDataImageUChar4")   ||
              read_image<4,char>(path,"itkDataImageChar4")             ||
              read_image<4,long>(path,"itkDataImageLong4")             ||
              read_image<4,int>(path,"itkDataImageInt4")               ||
              read_image<3,short>(path,"itkDataImageShort3")           ||
              read_image<4,short>(path,"itkDataImageShort4")           ||
              read_image<3,unsigned int>(path,"itkDataImageUInt3")     ||
              read_image<3,int>(path,"itkDataImageInt3")               ||
              read_image<3,unsigned long>(path,"itkDataImageULong3")   ||
              read_image<3,long>(path,"itkDataImageLong3")             ||
              read_image<3,float>(path,"itkDataImageFloat3")           ||
              read_image<4,float>(path,"itkDataImageFloat4")           ||
              read_image<3,double>(path,"itkDataImageDouble3")         ||
              read_image<4,double>(path,"itkDataImageDouble4")         ||
              read_image<3,itk::Vector<unsigned char,3> >(path,"itkDataImageVectorUChar3") ||  //  Added by Theo.
              read_image<3,itk::Vector<float,3> >(path,"itkDataImageVectorFloat3") ||
              read_image<3,itk::RGBAPixel<unsigned char> >(path,"itkDataImageRGBA3") ||
              read_image<3,itk::RGBPixel<unsigned char> >(path,"itkDataImageRGB3")))
        {
            qWarning() << "Unrecognized pixel type";
            return false;
        }
    } catch (itk::ExceptionObject &e) {
        qDebug() << e.GetDescription();
        return false;
    }

    this->setProgress (100);

    //this->io->RemoveAllObservers ();

    return true;

}

bool itkDataImageReaderBase::read (const QStringList& paths)
{
    if (!paths.count())
        return false;
    return this->read ( paths[0].toAscii().constData() );
}

void itkDataImageReaderBase::setProgress (int value)
{
    emit progressed (value);
}
