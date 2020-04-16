/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
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

        void SetDataImageReader(dtkAbstractDataReader* reader) { m_Reader = reader; }

    protected:
        DCMTKDataImageReaderCommand() { m_Reader = nullptr; }
        virtual ~DCMTKDataImageReaderCommand() {}

    private:
        dtkAbstractDataReader* m_Reader;
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

QList<itk::DCMTKImageIO::Pointer> * itkDCMTKDataImageReaderPrivate::ioPointers = NULL;
QList<QThread*> * itkDCMTKDataImageReaderPrivate::ioThreads = NULL;
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

itkDCMTKDataImageReader::itkDCMTKDataImageReader() : dtkAbstractDataReader(), d(new itkDCMTKDataImageReaderPrivate) { }


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
    for (int i = 0; i < paths.size(); i++)
        if (!d->io->CanReadFile(paths[i].toUtf8()))
            return false;
    return true;
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

        if (d->io->GetPixelType() == itk::ImageIOBase::SCALAR)
        {
            switch (d->io->GetComponentType())
            {
            case itk::ImageIOBase::UCHAR:
                imagetypestring << "UChar";
                break;
            case itk::ImageIOBase::CHAR:
                imagetypestring << "Char";
                break;
            case itk::ImageIOBase::USHORT:
                imagetypestring << "UShort";
                break;
            case itk::ImageIOBase::SHORT:
                imagetypestring << "Short";
                break;
            case itk::ImageIOBase::UINT:
                imagetypestring << "UInt";
                break;
            case itk::ImageIOBase::INT:
                imagetypestring << "Int";
                break;
            case itk::ImageIOBase::ULONG:
                imagetypestring << "ULong";
                break;
            case itk::ImageIOBase::LONG:
                imagetypestring << "Long";
                break;
            case itk::ImageIOBase::FLOAT:
                imagetypestring << "Float";
                break;
            case itk::ImageIOBase::DOUBLE:
                imagetypestring << "Double";
                break;
            default:
                qDebug() << "Unrecognized component type: " << d->io->GetComponentType();
                return false;
            }

            imagetypestring << d->io->GetNumberOfDimensions();
            medData = medAbstractDataFactory::instance()->create(imagetypestring.str().c_str());
            if (medData)
                this->setData(medData);
        }
        else if (d->io->GetPixelType() == itk::ImageIOBase::RGB)
        {

            switch (d->io->GetComponentType())
            {
            case itk::ImageIOBase::UCHAR:
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
        // PATIENT
        //PatientId
        medData->setMetaData(medMetaDataKeys::PatientName.key(), QString::fromLatin1(d->io->GetPatientName().c_str()));
        medData->setMetaData(medMetaDataKeys::Age.key(),         d->io->GetPatientAge().c_str());
        medData->setMetaData(medMetaDataKeys::BirthDate.key(),   d->io->GetPatientDOB().c_str());
        medData->setMetaData(medMetaDataKeys::Gender.key(),      d->io->GetPatientSex().c_str());
        medData->setMetaData(medMetaDataKeys::Description.key(), QString::fromLatin1(d->io->GetScanOptions().c_str()));

        // STUDY
        //StudyId
        medData->setMetaData(medMetaDataKeys::StudyID.key(),          QString::fromLatin1(d->io->GetStudyID().c_str()));
        medData->setMetaData(medMetaDataKeys::StudyInstanceUID.key(), d->io->GetStudyInstanceUID().c_str());
        medData->setMetaData(medMetaDataKeys::StudyDescription.key(), QString::fromLatin1(d->io->GetStudyDescription().c_str()));
        medData->setMetaData(medMetaDataKeys::Institution.key(),      QString::fromLatin1(d->io->GetInstitution().c_str()));
        medData->setMetaData(medMetaDataKeys::Referee.key(),          QString::fromLatin1(d->io->GetReferringPhysicianName().c_str()));
        //StudyDate
        //StudyTime

        // SERIES
        //SeriesID
        medData->setMetaData(medMetaDataKeys::SeriesInstanceUID.key(), d->io->GetSeriesInstanceUID().c_str());
        medData->setMetaData(medMetaDataKeys::SeriesNumber.key(),      d->io->GetSeriesNumber().c_str());
        medData->setMetaData(medMetaDataKeys::Modality.key(),          d->io->GetModality().c_str());
        medData->setMetaData(medMetaDataKeys::Performer.key(),         QString::fromLatin1(d->io->GetPerformingPhysicianName().c_str()));
        medData->setMetaData(medMetaDataKeys::Report.key(), "");
        medData->setMetaData(medMetaDataKeys::Protocol.key(),          QString::fromLatin1(d->io->GetProtocolName().c_str()));
        medData->setMetaData(medMetaDataKeys::SeriesDescription.key(), QString::fromLatin1(d->io->GetSeriesDescription().c_str()));
        //SeriesDate
        //SeriesTime
        //SeriesThumbnail

        // IMAGE
        //SOPInstanceUID
        medData->setMetaData(medMetaDataKeys::Columns.key(),         d->io->GetColumns().c_str());
        medData->setMetaData(medMetaDataKeys::Rows.key(),            d->io->GetRows().c_str());
        //Dimensions
        //NumberOfDimensions
        medData->setMetaData(medMetaDataKeys::Orientation.key(),     d->io->GetOrientation().c_str());

        // Add Origin
        QString origin = "";
        for (unsigned int i = 0; i < d->io->GetNumberOfDimensions(); ++i)
        {
            origin += QString::number(d->io->GetOrigin(i)) + QString(" ");
        }
        medData->setMetaData(medMetaDataKeys::Origin.key(), origin.trimmed());

        medData->setMetaData(medMetaDataKeys::SliceThickness.key(),  d->io->GetSliceThickness().c_str());
        //ImportationDate
        medData->setMetaData(medMetaDataKeys::AcquisitionDate.key(), d->io->GetAcquisitionDate().c_str());
        medData->setMetaData(medMetaDataKeys::AcquisitionTime.key(), d->io->GetAcquisitionTime().c_str());
        medData->setMetaData(medMetaDataKeys::Comments.key(),        QString::fromLatin1(d->io->GetAcquisitionComments().c_str()));

        QStringList filePaths;
        for (unsigned int i = 0; i < d->io->GetOrderedFileNames().size(); i++)
        {
            if (d->io->GetOrderedFileNames()[i] != "")
            {
                filePaths << QString::fromLatin1(d->io->GetOrderedFileNames()[i].c_str());
            }
        }

        medData->addMetaData(medMetaDataKeys::FilePaths.key(), filePaths);

        medData->setMetaData(medMetaDataKeys::Status.key(),          d->io->GetPatientStatus().c_str());
        medData->setMetaData(medMetaDataKeys::SequenceName.key(),    QString::fromLatin1(d->io->GetSequenceName().c_str()));
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

        // MR Image
        medData->setMetaData(medMetaDataKeys::FlipAngle.key(),      d->io->GetFlipAngle().c_str());
        medData->setMetaData(medMetaDataKeys::EchoTime.key(),       d->io->GetEchoTime().c_str());
        medData->setMetaData(medMetaDataKeys::RepetitionTime.key(), d->io->GetRepetitionTime().c_str());
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

