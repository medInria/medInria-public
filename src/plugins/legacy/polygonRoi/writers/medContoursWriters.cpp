#include "medContoursWriters.h"


#include <dtkLog/dtkLog.h>

#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <medContours.h>
#include <medMetaDataKeys.h>

#include <vtkDataWriter.h>
#include <vtkErrorCode.h>
#include <vtkFieldData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkXMLPolyDataWriter.h>


const QString medContoursWriter::metaDataFieldPrefix = "medMetaData::";
const QString medContoursWriter::header = "medInria contours";

medContoursWriter::medContoursWriter()
    : medAbstractDataWriter()
{
}

medContoursWriter::~medContoursWriter()
{
}

QString medContoursWriter::description() const
{
    return tr("Writer for Contours (Polygon ROI)");
}

QString medContoursWriter::identifier() const
{
    return QString("medContoursWriter");
}

QStringList medContoursWriter::supportedFileExtensions() const
{
    return QStringList() << ".ctrb";
}

bool medContoursWriter::registered()
{
    return medAbstractDataFactory::instance()->registerDataWriterType(QString("medContoursWriter"),
                                                                      medContoursWriter::s_handled(), createMedContoursWriter);
}

QStringList medContoursWriter::handled() const
{
    return s_handled();
}

QStringList medContoursWriter::s_handled()
{
    return QStringList() << QString("medContours");
}

bool medContoursWriter::canWrite(const QString& path)
{
    if (!this->data())
    {
        return false;
    }
    return dynamic_cast<vtkMetaDataSet*>((vtkObject*)(this->data()->data()));
}

bool medContoursWriter::write(const QString& path)
{
    bool bRes;
    if (!this->data())
    {
        return false;
    }

    medContours* ctr = dynamic_cast<medContours*>(this->data());
    if(!ctr)
    {
        return false;
    }

    vtkMetaDataSet* mesh = dynamic_cast<vtkMetaDataSet*>((vtkObject*)(this->data()->data()));
    if (!mesh)
    {
        return false;
    }

    if (mesh->GetType() != vtkMetaDataSet::VTK_META_SURFACE_MESH)
    {
        qDebug()<< metaObject()->className() << "::write - dataset is not a mesh";
        return false;
    }

    if ( path.endsWith(".ctr") )
    {
        // TODO ==> operator << to implement in medTagContours
        bRes = false; // writeASCIIFile(ctr, path, mesh);
    }
    else if ( path.endsWith(".ctrb") )
    {
        bRes = writeBinaryFile(path, ctr, mesh);
    }
    else
    {
        qDebug()<<metaObject()->className()<<":: write - xtension not supported.";
        bRes = false;
    }
    return bRes;

}

bool medContoursWriter::writeBinaryFile(const QString& path, medContours* ctr, vtkMetaDataSet* mesh)
{
    try
    {
        QFile file(path);
        if (file.open(QFile::WriteOnly))
        {
            QDataStream out(&file);

            qint64 version = EXPORT_VERSION_1;
            qint64 meshPos = 0;
            qint64 contoursPos = 0;
            qint64 endFilePos = 0;

            out << version << meshPos << contoursPos << endFilePos;
            meshPos = file.pos();

            addMetaDataAsFieldData(mesh);

            vtkSmartPointer<vtkDataWriter> writer = nullptr;
            writer = vtkSmartPointer<vtkPolyDataWriter>::New();
            writer->SetInputData(dynamic_cast<vtkPolyData*>(mesh->GetDataSet()));
            writer->SetFileTypeToBinary();
            writer->SetWriteToOutputString(1);
            writer->SetHeader(qPrintable(header));
            writer->Write();

            // write to output file
            int writeStatus = out.writeRawData(reinterpret_cast<const char*>(writer->GetBinaryOutputString()), writer->GetOutputStringLength());
            if (writeStatus == -1)
            {
                qDebug() << metaObject()->className() << ":: write - unable to write mesh as binary data.";
                file.close();
                return false;
            }

            clearMetaDataFieldData(mesh);
            contoursPos = file.pos();

            QVector<medTagContours> &contours = ctr->getTagContoursSet();
            out<<contours.size();
            for (medTagContours& ctr : contours)
                out<<ctr;
            endFilePos = file.pos();

            // rewrite file header
            file.seek(0);
            out << version << meshPos << contoursPos << endFilePos;

            file.close();
        }
    }
    catch(...)
    {
        qDebug()<< metaObject()->className() << "::write - error writing to "<<path;
        clearMetaDataFieldData(mesh);
        return false;
    }
    return true;
}

bool medContoursWriter::writeASCIIFile(medContours* ctr, const QString& path, vtkMetaDataSet* mesh)
{
    try
    {
        setlocale(LC_NUMERIC, "C");
        QLocale::setDefault(QLocale("C"));
        addMetaDataAsFieldData(mesh);
        mesh->Write(path.toLocal8Bit().constData());
        clearMetaDataFieldData(mesh);
        QFile file(path);

        if (file.open(QFile::Append))
        {
            QTextStream out(&file);
            QVector<medTagContours> contoursSet = ctr->getTagContoursSet();
            out<<"number of labelList\n"<<contoursSet.size();
            for (medTagContours& contours : contoursSet)
            {
                out<<contours;
            }
            file.close();
        }
    }
    catch(...)
    {
        qDebug()<< metaObject()->className() << "::write - error writing to "<<path;
        clearMetaDataFieldData(mesh);
        return false;
    }
    return true;

}

void medContoursWriter::addMetaDataAsFieldData(vtkMetaDataSet* dataSet)
{
    for (QString &key : data()->metaDataList())
    {
        vtkSmartPointer<vtkStringArray> metaDataArray = vtkSmartPointer<vtkStringArray>::New();
        QString arrayName = QString(metaDataFieldPrefix) + key;
        metaDataArray->SetName(arrayName.toStdString().c_str());

        foreach (QString value, data()->metaDataValues(key))
        {
            metaDataArray->InsertNextValue(value.toStdString().c_str());
        }

        dataSet->GetDataSet()->GetFieldData()->AddArray(metaDataArray);
    }
}

void medContoursWriter::clearMetaDataFieldData(vtkMetaDataSet* dataSet)
{
    vtkFieldData* fieldData = dataSet->GetDataSet()->GetFieldData();
    vtkSmartPointer<vtkFieldData> newFieldData = vtkSmartPointer<vtkFieldData>::New();

    for (int i = 0; i < fieldData->GetNumberOfArrays(); i++)
    {
        QString arrayName = fieldData->GetArrayName(i);

        if (!arrayName.startsWith(metaDataFieldPrefix))
        {
            newFieldData->AddArray(fieldData->GetAbstractArray(i));
        }
    }

    dataSet->GetDataSet()->SetFieldData(newFieldData);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////
dtkAbstractDataWriter* createMedContoursWriter()
{
    return new medContoursWriter;
}

