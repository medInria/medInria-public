#include "medDivideImageCommandLineExecutor.h"

medAbstractArithmeticOperationProcess* medDivideImageCommandLineExecutor::getProcess(QString key)
{
    QStringList keys=medCore::arithmeticalOperation::divideImage::pluginFactory().keys();
    if(keys.contains(key))
        return medCore::arithmeticalOperation::divideImage::pluginFactory().create(key);
    if(keys.size()>0)
        return medCore::arithmeticalOperation::divideImage::pluginFactory().create(keys.at(0));
    return NULL;
}
