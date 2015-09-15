#include "meditkMultiplyFilter.h"
#include "meditkMultiplyFilterPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkMultFilterPlugin
// ///////////////////////////////////////////////////////////////////

void meditkMultiplyFilterPlugin::initialize(void)
{
    qDebug()<<"loading filter plugin";
    medCore::filtering::multiply::pluginFactory().record("itkMultFilter", meditkMultiplyFilterCreator);
}

void meditkMultiplyFilterPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkMultiplyFilter)

