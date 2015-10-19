#include "meditkRegistrationDiffeomorphicDemonsProcess.h"
#include "meditkRegistrationDiffeomorphicDemonsProcessPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkRegistrationDiffeomorphicDemonsProcessPlugin
// ///////////////////////////////////////////////////////////////////

void meditkRegistrationDiffeomorphicDemonsProcessPlugin::initialize(void)
{
    medCore::registration::nonRigid::pluginFactory().record("itkRegistrationDiffeomorphicDemonsProcess", meditkRegistrationDiffeomorphicDemonsProcessCreator);
    medCore::registration::nonRigid::widgetFactory().record("itkRegistrationDiffeomorphicDemonsProcess", new meditkProcessRegistrationDiffeomorphicDemonsToolBox());
}

void meditkRegistrationDiffeomorphicDemonsProcessPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkRegistrationDiffeomorphicDemonsProcess)

