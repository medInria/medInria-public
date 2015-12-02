#include "medClosingOperationCommandLineExecutor.h"

medClosingOperationCommandLineExecutor::medClosingOperationCommandLineExecutor()
{

}

medAbstractMorphomathOperationProcess* medClosingOperationCommandLineExecutor::getProcess(QString key)
{
    QStringList keys=medCore::morphomathOperation::closingImage::pluginFactory().keys();
    if(keys.contains(key))
        return medCore::morphomathOperation::closingImage::pluginFactory().create(key);
    if(keys.size()>0)
        return medCore::morphomathOperation::closingImage::pluginFactory().create(keys.at(0));
    return NULL;
}
