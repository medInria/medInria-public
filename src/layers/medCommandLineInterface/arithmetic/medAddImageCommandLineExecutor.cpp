#include "medAddImageCommandLineExecutor.h"

medAddImageCommandLineExecutor::medAddImageCommandLineExecutor()
{

}

medAbstractArithmeticOperationProcess* medAddImageCommandLineExecutor::getProcess(QString key)
{
    QStringList keys=medCore::arithmeticalOperation::addImage::pluginFactory().keys();
    if(keys.contains(key))
        return medCore::arithmeticalOperation::addImage::pluginFactory().create(key);
    if(keys.size()>0)
        return medCore::arithmeticalOperation::addImage::pluginFactory().create(keys.at(0));
    return NULL;
}
