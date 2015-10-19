#include "meditkMedianFilter.h"
#include "meditkMedianFilterPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkMedianFilterPlugin
// ///////////////////////////////////////////////////////////////////

void meditkMedianFilterPlugin::initialize(void)
{
    qDebug()<<"loading filter plugin";
    medCore::filtering::median::pluginFactory().record("itkMedianFilter", meditkMedianFilterCreator);
}

void meditkMedianFilterPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkMedianFilter)

