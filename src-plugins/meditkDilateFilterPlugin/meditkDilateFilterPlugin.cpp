#include "meditkDilateFilter.h"
#include "meditkDilateFilterPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkDilateFilterPlugin
// ///////////////////////////////////////////////////////////////////

void meditkDilateFilterPlugin::initialize(void)
{
    qDebug()<<"loading filter plugin";
    medCore::filtering::dilate::pluginFactory().record("itkDilateFilter", meditkDilateFilterCreator);
}

void meditkDilateFilterPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkDilateFilter)

