/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDCMTKDataImageReader.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medMetaDataKeys.h>

#include <itkDCMTKImageIO.h>

#include <itkCommonEnums.h>
#include <itkImageIOBase.h>
#include <itkImageFileReader.h>
#include <itkRGBPixel.h>
#include <itkMetaDataDictionary.h>
#include <itkObjectFactoryBase.h>
#include <itkCommand.h>

// implement an observer
namespace itk {

    class DCMTKDataImageReaderCommand : public Command
    {

    public:

        typedef DCMTKDataImageReaderCommand     Self;
        typedef Command                        Superclass;
        typedef itk::SmartPointer<Self>        Pointer;
        typedef itk::SmartPointer<const Self>  ConstPointer;

        itkTypeMacro(DCMTKDataImageReaderCommand, Command)
            itkNewMacro(Self)

            void Execute(Object *caller, const EventObject &event);
        void Execute(const Object *caller, const EventObject &event);

        void SetDataImageReader(medAbstractDataReader* reader) { m_Reader = reader; }

    protected:
        DCMTKDataImageReaderCommand() { m_Reader = nullptr; }
        virtual ~DCMTKDataImageReaderCommand() {}

    private:
        medAbstractDataReader* m_Reader;
    };

    void DCMTKDataImageReaderCommand::Execute(Object *caller, const EventObject &event)
    {
        ImageIOBase *po = dynamic_cast<ImageIOBase *>(caller);

        if (!po)
            return;

        if (typeid(event) == typeid(itk::ProgressEvent))
        {
            if (m_Reader)
                m_Reader->setProgress((int)(po->GetProgress()*100.0));
        }
    }

    void DCMTKDataImageReaderCommand::Execute(const Object *caller, const EventObject &event)
    {
        ImageIOBase *po = dynamic_cast<ImageIOBase*>(const_cast<Object *>(caller));
        if (!po)
            return;

        if (typeid(event) == typeid(itk::ProgressEvent))
        {
            if (m_Reader)
                m_Reader->setProgress((int)(po->GetProgress()*100.0));
        }
    }
}


template <typename TYPE, unsigned DIM>
void ReadImage(medAbstractData* medData, itk::DCMTKImageIO::Pointer io, const QStringList& paths)
{
    typename itk::ImageFileReader<itk::Image<TYPE, DIM> >::Pointer Reader = itk::ImageFileReader<itk::Image<TYPE, DIM> >::New();
    Reader->SetImageIO(io);
    Reader->SetFileName(paths[0].toStdString());
    medData->setData(Reader->GetOutput());
    Reader->Update();
}

// /////////////////////////////////////////////////////////////////
// itkDCMTKDataImageReaderPrivate
// /////////////////////////////////////////////////////////////////

class itkDCMTKDataImageReaderPrivate
{
public:
    itkDCMTKDataImageReaderPrivate();
    ~itkDCMTKDataImageReaderPrivate();

    itk::DCMTKImageIO::Pointer io;


    /* All this below is to workaround the fact that deallocating a itk::DCMTKImageIO
     * calls a static method cleanup(), which should not be called if other threads
     * may still be reading DICOM data. So we keep pointers around until all threads
     * are done.
     *
     * See : http://pm-med.inria.fr/issues/1604
     */

    static itk::DCMTKImageIO::Pointer getNewIO();
    static void threadDone(itk::DCMTKImageIO::Pointer io);
    static void initialiseStatic();

    static QList<itk::DCMTKImageIO::Pointer> * ioPointers;
    static QList<QThread*> * ioThreads;
    static QAtomicPointer<QMutex> mutex;
};

QList<itk::DCMTKImageIO::Pointer> * itkDCMTKDataImageReaderPrivate::ioPointers = nullptr;
QList<QThread*> * itkDCMTKDataImageReaderPrivate::ioThreads = nullptr;
QAtomicPointer<QMutex> itkDCMTKDataImageReaderPrivate::mutex;

itkDCMTKDataImageReaderPrivate::itkDCMTKDataImageReaderPrivate()
{
    io = getNewIO();
}

itkDCMTKDataImageReaderPrivate::~itkDCMTKDataImageReaderPrivate()
{
    threadDone(io);
}

itk::DCMTKImageIO::Pointer itkDCMTKDataImageReaderPrivate::getNewIO()
{
    initialiseStatic();

    QMutexLocker lock(mutex);

    itk::DCMTKImageIO::Pointer io = itk::DCMTKImageIO::New();

    ioThreads->append(QThread::currentThread());
    ioPointers->append(io);

    return io;
}

void itkDCMTKDataImageReaderPrivate::threadDone(itk::DCMTKImageIO::Pointer io)
{
    QMutexLocker lock(mutex);

    QThread * thread = QThread::currentThread();
    ioThreads->removeOne(thread);

    if (ioThreads->size() == 0)
        ioPointers->clear();
}

void itkDCMTKDataImageReaderPrivate::initialiseStatic()
{
    if (!mutex) {
        QMutex * m = new QMutex();
        if (!mutex.testAndSetOrdered(NULL, m))
            delete m;
    }

    QMutexLocker lock(mutex);

    if (!ioThreads)
        ioThreads = new QList<QThread*>();

    if (!ioPointers)
        ioPointers = new QList<itk::DCMTKImageIO::Pointer>();
}


// /////////////////////////////////////////////////////////////////
// itkDCMTKDataImageReader
// /////////////////////////////////////////////////////////////////

const char itkDCMTKDataImageReader::ID[] = "itkDCMTKDataImageReader";

itkDCMTKDataImageReader::itkDCMTKDataImageReader() : medAbstractDataReader(), d(new itkDCMTKDataImageReaderPrivate) { }


itkDCMTKDataImageReader::~itkDCMTKDataImageReader()
{
    delete d;
    d = 0;
}


bool itkDCMTKDataImageReader::registered()
{
    return medAbstractDataFactory::instance()->registerDataReaderType("itkDCMTKDataImageReader", QStringList() << "itkDataImageDouble3"
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
        << "itkDataImageUChar4"
        << "itkDataImageShort3"
        << "itkDataImageShort4"
        << "itkDataImageUChar3"
        << "itkDataImageChar3"
        << "itkDataImageChar4"
        << "itkDataImageRGB3",
        createItkDCMTKDataImageReader);
}


QStringList itkDCMTKDataImageReader::handled() const
{
    return QStringList() << "itkDataImageDouble3"
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
        << "itkDataImageRGB3";
}

QString itkDCMTKDataImageReader::identifier() const
{
    return ID;
}

QString itkDCMTKDataImageReader::description() const
{
    return "Reader for DCMTK images";
}

bool itkDCMTKDataImageReader::canRead(const QString& path)
{
    return d->io->CanReadFile(path.toUtf8());
}

bool itkDCMTKDataImageReader::canRead(const QStringList& paths)
{
    int count = 0;
    for (int i = 0; i < paths.size(); i++)
    {
        if (d->io->CanReadFile(paths[i].toUtf8()))
        {
            count++;
        }
    }
    return count > (paths.size() >> 1);
}

bool itkDCMTKDataImageReader::readInformation(const QString& path)
{
    QStringList paths;
    paths << path;
    return readInformation(paths);
}

bool itkDCMTKDataImageReader::readInformation(const QStringList& paths)
{
    if (paths.size() == 0)
        return false;

    std::vector< std::string > filenames;
    for (int i = 0; i < paths.size(); i++)
        filenames.push_back(paths[i].toUtf8().data());

    d->io->SetFileNames(filenames);
    try
    {
        d->io->ReadImageInformation();
    }
    catch (itk::ExceptionObject &e)
    {
        qDebug() << e.GetDescription();
        return false;
    }

    medAbstractData* medData = dynamic_cast<medAbstractData*>(this->data());

    if (!medData)
    {
        std::ostringstream imagetypestring;
        imagetypestring << "itkDataImage";

        if (d->io->GetPixelType() == itk::IOPixelEnum::SCALAR)
        {
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
                imagetypestring << "Double";
                break;
            default:
                qDebug() << "Unrecognized component type: " << static_cast<int>(d->io->GetComponentType());
                return false;
            }

            imagetypestring << d->io->GetNumberOfDimensions();
            medData = medAbstractDataFactory::instance()->create(imagetypestring.str().c_str());
            if (medData)
                this->setData(medData);
        }
        else if (d->io->GetPixelType() == itk::IOPixelEnum::RGB)
        {

            switch (d->io->GetComponentType())
            {
            case itk::IOComponentEnum::UCHAR:
                medData = medAbstractDataFactory::instance()->create("itkDataImageRGB3");

                if (medData)
                    this->setData(medData);
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
    }

    if (medData)
    {
        for (const auto & [key, values] : d->io->GetMetaData())
        {
            QStringList valueList;
            for (auto val : values)
            {
                valueList << QString::fromStdString(val);
            }

            medData->setMetaData(QString::fromStdString(key), valueList);
        }
        // PATIENT
        //PatientId
        medData->setMetaData(medMetaDataKeys::key("PatientName"), QString::fromLatin1(d->io->GetPatientName().c_str()));
        medData->setMetaData(medMetaDataKeys::key("Age"),         d->io->GetPatientAge().c_str());
        medData->setMetaData(medMetaDataKeys::key("BirthDate"),   d->io->GetPatientDOB().c_str());
        medData->setMetaData(medMetaDataKeys::key("Gender"),      d->io->GetPatientSex().c_str());
        medData->setMetaData(medMetaDataKeys::key("PatientID"), QString::fromLatin1(d->io->GetPatientID().c_str()));
        medData->setMetaData(medMetaDataKeys::key("Description"), QString::fromLatin1(d->io->GetScanOptions().c_str()));

        // STUDY
        //StudyId
        medData->setMetaData(medMetaDataKeys::key("StudyID"),          QString::fromLatin1(d->io->GetStudyID().c_str()));
        medData->setMetaData(medMetaDataKeys::key("StudyInstanceUID"), d->io->GetStudyInstanceUID().c_str());
        medData->setMetaData(medMetaDataKeys::key("StudyDescription"), QString::fromLatin1(d->io->GetStudyDescription().c_str()));
        medData->setMetaData(medMetaDataKeys::key("Institution"),      QString::fromLatin1(d->io->GetInstitution().c_str()));
        medData->setMetaData(medMetaDataKeys::key("Referee"),          QString::fromLatin1(d->io->GetReferringPhysicianName().c_str()));
        //StudyDate
        //StudyTime

        // SERIES
        //SeriesID
        medData->setMetaData(medMetaDataKeys::key("SeriesInstanceUID"), d->io->GetSeriesInstanceUID().c_str());
        medData->setMetaData(medMetaDataKeys::key("SeriesNumber"),      d->io->GetSeriesNumber().c_str());
        medData->setMetaData(medMetaDataKeys::key("Modality"),          d->io->GetModality().c_str());
        medData->setMetaData(medMetaDataKeys::key("Performer"),         QString::fromLatin1(d->io->GetPerformingPhysicianName().c_str()));
        medData->setMetaData(medMetaDataKeys::key("Report"), "");
        medData->setMetaData(medMetaDataKeys::key("Protocol"),          QString::fromLatin1(d->io->GetProtocolName().c_str()));
        medData->setMetaData(medMetaDataKeys::key("SeriesDescription"), QString::fromLatin1(d->io->GetSeriesDescription().c_str()));
        //SeriesDate
        //SeriesTime
        //SeriesThumbnail

        // IMAGE
        //SOPInstanceUID
        medData->setMetaData(medMetaDataKeys::key("Columns"),         d->io->GetColumns().c_str());
        medData->setMetaData(medMetaDataKeys::key("Rows"),            d->io->GetRows().c_str());
        //Dimensions
        //NumberOfDimensions
        medData->setMetaData(medMetaDataKeys::key("Orientation"),     d->io->GetOrientation().c_str());

        // Patient position
        std::string patientPos = d->io->GetMetaDataValueString("(0018,5100)", 0);
        medData->setMetaData(medMetaDataKeys::key("PatientPosition"), patientPos.c_str());
        // Patient orientation
        std::string patientOrient = d->io->GetMetaDataValueString("(0020,0020)", 0);
        medData->setMetaData(medMetaDataKeys::key("PatientOrientation"), patientOrient.c_str());

        // Image Type
        QString imageType = QString::fromStdString(d->io->GetMetaDataValueString("(0008,0008)", 0));
        // it seems '\' characters are replaced by whitespaces. This is not correct
        // for this tag.
        imageType = imageType.replace(' ', "\\");
        medData->setMetaData(medMetaDataKeys::key("ImageType"), imageType.toStdString().c_str());

        // Acquisition number
        std::string acquisitionNumber = d->io->GetMetaDataValueString("(0020,0012)", 0);
        medData->setMetaData(medMetaDataKeys::key("AcquisitionNumber"), acquisitionNumber.c_str());

        // Add Origin
        QString origin = "";
        for (unsigned int i = 0; i < d->io->GetNumberOfDimensions(); ++i)
        {
            origin += QString::number(d->io->GetOrigin(i)) + QString(" ");
        }
        medData->setMetaData(medMetaDataKeys::key("Origin"), origin.trimmed());

        medData->setMetaData(medMetaDataKeys::key("SliceThickness"),  d->io->GetSliceThickness().c_str());
        //ImportationDate
        medData->setMetaData(medMetaDataKeys::key("AcquisitionDate"), d->io->GetAcquisitionDate().c_str());
        medData->setMetaData(medMetaDataKeys::key("AcquisitionTime"), d->io->GetAcquisitionTime().c_str());
        medData->setMetaData(medMetaDataKeys::key("Comments"),        QString::fromLatin1(d->io->GetAcquisitionComments().c_str()));

        QStringList filePaths;
        for (unsigned int i = 0; i < d->io->GetOrderedFileNames().size(); i++)
        {
            if (d->io->GetOrderedFileNames()[i] != "")
            {
                filePaths << QString::fromLatin1(d->io->GetOrderedFileNames()[i].c_str());
            }
        }

        medData->addMetaData(medMetaDataKeys::key("FilePaths"), filePaths);

        medData->setMetaData(medMetaDataKeys::key("Status"),          d->io->GetPatientStatus().c_str());
        medData->setMetaData(medMetaDataKeys::key("SequenceName"),    QString::fromLatin1(d->io->GetSequenceName().c_str()));
        //Size
        //VolumeUID
        //Spacing
        //XSpacing
        //YSpacing
        //ZSpacing
        //NumberOfComponents
        //ComponentType
        //PixelType
        //medDataType
        //PreferredDataReader
        //ImageID
        //ThumbnailPath

        QString modality = medData->metadata(medMetaDataKeys::key("Modality"));
        if (modality.contains("CT"))
        {
            std::string kvp = d->io->GetMetaDataValueString("(0018,0060)", 0 );
            medData->setMetaData(medMetaDataKeys::key("KVP"), kvp.c_str());
        }
        else if (modality.contains("MR"))
        {
            // MR Image
            medData->setMetaData(medMetaDataKeys::key("FlipAngle"), d->io->GetFlipAngle().c_str());
            medData->setMetaData(medMetaDataKeys::key("EchoTime"), d->io->GetEchoTime().c_str());
            medData->setMetaData(medMetaDataKeys::key("RepetitionTime"), d->io->GetRepetitionTime().c_str());
        }

        // Frame of reference
        std::string frameOfRef = d->io->GetMetaDataValueString("(0020,0052)", 0);
        medData->setMetaData(medMetaDataKeys::key("FrameOfReferenceUID"), frameOfRef.c_str());
        std::string posRefIndicator = d->io->GetMetaDataValueString("(0020,1040)", 0);
        medData->setMetaData(medMetaDataKeys::key("PositionReferenceIndicator"), posRefIndicator.c_str());

        // Equipement
        std::string manufacturer = d->io->GetMetaDataValueString("(0008,0070)", 0);
        medData->setMetaData(medMetaDataKeys::key("Manufacturer"), manufacturer.c_str());
    }
    else
    {
        qDebug() << "data is empty";
        return false;
    }

    return true;
}

bool itkDCMTKDataImageReader::read(const QString& path)
{
    QStringList paths;
    paths << path;
    return read(paths);
}


bool itkDCMTKDataImageReader::read(const QStringList& paths)
{
    if (paths.size() == 0)
        return false;

    this->readInformation(paths);

    itk::DCMTKDataImageReaderCommand::Pointer command = itk::DCMTKDataImageReaderCommand::New();
    command->SetDataImageReader(this);
    d->io->AddObserver(itk::ProgressEvent(), command);

    if (medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data()))
    {

        try
        {
            if (medData->identifier() == "itkDataImageUChar3") { ReadImage<unsigned char, 3>(medData, d->io, paths); }
            else if (medData->identifier() == "itkDataImageChar3") { ReadImage<char, 3>(medData, d->io, paths); }
            else if (medData->identifier() == "itkDataImageUShort3") { ReadImage<unsigned short, 3>(medData, d->io, paths); }
            else if (medData->identifier() == "itkDataImageShort3") { ReadImage<short, 3>(medData, d->io, paths); }
            else if (medData->identifier() == "itkDataImageUInt3") { ReadImage<unsigned int, 3>(medData, d->io, paths); }
            else if (medData->identifier() == "itkDataImageInt3") { ReadImage<int, 3>(medData, d->io, paths); }
            else if (medData->identifier() == "itkDataImageInt4") { ReadImage<int, 4>(medData, d->io, paths); }
            else if (medData->identifier() == "itkDataImageLong4") { ReadImage<long, 4>(medData, d->io, paths); }
            else if (medData->identifier() == "itkDataImageUInt4") { ReadImage<unsigned int, 4>(medData, d->io, paths); }
            else if (medData->identifier() == "itkDataImageULong4") { ReadImage<unsigned long, 4>(medData, d->io, paths); }
            else if (medData->identifier() == "itkDataImageULong3") { ReadImage<unsigned long, 3>(medData, d->io, paths); }
            else if (medData->identifier() == "itkDataImageLong3") { ReadImage<long, 3>(medData, d->io, paths); }
            else if (medData->identifier() == "itkDataImageFloat3") { ReadImage<float, 3>(medData, d->io, paths); }
            else if (medData->identifier() == "itkDataImageDouble3") { ReadImage<double, 3>(medData, d->io, paths); }
            else if (medData->identifier() == "itkDataImageRGB3") { ReadImage<itk::RGBPixel<unsigned char>, 3>(medData, d->io, paths); }
            else if (medData->identifier() == "itkDataImageUShort4") { ReadImage<unsigned short, 4>(medData, d->io, paths); }
            else if (medData->identifier() == "itkDataImageUChar4") { ReadImage<unsigned char, 4>(medData, d->io, paths); }
            else if (medData->identifier() == "itkDataImageShort4") { ReadImage<short, 4>(medData, d->io, paths); }
            else if (medData->identifier() == "itkDataImageChar4") { ReadImage<char, 4>(medData, d->io, paths); }
            else
            {
                qWarning() << "Unrecognized pixel type";
                return false;
            }
        }
        catch (itk::ExceptionObject &e)
        {
            qDebug() << e.GetDescription();
            return false;
        }

        // copy over the dicom dictionary into metadata
        typedef itk::DCMTKImageIO::MetaDataVectorStringType MetaDataVectorStringType;
        typedef itk::DCMTKImageIO::StringVectorType         StringVectorType;

        const itk::MetaDataDictionary& dictionary = d->io->GetMetaDataDictionary();
        itk::MetaDataDictionary::ConstIterator it = dictionary.Begin();
        while (it != dictionary.End())
        {
            if (MetaDataVectorStringType* metaData = dynamic_cast<MetaDataVectorStringType*>(it->second.GetPointer()))
            {
                const StringVectorType &values = metaData->GetMetaDataObjectValue();
                for (unsigned int i = 0; i < values.size(); i++)
                {
                    medData->addMetaData(it->first.c_str(), values[i].c_str());
                }
            }
            ++it;
        }
    }

    d->io->RemoveAllObservers();

    return true;
}


void itkDCMTKDataImageReader::setProgress(int value)
{
    emit progressed(value); qApp->processEvents();
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkDCMTKDataImageReader()
{
    return new itkDCMTKDataImageReader;
}

