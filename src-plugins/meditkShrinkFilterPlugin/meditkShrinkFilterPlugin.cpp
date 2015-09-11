#include "meditkShrinkFilter.h"
#include "meditkShrinkFilterPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkShrinkFilterPlugin
// ///////////////////////////////////////////////////////////////////

void meditkShrinkFilterPlugin::initialize(void)
{
    qDebug()<<"loading filter plugin";
    medCore::filtering::shrink::pluginFactory().record("itkShrinkFilter", meditkShrinkFilterCreator);
}

void meditkShrinkFilterPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkShrinkFilter)

