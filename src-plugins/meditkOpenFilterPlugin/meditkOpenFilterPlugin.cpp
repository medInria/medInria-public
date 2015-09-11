#include "meditkOpenFilter.h"
#include "meditkOpenFilterPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkOpenFilterPlugin
// ///////////////////////////////////////////////////////////////////

void meditkOpenFilterPlugin::initialize(void)
{
    qDebug()<<"loading filter plugin";
    medCore::filtering::open::pluginFactory().record("itkOpenFilter", meditkOpenFilterCreator);
}

void meditkOpenFilterPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkOpenFilter)

