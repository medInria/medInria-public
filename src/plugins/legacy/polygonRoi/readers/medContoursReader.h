#pragma once

#include <polygonRoiPluginExport.h>

#include <dtkCoreSupport/dtkAbstractDataReader.h>

#include <medAbstractData.h>
#include <vtkDataSetReader.h>
#include <vtkMetaDataSet.h>
#include <vtkSmartPointer.h>


class ContoursReaderPrivate;
/**
 * Reads internal EP map files. Those file have the *.map file
 * extension.
 */
class medContoursReader: public dtkAbstractDataReader
{
    Q_OBJECT

public:
    medContoursReader();
    virtual ~medContoursReader();

    static bool registered();
    virtual QString identifier()  const;
    virtual QString description() const;
    virtual QStringList handled() const;
    static QStringList s_handled();

    virtual QStringList supportedFileExtensions() const;

public slots:

    virtual bool canRead(const QString& path);
    virtual bool canRead(const QStringList& paths);

    virtual bool readInformation(const QString& path);
    virtual bool readInformation(const QStringList& paths);

    virtual bool read(const QString& path);
    virtual bool read(const QStringList& paths);

    virtual void setProgress(int value);

protected:
    bool readBinaryFile(QString path);
    bool readASCIIFile(QString path);
    vtkSmartPointer<vtkMetaDataSet> retrieveMeshFromReader(vtkSmartPointer<vtkDataSetReader> reader);

private:
    bool extractMetaDataFromFieldData(vtkMetaDataSet *dataSet);

    ContoursReaderPrivate* d;
};

dtkAbstractDataReader* createMedContoursReader();

