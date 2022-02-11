#include "medPipelineInstanceWriter.h"

#include <medAbstractDataFactory.h>

#include "medPipelineInstance.h"
#include "medPythonPipelineInstance.h"

namespace med::pipeline
{

class PipelineInstanceWriterPrivate
{
public:
    static const QStringList handled;
};

const QStringList PipelineInstanceWriterPrivate::handled = QStringList()
        << PipelineInstance::staticIdentifier() << PythonPipelineInstance::staticIdentifier();

bool PipelineInstanceWriter::registered()
{
    return medAbstractDataFactory::instance()->registerDataWriterType(staticMetaObject.className(),
                                                                      PipelineInstanceWriterPrivate::handled, create);
}

dtkAbstractDataWriter* PipelineInstanceWriter::create()
{
    return new PipelineInstanceWriter();
}

PipelineInstanceWriter::PipelineInstanceWriter() : d(new PipelineInstanceWriterPrivate())
{
}

PipelineInstanceWriter::~PipelineInstanceWriter()
{
    delete d;
}

QString PipelineInstanceWriter::description() const
{
    return tr("Write a pipeline instance");
}

QString PipelineInstanceWriter::identifier() const
{
    return metaObject()->className();
}

QString PipelineInstanceWriter::name() const
{
    return tr("Pipeline instance writer");
}

QStringList PipelineInstanceWriter::handled() const
{
    return d->handled;
}

QStringList PipelineInstanceWriter::supportedFileExtensions() const
{
    return QStringList() << ".msc";
}

bool PipelineInstanceWriter::canWrite(const QString& path)
{
    return dynamic_cast<PipelineInstance*>(data());
}

bool PipelineInstanceWriter::write(const QString& path)
{
    QFile file(QString::fromUtf8(path.toStdString().c_str()));

    if(!file.open(QIODevice::WriteOnly))
    {
        return false;
    }
    else
    {
        QXmlStreamWriter stream(&file);
        stream.setAutoFormatting(true);
        return writeToStream(stream);
    }
}

bool PipelineInstanceWriter::writeToStream(QXmlStreamWriter& stream)
{
    try
    {
        stream.writeStartDocument();

        PythonPipelineInstance* pythonInstance = dynamic_cast<PythonPipelineInstance*>(data());
        PipelineInstance* instance = pythonInstance;

        if (pythonInstance != nullptr)
        {
            /* The module name and source code is copied and stored in the file. This means that
         * if the pipeline code is changed, the existing pipelines are not affected.
         */
            stream.writeStartElement("pythonPipeline");
            stream.writeAttribute("identifier", pythonInstance->getPipeline().dynamicCast<PythonPipeline>()->getClassName());
            stream.writeAttribute("referenceModule", pythonInstance->getReferenceModuleName());
            stream.writeTextElement("sourceCode", pythonInstance->getSourceCode());
        }
        else
        {
            instance = static_cast<PipelineInstance*>(data());
            stream.writeStartElement("cppPipeline");
            stream.writeAttribute("identifier", instance->getPipeline()->metaObject()->className());
        }

        instance->write(stream);
        stream.writeEndElement();
        stream.writeEndDocument();

        return true;
    }
    catch (std::exception& e)
    {
        PipelineWorkspace::instance()->displayException("Error while writing pipeline", e);
        return false;
    }
}

} // namespace med::pipeline
