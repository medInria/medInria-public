#include "meditkMultFilter.h"
#include "meditkMultFilterPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkMultFilterPlugin
// ///////////////////////////////////////////////////////////////////

void meditkMultFilterPlugin::initialize(void)
{
    qDebug()<<"loading filter plugin";
    medCore::filtering::multiply::pluginFactory().record("itkMultFilter", meditkMultFilterCreator);
}

void meditkMultFilterPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkMultFilter)

