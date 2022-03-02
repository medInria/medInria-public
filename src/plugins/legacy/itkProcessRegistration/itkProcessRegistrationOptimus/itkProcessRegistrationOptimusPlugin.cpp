#include "itkProcessRegistrationOptimus.h"
#include "itkProcessRegistrationOptimusPlugin.h"
#include "itkProcessRegistrationOptimusToolBox.h"

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// itkProcessRegistrationOptimusPlugin
// /////////////////////////////////////////////////////////////////

itkProcessRegistrationOptimusPlugin::itkProcessRegistrationOptimusPlugin(QObject *parent) : dtkPlugin(parent)
{

}

itkProcessRegistrationOptimusPlugin::~itkProcessRegistrationOptimusPlugin(void)
{
}

bool itkProcessRegistrationOptimusPlugin::initialize(void)
{
    if(!itkProcessRegistrationOptimus::registered())
    {
        dtkWarn() << "Unable to register itkProcessRegistrationOptimus type";
    }

    if(!itkProcessRegistrationOptimusToolBox::registered())
    {
        dtkWarn() << "Unable to register itkProcessRegistrationOptimus toolbox";
    }

    return true;
}

bool itkProcessRegistrationOptimusPlugin::uninitialize(void)
{
    return true;
}

QString itkProcessRegistrationOptimusPlugin::name(void) const
{
    return "itkProcessRegistrationOptimusPlugin";
}

QString itkProcessRegistrationOptimusPlugin::description(void) const
{
    return "Optimus registration method.\n<br/>"
            "This method is based on the New UOA optimizer. <br/>"
            "The metric used is the mutual information. The transformation "
            "computed is a rigid transformation."
            "<a href=\"http://www.springerlink.com/content/x7m14k3477273072/\">"
            "http://www.springerlink.com/content/x7m14k3477273072/</a>";
}

QString itkProcessRegistrationOptimusPlugin::contact(void) const
{
    return QString::fromUtf8("Benoît Bleuzé benoit.bleuze@inria.fr");
}

QStringList itkProcessRegistrationOptimusPlugin::authors(void) const
{
    QStringList list;
    list << "Olivier Clatz";
    return list;
}

QStringList itkProcessRegistrationOptimusPlugin::contributors(void) const
{
    QStringList list;
    list <<  QString::fromUtf8("Benoît Bleuzé")
          << "Vincent Garcia";
    return list;
}

QString itkProcessRegistrationOptimusPlugin::version(void) const
{
    return ITKPROCESSREGISTRATIONOPTIMUSPLUGIN_VERSION;
}

QStringList itkProcessRegistrationOptimusPlugin::tags(void) const
{
    return QStringList();
}

QStringList itkProcessRegistrationOptimusPlugin::types(void) const
{
    return QStringList() << "itkProcessRegistrationOptimus";
}
