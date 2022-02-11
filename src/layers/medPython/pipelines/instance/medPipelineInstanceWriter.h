#pragma once

#include <dtkCoreSupport/dtkAbstractDataWriter.h>

#include "medPipelineExport.h"

namespace med::pipeline
{

class PipelineInstanceWriterPrivate;

class MEDPIPELINE_EXPORT PipelineInstanceWriter : public dtkAbstractDataWriter
{
    Q_OBJECT

public:
    static bool registered();

    static dtkAbstractDataWriter* create();

    PipelineInstanceWriter();
    virtual ~PipelineInstanceWriter();

    virtual QString description() const;
    virtual QString identifier() const;
    virtual QString name() const;

    virtual QStringList handled() const;
    virtual QStringList supportedFileExtensions() const;

public slots:
    virtual bool canWrite(const QString& path);
    virtual bool write(const QString& path);

private:
    PipelineInstanceWriterPrivate* const d;

    bool writeToStream(QXmlStreamWriter& stream);
};

} // namespace med::pipeline
