#include "medPipelineInstanceReader.h"

#include <medAbstractDataFactory.h>
#include <medMetaDataKeys.h>

#include "medPipelineChooserToolBox.h"
#include "medPipelineFactory.h"
#include "medPythonPipelineInstance.h"

namespace med::pipeline
{

class PipelineInstanceReaderPrivate
{
public:
    static const QStringList handled;
};

const QStringList PipelineInstanceReaderPrivate::handled = QStringList()
        << PipelineInstance::staticIdentifier() << PythonPipelineInstance::staticIdentifier();

bool PipelineInstanceReader::registered()
{
    return medAbstractDataFactory::instance()->registerDataReaderType(staticMetaObject.className(),
                                                                      PipelineInstanceReaderPrivate::handled, create);
}

dtkAbstractDataReader* PipelineInstanceReader::create()
{
    return new PipelineInstanceReader();
}

PipelineInstanceReader::PipelineInstanceReader() : d(new PipelineInstanceReaderPrivate())
{
}

PipelineInstanceReader::~PipelineInstanceReader()
{
    delete d;
}

QString PipelineInstanceReader::description() const
{
    return tr("Read a pipeline instance");
}

QString PipelineInstanceReader::identifier() const
{
    return metaObject()->className();
}

QString PipelineInstanceReader::name() const
{
    return tr("Pipeline instance reader");
}

QStringList PipelineInstanceReader::handled() const
{
    return d->handled;
}

bool PipelineInstanceReader::canRead(const QString& path)
{
    if (QFileInfo(path).suffix() == "msc")
    {
        QFile file(path);

        if(file.open(QIODevice::ReadOnly))
        {
            QXmlStreamReader stream(&file);
            stream.readNextStartElement();
            QString pipelineType = stream.name().toString();
            return (pipelineType == "cppPipeline") || (pipelineType == "pythonPipeline");
            return true;
        }
    }
    return false;
}

bool PipelineInstanceReader::readInformation(const QString& path)
{
    return readFile(path, true);
}

bool PipelineInstanceReader::read(const QString& path)
{
    return readFile(path);
}

bool PipelineInstanceReader::readFile(const QString& path, bool readOnlyMetaData)
{
    // Pipelines should only be opened when the pipeline chooser toolbox is active
    if (PipelineWorkspace::instance() && PipelineWorkspace::instance()->getToolBox<PipelineChooserToolBox>()->isVisible())
    {
        QFile file(path);

        if(!file.open(QIODevice::ReadOnly))
        {
            return false;
        }
        else
        {
            QXmlStreamReader stream(&file);
            bool success = readStream(stream, readOnlyMetaData);
            file.close();

            if (success)
            {
                return true;
            }
            else
            {
                delete data();
                setData(0);
                return false;
            }
        }
    }
    else
    {
        qDebug()<< metaObject()->className()
                << "::readFile - please open your pipeline in Pipelines workspace";
    }
    return false;
}

bool PipelineInstanceReader::readStream(QXmlStreamReader& stream, bool readOnlyMetaData)
{
    stream.readNextStartElement();

    PipelineInstance* instance = nullptr;

    try
    {
        QString pipelineIdentifier = stream.attributes().value("identifier").toString();

        if (stream.name() == "pythonPipeline")
        {
            QString referenceModuleName = stream.attributes().value("referenceModule").toString();
            stream.readNextStartElement();

            if (stream.name() == "sourceCode")
            {
                QString sourceCode = stream.readElementText();
                PythonPipeline* pipeline = dynamic_cast<PythonPipeline*>(PipelineFactory::instance()->createPipeline(pipelineIdentifier, sourceCode));
                instance = new PythonPipelineInstance(pipeline, referenceModuleName, sourceCode);
            }
            else
            {
                throwXMLException(stream);
            }
        }
        else
        {
            AbstractPipeline* pipeline = PipelineFactory::instance()->createPipeline(pipelineIdentifier);
            instance = new PipelineInstance(pipeline);
        }

        setData(instance);

        if (!instance->read(stream, readOnlyMetaData))
        {
            throwXMLException(stream);
        }
    }
    catch (std::exception& e)
    {
        PipelineWorkspace::instance()->displayException("Error while loading pipeline", e);
        return false;
    }

    return true;
}

} // namespace med::pipeline
