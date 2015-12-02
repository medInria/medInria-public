#include "medMultiplyImageCommandLineExecutor.h"

medMultiplyImageCommandLineExecutor::medMultiplyImageCommandLineExecutor()
{

}

medAbstractArithmeticOperationProcess* medMultiplyImageCommandLineExecutor::getProcess(QString key)
{
    QStringList keys=medCore::arithmeticalOperation::multiplyImage::pluginFactory().keys();
    if(keys.contains(key))
        return medCore::arithmeticalOperation::multiplyImage::pluginFactory().create(key);
    if(keys.size()>0)
        return medCore::arithmeticalOperation::multiplyImage::pluginFactory().create(keys.at(0));
    return NULL;
}
