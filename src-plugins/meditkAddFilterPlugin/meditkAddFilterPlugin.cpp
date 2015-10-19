#include "meditkAddFilter.h"
#include "meditkAddFilterPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkAddFilterPlugin
// ///////////////////////////////////////////////////////////////////

void meditkAddFilterPlugin::initialize(void)
{
    qDebug()<<"loading filter plugin";
    medCore::filtering::add::pluginFactory().record("itkAddFilter", meditkAddFilterCreator);
}

void meditkAddFilterPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkAddFilter)

