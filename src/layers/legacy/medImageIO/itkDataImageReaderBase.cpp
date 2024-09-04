/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataImageReaderBase.h>

#include <medAbstractImageData.h>
#include <medMetaDataKeys.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <itkImageFileReader.h>
#include <itkRGBPixel.h>
#include <itkVector.h>
#include <itkRGBAPixel.h>
#include <itkObjectFactoryBase.h>
#include <itkMetaDataObject.h>

itkDataImageReaderBase::itkDataImageReaderBase() : dtkAbstractDataReader()
{
    this->io = 0;
    this->itkKeyToMedKey["intent_name"] = medAbstractImageData::PixelMeaningMetaData;
    this->itkKeyToMedKey["MED_MODALITY"] = medMetaDataKeys::Modality.key();
    this->itkKeyToMedKey["MED_ORIENTATION"] = medMetaDataKeys::Orientation.key();
    // retrocompatibility
    this->itkKeyToMedKey["SeriesDicomID"] = medMetaDataKeys::SeriesInstanceUID.key();
    this->itkKeyToMedKey["StudyDicomID"] = medMetaDataKeys::StudyInstanceUID.key();
}


itkDataImageReaderBase::~itkDataImageReaderBase()
{
}

bool itkDataImageReaderBase::canRead (const QString& path)
{
    if (this->io.IsNull())
        return false;

    // Avoid to display log of each metadata not read by itk::ImageIOBase
    this->io->SetGlobalWarningDisplay(false);

    if (!this->io->CanReadFile( path.toUtf8().constData() ))
    {
        return false;
    }
    else
    {
        // regular image readers can only read images with 4 or less
        // components if the pixel type is vector. images with more
        // will be handled by more specific image readers
        // (e.g. tensors if 6 or 9 components)

        this->io->SetFileName( path.toUtf8().constData() );
        try {
           this->io->ReadImageInformation();
        }
        catch (itk::ExceptionObject &e) {
           qDebug() << e.GetDescription();
           return false;
        }

        if (this->io->GetPixelType() == itk::IOPixelEnum::VECTOR)
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
    return this->canRead ( paths[0] );
}

bool itkDataImageReaderBase::readInformation (const QString& path)
{
    if (this->io.IsNull())
        return false;

    this->io->SetFileName(path.toUtf8().constData());
    try
    {
        this->io->ReadImageInformation();
    }
    catch (itk::ExceptionObject &e)
    {
        qDebug() << e.GetDescription();
        return false;
    }

    medAbstractData *medData = nullptr;
    if (this->io->GetPixelType()==itk::IOPixelEnum::SCALAR )
    {
        const int  dim  = this->io->GetNumberOfDimensions();
        if (!(dim>0 && dim<=4))
        {
            qDebug() << "Unrecognized component type";
            return false;
        }
        const char cdim = '0'+((dim<=3) ? 3 : 4);

        switch (this->io->GetComponentType())
        {

            case itk::IOComponentEnum::UCHAR:
                medData = medAbstractDataFactory::instance()->create(QString("itkDataImageUChar").append(cdim));
                break;

            case itk::IOComponentEnum::CHAR:
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageChar").append(cdim));
                break;

            case itk::IOComponentEnum::USHORT:
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageUShort").append(cdim));
                break;

            case itk::IOComponentEnum::SHORT:
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageShort").append(cdim));
                break;

            case itk::IOComponentEnum::UINT:
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageUInt").append(cdim));
                break;

            case itk::IOComponentEnum::INT:
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageInt").append(cdim));
                break;

            case itk::IOComponentEnum::ULONG:
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageULong").append(cdim));
                break;

            case itk::IOComponentEnum::LONG:
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageLong").append(cdim));
                break;

            case itk::IOComponentEnum::FLOAT:
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageDouble").append(cdim));  // Bug ???
                break;

            case itk::IOComponentEnum::DOUBLE:
                medData = medAbstractDataFactory::instance()->create (QString("itkDataImageDouble").append(cdim));  // Bug (added 4 which was not existing) ??
                break;

            default:
                qDebug() << "Unrecognized component type";
                return false;
        }
    }
    else if (this->io->GetPixelType()==itk::IOPixelEnum::RGB)
    {

        switch (this->io->GetComponentType())
        {

            case itk::IOComponentEnum::UCHAR:
                medData = medAbstractDataFactory::instance()->create ("itkDataImageRGB3");
                break;

            default:
                qDebug() << "Unrecognized component type";
                return false;
        }
    }
    else if (this->io->GetPixelType()==itk::IOPixelEnum::VECTOR)
    { //   Added by Theo.
        switch (this->io->GetComponentType())
        {

            case itk::IOComponentEnum::UCHAR:
                medData = medAbstractDataFactory::instance()->create ("itkDataImageVectorUChar3");
                break;
            case itk::IOComponentEnum::FLOAT:
                medData = medAbstractDataFactory::instance()->create ("itkDataImageVectorFloat3");
                break;
            case itk::IOComponentEnum::DOUBLE:
                medData = medAbstractDataFactory::instance()->create ("itkDataImageVectorDouble3");
                break;
            default:
                qDebug() << "Unrecognized component type";
                return false;
        }
    }
    else if ( this->io->GetPixelType()==itk::IOPixelEnum::RGBA )
    {

        switch (this->io->GetComponentType())
        {

        case itk::IOComponentEnum::UCHAR:
            medData = medAbstractDataFactory::instance()->create ("itkDataImageRGBA3");
            break;

        default:
            qDebug() << "Unrecognized component type";
            return false;
        }
    }
    else
    {
        qDebug() << "Unsupported pixel type";
        return false;
    }

    // [HACK] : Some data have informations in their header but itk was not able to read them at this stage.
    // the itk::readImageInformation method read only default tag defined by itk
    // In case data was produced in MUSIC for instance, a lot of tags are added (for identification purpose)
    // A not exhaustive list of added tags :
    // ContainsBasicInfo Description FilePaths ITK_InputFilterName PatientID PatientName SOPInstanceUID
    // SeriesDescription SeriesDicomID SeriesID SeriesInstanceUID SeriesThumbnail Size StudyDescription StudyID
    // We decide to read all available tags here  if possible
    itk::MetaDataDictionary& metaDataDictionary = this->io->GetMetaDataDictionary();
    std::vector<std::string> keys = metaDataDictionary.GetKeys();

    for (unsigned int i = 0; i < keys.size(); i++)
    {
        std::string key = keys[i];
        std::string value;
        itk::ExposeMetaData(metaDataDictionary, key, value);

        QString metaDataKey = convertItkKeyToMedKey(key);
        if (!metaDataKey.isEmpty())
        {
            medData->setMetaData(metaDataKey, QString(value.c_str()));
        }
        else
        {
            qDebug() << metaObject()->className() << ":: found unknown key:" << QString::fromStdString(key);
        }
    }
    // [END OF HACK]

    if (medData)
    {
        this->setData(medData);
        medData->addMetaData ("FilePath", QStringList() << path);
        return true;
    }
    else
    {
        return false;
    }
}

bool itkDataImageReaderBase::readInformation (const QStringList& paths)
{
    if (!paths.count())
        return false;
    return this->readInformation ( paths[0] );
}

template <unsigned DIM,typename T>
bool itkDataImageReaderBase::read_image(const QString& path,const char* type)
{
    medAbstractData* medData = dynamic_cast<medAbstractData*>(this->data());
    if (medData && medData->identifier()!=type)
        return false;

    typedef itk::Image<T,DIM> Image;
    typename itk::ImageFileReader<Image>::Pointer TReader = itk::ImageFileReader<Image>::New();
    TReader->SetImageIO(this->io);
    TReader->SetFileName(path.toUtf8().constData());
    TReader->SetUseStreaming(true);
    TReader->Update();

    typename Image::Pointer im = TReader->GetOutput();
    medData->setData(im);

    extractMetaData();

    return true;
}

QString itkDataImageReaderBase::convertItkKeyToMedKey(std::string& keyToConvert)
{
    QString convertedKey = "";

    QString itkKey = QString::fromStdString(keyToConvert);
    if (this->itkKeyToMedKey.contains(itkKey))
    {
        convertedKey = this->itkKeyToMedKey[itkKey];
    }
    else
    {
        const medMetaDataKeys::Key* medKey = medMetaDataKeys::Key::fromKeyName(itkKey);
        if (medKey)
        {
            convertedKey = medKey->key();
        }
    }

    return convertedKey;
}

void itkDataImageReaderBase::extractMetaData()
{
    itk::Object* itkImage = static_cast<itk::Object*>(data()->data());
    itk::MetaDataDictionary& metaDataDictionary = itkImage->GetMetaDataDictionary();
    std::vector<std::string> keys = metaDataDictionary.GetKeys();

    for (unsigned int i = 0; i < keys.size(); i++)
    {
        std::string key = keys[i];
        std::string value;
        itk::ExposeMetaData(metaDataDictionary, key, value);

        QString metaDataKey = convertItkKeyToMedKey(key);
        if (!metaDataKey.isEmpty())
        {
            data()->setMetaData(metaDataKey, QString(value.c_str()));
        }
        else
        {
            qDebug() << metaObject()->className() << ":: found unknown key:" << QString::fromStdString(key);
        }
    }
}

bool itkDataImageReaderBase::read(const QString& path)
{
    if (this->io.IsNull())
        return false;

    this->setProgress(0);

    if ( ! this->readInformation(path) )
        return false;

    this->setProgress(50);

    /*
    itk::DataImageReaderCommand::Pointer command = itk::DataImageReaderCommand::New();
    command->SetDCMTKDataImageReader ( this );
    this->io->AddObserver ( itk::ProgressEvent(), command);
    */

    try
    {
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
              read_image<3,itk::Vector<double,3> >(path,"itkDataImageVectorDouble3") ||
              read_image<3,itk::RGBAPixel<unsigned char> >(path,"itkDataImageRGBA3") ||
              read_image<3,itk::RGBPixel<unsigned char> >(path,"itkDataImageRGB3")))
        {
            qWarning() << "Unrecognized pixel type";
            return false;
        }
    }
    catch (itk::ExceptionObject &e)
    {
        qWarning() << e.GetDescription();
        return false;
    }

    this->setProgress (100);

    return true;
}

bool itkDataImageReaderBase::read (const QStringList& paths)
{
    if (paths.count() < 1)
        return false;
    return this->read(paths[0]);
}

void itkDataImageReaderBase::setProgress (int value)
{
    emit progressed (value);
}
