#include "medPipelineDelegate.h"

#include "medPipelineFactory.h"
#include "medPipelineValuesToolBox.h"

namespace med::pipeline
{

void PipelineDelegate::registerDelegate()
{
    PipelineFactory::instance()->registerDelegate<PipelineDelegate>();
}

PipelineDelegate::PipelineDelegate(void* argument)
{
}

PipelineDelegate::~PipelineDelegate()
{
}

QString PipelineDelegate::getClassName()
{
    return metaObject()->className();
}

void PipelineDelegate::startStep(QString name, PipelineWorkingState* state)
{
    qDebug() << "Calling start: " << name;
    name.prepend("start");
    name.append("Step");
    QMetaObject::invokeMethod(this, name.toUtf8().constData(), Qt::DirectConnection, Q_ARG(PipelineWorkingState*, state));
}

void PipelineDelegate::endStep(QString name, PipelineWorkingState* state)
{
    qDebug() << "Calling end: " << name;
    name.prepend("end");
    name.append("Step");
    QMetaObject::invokeMethod(this, name.toUtf8().constData(), Qt::DirectConnection, Q_ARG(PipelineWorkingState*, state));
}

PipelineWorkspace* PipelineDelegate::getWorkspace()
{
    return PipelineWorkspace::instance();
}

} // namespace med::pipeline
