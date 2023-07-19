/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkGDCMDataImageReader.h"

#include <medAbstractDataFactory.h>
#include <medMetaDataKeys.h>

#include <itkDataImageReaderCommand.h>

#include <gdcmReader.h>

const char itkGDCMDataImageReader::ID[] = "itkGDCMDataImageReader";

template<typename TYPE>
void Read3DImage(medAbstractData* medData,itk::GDCMImageIO::Pointer io,const itkGDCMDataImageReader::FileList& filelist)
{
    auto reader = itk::ImageSeriesReader<itk::Image<TYPE,3> >::New();
    reader->SetImageIO(io);
    reader->SetFileNames(filelist);
    medData->setData(reader->GetOutput());
    reader->Update();
}

template <typename TYPE>
void Read4DImage(medAbstractData* medData, itk::GDCMImageIO::Pointer io, itkGDCMDataImageReader::FileListMapType map) {
    typedef itk::Image<TYPE,4>                   ImageType;
    typedef itk::Image<TYPE,3>                   SubImageType;
    typedef itk::ImageSeriesReader<SubImageType> SeriesReaderType;
    typedef typename ImageType::RegionType       RegionType;
    typedef typename ImageType::SpacingType      SpacingType;
    typedef typename ImageType::PointType        PointType;
    typedef typename ImageType::DirectionType    DirectionType;
    typedef itk::ImageRegionIterator<ImageType>  IteratorType;

    typename ImageType::Pointer image = ImageType::New();

    bool metadatacopied = false;
    IteratorType itOut;

    std::cout << "Building volume containing\t " << map.size() << "\t subvolumes..." << std::flush;

    for (itkGDCMDataImageReader::FileListMapType::iterator it=map.begin();it!=map.end();++it) {
        typename SeriesReaderType::Pointer seriesreader = SeriesReaderType::New();
        seriesreader->UseStreamingOn();

        typename SubImageType::Pointer t_image = nullptr;

        seriesreader->SetFileNames(it->second);
        seriesreader->SetImageIO(io);
        seriesreader->Update();

        t_image = seriesreader->GetOutput();

        if (!metadatacopied)
        {
            RegionType region;
            region.SetSize(0,t_image->GetLargestPossibleRegion().GetSize()[0]);
            region.SetSize(1,t_image->GetLargestPossibleRegion().GetSize()[1]);
            region.SetSize(2,t_image->GetLargestPossibleRegion().GetSize()[2]);
            region.SetSize(3,map.size());
            image->SetRegions(region);
            image->Allocate();

            SpacingType spacing;
            spacing[0] = t_image->GetSpacing()[0];
            spacing[1] = t_image->GetSpacing()[1];
            spacing[2] = t_image->GetSpacing()[2];
            spacing[3] = 1;
            image->SetSpacing(spacing);

            PointType origin;
            origin[0] = t_image->GetOrigin()[0];
            origin[1] = t_image->GetOrigin()[1];
            origin[2] = t_image->GetOrigin()[2];
            origin[3] = 0;
            image->SetOrigin(origin);

            DirectionType direction;
            for (unsigned int i=0; i<4; i++)
                for (unsigned int j=0; j<4; j++)
                {
                    if ((i < 3) && (j < 3))
                    {
                        direction[i][j] = t_image->GetDirection()[i][j];
                    }
                    else
                    {
                        direction[i][j] = (i == j) ? 1 : 0;
                    }
                }
            image->SetDirection(direction);
            itOut = IteratorType(image,region);

            image->SetMetaDataDictionary(io->GetMetaDataDictionary());

            metadatacopied = true;
        }

        itk::ImageRegionIterator<SubImageType> itIn(t_image,t_image->GetLargestPossibleRegion());
        while (!itIn.IsAtEnd())
        {
            itOut.Set(itIn.Get());
            ++itIn;
            ++itOut;
        }
    }
    std::cout << "done" << std::endl;
    medData->setData(image);
}

// /////////////////////////////////////////////////////////////////
// itkGDCMDataImageReaderPrivate
// /////////////////////////////////////////////////////////////////

class itkGDCMDataImageReaderPrivate
{
public:
    itkGDCMDataImageReaderPrivate();
    ~itkGDCMDataImageReaderPrivate();

    itk::GDCMImageIO::Pointer io;

    /* All this below is to workaround the fact that deallocating a itk::GDCMImageIO
     * calls a static method cleanup(), which should not be called if other threads
     * may still be reading DICOM data. So we keep pointers around until all threads
     * are done.
     */

    static itk::GDCMImageIO::Pointer getNewIO();
    static void threadDone(itk::GDCMImageIO::Pointer io);
    static void initialiseStatic();

    static QList<itk::GDCMImageIO::Pointer> * ioPointers;
    static QList<QThread*> * ioThreads;
    static QAtomicPointer<QMutex> mutex;
};

QList<itk::GDCMImageIO::Pointer> * itkGDCMDataImageReaderPrivate::ioPointers = nullptr;
QList<QThread*> * itkGDCMDataImageReaderPrivate::ioThreads = nullptr;
QAtomicPointer<QMutex> itkGDCMDataImageReaderPrivate::mutex;

itkGDCMDataImageReaderPrivate::itkGDCMDataImageReaderPrivate()
{
    io = getNewIO();
}

itkGDCMDataImageReaderPrivate::~itkGDCMDataImageReaderPrivate()
{
    threadDone(io);

    QMutex *m_ptr = mutex.fetchAndStoreOrdered(nullptr);
    if (m_ptr)
    {
        m_ptr->unlock();
        delete m_ptr;
    }
}

itk::GDCMImageIO::Pointer itkGDCMDataImageReaderPrivate::getNewIO()
{
    initialiseStatic();

    QMutexLocker lock(mutex);

    itk::GDCMImageIO::Pointer io = itk::GDCMImageIO::New();

    ioThreads->append(QThread::currentThread());
    ioPointers->append(io);

    return io;
}

void itkGDCMDataImageReaderPrivate::threadDone(itk::GDCMImageIO::Pointer io)
{
    QMutexLocker lock(mutex);

    QThread * thread = QThread::currentThread();
    ioThreads->removeOne(thread);

    if (ioThreads->size() == 0)
    {
        ioPointers->clear();
    }

    delete ioPointers;
    ioPointers = nullptr;

    delete ioThreads;
    ioThreads = nullptr;
}

void itkGDCMDataImageReaderPrivate::initialiseStatic()
{
    if (!mutex)
    {
        QMutex * m = new QMutex();
        if (!mutex.testAndSetOrdered(nullptr, m))
        {
            delete m;
        }
    }

    QMutexLocker lock(mutex);

    if (!ioThreads)
    {
        ioThreads = new QList<QThread*>();
    }

    if (!ioPointers)
    {
        ioPointers = new QList<itk::GDCMImageIO::Pointer>();
    }
}

// /////////////////////////////////////////////////////////////////
// itkGDCMDataImageReader
// /////////////////////////////////////////////////////////////////

itkGDCMDataImageReader::itkGDCMDataImageReader() : dtkAbstractDataReader(), d(new itkGDCMDataImageReaderPrivate)
{
    // This adds a list of tags to be scanned in the data
    this->m_Scanner.AddTag(gdcm::Tag(0x0010, 0x0010)); //PatientName
    this->m_Scanner.AddTag(gdcm::Tag(0x0010, 0x1010)); //Age
    this->m_Scanner.AddTag(gdcm::Tag(0x0010, 0x0030)); //BirthDate
    this->m_Scanner.AddTag(gdcm::Tag(0x0010, 0x0040)); //Gender
    this->m_Scanner.AddTag(gdcm::Tag(0x0018, 0x0022)); //Description: Scan Options

    // Study
    this->m_Scanner.AddTag(gdcm::Tag(0x0020, 0x000d)); //StudyID
    this->m_Scanner.AddTag(gdcm::Tag(0x0020, 0x000D)); //StudyInstanceUID
    this->m_Scanner.AddTag(gdcm::Tag(0x0008, 0x1030)); //StudyDescription
    this->m_Scanner.AddTag(gdcm::Tag(0x0008, 0x0080)); //Institution
    this->m_Scanner.AddTag(gdcm::Tag(0x0008, 0x0090)); //Referee
    
    // Series
    this->m_Scanner.AddTag(gdcm::Tag(0x0020, 0x000e)); //SeriesID
    this->m_Scanner.AddTag(gdcm::Tag(0x0020, 0x000E)); //SeriesInstanceUID
    this->m_Scanner.AddTag(gdcm::Tag(0x0020, 0x0011)); //SeriesNumber
    this->m_Scanner.AddTag(gdcm::Tag(0x0008, 0x0060)); //Modality
    this->m_Scanner.AddTag(gdcm::Tag(0x0018, 0x0060)); //KVP
    this->m_Scanner.AddTag(gdcm::Tag(0x0018, 0x1314)); //FlipAngle
    this->m_Scanner.AddTag(gdcm::Tag(0x0018, 0x0081)); //EchoTime
    this->m_Scanner.AddTag(gdcm::Tag(0x0018, 0x0080)); //RepetitionTime
    this->m_Scanner.AddTag(gdcm::Tag(0x0008, 0x1050)); //Performer
    this->m_Scanner.AddTag(gdcm::Tag(0x0040, 0x0275)); //Report
    this->m_Scanner.AddTag(gdcm::Tag(0x0018, 0x1030)); //Protocol
    this->m_Scanner.AddTag(gdcm::Tag(0x0008, 0x103e)); //SeriesDescription

    // Image
    this->m_Scanner.AddTag(gdcm::Tag(0x0018, 0x0024)); //SequenceName
    this->m_Scanner.AddTag(gdcm::Tag(0x0018, 0x0050)); //SliceThickness
    this->m_Scanner.AddTag(gdcm::Tag(0x0020, 0x0037)); //Orientation
    this->m_Scanner.AddTag(gdcm::Tag(0x0028, 0x0010)); //Rows
    this->m_Scanner.AddTag(gdcm::Tag(0x0028, 0x0011)); //Columns
    this->m_Scanner.AddTag(gdcm::Tag(0x0020, 0x0032)); //PatientPosition
    this->m_Scanner.AddTag(gdcm::Tag(0x0020, 0x0020)); //PatientOrientation
    this->m_Scanner.AddTag(gdcm::Tag(0x0008, 0x0008)); //ImageType
    this->m_Scanner.AddTag(gdcm::Tag(0x0020, 0x0012)); //AcquisitionNumber
    this->m_Scanner.AddTag(gdcm::Tag(0x0008, 0x0022)); //AcquisitionTime
    this->m_Scanner.AddTag(gdcm::Tag(0x0008, 0x0032)); //AcquisitionDate
    this->m_Scanner.AddTag(gdcm::Tag(0x0018, 0x4000)); //Comments

    // NON-STANDARD TAG
    this->m_Scanner.AddTag(gdcm::Tag(0x0011, 0x1010)); //Status
}

itkGDCMDataImageReader::~itkGDCMDataImageReader()
{
    delete d;
    d = nullptr;
}

bool itkGDCMDataImageReader::registered()
{
    return medAbstractDataFactory::instance()->registerDataReaderType(
                "itkGDCMDataImageReader",
                QStringList() << "itkDataImageDouble3"
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
                createItkGDCMDataImageReader);
}


QStringList itkGDCMDataImageReader::handled() const
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

QString itkGDCMDataImageReader::identifier() const
{
    return ID;
}

QString itkGDCMDataImageReader::description() const
{
    return "Reader for GDCM images";
}

bool itkGDCMDataImageReader::canRead(const QString &path)
{
    return d->io->CanReadFile(path.toUtf8().constData());
}

bool itkGDCMDataImageReader::canRead(const QStringList &paths)
{
    for (int i=0; i<paths.size(); i++)
    {
        if (!d->io->CanReadFile(paths[i].toUtf8().constData()))
        {
            return false;
        }
    }
    return true;
}

bool itkGDCMDataImageReader::readInformation(const QString &path)
{
    QStringList paths;
    paths << path;
    return readInformation(paths);
}

bool itkGDCMDataImageReader::readInformation(const QStringList &paths)
{
    if (paths.size()==0)
    {
        return false;
    }

    FileList filenames;
    for (int i=0; i<paths.size(); i++)
    {
        filenames.push_back(paths[i].toUtf8().constData());
    }

    FileListMapType map = this->sort(filenames);

    std::string firstfilename = (*map.begin()).second[0];

    d->io->SetFileName(firstfilename.c_str());
    try
    {
        d->io->ReadImageInformation();
    }
    catch(itk::ExceptionObject &e)
    {
        qDebug() << "GDCM: "<<e.GetDescription();
        return false;
    }

    medAbstractData* medData = dynamic_cast<medAbstractData*>(this->data());

    if (!medData)
    {
        unsigned int imagedimension = 3;

        if (map.size() > 1)
        {
            std::cout<<"GDCM: 4th dimension encountered"<<std::endl;
            imagedimension = 4;
        }

        std::ostringstream imagetypestring;
        imagetypestring << "itkDataImage";

        if (d->io->GetPixelType() != itk::IOPixelEnum::SCALAR)
        {
            qDebug() << "GDCM: unsupported pixel type";
            return false;
        }

        switch (d->io->GetComponentType())
        {
        case itk::IOComponentEnum::UCHAR:
            imagetypestring << "UChar";
            break;
        case itk::IOComponentEnum::CHAR:
            imagetypestring << "Char";
            break;
        case itk::IOComponentEnum::USHORT:
            imagetypestring << "UShort";
            break;
        case itk::IOComponentEnum::SHORT:
            imagetypestring << "Short";
            break;
        case itk::IOComponentEnum::UINT:
            imagetypestring << "UInt";
            break;
        case itk::IOComponentEnum::INT:
            imagetypestring << "Int";
            break;
        case itk::IOComponentEnum::ULONG:
            imagetypestring << "ULong";
            break;
        case itk::IOComponentEnum::LONG:
            imagetypestring << "Long";
            break;
        case itk::IOComponentEnum::FLOAT:
            imagetypestring << "Float";
            break;
        case itk::IOComponentEnum::DOUBLE:
            /**
      @todo Handle properly double pixel values.
      For the moment it is only handled in 3D, not in 4D, and it is very
      common to have 4D double images (cardiac).
      This hack just downcast images in short when the dimension is 4.
      which is WRONG.
   */
            if (imagedimension == 4)
                imagetypestring << "Short";
            else
                imagetypestring << "Double";
            break;
        default:
            qDebug() << "GDCM: unrecognized component type:\t " << static_cast<int>(d->io->GetComponentType());
            return false;
        }

        imagetypestring << imagedimension;
        if (imagedimension == 4)
        {
            qDebug() << "GDCM: image type given :\t" << imagetypestring.str().c_str();
        }

        medData = medAbstractDataFactory::instance()->create(imagetypestring.str().c_str());
        if (medData)
        {
            this->setData(medData);
        }
    }

    if (medData)
    {
        //todo ajouter toutes les autres medMetaDataKeys

        // PATIENT
        setMetaData(medMetaDataKeys::PatientName.key(), medData, firstfilename, 0x0010, 0x0010);
        setMetaData(medMetaDataKeys::Age.key(),         medData, firstfilename, 0x0010, 0x1010);
        setMetaData(medMetaDataKeys::BirthDate.key(),   medData, firstfilename, 0x0010, 0x0030);
        setMetaData(medMetaDataKeys::Gender.key(),      medData, firstfilename, 0x0010, 0x0040);
        setMetaData(medMetaDataKeys::Description.key(), medData, firstfilename, 0x0018, 0x0022); // Scan Options

        // STUDY
        setMetaData(medMetaDataKeys::StudyID.key(),          medData, firstfilename, 0x0020, 0x000d);
        setMetaData(medMetaDataKeys::StudyInstanceUID.key(), medData, firstfilename, 0x0020, 0x000D);
        setMetaData(medMetaDataKeys::StudyDescription.key(), medData, firstfilename, 0x0008, 0x1030);
        setMetaData(medMetaDataKeys::Institution.key(),      medData, firstfilename, 0x0008, 0x0080);
        setMetaData(medMetaDataKeys::Referee.key(),          medData, firstfilename, 0x0008, 0x0090);

        // SERIES
        setMetaData(medMetaDataKeys::SeriesID.key(),          medData, firstfilename, 0x0020, 0x000e);
        setMetaData(medMetaDataKeys::SeriesInstanceUID.key(), medData, firstfilename, 0x0020, 0x000E);
        setMetaData(medMetaDataKeys::SeriesNumber.key(),      medData, firstfilename, 0x0020, 0x0011);

        auto modality = QStringList() << this->m_Scanner.GetValue(firstfilename.c_str(), gdcm::Tag(0x0008, 0x0060));
        medData->setMetaData(medMetaDataKeys::Modality.key(), modality);

        if (modality.contains("CT"))
        {
            setMetaData(medMetaDataKeys::KVP.key(), medData, firstfilename, 0x0018, 0x0060);
        }
        else if (modality.contains("MR"))
        {
            setMetaData(medMetaDataKeys::FlipAngle.key(),      medData, firstfilename, 0x0018, 0x1314);
            setMetaData(medMetaDataKeys::EchoTime.key(),       medData, firstfilename, 0x0018, 0x0081);
            setMetaData(medMetaDataKeys::RepetitionTime.key(), medData, firstfilename, 0x0018, 0x0080);
        }

        setMetaData(medMetaDataKeys::Performer.key(),         medData, firstfilename, 0x0008, 0x1050); //performingPhysicianName
        setMetaData(medMetaDataKeys::Report.key(),            medData, firstfilename, 0x0040, 0x0275);
        setMetaData(medMetaDataKeys::Protocol.key(),          medData, firstfilename, 0x0018, 0x1030);
        setMetaData(medMetaDataKeys::SeriesDescription.key(), medData, firstfilename, 0x0008, 0x103e);

        // IMAGE
        setMetaData(medMetaDataKeys::SequenceName.key(),       medData, firstfilename, 0x0018, 0x0024);
        setMetaData(medMetaDataKeys::SliceThickness.key(),     medData, firstfilename, 0x0018, 0x0050);
        setMetaData(medMetaDataKeys::Orientation.key(),        medData, firstfilename, 0x0020, 0x0037);
        setMetaData(medMetaDataKeys::Rows.key(),               medData, firstfilename, 0x0028, 0x0010);
        setMetaData(medMetaDataKeys::Columns.key(),            medData, firstfilename, 0x0028, 0x0011);
        setMetaData(medMetaDataKeys::PatientPosition.key(),    medData, firstfilename, 0x0020, 0x0032);
        setMetaData(medMetaDataKeys::PatientOrientation.key(), medData, firstfilename, 0x0020, 0x0020);

        QString imageType = this->m_Scanner.GetValue(firstfilename.c_str(), gdcm::Tag(0x0008, 0x0008));
        // It seems '\' characters are replaced by whitespaces
        imageType = imageType.replace(' ', "\\");
        medData->setMetaData(medMetaDataKeys::ImageType.key(), imageType);

        setMetaData(medMetaDataKeys::AcquisitionNumber.key(), medData, firstfilename, 0x0020, 0x0012);
        // Todo: check time and date, it was switched in itkdcmtk file
        setMetaData(medMetaDataKeys::AcquisitionTime.key(), medData, firstfilename, 0x0008, 0x0022);
        setMetaData(medMetaDataKeys::AcquisitionDate.key(), medData, firstfilename, 0x0008, 0x0032);
        setMetaData(medMetaDataKeys::Comments.key(),        medData, firstfilename, 0x0018, 0x4000);

        // NON-STANDARD TAG
        auto patientStatus = QStringList() << this->m_Scanner.GetValue(firstfilename.c_str(), gdcm::Tag(0x0011, 0x1010));
        medData->setMetaData(medMetaDataKeys::Status.key(), patientStatus);

        // OTHER
        QString origin = "";
        for (unsigned int i = 0; i < d->io->GetNumberOfDimensions(); ++i)
        {
            origin += QString::number(d->io->GetOrigin(i)) + QString(" ");
        }
        medData->setMetaData(medMetaDataKeys::Origin.key(), origin.trimmed());

        QStringList filePaths;
        FileList orderedfilelist = this->unfoldMap(map);
        for (unsigned int i=0; i<orderedfilelist.size(); i++)
        {
            filePaths << orderedfilelist[i].c_str();
        }
        medData->addMetaData(medMetaDataKeys::FilePaths.key(), filePaths);
    }

    return true;
}

void itkGDCMDataImageReader::setMetaData(QString key, medAbstractData* medData, 
                                         std::string firstfilename, uint16_t groupe, uint16_t element)
{
    auto keyValue = QStringList() << this->m_Scanner.GetValue(firstfilename.c_str(), gdcm::Tag(groupe, element));
    medData->setMetaData(key, keyValue);
}

bool itkGDCMDataImageReader::read(const QString &path)
{
    QStringList paths;
    paths << path;
    return read(paths);
}

bool itkGDCMDataImageReader::read (const QStringList &paths)
{
    if (paths.size()==0)
    {
        return false;
    }

    this->readInformation(paths);

    FileList filenames;
    for (int i=0;i<paths.size();i++)
    {
        filenames.push_back(paths[i].toUtf8().constData());
    }

    FileListMapType map = this->sort(filenames);

    if (!map.size())
    {
        qDebug() << "GDCM: no image can be build from file list (empty map)";
        return false;
    }

    itk::DataImageReaderCommand::Pointer command = itk::DataImageReaderCommand::New();
    command->SetDataImageReader(this);
    d->io->AddObserver(itk::ProgressEvent(),command);

    if (medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data()))
    {
        QStringList qfilelist = medData->metaDataValues("FilePaths");
        FileList filelist;
        for (int i=0;i<qfilelist.size();i++)
        {
            filelist.push_back(qfilelist[i].toUtf8().constData());
        }

        std::cout << "GDCM: reading : "    << medData->identifier().toUtf8().constData() << std::endl;
        std::cout << "GDCM: containing : " << map.size() << " volumes" << std::endl;

        try {
            if      (medData->identifier()=="itkDataImageUChar3")  { Read3DImage<unsigned char>(medData,d->io,filelist);  }
            else if (medData->identifier()=="itkDataImageChar3")   { Read3DImage<char>(medData,d->io,filelist);           }
            else if (medData->identifier()=="itkDataImageUShort3") { Read3DImage<unsigned short>(medData,d->io,filelist); }
            else if (medData->identifier()=="itkDataImageShort3")  { Read3DImage<short>(medData,d->io,filelist);          }
            else if (medData->identifier()=="itkDataImageUInt3")   { Read3DImage<unsigned int>(medData,d->io,filelist);   }
            else if (medData->identifier()=="itkDataImageInt3")    { Read3DImage<int>(medData,d->io,filelist);            }
            else if (medData->identifier()=="itkDataImageULong3")  { Read3DImage<unsigned long>(medData,d->io,filelist);  }
            else if (medData->identifier()=="itkDataImageLong3")   { Read3DImage<long>(medData,d->io,filelist);           }
            else if (medData->identifier()=="itkDataImageFloat3")  { Read3DImage<float>(medData,d->io,filelist);          }
            else if (medData->identifier()=="itkDataImageDouble3") { Read3DImage<double>(medData,d->io,filelist);         }
            else if (medData->identifier()=="itkDataImageUChar4")  { Read4DImage<unsigned char>(medData,d->io,map);          }
            else if (medData->identifier()=="itkDataImageUShort4") { Read4DImage<unsigned short>(medData,d->io,map);         }
            else if (medData->identifier()=="itkDataImageShort4")  { Read4DImage<short>(medData,d->io,map);                  }
            else if (medData->identifier()=="itkDataImageUInt4")   { Read4DImage<unsigned int>(medData,d->io,map);           }
            else if (medData->identifier()=="itkDataImageULong4")  { Read4DImage<unsigned long>(medData,d->io,map);          }
            else if (medData->identifier()=="itkDataImageInt4")    { Read4DImage<int>(medData,d->io,map);                    }
            else if (medData->identifier()=="itkDataImageLong4")   { Read4DImage<long>(medData,d->io,map);                   }
            else if (medData->identifier()=="itkDataImageChar4")   { Read4DImage<char>(medData,d->io,map);                   }
            else if (medData->identifier()=="itkDataImageDouble4") {
                /**
                @todo Handle properly double pixel values.
                For the moment it is only handled in 3D, not in 4D, and it is very
                common to have 4D double images (cardiac).
                This hack just downcast images in short when the dimension is 4.
                which is WRONG.
                 */
                Read4DImage<short>(medData,d->io,map);
            } else
            {
                qDebug() << "GDCM: unhandled medData type : " << medData->identifier();
                return false;
            }
        } catch (itk::ExceptionObject &e)
        {
            qDebug() << "GDCM: "<<e.GetDescription();
            return false;
        }
    }

    if (medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data()))
    {
        // copy over the dicom dictionary into metadata
        typedef itk::MetaDataObject <std::vector<std::string> > MetaDataVectorStringType;
        typedef std::vector<std::string>                        StringVectorType;

        const itk::MetaDataDictionary& dictionary = d->io->GetMetaDataDictionary();

        itk::MetaDataDictionary::ConstIterator it = dictionary.Begin();
        while(it!=dictionary.End())
        {
            if( MetaDataVectorStringType* metaData = dynamic_cast<MetaDataVectorStringType*>( it->second.GetPointer() ) )
            {
                const StringVectorType &values = metaData->GetMetaDataObjectValue();
                for (unsigned int i=0; i<values.size(); i++)
                {
                    medData->addMetaData( it->first.c_str(), values[i].c_str());
                }
            }
            ++it;
        }
    }

    d->io->RemoveAllObservers();

    return true;
}

itkGDCMDataImageReader::FileListMapType itkGDCMDataImageReader::sort (FileList filelist)
{
    this->m_Scanner.Scan(filelist);
    FileListMapType ret;

    if (!filelist.size())
    {
        return ret;
    }

    const char *reference = filelist[0].c_str();

    if (filelist.size() == 1)
    {
        FileListMapType::value_type newpair("unique_file",filelist);
        ret.insert(newpair);
        return ret;
    }

    const gdcm::Tag orientationtag(0x20,0x37);

    gdcm::Scanner::ValuesType orientations = this->m_Scanner.GetValues(orientationtag);
    if (orientations.size() != 1)
    {
        qDebug() <<"GDCM: more than one Orientation in filenames (or no Orientation)";
        return ret;
    }

    gdcm::Scanner::TagToValue const &t2v = this->m_Scanner.GetMapping(reference);
    gdcm::Scanner::TagToValue::const_iterator firstit = t2v.find(orientationtag);
    if ((*firstit).first != orientationtag)
    {
        qDebug() <<"GDCM: could not find any orientation information in the header of the reference file";
        return ret;
    }

    const char *dircos = (*firstit).second;
    std::stringstream ss;
    ss.str(dircos);

    gdcm::Element<gdcm::VR::DS,gdcm::VM::VM6> cosines;
    cosines.Read(ss);

    // http://www.itk.org/pipermail/insight-users/2003-September/004762.html
    // Compute normal:
    // The steps I take when reconstructing a volume are these: First,
    // calculate the slice normal from IOP:
    double normal[3];
    normal[0] = cosines[1]*cosines[5] - cosines[2]*cosines[4];
    normal[1] = cosines[2]*cosines[3] - cosines[0]*cosines[5];
    normal[2] = cosines[0]*cosines[4] - cosines[1]*cosines[3];

    SortedMapType sorted;

    FileList::const_iterator it;

    for(it = filelist.begin(); it != filelist.end(); ++it)
    {
        const char *filename = (*it).c_str();
        bool iskey = this->m_Scanner.IsKey(filename);
        if (iskey)
        {
            const char *value = this->m_Scanner.GetValue(filename,gdcm::Tag(0x20,0x32));
            if (value)
            {
                gdcm::Element<gdcm::VR::DS,gdcm::VM::VM3> ipp;
                std::stringstream ss;
                ss.str(value);
                ipp.Read(ss);
                double dist = 0;
                for (int i = 0; i < 3; ++i)
                {
                    dist += normal[i]*ipp[i];
                }

                bool found = 0;
                SortedMapType::iterator finder;
                for (finder = sorted.begin(); finder != sorted.end(); ++finder)
                {
                    if (std::abs((*finder).first - dist) < itk::NumericTraits<double>::min())
                    {
                        found = 1;
                        break;
                    }
                }

                if (!found)
                {
                    FileList newlist;
                    newlist.push_back(filename);
                    SortedMapType::value_type newpair(dist,newlist);
                    sorted.insert(newpair);
                }
                else
                {
                    (*finder).second.push_back(filename);
                }
            }
        }
        else
        {
            qDebug() << "GDCM: the file "
                     << filename
                     <<" does not appear in the scanner mappings, skipping. ";
        }
    }

    if ((filelist.size() % sorted.size()) != 0)
    {
        qDebug() << "GDCM: there appears to be inconsistent file list sizes\n "
                 << "Scanner outputs "<<sorted.size()<<" different image positions\n "
                 << "within a total list of "<<filelist.size()<<" files.\n"
                 << "no sorting will be performed\n";
        return ret;

        ///\todo Here we could take into account the files that are consistent
        /// to each other and leave the rest, to build a truncated volume.
    }

    unsigned int nb_of_volumes = filelist.size() / sorted.size();

    for (unsigned int i=0; i<nb_of_volumes; i++)
    {
        std::ostringstream os;
        os<<".";
        if (i < 100)
            os << "0";
        if (i < 10)
            os << "0";
        os<<i;

        FileList newfilelist;

        SortedMapType::const_iterator toinsert;

        for (toinsert = sorted.begin(); toinsert != sorted.end(); ++toinsert)
        {
            newfilelist.push_back((*toinsert).second[i]);
        }

        FileListMapType::value_type newpair(os.str(),newfilelist);
        ret.insert(newpair);
    }

    return ret;
}

itkGDCMDataImageReader::FileList itkGDCMDataImageReader::unfoldMap (FileListMapType map)
{
    FileList ret;

    FileListMapType::const_iterator it;
    for (it = map.begin(); it != map.end(); ++it)
    {
        FileList filelist = (*it).second;
        for (unsigned int i=0; i<filelist.size(); i++)
        {
            ret.push_back(filelist[i]);
        }
    }

    return ret;
}


void itkGDCMDataImageReader::setProgress (int value)
{
    emit progressed (value);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkGDCMDataImageReader()
{
    return new itkGDCMDataImageReader;
}
