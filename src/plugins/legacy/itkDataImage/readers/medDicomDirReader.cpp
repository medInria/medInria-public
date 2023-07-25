/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDicomDirReader.h"

const char medDicomDirReader::ID[] = "medDicomDirReader";

// /////////////////////////////////////////////////////////////////
// medDicomDirReaderPrivate
// /////////////////////////////////////////////////////////////////

class medDicomDirReaderPrivate
{
public:
    DcmFileFormat fileformat;

    QString dicomdirPath;        // ex: /home/toto/dicoms/DICOMDIR (file)
    QString dicomsDirectoryPath; // ex: /home/toto/dicoms/DICOM    (directory)
    std::vector<std::string> filelist;
};

// /////////////////////////////////////////////////////////////////
// medDicomDirReader
// /////////////////////////////////////////////////////////////////

medDicomDirReader::medDicomDirReader() : dtkAbstractDataReader(), d(new medDicomDirReaderPrivate)
{
    d->fileformat = nullptr;
}

medDicomDirReader::~medDicomDirReader()
{
    delete d;
    d = nullptr;
}

bool medDicomDirReader::registered()
{
    return medAbstractDataFactory::instance()->registerDataReaderType(
        "medDicomDirReader",
        QStringList()
            << "itkDataImageDouble3"
            << "itkDataImageFloat3"
            << "itkDataImageULong3"
            << "itkDataImageLong3"
            << "itkDataImageUInt3"
            << "itkDataImageInt3"
            << "itkDataImageInt4"
            << "itkDataImageLong4"
            << "itkDataImageUInt4"
            << "itkDataImageULong4"
            << "itkDataImageUShort3"
            << "itkDataImageUShort4"
            << "itkDataImageShort3"
            << "itkDataImageShort4"
            << "itkDataImageUChar3"
            << "itkDataImageUChar4"
            << "itkDataImageChar3"
            << "itkDataImageChar4"
            << "itkDataImageRGB3",
        createmedDicomDirReader);
}

QStringList medDicomDirReader::handled() const
{
    return QStringList() << "itkDataImageDouble3"
                         << "itkDataImageFloat3"
                         << "itkDataImageULong3"
                         << "itkDataImageLong3"
                         << "itkDataImageUInt3"
                         << "itkDataImageInt3"
                         << "itkDataImageInt4"
                         << "itkDataImageUInt4"
                         << "itkDataImageULong4"
                         << "itkDataImageLong4"
                         << "itkDataImageUShort3"
                         << "itkDataImageUShort4"
                         << "itkDataImageShort3"
                         << "itkDataImageShort4"
                         << "itkDataImageUChar3"
                         << "itkDataImageUChar4"
                         << "itkDataImageChar3"
                         << "itkDataImageChar4"
                         << "itkDataImageRGB3";
}

QString medDicomDirReader::identifier() const
{
    return ID;
}

QString medDicomDirReader::description() const
{
    return "Reader of DICOMDIR images with DCMTK, ITK::GDCM and VTK";
}

bool medDicomDirReader::canRead(const QString &path)
{
    bool res = false;
    if (path.endsWith("DICOMDIR", Qt::CaseInsensitive))
    {
        OFCondition status = d->fileformat.loadFile(path.toStdString());
        if (status.good())
        {
            res = true;
        }
    }
    return res;
}

bool medDicomDirReader::canRead(const QStringList &paths)
{
    for (auto path : paths)
    {
        if (!this->canRead(path))
        {
            return false;
        }
    }
    return true;
}

bool medDicomDirReader::readInformation(const QString &path)
{
    QStringList paths;
    paths << path;
    return readInformation(paths);
}

/**
 * @brief This method creates a medAbstractData shell, 
 * collects info from the first DICOMDIR and the dataset 
 * and set these metadata into the data shell.
 * 
 * @param paths 
 * @return true 
 * @return false 
 */
bool medDicomDirReader::readInformation(const QStringList &paths)
{
    if (paths.size() == 0)
    {
        return false;
    }

    saveNeededPaths(paths[0]);
    auto pixelType = GetPixelType();
    
    // Create the shell of the new medAbstractData
    medAbstractData *medData = dynamic_cast<medAbstractData *>(this->data());
    if (!medData)
    {
        medData = medAbstractDataFactory::instance()->create(pixelType);
        this->setData(medData);
    }

    // Collect metadata from the DICOMDIR
    // Current definition of DCMTK 3.6.2 metadata tags:
    // https://github.com/DCMTK/dcmtk/blob/DCMTK-3.6.2/dcmdata/include/dcmtk/dcmdata/dcdeftag.h

    DcmMetaInfo *metaInfo = d->fileformat.getMetaInfo();
    DcmDataset *dataset = d->fileformat.getDataset();
    if (medData && metaInfo)
    {
        OFString studyInstanceUID;
        if (metaInfo->findAndGetOFString(DCM_StudyInstanceUID, studyInstanceUID).good())
        {
            medData->setMetaData(medMetaDataKeys::StudyInstanceUID.key(), studyInstanceUID.c_str());
        }

        OFString sopInstanceUID;
        if (metaInfo->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID).good())
        {
            medData->setMetaData(medMetaDataKeys::SOPInstanceUID.key(), sopInstanceUID.c_str());
        }

        OFString manufacturer;
        if (metaInfo->findAndGetOFString(DCM_Manufacturer, manufacturer).good())
        {
            medData->setMetaData(medMetaDataKeys::Manufacturer.key(), manufacturer.c_str());
        }

        OFString transferSyntaxUID;
        if (metaInfo->findAndGetOFString(DCM_TransferSyntaxUID, transferSyntaxUID).good())
        {
            medData->setMetaData(medMetaDataKeys::TransferSyntaxUID.key(), transferSyntaxUID.c_str());
        }
    }

    if (medData && dataset)
    {
        // Check if the file is a DICOMDIR
        if (dataset->tagExistsWithValue(DCM_DirectoryRecordSequence))
        {
            DcmStack stack;
            if (dataset->search(DCM_DirectoryRecordSequence, stack, ESM_fromHere, OFTrue).good())
            {
                DcmObject *dobj = stack.top();
                if (dobj->ident() == EVR_SQ)
                {
                    DcmSequenceOfItems *sequence = dynamic_cast<DcmSequenceOfItems *>(dobj);
                    if (sequence)
                    {
                        // Check records in directory
                        for (unsigned long i = 0; i < sequence->card(); ++i)
                        {
                            DcmItem *item = sequence->getItem(i);
                            if (item)
                            {
                                OFString patientName;
                                if (item->findAndGetOFString(DCM_PatientName, patientName).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::PatientName.key(), patientName.c_str());
                                }

                                OFString patientID;
                                if (item->findAndGetOFString(DCM_PatientID, patientID).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::PatientID.key(), patientID.c_str());
                                }

                                OFString patientSex;
                                if (item->findAndGetOFString(DCM_PatientSex, patientSex).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::Gender.key(), patientSex.c_str());
                                }

                                OFString patientBirthDate;
                                if (item->findAndGetOFString(DCM_PatientBirthDate, patientBirthDate).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::BirthDate.key(), patientBirthDate.c_str());
                                }

                                OFString studyDate;
                                if (item->findAndGetOFString(DCM_StudyDate, studyDate).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::StudyDate.key(), studyDate.c_str());
                                }

                                OFString studyTime;
                                if (item->findAndGetOFString(DCM_StudyTime, studyTime).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::StudyTime.key(), studyTime.c_str());
                                }

                                OFString referringPhysicianName;
                                if (item->findAndGetOFString(DCM_ReferringPhysicianName, referringPhysicianName).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::Referee.key(), referringPhysicianName.c_str());
                                }

                                OFString studyID;
                                if (item->findAndGetOFString(DCM_StudyID, studyID).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::StudyID.key(), studyID.c_str());
                                }

                                OFString seriesInstanceUID;
                                if (item->findAndGetOFString(DCM_SeriesInstanceUID, seriesInstanceUID).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::SeriesInstanceUID.key(), seriesInstanceUID.c_str());
                                }

                                OFString seriesNumber;
                                if (item->findAndGetOFString(DCM_SeriesNumber, seriesNumber).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::SeriesNumber.key(), seriesNumber.c_str());
                                }

                                OFString seriesDescription;
                                if (item->findAndGetOFString(DCM_SeriesDescription, seriesDescription).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::SeriesDescription.key(), seriesDescription.c_str());
                                }

                                OFString seriesDate;
                                if (item->findAndGetOFString(DCM_SeriesDate, seriesDate).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::SeriesDate.key(), seriesDate.c_str());
                                }

                                OFString seriesTime;
                                if (item->findAndGetOFString(DCM_SeriesTime, seriesTime).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::SeriesTime.key(), seriesTime.c_str());
                                }

                                OFString studyDescription;
                                if (item->findAndGetOFString(DCM_StudyDescription, studyDescription).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::StudyDescription.key(), studyDescription.c_str());
                                }

                                OFString institutionName;
                                if (item->findAndGetOFString(DCM_InstitutionName, institutionName).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::Institution.key(), institutionName.c_str());
                                }

                                OFString modality;
                                if (item->findAndGetOFString(DCM_Modality, modality).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::Modality.key(), modality.c_str());
                                }

                                OFString protocolName;
                                if (item->findAndGetOFString(DCM_ProtocolName, protocolName).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::Protocol.key(), protocolName.c_str());
                                }

                                OFString patientOrientation;
                                if (item->findAndGetOFString(DCM_PatientOrientation, patientOrientation).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::PatientOrientation.key(), patientOrientation.c_str());
                                }

                                OFString patientPosition;
                                if (item->findAndGetOFString(DCM_PatientPosition, patientPosition).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::PatientPosition.key(), patientPosition.c_str());
                                }

                                OFString frameOfReferenceUID;
                                if (item->findAndGetOFString(DCM_FrameOfReferenceUID, frameOfReferenceUID).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::FrameOfReferenceUID.key(), frameOfReferenceUID.c_str());
                                }

                                OFString flipAngle;
                                if (item->findAndGetOFString(DCM_FlipAngle, flipAngle).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::FlipAngle.key(), flipAngle.c_str());
                                }

                                OFString echoTime;
                                if (item->findAndGetOFString(DCM_EchoTime, echoTime).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::EchoTime.key(), echoTime.c_str());
                                }

                                OFString repetitionTime;
                                if (item->findAndGetOFString(DCM_RepetitionTime, repetitionTime).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::RepetitionTime.key(), repetitionTime.c_str());
                                }

                                OFString KVP;
                                if (item->findAndGetOFString(DCM_KVP, KVP).good())
                                {
                                    medData->setMetaData(medMetaDataKeys::KVP.key(), KVP.c_str());
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}

/**
 * @brief Get DICOM paths from the DICOMDIR path
 * 
 * @param dicomdirPath 
 */
void medDicomDirReader::saveNeededPaths(QString dicomdirPath)
{
    d->dicomdirPath = dicomdirPath;

    DcmDataset *dataset = d->fileformat.getDataset();
    if (dataset)
    {
        DcmSequenceOfItems *seq = nullptr;
        if (dataset->findAndGetSequence(DCM_DirectoryRecordSequence, seq).good() && seq)
        {
            for (unsigned long i = 0; i < seq->card(); ++i)
            {
                DcmItem *item = seq->getItem(i);
                if (item)
                {
                    // Get the name of DICOMs directory
                    OFString referencedFileID;
                    if (item->findAndGetOFString(DCM_ReferencedFileID, referencedFileID).good())
                    {
                        getDicomsDirectoryPath(referencedFileID);
                        getFilenamesList();
                        break;
                    }
                }
            }
        }
    }
}

/**
 * @brief Compute the path of DICOMs directory
 * 
 * @param dicomsDirectoryName 
 */
void medDicomDirReader::getDicomsDirectoryPath(OFString dicomsDirectoryName)
{
    QFileInfo fileInfo(d->dicomdirPath);
    d->dicomsDirectoryPath = fileInfo.path() + QDir::separator() + dicomsDirectoryName.c_str();
}

/**
 * @brief Get the list of filenames in DICOM directory recursively
 * 
 */
void medDicomDirReader::getFilenamesList()
{
    auto fileNamesListGenerator = itk::GDCMSeriesFileNames::New();
    fileNamesListGenerator->SetRecursive(true);
    fileNamesListGenerator->SetDirectory(d->dicomsDirectoryPath.toUtf8().constData());
    d->filelist = fileNamesListGenerator->GetInputFileNames();
}

/**
 * @brief Get pixel type from one of the DICOMs. Currently it does not support 4D data.
 * 
 * @return QString 
 */
QString medDicomDirReader::GetPixelType()
{
    QString pixelType = "itkDataImageFloat3";

    auto reader = vtkSmartPointer<vtkDICOMImageReader>::New();
    reader->SetFileName(d->filelist[0].c_str());
    reader->Update();
    vtkImageData* imageData = reader->GetOutput();

    if (imageData)
    {
        int scalarType = imageData->GetScalarType();

        switch (scalarType)
        {
            case VTK_UNSIGNED_CHAR:
                pixelType = "itkDataImageUChar3";
                break;
            case VTK_CHAR:
                pixelType = "itkDataImageChar3";
                break;
            case VTK_UNSIGNED_SHORT:
                pixelType = "itkDataImageUShort3";
                break;
            case VTK_SHORT:
                pixelType = "itkDataImageShort3";
                break;
            case VTK_UNSIGNED_INT:
                pixelType = "itkDataImageUInt3";
                break;
            case VTK_INT:
                pixelType = "itkDataImageInt3";
                break;
            case VTK_FLOAT:
                pixelType = "itkDataImageFloat3";
                break;
            case VTK_DOUBLE:
                pixelType = "itkDataImageDouble3";
                break;
            default:
                std::cout << "DICOMDIR: unhandled data type" << std::endl;
                break;
        }
    }

    return pixelType;
}

bool medDicomDirReader::read(const QString &path)
{
    QStringList paths;
    paths << path;
    return read(paths);
}

/**
 * @brief Read the first DICOMDIR path and load data.
 * 
 * @param paths 
 * @return true 
 * @return false 
 */
bool medDicomDirReader::read(const QStringList &paths)
{
    bool res = false;

    if (paths.size() == 0)
    {
        return res;
    }

    if (readInformation(paths))
    {
        res = loadDICOMImagesFromDirectory(d->dicomsDirectoryPath);
    }

    return res;
}

/**
 * @brief Load data series and set medAbstractData data. Do not works on 4D data for now.
 * 
 * @param dicomDirPath 
 * @return true 
 * @return false 
 */
bool medDicomDirReader::loadDICOMImagesFromDirectory(const QString &dicomDirPath)
{
    bool res = true;

    if (auto medData = dynamic_cast<medAbstractData *>(this->data()))
    {
        auto io = itk::GDCMImageIO::New();
        try
        {
            if (medData->identifier() == "itkDataImageUChar3")
            {
                read3DImage<unsigned char>(medData, io, d->filelist);
            }
            else if (medData->identifier() == "itkDataImageChar3")
            {
                read3DImage<char>(medData, io, d->filelist);
            }
            else if (medData->identifier() == "itkDataImageUShort3")
            {
                read3DImage<unsigned short>(medData, io, d->filelist);
            }
            else if (medData->identifier() == "itkDataImageShort3")
            {
                read3DImage<short>(medData, io, d->filelist);
            }
            else if (medData->identifier() == "itkDataImageUInt3")
            {
                read3DImage<unsigned int>(medData, io, d->filelist);
            }
            else if (medData->identifier() == "itkDataImageInt3")
            {
                read3DImage<int>(medData, io, d->filelist);
            }
            else if (medData->identifier() == "itkDataImageULong3")
            {
                read3DImage<unsigned long>(medData, io, d->filelist);
            }
            else if (medData->identifier() == "itkDataImageLong3")
            {
                read3DImage<long>(medData, io, d->filelist);
            }
            else if (medData->identifier() == "itkDataImageFloat3")
            {
                read3DImage<float>(medData, io, d->filelist);
            }
            else if (medData->identifier() == "itkDataImageDouble3")
            {
                read3DImage<double>(medData, io, d->filelist);
            }
            else
            {
                qDebug() << "DICOMDIR: unhandled medData type : " << medData->identifier();
                res = false;
            }
        }
        catch (itk::ExceptionObject &e)
        {
            qDebug() << "DICOMDIR: " << e.GetDescription();
            res = false;
        }
    }
    return res;
}

template <typename TYPE>
void medDicomDirReader::read3DImage(medAbstractData *medData,
                                    itk::GDCMImageIO::Pointer io,
                                    std::vector<std::string> filelist)
{
    auto reader = itk::ImageSeriesReader<itk::Image<TYPE, 3>>::New();
    reader->SetImageIO(io);
    reader->SetFileNames(filelist);
    medData->setData(reader->GetOutput());
    reader->Update();
}

void medDicomDirReader::setProgress(int value)
{
    emit progressed(value);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createmedDicomDirReader()
{
    return new medDicomDirReader;
}
