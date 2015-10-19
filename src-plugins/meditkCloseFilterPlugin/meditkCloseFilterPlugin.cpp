#include "meditkCloseFilter.h"
#include "meditkCloseFilterPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkCloseFilterPlugin
// ///////////////////////////////////////////////////////////////////

void meditkCloseFilterPlugin::initialize(void)
{
    qDebug()<<"loading filter plugin";
    medCore::filtering::close::pluginFactory().record("itkCloseFilter", meditkCloseFilterCreator);
}

void meditkCloseFilterPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkCloseFilter)

