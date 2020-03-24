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

    bool canRead(const QString& path) override;
    bool canRead(const QStringList& paths) override;

    bool readInformation(const QString& path) override;
    bool readInformation(const QStringList& paths) override;

    bool read(const QString& path) override;
    bool read(const QStringList& paths) override;

    void setProgress(int value) override;

protected:
    bool readBinaryFile(QString path);
    vtkSmartPointer<vtkMetaDataSet> retrieveMeshFromReader(vtkSmartPointer<vtkDataSetReader> reader);

private:
    bool extractMetaDataFromFieldData(vtkMetaDataSet *dataSet);

    ContoursReaderPrivate* d;
};

dtkAbstractDataReader* createMedContoursReader();

