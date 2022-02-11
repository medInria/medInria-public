#include "medPythonPipelineInstance.h"

#include <stdexcept>

#include <medAbstractDataFactory.h>
#include <medPython.h>

#include "medPipelineControlToolBox.h"
#include "medPipelineFactory.h"

namespace med::pipeline
{

class PythonPipelineInstancePrivate
{
public:
    QString referenceModuleName;
    QString moduleName;
    QString sourceCode;
};

bool PythonPipelineInstance::registered()
{
    return medAbstractDataFactory::instance()->registerDataType<PythonPipelineInstance>();
}

PythonPipelineInstance::PythonPipelineInstance() :
    d(new PythonPipelineInstancePrivate())
{
}

PythonPipelineInstance::PythonPipelineInstance(PythonPipeline* pipeline) :
    PipelineInstance(pipeline), d(new PythonPipelineInstancePrivate())
{
    retrieveModuleNameAndSourceCode();
    d->referenceModuleName = d->moduleName;
}

PythonPipelineInstance::PythonPipelineInstance(PythonPipeline* pipeline,
                                               QString referenceModuleName,
                                               QString sourceCode) :
    PipelineInstance(pipeline), d(new PythonPipelineInstancePrivate())
{
    d->referenceModuleName = referenceModuleName;
    d->sourceCode = sourceCode;
}

PythonPipelineInstance::PythonPipelineInstance(const PythonPipelineInstance& other) :
    d(new PythonPipelineInstancePrivate())
{
    *d = *other.d;
}

PythonPipelineInstance::~PythonPipelineInstance()
{
    delete d;
}

QString PythonPipelineInstance::getReferenceModuleName()
{
    return d->referenceModuleName;
}

QString PythonPipelineInstance::getModuleName()
{
    return d->moduleName;
}

QString PythonPipelineInstance::getSourceCode()
{
    return d->sourceCode;
}

void PythonPipelineInstance::createDelegateIfNeeded()
{
    PipelineInstance::createDelegateIfNeeded();

    if (PipelineWorkspace::instance()->getToolBox<PipelineControlToolBox>()->developerModeEnabled()
        && (getPipelineDelegate() == getPipeline()))
    {
        //PythonModuleManager::instance()->loadPermanentModule(d->referenceModuleName);
        QSharedPointer<PipelineDelegate> delegate(PipelineFactory::instance()->createPipeline(getPipeline()->getClassName()));
        setPipelineDelegate(delegate);
    }
}

/* Python classes keep a reference to the module they belong to, and Python modules keep a reference
 * to the file they were loaded from. This functions retrieves the module name and source code through
 * those references and stores them in the instance.
 */
void PythonPipelineInstance::retrieveModuleNameAndSourceCode()
{
    PythonPipeline* pythonPipeline = static_cast<PythonPipeline*>(getPipeline().data());
    python::Object pipelineClass = python::Object::borrowed(pythonPipeline->getPythonClass());
    python::Object moduleName = pipelineClass.attribute("__module__");
    d->moduleName = moduleName.convert<QString>();
    python::Object module = python::import(d->moduleName);
    QString fileName = module.attribute("__file__").convert<QString>();
    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly | QFile::Text))
    {
        QTextStream stream(&file);
        d->sourceCode = stream.readAll();
    }
    else
    {
        QString message = QString("medPythonPipelineInstance: Can't read file ") + fileName;
        throw std::runtime_error(message.toStdString().c_str());
    }
}

} // namespace med::pipeline
