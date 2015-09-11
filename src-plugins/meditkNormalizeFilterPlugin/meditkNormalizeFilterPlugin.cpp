#include "meditkNormalizeFilter.h"
#include "meditkNormalizeFilterPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkNormalizeFilterPlugin
// ///////////////////////////////////////////////////////////////////

void meditkNormalizeFilterPlugin::initialize(void)
{
    qDebug()<<"loading filter plugin";
    medCore::filtering::normalize::pluginFactory().record("itkNormalizeFilter", meditkNormalizeFilterCreator);
}

void meditkNormalizeFilterPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkNormalizeFilter)

