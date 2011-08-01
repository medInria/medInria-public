#ifndef MEDCOMPOSITEDATASETSWRITER_H
#define MEDCOMPOSITEDATASETSWRITER_H

#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <medCompositeDataSetsPluginExport.h>
#include <medCompositeDataSetsBase.h>

class MEDCOMPOSITEDATASETSPLUGIN_EXPORT medCompositeDataSetsWriter : public dtkAbstractDataWriter {

    Q_OBJECT

public:

    medCompositeDataSetsWriter();
    virtual ~medCompositeDataSetsWriter();

    virtual QString description() const { return "Writer for composite data sets"; }

    virtual QStringList handled() const { return MedInria::medCompositeDataSetsBase::handled(); }

    static bool registered() {
        return dtkAbstractDataFactory::instance()->registerDataWriterType("medCompositeDataSetsWriter",MedInria::medCompositeDataSetsBase::handled(),create);
    }

    static dtkAbstractDataWriter* create() { return new medCompositeDataSetsWriter(); }

public slots:

    bool write(const QString& path);
    bool write(const QStringList& files);
    bool canWrite(const QString& path);
    bool canWrite(const QStringList& files);

    void setProgress(int value);

private:

    MedInria::medCompositeDataSetsBase* writer;
};

#endif
