#include "medSubstractImageCommandLineExecutor.h"

medSubstractImageCommandLineExecutor::medSubstractImageCommandLineExecutor()
{

}

medAbstractArithmeticOperationProcess* medSubstractImageCommandLineExecutor::getProcess(QString key)
{
    QStringList keys=medCore::arithmeticalOperation::subtractImage::pluginFactory().keys();
    if(keys.contains(key))
        return medCore::arithmeticalOperation::subtractImage::pluginFactory().create(key);
    if(keys.size()>0)
        return medCore::arithmeticalOperation::subtractImage::pluginFactory().create(keys.at(0));
    return NULL;
}
