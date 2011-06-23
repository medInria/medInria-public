#ifndef MEDCOMPOSITEDATASETSREADER_H
#define MEDCOMPOSITEDATASETSREADER_H

#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkZip/dtkZip.h>
#include <dtkZip/dtkZipFile.h>

#include <medCompositeDataSetsPluginExport.h>
#include <medCompositeDataSetsBase.h>

class MEDCOMPOSITEDATASETSPLUGIN_EXPORT medCompositeDataSetsReader: public dtkAbstractDataReader
{
    Q_OBJECT

public:

    medCompositeDataSetsReader(): desc(0) { };

    virtual ~medCompositeDataSetsReader() {
        if (!desc)
            delete desc;
        desc = 0;
    }

    virtual QString description() const;
    virtual QStringList handled() const;
    
//    bool enabled() const;
//    void  enable();
//    void disable();
    
    dtkAbstractData *data();
    
    virtual void setData(dtkAbstractData *data);

    void cleanup() {
    }

public slots:

    virtual bool canRead(const QString& file);
    virtual bool read(const QString& file);
    virtual void readInformation(const QString& path);
    virtual void setProgress(int value);

private:

    QString    dname;
    QIODevice* desc;

    MedInria::medCompositeDataSetsBase* reader;
};

dtkAbstractData *createMedCompositeDataSetsReader(void);

#endif
