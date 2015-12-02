#include "medDilateOperationCommandLineExecutor.h"

medDilateOperationCommandLineExecutor::medDilateOperationCommandLineExecutor()
{

}

medAbstractMorphomathOperationProcess* medDilateOperationCommandLineExecutor::getProcess(QString key)
{
    QStringList keys=medCore::morphomathOperation::dilateImage::pluginFactory().keys();
    if(keys.contains(key))
        return medCore::morphomathOperation::dilateImage::pluginFactory().create(key);
    if(keys.size()>0)
        return medCore::morphomathOperation::dilateImage::pluginFactory().create(keys.at(0));
    return NULL;
}
