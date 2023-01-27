/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDicomDataImageWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medMetaDataKeys.h>
#include <medUtilitiesITK.h>

#include <itkCastImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkMetaDataObject.h>

#include <gdcmUIDGenerator.h>

#include <string>

static QString s_identifier() {
    return "itkDicomDataImageWriter";
}

static QStringList s_handled() {
    return QStringList()  << "itkDataImageChar3"
                          << "itkDataImageUChar3"
                          << "itkDataImageShort3"
                          << "itkDataImageUShort3"
                          << "itkDataImageInt3"
                          << "itkDataImageUInt3"
                          << "itkDataImageLong3"
                          << "itkDataImageULong3"
                          << "itkDataImageFloat3"
                          << "itkDataImageDouble3";
}

template <class ImageType> itk::Image<int, 3>::Pointer castToInt3(dtkAbstractData *input)
{
    typedef itk::CastImageFilter<ImageType, itk::Image<int, 3>> CasterType;
    typename CasterType::Pointer caster = CasterType::New();
    caster->SetInput(dynamic_cast<ImageType *>((itk::Object *)(input->data())));
    caster->Update();
    return caster->GetOutput();
}

template <class PixelType> void setDataCopy(dtkSmartPointer<medAbstractData> dataCopy, dtkAbstractData *input)
{
    typedef itk::Image<PixelType, 3> ImageTypeInput;
    dataCopy->setData(castToInt3<ImageTypeInput>(input));
}

itkDicomDataImageWriter::itkDicomDataImageWriter(): itkDataImageWriterBase() {
    this->io = itk::GDCMImageIO::New();    
}

itkDicomDataImageWriter::~itkDicomDataImageWriter()
{
}

QStringList itkDicomDataImageWriter::handled() const
{
    return s_handled();
}

QStringList itkDicomDataImageWriter::supportedFileExtensions() const
{
    return QStringList() << ".dcm";
}

bool itkDicomDataImageWriter::registered()
{
    return medAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), s_handled(), create);
}

QString itkDicomDataImageWriter::identifier() const
{
    return s_identifier();
}

QString itkDicomDataImageWriter::description() const
{
    return "Dicom image exporter";
}

QString itkDicomDataImageWriter::sopClassUID(QString modality)
{
    if( modality == QString("CT"))
    {
        return "1.2.840.10008.5.1.4.1.1.2";
    }
    else if( modality == QString("MR"))
    {
        return "1.2.840.10008.5.1.4.1.1.4";
    }
    else if( modality == QString("CR"))
    {
        return "1.2.840.10008.5.1.4.1.1.1";
    }
    else if( modality == QString("MN"))
    {
        return "1.2.840.10008.5.1.4.1.1.20";
    }
    else if( modality == QString("US"))
    {
        return "1.2.840.10008.5.1.4.1.1.6.1";
    }
    else
    {
        qDebug( "Unknown modality. Using CT Image Storage SOP class UID.");
        return "1.2.840.10008.5.1.4.1.1.2";
    }
}

void itkDicomDataImageWriter::fillDictionaryFromMetaDataKey(itk::MetaDataDictionary &dictionary, bool &studyUIDExistance)
{
    bool studyTimeExistance = false;
    bool studyDateExistance = false;
    bool seriesTimeExistance = false;
    bool seriesDateExistance = false;

    //Get metaDataKey from data
    for (QString metaDataKey: data()->metaDataList())
    {
        if (metaDataKey == medMetaDataKeys::Modality.key())
        {
            // SOP class UID
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|0016", sopClassUID(data()->metadata(metaDataKey)).toStdString());
            // Modality
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|0060", data()->metadata(metaDataKey).toStdString());
        }
        if (metaDataKey == medMetaDataKeys::BirthDate.key())
        {
            // Patient's Birthdate
            itk::EncapsulateMetaData<std::string>(dictionary, "0010|0030", data()->metadata(metaDataKey).toStdString());
        }
        if (metaDataKey == medMetaDataKeys::Gender.key())
        {
            // Patient's Sex
            itk::EncapsulateMetaData<std::string>(dictionary, "0010|0040", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == medMetaDataKeys::PatientName.key())
        {
            // Patient's Name
            itk::EncapsulateMetaData<std::string>(dictionary, "0010|0010",  data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == medMetaDataKeys::PatientID.key())
        {
            // Patient's Id
            itk::EncapsulateMetaData<std::string>(dictionary, "0010|0020", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == medMetaDataKeys::Age.key())
        {
            // Patient's Age
            itk::EncapsulateMetaData<std::string>(dictionary, "0010|1010", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == medMetaDataKeys::SeriesDescription.key())
        {
            // Series Description
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|103e", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == medMetaDataKeys::SeriesNumber.key())
        {
            // Series Number
            itk::EncapsulateMetaData<std::string>(dictionary, "0020|0011",  data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == medMetaDataKeys::Referee.key())
        {
            //Referee physician name
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|0090", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == medMetaDataKeys::Performer.key())
        {
            //Performer physician name
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|1050", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == medMetaDataKeys::Protocol.key())
        {
            //Protocol
            itk::EncapsulateMetaData<std::string>(dictionary, "0018|1030", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == medMetaDataKeys::Description.key())
        {
            //Scan options
            itk::EncapsulateMetaData<std::string>(dictionary, "0018|0022", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == medMetaDataKeys::StudyInstanceUID.key())
        {
            // Study Instance UID
            if(!data()->metadata(metaDataKey).trimmed().isEmpty())
            {
                studyUIDExistance = true;
                itk::EncapsulateMetaData<std::string>(dictionary, "0020|000d",  data()->metadata(metaDataKey).toStdString());
            }
        }
        if(metaDataKey == medMetaDataKeys::StudyID.key())
        {
            // Study ID
            itk::EncapsulateMetaData<std::string>(dictionary, "0020|0010",  data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == medMetaDataKeys::StudyDescription.key())
        {
            // Study Description
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|1030", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == medMetaDataKeys::Institution.key())
        {
            // Institution Name
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|0080", data()->metadata(metaDataKey).toStdString());
        }
        if (metaDataKey == medMetaDataKeys::AcquisitionNumber.key())
        {
            itk::EncapsulateMetaData<std::string>(dictionary, "0020|0012", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == medMetaDataKeys::AcquisitionDate.key())
        {
            // Acquisition Date
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|0022", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == medMetaDataKeys::AcquisitionTime.key())
        {
            // Acquisition Date
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|0032", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == medMetaDataKeys::SequenceName.key())
        {
            // Sequence Name
            itk::EncapsulateMetaData<std::string>(dictionary, "0018|0024", data()->metadata(metaDataKey).toStdString());
        }
        if (metaDataKey == medMetaDataKeys::PatientPosition.key())
        {
            // Patient Position
            itk::EncapsulateMetaData<std::string>(dictionary, "0018|5100", data()->metadata(metaDataKey).toStdString());
        }
        if (metaDataKey == medMetaDataKeys::PatientOrientation.key())
        {
            // Patient Orientation
            itk::EncapsulateMetaData<std::string>(dictionary, "0020|0020", data()->metadata(metaDataKey).toStdString());
        }
        if (metaDataKey == medMetaDataKeys::ImageType.key())
        {
            // Image type
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|0008", data()->metadata(metaDataKey).toStdString());
        }
        if (metaDataKey == medMetaDataKeys::PositionReferenceIndicator.key())
        {
            itk::EncapsulateMetaData<std::string>(dictionary, "0020|1040", data()->metadata(metaDataKey).toStdString());
        }
        if (metaDataKey == medMetaDataKeys::Manufacturer.key())
        {
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|0070", data()->metadata(metaDataKey).toStdString());
        }       
        if(metaDataKey == medMetaDataKeys::StudyTime.key())
        {
            // Study Time
            if(!data()->metadata(metaDataKey).trimmed().isEmpty())
            {
                studyTimeExistance = true;
                itk::EncapsulateMetaData<std::string>(dictionary, "0008|0030", data()->metadata(metaDataKey).toStdString());
            }
        }
        if(metaDataKey == medMetaDataKeys::StudyDate.key())
        {
            // Study Date
            if(!data()->metadata(metaDataKey).trimmed().isEmpty())
            {
                studyDateExistance = true;
                itk::EncapsulateMetaData<std::string>(dictionary, "0008|0020", data()->metadata(metaDataKey).toStdString());
            }
        }
        if(metaDataKey == medMetaDataKeys::SeriesTime.key())
        {
            // Series Time
            if(!data()->metadata(metaDataKey).trimmed().isEmpty())
            {
                seriesTimeExistance = true;
                itk::EncapsulateMetaData<std::string>(dictionary, "0008|0031", data()->metadata(metaDataKey).toStdString());
            }
        }
        if(metaDataKey == medMetaDataKeys::SeriesDate.key())
        {
            // Series Date
            if(!data()->metadata(metaDataKey).trimmed().isEmpty())
            {
                seriesDateExistance = true;
                itk::EncapsulateMetaData<std::string>(dictionary, "0008|0021", data()->metadata(metaDataKey).toStdString());
            }
        }
    }

    // Add inside input data current Time and date if they do not exist
    // by default if not present the writer use the current time but the fractional part of a second
    // change between slices which can be problematic for some software
    if (!studyTimeExistance)
    {
        QString currentTime = QDateTime::currentDateTime().toString("hhmmss.zzzzzz");
        itk::EncapsulateMetaData<std::string>(dictionary, "0008|0030", currentTime.toStdString());
    }
    if (!studyDateExistance)
    {
        QString currentDate = QDateTime::currentDateTime().toString("yyyyMMdd");
        itk::EncapsulateMetaData<std::string>(dictionary, "0008|0020", currentDate.toStdString());
    }
    if (!seriesTimeExistance)
    {
        QString currentTime = QDateTime::currentDateTime().toString("hhmmss.zzzzzz");
        itk::EncapsulateMetaData<std::string>(dictionary, "0008|0031", currentTime.toStdString());
    }
    if (!seriesDateExistance)
    {
        QString currentDate = QDateTime::currentDateTime().toString("yyyyMMdd");
        itk::EncapsulateMetaData<std::string>(dictionary, "0008|0021", currentDate.toStdString());
    }
}

void itkDicomDataImageWriter::fillDictionaryWithModalityDependentData(itk::MetaDataDictionary& dictionary)
{
    QString modality = data()->metadata(medMetaDataKeys::Modality.key());
    if (modality.contains("MR"))
    {
        itk::EncapsulateMetaData<std::string>(dictionary, "0018|0081", data()->metadata(medMetaDataKeys::EchoTime.key()).toStdString());
        itk::EncapsulateMetaData<std::string>(dictionary, "0018|1314", data()->metadata(medMetaDataKeys::FlipAngle.key()).toStdString());
        itk::EncapsulateMetaData<std::string>(dictionary, "0018|0080", data()->metadata(medMetaDataKeys::RepetitionTime.key()).toStdString());
    }
    else if (modality.contains("CT"))
    {
        itk::EncapsulateMetaData<std::string>(dictionary, "0018|0060", data()->metadata(medMetaDataKeys::KVP.key()).toStdString());
    }
}

template <class PixelType> void itkDicomDataImageWriter::fillDictionaryWithSharedData(itk::MetaDataDictionary &dictionary, bool studyUIDExistance,
                                                                                      itk::GDCMImageIO::Pointer gdcmIO, int &numberOfSlices)
{
        typedef itk::Image<PixelType,3> Image3DType;

        itk::Object* itkImage = static_cast<itk::Object*>(dataCopy->data());
        typename Image3DType::Pointer image = dynamic_cast<Image3DType*>(itkImage);

        std::ostringstream value;

        double spacing[3];
        spacing[0] = image->GetSpacing()[0];
        spacing[1] = image->GetSpacing()[1];
        spacing[2] = image->GetSpacing()[2];

        int imDim[3];
        imDim[0]= image->GetLargestPossibleRegion().GetSize()[0];
        imDim[1]= image->GetLargestPossibleRegion().GetSize()[1];
        imDim[2]= image->GetLargestPossibleRegion().GetSize()[2];

        numberOfSlices = imDim[2];

        //Columns (X dimension)
        value.str("");
        value << imDim[0];
        itk::EncapsulateMetaData<std::string>(dictionary, "0028|0011", value.str());

        //Rows (Y dimension)
        value.str("");
        value << imDim[1];
        itk::EncapsulateMetaData<std::string>(dictionary, "0028|0010", value.str());

        //Spacing
        value.str("");
        value << spacing[0] << "\\" << spacing[1];
        itk::EncapsulateMetaData<std::string>(dictionary, "0028|0030", value.str());

        // Slice Thickness
        value.str("");
        value << spacing[2];
        itk::EncapsulateMetaData<std::string>(dictionary, "0018|0050", value.str() );

        // Image Orientation (Patient)
        typename Image3DType::DirectionType oMatrix = image->GetDirection();

        QString orientationPatientMatrix = QString::number(oMatrix[0][0]) + "\\" + QString::number(oMatrix[1][0]) + "\\"
                + QString::number(oMatrix[2][0]) + "\\" + QString::number(oMatrix[0][1]) +
                "\\" + QString::number(oMatrix[1][1])  + "\\" + QString::number(oMatrix[2][1]);

        itk::EncapsulateMetaData<std::string>(dictionary, "0020|0037", orientationPatientMatrix.toStdString() );

        if (!studyUIDExistance)
        {
            gdcm::UIDGenerator stuuid;
            std::string studyInstanceUID = stuuid.Generate();
            itk::EncapsulateMetaData<std::string>(dictionary, "0020|000d", studyInstanceUID);
        }
        else
        {
            gdcmIO->SetKeepOriginalUID(true);
        }

        // To keep the new series in the same study as the original we need
        // to keep the same study UID. But we need new series UID.
        gdcm::UIDGenerator suid;
        std::string seriesUID = suid.Generate();

        // Set the UID's for the study
        itk::EncapsulateMetaData<std::string>(dictionary,"0020|000e", seriesUID);

        // Frame of Reference
        std::string frameOfRef = data()->metadata(medMetaDataKeys::FrameOfReferenceUID.key()).toStdString();
        if (frameOfRef.empty())
        {
            // create a new frame of reference UID
            gdcm::UIDGenerator fuid;
            frameOfRef = fuid.Generate();
        }
        itk::EncapsulateMetaData<std::string>(dictionary, "0020|0052", frameOfRef);
}

template <class PixelType> bool itkDicomDataImageWriter::fillDictionaryAndWriteDicomSlice(itk::MetaDataDictionary &dictionary, const QString &path,
                                                                                          itk::GDCMImageIO::Pointer gdcmIO, int slice)
{
    QString filePath = path.left(path.length() - 4);
    QFileInfo fi(path);
    QString filename =  fi.baseName();

    typedef itk::Image<PixelType,3> Image3DType;
    typedef itk::Image<PixelType,2> Image2DType;
    typedef itk::ImageFileWriter<Image2DType> WriterType;

    itk::Object* itkImage = static_cast<itk::Object*>(dataCopy->data());
    typename Image3DType::Pointer image = dynamic_cast<Image3DType*>(itkImage);

    std::ostringstream value;

    gdcm::UIDGenerator sopuid;
    std::string sopInstanceUID = sopuid.Generate();
    itk::EncapsulateMetaData<std::string>(dictionary,"0008|0018", sopInstanceUID);
    itk::EncapsulateMetaData<std::string>(dictionary,"0002|0003", sopInstanceUID);

    // Instance Number
    itk::EncapsulateMetaData<std::string>(dictionary, "0020|0013",std::to_string(slice + 1) );

    typename Image3DType::PointType origin;
    typename Image3DType::IndexType index;
    index.Fill(0);
    index[2] = slice;

    // Image Position Patient
    image->TransformIndexToPhysicalPoint(index, origin);

    QString orientation = QString::number(origin[0]) + "\\" + QString::number(origin[1]) + "\\" + QString::number(origin[2]);
    itk::EncapsulateMetaData<std::string>(dictionary, "0020|0032", orientation.toStdString() );

    typename Image3DType::RegionType extractRegion;
    typename Image3DType::SizeType   extractSize;
    typename Image3DType::IndexType  extractIndex;
    extractSize = image->GetLargestPossibleRegion().GetSize();
    extractIndex.Fill(0);
    extractIndex[2] = slice;
    extractSize[2] = 0;
    extractRegion.SetSize(extractSize);
    extractRegion.SetIndex(extractIndex);

    typedef itk::ExtractImageFilter<Image3DType, Image2DType> ExtractType;
    typename ExtractType::Pointer extract = ExtractType::New();
    extract->SetDirectionCollapseToGuess();
    extract->SetInput(image);
    extract->SetExtractionRegion(extractRegion);
    extract->GetOutput()->SetMetaDataDictionary(dictionary);
    extract->Update();

    itk::ImageRegionIterator<Image2DType> it( extract->GetOutput(), extract->GetOutput()->GetLargestPossibleRegion() );
    // Do not use the Image2DType::PixelType type, as it creates problems with
    // char and uchar images. Furthermore, the Window center and width tags
    // are coded as 'Decimal String' in the DICOM dictionary, so floating
    // point value are fine.
    double minValue = std::numeric_limits<double>::max();
    double maxValue = std::numeric_limits<double>::lowest();
    for( it.GoToBegin(); !it.IsAtEnd(); ++it )
    {
        double p = static_cast<double>(it.Get());
        if( p > maxValue )
        {
            maxValue = p;
        }
        if( p < minValue )
        {
            minValue = p;
        }
    }
    double windowCenter = (minValue + maxValue) / 2;
    double windowWidth = std::max(1., maxValue - minValue);

    value.str("");
    value << windowCenter;
    itk::EncapsulateMetaData<std::string>(dictionary, "0028|1050", value.str() );
    value.str("");
    value << windowWidth;
    itk::EncapsulateMetaData<std::string>(dictionary, "0028|1051", value.str() );

    QDir dir(filePath);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }
    QString newFilename = filePath +"/"+ filename + "-" + QString::number(1000+slice) + path.right(4);

    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(newFilename.toStdString());
    writer->SetInput(extract->GetOutput() );
    writer->SetUseCompression(false);
    try
    {
        writer->SetImageIO(gdcmIO);
        writer->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
        std::cerr << "Exception thrown while writing the file " << std::endl;
        std::cerr << excp << std::endl;
        return EXIT_FAILURE;
    }
    writer->Update();
    return true;
}

template <class PixelType> bool itkDicomDataImageWriter::writeDicom(const QString &path)
{
    //Useful when writting Image position and orientiation matrix
    setlocale(LC_NUMERIC, "C");
    QLocale::setDefault(QLocale("C"));

    typedef itk::GDCMImageIO ImageIOType;
    typename ImageIOType::Pointer gdcmIO = ImageIOType::New();

    itk::MetaDataDictionary dictionary;
    bool studyUIDExistance = false;
    int  numberOfSlices = 0;

    fillDictionaryFromMetaDataKey(dictionary, studyUIDExistance);
    fillDictionaryWithModalityDependentData(dictionary);
    fillDictionaryWithSharedData<PixelType>(dictionary, studyUIDExistance, gdcmIO, numberOfSlices);

    for( int slice = 0; slice < numberOfSlices; slice++ )
    {
        if(!fillDictionaryAndWriteDicomSlice<PixelType>(dictionary, path, gdcmIO, slice))
            return false;
    }

    return true;
}


bool itkDicomDataImageWriter::write(const QString &path)
{
    if (!this->data())
        return false;

    QString id = data()->identifier();

    try
    {
        // Create a copy of data which can be modified if data is of type double, float, unsigned long or long
        dataCopy = medAbstractDataFactory::instance()->createSmartPointer(id);
        if (!dataCopy)
        {
            qWarning() << "Pixel type not supported by the software";
            return false;
        }
        dataCopy->setData(data()->data());

        if (id == "itkDataImageChar3")
        {
            writeDicom<char>(path);
        }
        else if (id == "itkDataImageUChar3")
        {
            writeDicom<unsigned char>(path);
        }
        else if (id == "itkDataImageShort3")
        {
            writeDicom<short>(path);
        }
        else if (id == "itkDataImageUShort3")
        {
            writeDicom<unsigned short>(path);
        }
        else if (id == "itkDataImageInt3")
        {
            writeDicom<int>(path);
        }
        else if (id == "itkDataImageUInt3")
        {
            writeDicom<unsigned int>(path);
        }
        else
        {
            typedef itk::Image<int, 3> ImageTypeOutput;
            dataCopy = medAbstractDataFactory::instance()->createSmartPointer(medUtilitiesITK::itkDataImageId<ImageTypeOutput>());
            if (id == "itkDataImageLong3")
            {
                setDataCopy<long>(dataCopy, data());
            }
            else if (id == "itkDataImageULong3")
            {
                setDataCopy<unsigned long>(dataCopy, data());
            }
            else if (id == "itkDataImageFloat3")
            {
                setDataCopy<float>(dataCopy, data());
            }
            else if (id == "itkDataImageDouble3")
            {
                setDataCopy<double>(dataCopy, data());
            }
            else
            {
                qWarning() << "Pixel type not yet supported";
                return false;
            }

            qWarning() << "Beware your Pixel type is not supported by itk Dicom writer, "
                          "the image will be cast in int to allow export anyway.";
            writeDicom<int>(path);
        }
    }
    catch (itk::ExceptionObject &e)
    {
        qDebug() << e.GetDescription();
        return false;
    }
    return true;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter * itkDicomDataImageWriter::create()
{
    return new itkDicomDataImageWriter;
}

