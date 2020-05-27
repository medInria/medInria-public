#include "medContoursReader.h"

#include <medAbstractDataFactory.h>
#include <medContoursWriters.h>
#include <medTagContours.h>
#include <medMessageController.h>
#include <medMetaDataKeys.h>

#include <vtkDataSetReader.h>
#include <vtkDoubleArray.h>
#include <vtkErrorCode.h>
#include <vtkFieldData.h>
#include <vtkIdTypeArray.h>
#include <vtkMath.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkStringArray.h>

// ------------------------------------------------------------------------- //
// ------------------------ Contour Reader Private -------------------------- //
// ------------------------------------------------------------------------- //
class ContoursReaderPrivate
{
public:
    qint64 fileVersion;
    bool isBinary;
};


// ------------------------------------------------------------------------- //
// ---------------------------- Contour Reader ------------------------------ //
// ------------------------------------------------------------------------- //
medContoursReader::medContoursReader()
    : dtkAbstractDataReader(),
      d(new ContoursReaderPrivate())
{
    d->isBinary = true;
    d->fileVersion = medContoursWriter::NUMBER_OF_EXPORT_VERSIONS;
}

medContoursReader::~medContoursReader()
{
    delete d;
}

bool medContoursReader::registered()
{
    return medAbstractDataFactory::instance()->registerDataReaderType("ContoursReader",
                                                                      medContoursReader::s_handled(),
                                                                      createMedContoursReader);
}

QString medContoursReader::identifier() const
{
    return QString("ContoursReader");
}

QString medContoursReader::description() const
{
    return QString("Reader for Contours (Polygon ROI)");
}

QStringList medContoursReader::handled() const
{
    return s_handled();
}

QStringList medContoursReader::s_handled()
{
    return QStringList() << "medContours";
}

QStringList medContoursReader::supportedFileExtensions() const
{
    return QStringList() << ".ctrb";
}

bool medContoursReader::canRead(const QString& path)
{
    return path.endsWith(".ctrb");
}

bool medContoursReader::canRead(const QStringList& paths)
{
    if (paths.empty())
    {
        return false;
    }
    return canRead(qPrintable(paths.first()));
}

bool medContoursReader::readInformation(const QString& path)
{
    bool bRes = true;
    QFile file(path);
    if (path.endsWith(".ctrb"))
    {
        d->isBinary = true;
        // binary format, read only the header
        if (file.open(QFile::ReadOnly))
        {
            QDataStream in(&file);
            qint64 dummy = 0;
            in >> d->fileVersion >> dummy >> dummy >> dummy;
            file.close();

            if ((d->fileVersion < medContoursWriter::EXPORT_VERSION_1) ||
                (d->fileVersion >= medContoursWriter::NUMBER_OF_EXPORT_VERSIONS))
            {
                qDebug() << metaObject()->className() << ":: readInformation - unknown contours binary export version.";
                bRes = false;
            }

            this->setData(medAbstractDataFactory::instance()->create(QString("medContours")));
        }
    }
    else
    {
        d->isBinary = false;
        // Debug ASCII format
        vtkSmartPointer<vtkDataSetReader> reader = vtkSmartPointer<vtkDataSetReader>::New();
        reader->SetFileName(qPrintable(path));
        if (reader->OpenVTKFile() == 0)
        {
            qDebug()<< metaObject()->className()
                    << "::readInformation - failed to open underlying vtk file.";
            bRes = false;
        }

        reader->ReadHeader();
        QString header(reader->GetHeader());
        if (!header.contains(medContoursWriter::header))
        {
            qDebug()<< metaObject()->className()
                    << "::readInformation - file does not have the correct header.";
            bRes = false;
        }
        this->setData(medAbstractDataFactory::instance()->create(QString("medContours")));
    }

    return bRes;
}

bool medContoursReader::readInformation(const QStringList& paths)
{
    if (paths.empty())
    {
        return false;
    }
    return readInformation(qPrintable(paths.first()));
}

bool medContoursReader::read(const QString& path)
{
    setProgress(0);

    if (readInformation(path))
    {
        setProgress(10);

        if (d->isBinary)
        {
            return readBinaryFile(path);
        }
        else
        {
            // TODO ==> Read ASCII File to implement
            // TODO ==> operator >> to implement in medTagContours
            return false;//readASCIIFile(path);
        }
    }

    return false;
}

bool medContoursReader::readBinaryFile(QString path)
{
    bool bRes = true;
    QFile file(path);
    if (file.open(QFile::ReadOnly))
    {
        if (d->fileVersion == medContoursWriter::EXPORT_VERSION_1)
        {
            QDataStream in(&file);

            qint64 dummy = 0, startMeshPos = 0, endMeshPos = 0, fileEndPos;
            // skip header
            in >> dummy >> startMeshPos >> endMeshPos >> fileEndPos;

            // the VTK mesh part starts here. Determine size of the
            // mesh block
            qint64 meshLen = endMeshPos - startMeshPos;

            // hopefully this cast goes well...
            int len = static_cast<int>(meshLen);
            char* inputString = new char[len];
            if (in.readRawData(inputString, len) == -1)
            {
                qDebug() << metaObject()->className() << ":: read - failed to read binary mesh data.";
                delete[] inputString;
                file.close();
                bRes = false;
            }
            setProgress(25);

            vtkSmartPointer<vtkDataSetReader> reader = vtkSmartPointer<vtkDataSetReader>::New();
            reader->SetReadFromInputString(1);
            reader->SetBinaryInputString(inputString, len);

            vtkSmartPointer<vtkMetaDataSet> dataSet = retrieveMeshFromReader(reader);
            if (!dataSet)
            {
                qDebug() << metaObject()->className() << ":: readBinaryFile - failure to read Contours.";
                bRes = false;
            }
            setProgress(60);

            // retrieve contours with label
            qint32 *size = new qint32();
            in >> *size;
            QVector<medTagContours> contoursSet;

            for (int i=0; i<*size; i++)
            {
                medTagContours contours;
                in >> contours;
                contoursSet.append(contours);
            }

            setProgress(90);

            if (file.pos() != fileEndPos)
            {
                qDebug() << metaObject()->className() << ":: wrong pos :" <<
                            file.pos() << "instead of" << fileEndPos;
            }

            // set the mesh then the EP data
            this->data()->setData(dataSet);
            this->data()->setData(&contoursSet, 1);

            file.close();
        }
        else
        {
            qDebug() << metaObject()->className() << ":: unsupported binary file.";
            file.close();
            bRes = false;
        }
    }
    setProgress(100);
    return bRes;
}

vtkSmartPointer<vtkMetaDataSet> medContoursReader::retrieveMeshFromReader(vtkSmartPointer<vtkDataSetReader> reader)
{
    vtkSmartPointer<vtkMetaDataSet> dataSet = nullptr;
    try
    {
        reader->Update();
        vtkDataSet* readerOutput = nullptr;
        if (reader->IsFilePolyData() != 0)
        {
            dataSet = vtkSmartPointer<vtkMetaSurfaceMesh>::New();
            readerOutput = reader->GetPolyDataOutput();
        }
        else
        {
            // not a surface . Abort
            qDebug()<< metaObject()->className() << ":: labeled contours not a surface nor a volume.";
            return nullptr;
        }

        dataSet->SetDataSet(readerOutput);

        // Allocate the medAbstractData
        this->setData(medAbstractDataFactory::instance()->create(QString("medContours")));

        if (!extractMetaDataFromFieldData(dataSet))
        {
            // not a critical error
            qDebug() << metaObject()->className() <<":: no metadata found in mesh.";
        }

    }
    catch (vtkErrorCode::ErrorIds error)
    {
        qDebug()<< metaObject()->className()
                <<"::read - "<< vtkErrorCode::GetStringFromErrorCode(error);
        return nullptr;
    }
    catch (...)
    {
        qDebug() << metaObject()->className() << "::read - error reading mesh.";
        return nullptr;
    }

    return dataSet;
}

bool medContoursReader::extractMetaDataFromFieldData(vtkMetaDataSet* dataSet)
{
    bool foundMetaData = false;
    vtkFieldData* fieldData = dataSet->GetDataSet()->GetFieldData();
    vtkSmartPointer<vtkFieldData> newFieldData = vtkSmartPointer<vtkFieldData>::New();

    for (int i = 0; i < fieldData->GetNumberOfArrays(); i++)
    {
        QString arrayName = fieldData->GetArrayName(i);

        if (arrayName.startsWith(medContoursWriter::metaDataFieldPrefix))
        {
            foundMetaData = true;
            vtkStringArray* array = static_cast<vtkStringArray*>(fieldData->GetAbstractArray(i));
            QString metaDataKey = arrayName.remove(0, medContoursWriter::metaDataFieldPrefix.length());

            for (int j = 0; j < array->GetSize(); j++)
            {
                data()->addMetaData(metaDataKey, QString(array->GetValue(j)));
            }
        }
        else
        {
            newFieldData->AddArray(fieldData->GetAbstractArray(i));
        }
    }
    dataSet->GetDataSet()->SetFieldData(newFieldData);

    return foundMetaData;
}

bool medContoursReader::read(const QStringList& paths)
{
    bool bRes;
    if (paths.empty())
    {
        bRes = false;
    }
    else
    {
        bRes = read(qPrintable(paths.first()));
    }
    return bRes;
}

void medContoursReader::setProgress(int value)
{
    emit progressed(value);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createMedContoursReader() {
    return new medContoursReader;
}
