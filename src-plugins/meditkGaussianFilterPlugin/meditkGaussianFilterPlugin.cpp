#include "meditkGaussianFilter.h"
#include "meditkGaussianFilterPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkGaussianFilterPlugin
// ///////////////////////////////////////////////////////////////////

void meditkGaussianFilterPlugin::initialize(void)
{
    qDebug()<<"loading filter plugin";
    medCore::filtering::gaussian::pluginFactory().record("itkGaussianFilter", meditkGaussianFilterCreator);
}

void meditkGaussianFilterPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkGaussianFilter)

