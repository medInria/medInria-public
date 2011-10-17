#ifndef MEDCOMPOSITEDATASETSWRITER_H
#define MEDCOMPOSITEDATASETSWRITER_H

#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <medCompositeDataSetsPluginExport.h>
#include <medCompositeDataSetsBase.h>

class MEDCOMPOSITEDATASETSPLUGIN_EXPORT medCompositeDataSetsWriter: public dtkAbstractDataWriter {
    Q_OBJECT

public:

    medCompositeDataSetsWriter(): writer(0) { }
    virtual ~medCompositeDataSetsWriter() { }

    virtual QString description() const { return "Writer for composite data sets"; }

    virtual QStringList handled() const { return MedInria::medCompositeDataSetsBase::handled(); }

    static dtkAbstractDataWriter* create() { return new medCompositeDataSetsWriter(); }

    static bool initialize() {
        return dtkAbstractDataFactory::instance()->registerDataWriterType("medCompositeDataSetsWriter",MedInria::medCompositeDataSetsBase::handled(),create);
    }

public slots:

    bool write(const QString& path);
    //bool write(const QStringList& files);
    bool canWrite(const QString& path) { return true; }
    //bool canWrite(const QStringList& files);

    //void setProgress(int value);

private:

    MedInria::medCompositeDataSetsBase* writer;
};

#endif
