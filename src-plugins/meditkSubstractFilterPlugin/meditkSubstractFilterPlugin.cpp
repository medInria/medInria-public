#include "meditkSubstractFilter.h"
#include "meditkSubstractFilterPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkSubstractFilterPlugin
// ///////////////////////////////////////////////////////////////////

void meditkSubstractFilterPlugin::initialize(void)
{
    qDebug()<<"loading filter plugin";
    medCore::filtering::substract::pluginFactory().record("itkSubstractFilter", meditkSubstractFilterCreator);
}

void meditkSubstractFilterPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkSubstractFilter)

