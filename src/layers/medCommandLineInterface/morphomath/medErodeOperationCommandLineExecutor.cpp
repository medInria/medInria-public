#include "medErodeOperationCommandLineExecutor.h"

medErodeOperationCommandLineExecutor::medErodeOperationCommandLineExecutor()
{

}

medAbstractMorphomathOperationProcess* medErodeOperationCommandLineExecutor::getProcess(QString key)
{
    QStringList keys=medCore::morphomathOperation::erodeImage::pluginFactory().keys();
    if(keys.contains(key))
        return medCore::morphomathOperation::erodeImage::pluginFactory().create(key);
    if(keys.size()>0)
        return medCore::morphomathOperation::erodeImage::pluginFactory().create(keys.at(0));
    return NULL;
}
