#include "meditkInvertFilter.h"
#include "meditkInvertFilterPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkInvertFilterPlugin
// ///////////////////////////////////////////////////////////////////

void meditkInvertFilterPlugin::initialize(void)
{
    qDebug()<<"loading filter plugin";
    medCore::filtering::invert::pluginFactory().record("itkInvertFilter", meditkInvertFilterCreator);
}

void meditkInvertFilterPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkInvertFilter)

