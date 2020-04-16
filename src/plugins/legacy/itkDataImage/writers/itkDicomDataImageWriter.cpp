/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDicomDataImageWriter.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>


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
    //Get metaDataKey from data
    foreach (QString metaDataKey, data()->metaDataList())
    {
        if(metaDataKey == QString("FlipAngle"))
        {
            //Flip angle
            itk::EncapsulateMetaData<std::string>(dictionary, "0018|1314", data()->metadata(metaDataKey).toStdString());
        }
        if (metaDataKey == QString("Modality"))
        {
            // SOP class UID
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|0016", sopClassUID(data()->metadata(metaDataKey)).toStdString());
            // Modality
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|0060", data()->metadata(metaDataKey).toStdString());
        }
        if (metaDataKey == QString("BirthDate"))
        {
            // Patient's Birthdate
            itk::EncapsulateMetaData<std::string>(dictionary, "0010|0030", data()->metadata(metaDataKey).toStdString());
        }
        if (metaDataKey == QString("Gender"))
        {
            // Patient's Sex
            itk::EncapsulateMetaData<std::string>(dictionary, "0010|0040", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == QString("PatientName"))
        {
            // Patient's Name
            itk::EncapsulateMetaData<std::string>(dictionary, "0010|0010",  data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == QString("PatientID"))
        {
            // Patient's Id
            itk::EncapsulateMetaData<std::string>(dictionary, "0010|0020", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == QString("Age"))
        {
            // Patient's Age
            itk::EncapsulateMetaData<std::string>(dictionary, "0010|0010", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == QString("EchoTime"))
        {
            // Echo time
            itk::EncapsulateMetaData<std::string>(dictionary, "0018|0081",  data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == QString("SeriesDescription"))
        {
            // Series Description
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|103e", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == QString("SeriesNumber"))
        {
            // Series Number
            itk::EncapsulateMetaData<std::string>(dictionary, "0020|0011",  data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == QString("Referee"))
        {
            //Referee physician name
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|0090", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == QString("Performer"))
        {
            //Performer physician name
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|1050", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == QString("Protocol"))
        {
            //Protocol
            itk::EncapsulateMetaData<std::string>(dictionary, "0018|1030", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == QString("RepetitionTime"))
        {
            //RepetitionTime
            itk::EncapsulateMetaData<std::string>(dictionary, "0018|0080", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == QString("Descriptions"))
        {
            //Scan options
            itk::EncapsulateMetaData<std::string>(dictionary, "0018|0022", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == QString("StudyInstanceUID"))
        {
            // Study Instance UID
            if(!data()->metadata(metaDataKey).trimmed().isEmpty())
            {
                studyUIDExistance = true;
                itk::EncapsulateMetaData<std::string>(dictionary, "0020|000d",  data()->metadata(metaDataKey).toStdString());
            }
        }
        if(metaDataKey == QString("StudyID"))
        {
            // Study ID
            itk::EncapsulateMetaData<std::string>(dictionary, "0020|0010",  data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == QString("StudyDescription"))
        {
            // Study Description
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|1030", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == QString("Institution"))
        {
            // Institution Name
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|0080", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == QString("AcquisitionDate"))
        {
            // Acquisition Date
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|0022", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == QString("AcquisitionTime"))
        {
            // Acquisition Date
            itk::EncapsulateMetaData<std::string>(dictionary, "0008|0032", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == QString("SequenceName"))
        {
            // Sequence Name
            itk::EncapsulateMetaData<std::string>(dictionary, "0018|0024", data()->metadata(metaDataKey).toStdString());
        }
        if(metaDataKey == QString("Repetition"))
        {
            // Sequence Name
            itk::EncapsulateMetaData<std::string>(dictionary, "0018|0080", data()->metadata(metaDataKey).toStdString());
        }
    }

}

template <class PixelType> void itkDicomDataImageWriter::fillDictionaryWithSharedData(itk::MetaDataDictionary &dictionary, bool studyUIDExistance,
                                                                                      itk::GDCMImageIO::Pointer gdcmIO, int &numberOfSlices)
{
        typedef itk::Image<PixelType,3> Image3DType;

        itk::Object* itkImage = static_cast<itk::Object*>(data()->data());
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
        // to keep the same study UID. But we need new series and frame of
        // reference UID's.
        gdcm::UIDGenerator suid;
        std::string seriesUID = suid.Generate();
        gdcm::UIDGenerator fuid;
        std::string frameOfReferenceUID = fuid.Generate();

        // Set the UID's for the study  and frame of reference
        itk::EncapsulateMetaData<std::string>(dictionary,"0020|000e", seriesUID);
        itk::EncapsulateMetaData<std::string>(dictionary,"0020|0052", frameOfReferenceUID);
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

    itk::Object* itkImage = static_cast<itk::Object*>(data()->data());
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
    typename Image2DType::PixelType minValue = itk::NumericTraits<typename Image2DType::PixelType>::max();
    typename Image2DType::PixelType maxValue = itk::NumericTraits<typename Image2DType::PixelType>::min();
    for( it.GoToBegin(); !it.IsAtEnd(); ++it )
    {
        typename Image2DType::PixelType p = it.Get();
        if( p > maxValue )
        {
            maxValue = p;
        }
        if( p < minValue )
        {
            minValue = p;
        }
    }
    typename Image2DType::PixelType windowCenter = (minValue + maxValue) / 2;
    typename Image2DType::PixelType windowWidth = (maxValue - minValue);

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

    QString id = data()->identifier() ;

    try {
        if ( id == "itkDataImageChar3" )
        {
            writeDicom<char>(path);
        }
        else if ( id == "itkDataImageUChar3" )
        {
            writeDicom<unsigned char>(path);
        }
        else if ( id == "itkDataImageShort3" )
        {
            writeDicom<short>(path);
        }
        else if ( id == "itkDataImageUShort3" )
        {
            writeDicom<unsigned short>(path);
        }
        else if ( id == "itkDataImageInt3" )
        {
            writeDicom<int>(path);
        }
        else if ( id == "itkDataImageUInt3" )
        {
            writeDicom<unsigned int>(path);
        }
        else if ( id == "itkDataImageLong3" )
        {
            writeDicom<long>(path);
        }
        else if ( id== "itkDataImageULong3" )
        {
            writeDicom<unsigned long>(path);
        }
        else if ( id == "itkDataImageFloat3" )
        {
            writeDicom<float>(path);
        }
        else if ( id == "itkDataImageDouble3" )
        {
            writeDicom<double>(path);
        }
        else
        {
            qWarning() << "Pixel type not yet supported";
            return false;
        }
    }
    catch(itk::ExceptionObject &e)
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

