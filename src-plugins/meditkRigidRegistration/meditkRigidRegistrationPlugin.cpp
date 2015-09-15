#include "meditkRigidRegistration.h"
#include "meditkRigidRegistrationPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// meditkRigidRegistrationPlugin
// ///////////////////////////////////////////////////////////////////

void meditkRigidRegistrationPlugin::initialize(void)
{
    medCore::registration::rigid::pluginFactory().record("itkRigidRegistration", meditkRigidRegistrationCreator);
}

void meditkRigidRegistrationPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(meditkRigidRegistration)

