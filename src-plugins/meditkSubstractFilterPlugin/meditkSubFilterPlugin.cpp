#include "meditkSubFilter.h"
#include "meditkSubFilterPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkSubFilterPlugin
// ///////////////////////////////////////////////////////////////////

void meditkSubFilterPlugin::initialize(void)
{
    qDebug()<<"loading filter plugin";
    medCore::filtering::Sub::pluginFactory().record("itkSubFilter", meditkSubFilterCreator);
}

void meditkSubFilterPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkSubFilter)

