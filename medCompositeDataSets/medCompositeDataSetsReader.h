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

public slots:

    virtual bool canRead(const QString& file);
//    virtual bool canRead(const QStringList& files);
    
    virtual bool read(const QString& file);
//    virtual bool read(const QStringList& files);
    
    virtual void readInformation(const QString& path);
//    virtual void readInformation(const QStringList& paths);
    
    virtual void setProgress(int value);

private:

    bool is_valid_desc();

    bool        is_zip_archive;
    dtkZip      zipfile;
    QIODevice*  desc;
    dtkZipFile* zipdesc;
};

dtkAbstractData *createMedCompositeDataSetsReader(void);

#endif
