#pragma once

#include <dtkCoreSupport/dtkAbstractDataReader.h>

#include "medPipelineExport.h"

namespace med::pipeline
{

class PipelineInstanceReaderPrivate;

class MEDPIPELINE_EXPORT PipelineInstanceReader : public dtkAbstractDataReader
{
    Q_OBJECT

public:
    static bool registered();

    static dtkAbstractDataReader* create();

    PipelineInstanceReader();
    virtual ~PipelineInstanceReader();

    virtual QString description() const;
    virtual QString identifier() const;
    virtual QString name() const;

    virtual QStringList handled() const;

public slots:
    virtual bool canRead(const QString& path);
    virtual bool readInformation(const QString& path);
    virtual bool read(const QString& path);

private:
    PipelineInstanceReaderPrivate* const d;

    bool readFile(const QString& path, bool readOnlyMetaData = false);
    bool readStream(QXmlStreamReader& stream, bool readOnlyMetaData = false);
};

} // namespace med::pipeline

Q_DECLARE_METATYPE(med::pipeline::PipelineInstanceReader)


