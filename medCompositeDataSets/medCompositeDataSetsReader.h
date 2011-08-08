#ifndef MEDCOMPOSITEDATASETSREADER_H
#define MEDCOMPOSITEDATASETSREADER_H

#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <medCompositeDataSetsPluginExport.h>
#include <medCompositeDataSetsBase.h>

#include <dirTools.h>

class MEDCOMPOSITEDATASETSPLUGIN_EXPORT medCompositeDataSetsReader: public dtkAbstractDataReader {
    Q_OBJECT

public:

    medCompositeDataSetsReader(): desc(0) { }

    virtual ~medCompositeDataSetsReader() {
        if (!desc)
            delete desc;
        desc = 0;
    }

    virtual QString description() const { return "Reader for composite data sets";              }
    virtual QStringList handled() const { return MedInria::medCompositeDataSetsBase::handled(); }
    
//    bool enabled() const;
//    void  enable();
//    void disable();
    
//    dtkAbstractData *data();
    
//    virtual void setData(dtkAbstractData *data);

    void cleanup() { RemoveDirectory(QDir(outdir)); }

    static bool initialize() {
        return dtkAbstractDataFactory::instance()->registerDataReaderType("medCompositeDataSetsReader",MedInria::medCompositeDataSetsBase::handled(),create);
    }

    static dtkAbstractDataReader* create() { return new medCompositeDataSetsReader(); }

public slots:

    virtual bool canRead(const QString& file);

    virtual bool read(const QString& file);
    virtual void readInformation(const QString& path);
    virtual void setProgress(int value);

private:

    bool       is_zip_file;
    bool       in_error;
    QString    outdir;
    QIODevice* desc;

    MedInria::medCompositeDataSetsBase* reader;
};

#endif
