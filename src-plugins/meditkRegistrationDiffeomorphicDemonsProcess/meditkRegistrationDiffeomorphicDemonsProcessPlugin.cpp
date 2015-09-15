#include "meditkRegistrationDiffeomorphicDemonsProcess.h"
#include "meditkRegistrationDiffeomorphicDemonsProcessPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkRegistrationDiffeomorphicDemonsProcessPlugin
// ///////////////////////////////////////////////////////////////////

void meditkRegistrationDiffeomorphicDemonsProcessPlugin::initialize(void)
{
    medCore::registration::pluginFactory().record("itkRegistrationDiffeomorphicDemonsProcess", meditkRegistrationDiffeomorphicDemonsProcessCreator);
}

void meditkRegistrationDiffeomorphicDemonsProcessPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkRegistrationDiffeomorphicDemonsProcess)

