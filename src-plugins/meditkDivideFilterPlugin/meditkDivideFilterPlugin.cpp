#include "meditkDivideFilter.h"
#include "meditkDivideFilterPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkDivideFilterPlugin
// ///////////////////////////////////////////////////////////////////

void meditkDivideFilterPlugin::initialize(void)
{
    qDebug()<<"loading filter plugin";
    medCore::filtering::divide::pluginFactory().record("itkDivideFilter", meditkDivideFilterCreator);
}

void meditkDivideFilterPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkDivideFilter)

