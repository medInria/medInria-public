#include "medPipelineFactory.h"

#include <stdexcept>

#include "medPythonPipelineInstance.h"

namespace med::pipeline
{

class PipelineFactoryPrivate
{
public:
    static QScopedPointer<PipelineFactory> instance;

    QHash<QString, PipelineFactory::delegateCreatorFunction> delegateCreators;
    QHash<QString, PipelineFactory::pipelineCreatorFunction> pipelineCreators;
    QHash<QString, void*> delegateCreatorArguments;
    QHash<QString, void*> pipelineCreatorArguments;
};

QScopedPointer<PipelineFactory> PipelineFactoryPrivate::instance;

void PipelineFactory::initialize()
{
    if (PipelineFactoryPrivate::instance.isNull())
    {
        PipelineFactoryPrivate::instance.reset(new PipelineFactory);
    }
}

PipelineFactory* PipelineFactory::instance()
{
    return PipelineFactoryPrivate::instance.data();
}

PipelineFactory::PipelineFactory() :
    d(new PipelineFactoryPrivate())
{
}

PipelineFactory::~PipelineFactory()
{
    delete d;
}

void PipelineFactory::registerDelegate(QString identifier, delegateCreatorFunction creator, void* argument)
{
    d->delegateCreators[identifier] = creator;
    d->delegateCreatorArguments[identifier] = argument;
}

void PipelineFactory::registerPipeline(QString identifier, pipelineCreatorFunction creator, void* argument)
{
    d->pipelineCreators[identifier] = creator;
    d->pipelineCreatorArguments[identifier] = argument;
}

PipelineDelegate* PipelineFactory::createDelegate(QString identifier)
{
    delegateCreatorFunction creator = d->delegateCreators[identifier];

    if (creator == nullptr)
    {
        throwNoCreatorException(identifier);
    }

    return creator(d->delegateCreatorArguments[identifier]);
}

AbstractPipeline* PipelineFactory::createPipeline(QString identifier, QString sourceCode)
{
    AbstractPipeline* pipeline = nullptr;

    if (sourceCode.isNull())
    {
        pipelineCreatorFunction creator = d->pipelineCreators[identifier];

        if (creator == nullptr)
        {
            throwNoCreatorException(identifier);
        }

        pipeline = creator(d->pipelineCreatorArguments[identifier]);
    }
    else
    {
        python::Object sourceObjects = python::runSourceCode(sourceCode);
        python::Object pythonPipeline = sourceObjects[identifier]();
        pipeline = pythonPipeline.cast<AbstractPipeline>();
    }

    return pipeline;
}

void PipelineFactory::throwNoCreatorException(QString identifier)
{
    QString message = QString("medPipelineFactory: No creator registered for ") + identifier;
    throw std::runtime_error(message.toStdString().c_str());
}

} // namespace med::pipeline
