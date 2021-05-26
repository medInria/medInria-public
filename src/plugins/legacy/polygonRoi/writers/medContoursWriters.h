#pragma once

#include <dtkCoreSupport/dtkAbstractDataWriter.h>

#include <medContours.h>
#include <vtkMetaDataSet.h>


/**
 * Writes Contours onto disk with the custom .ctr/.bctr file extension.
 * .bctr  is a binary file composed of 3 main parts:
 * 1 - A header of size 4 * 8 bytes
 * 2 - A VTK mesh in binary
 * 3 - a set of LabelDataSet readable as QByteStream
 *
 * qint64 version = EXPORT
 * qint64 meshPos = 0;
 * qint64 labelPos = 0;
 * qint64 endFilePos = 0;
 */
class  medContoursWriter : public dtkAbstractDataWriter
{
    Q_OBJECT

public:
    enum ExportVersion
    {
        EXPORT_VERSION_1 = 1,
        NUMBER_OF_EXPORT_VERSIONS
    };

    static const QString metaDataFieldPrefix;
    static const QString header;

    medContoursWriter();
    virtual ~medContoursWriter();

    virtual QStringList handled() const;
    static  QStringList s_handled();

    virtual QString description() const;
    virtual QString identifier() const;

    virtual QStringList supportedFileExtensions() const;

    static bool registered();

public slots:
    bool write    (const QString& path);
    bool canWrite (const QString& path);

private:
    void addMetaDataAsFieldData(vtkMetaDataSet* dataSet);
    void clearMetaDataFieldData(vtkMetaDataSet* dataSet);
    bool writeASCIIFile(medContours* ctr, const QString& path, vtkMetaDataSet* mesh);
    bool writeBinaryFile(const QString &path, medContours *ctr, vtkMetaDataSet *mesh);
};

dtkAbstractDataWriter* createMedContoursWriter();

