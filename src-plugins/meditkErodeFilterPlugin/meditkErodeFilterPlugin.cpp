#include "meditkErodeFilter.h"
#include "meditkErodeFilterPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkErodeFilterPlugin
// ///////////////////////////////////////////////////////////////////

void meditkErodeFilterPlugin::initialize(void)
{
    qDebug()<<"loading filter plugin";
    medCore::filtering::erode::pluginFactory().record("itkErodeFilter", meditkErodeFilterCreator);
}

void meditkErodeFilterPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkErodeFilter)

