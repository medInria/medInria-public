#pragma once

#include "medAbstractPipeline.h"
#include "medPipelineExport.h"

namespace med::pipeline
{

class PipelineFactoryPrivate;

class MEDPIPELINE_EXPORT PipelineFactory
{
public:
    static void initialize();
    static PipelineFactory* instance();

    ~PipelineFactory();

    typedef PipelineDelegate* (*delegateCreatorFunction) (void*);
    typedef AbstractPipeline* (*pipelineCreatorFunction) (void*);

    template <typename TYPE>
    void registerDelegate();

    void registerDelegate(QString identifier, delegateCreatorFunction creator, void* argument = nullptr);

    template <typename TYPE>
    void registerPipeline();

    void registerPipeline(QString identifier, pipelineCreatorFunction creator, void* argument = nullptr);

    PipelineDelegate* createDelegate(QString identifier);
    AbstractPipeline* createPipeline(QString identifier, QString sourceCode = QString());

private:
    PipelineFactory();

    template <typename TYPE>
    static PipelineDelegate* createDelegate(void* argument);

    template <typename TYPE>
    static AbstractPipeline* createPipeline(void* argument);

    PipelineFactoryPrivate* const d;

    void throwNoCreatorException(QString identifier);
};

template <typename TYPE>
void PipelineFactory::registerDelegate()
{
    registerDelegate(TYPE::staticMetaObject.className(), createDelegate<TYPE>, nullptr);
}

template <typename TYPE>
void PipelineFactory::registerPipeline()
{
    registerPipeline(TYPE::staticMetaObject.className(), createPipeline<TYPE>, nullptr);
}

template <typename TYPE>
PipelineDelegate* PipelineFactory::createDelegate(void* argument)
{
    return new TYPE(argument);
}

template <typename TYPE>
AbstractPipeline* PipelineFactory::createPipeline(void* argument)
{
    return new TYPE(argument);
}

} // namespace med::pipeline

