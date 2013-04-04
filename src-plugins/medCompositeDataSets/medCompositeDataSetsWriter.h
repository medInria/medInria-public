#pragma once

#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <medCompositeDataSetsPluginExport.h>
#include <medCompositeDataSetsBase.h>

class MEDCOMPOSITEDATASETSPLUGIN_EXPORT medCompositeDataSetsWriter: public dtkAbstractDataWriter {
    Q_OBJECT

public:

    medCompositeDataSetsWriter(): writer(0) { }
    virtual ~medCompositeDataSetsWriter() { }

    virtual QString description() const { return tr("Writer for composite data sets"); }
    virtual QString identifier()  const { return "medCompositeDataSetsWriter";         }

    virtual QStringList handled() const { return MedInria::medCompositeDataSetsBase::handled(); }

    static dtkAbstractDataWriter* create() { return new medCompositeDataSetsWriter(); }

    static bool initialize() {
        return dtkAbstractDataFactory::instance()->registerDataWriterType("medCompositeDataSetsWriter",MedInria::medCompositeDataSetsBase::handled(),create);
    }

public slots:

    bool write(const QString& path);
    bool canWrite(const QString& path) { return true; }

    //void setProgress(int value);

private:

    MedInria::medCompositeDataSetsBase* writer;
};


