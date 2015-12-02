#include "medOpeningOperationCommandLineExecutor.h"

medOpeningOperationCommandLineExecutor::medOpeningOperationCommandLineExecutor()
{

}

medAbstractMorphomathOperationProcess* medOpeningOperationCommandLineExecutor::getProcess(QString key)
{
    QStringList keys=medCore::morphomathOperation::openingImage::pluginFactory().keys();
    if(keys.contains(key))
        return medCore::morphomathOperation::openingImage::pluginFactory().create(key);
    if(keys.size()>0)
        return medCore::morphomathOperation::openingImage::pluginFactory().create(keys.at(0));
    return NULL;
}
