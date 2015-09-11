#include "meditkWindowingFilter.h"
#include "meditkWindowingFilterPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkWindowingFilterPlugin
// ///////////////////////////////////////////////////////////////////

void meditkWindowingFilterPlugin::initialize(void)
{
    qDebug()<<"loading filter plugin";
    medCore::filtering::windowing::pluginFactory().record("itkWindowingFilter", meditkWindowingFilterCreator);
}

void meditkWindowingFilterPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkWindowingFilter)

