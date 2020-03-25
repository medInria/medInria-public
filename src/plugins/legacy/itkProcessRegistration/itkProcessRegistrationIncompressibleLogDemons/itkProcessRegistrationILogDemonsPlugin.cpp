#include "itkProcessRegistrationILogDemons.h"
#include "itkProcessRegistrationILogDemonsPlugin.h"
#include "itkProcessRegistrationILogDemonsToolBox.h"

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// itkProcessRegistrationILogDemonsPlugin
// /////////////////////////////////////////////////////////////////

itkProcessRegistrationILogDemonsPlugin::itkProcessRegistrationILogDemonsPlugin(QObject *parent) : dtkPlugin(parent)
{

}

itkProcessRegistrationILogDemonsPlugin::~itkProcessRegistrationILogDemonsPlugin(void)
{
}

bool itkProcessRegistrationILogDemonsPlugin::initialize(void)
{
    if(!itkProcessRegistrationILogDemons::registered())
    {
        dtkWarn() << "Unable to register itkProcessRegistrationILogDemons type";
    }
    
    if(!itkProcessRegistrationILogDemonsToolBox::registered())
    {
        dtkWarn() << "Unable to register itkProcessRegistrationILogDemons toolbox";
    }
    
    return true;
}

bool itkProcessRegistrationILogDemonsPlugin::uninitialize(void)
{
    return true;
}

QString itkProcessRegistrationILogDemonsPlugin::name(void) const
{
    return "itkProcessRegistrationILogDemonsPlugin";
}

QString itkProcessRegistrationILogDemonsPlugin::contact(void) const
{
    return QString::fromUtf8("benoit.bleuze@inria.fr");
}

QString itkProcessRegistrationILogDemonsPlugin::description(void) const
{
    return tr("Incompressible Log-demons registration method.<br/>\n"
            "The computed transformation is a stationary velocity field. "
            "Moreover, a dense "
            "displacement field is computed from the stationary velocity "
            "field and is returned. <br/> "
            "Explanations on the values can be found in the parameters' "
            "tooltips.<br/><br/>\n"
            "<a href=\"http://www-sop.inria.fr/asclepios/Publications/Tommaso.Mansi/mansi10IJCV.pdf\" >"
            "Tommaso Mansi, Xavier Pennec, Maxime Sermesant, Hervé Delingette, and Nicholas Ayache. iLogDemons: A Demons-Based Registration Algorithm for Tracking Incompressible Elastic Biological Tissues. International Journal of Computer Vision, 92(1):92-111, 2011. HAL ID: inria-00616187</a>");
}

QString itkProcessRegistrationILogDemonsPlugin::version(void) const
{
    return ITKPROCESSREGISTRATIONILOGDEMONSPLUGIN_VERSION;
}

QStringList itkProcessRegistrationILogDemonsPlugin::tags(void) const
{
    return QStringList();
}
QStringList itkProcessRegistrationILogDemonsPlugin::authors(void) const
{
    QStringList list;
    list << "Tommaso Mansi" << "Vincent Garcia";
    return list;
}

QStringList itkProcessRegistrationILogDemonsPlugin::contributors(void) const
{
    QStringList list;
    list<< QString::fromUtf8("Benoît Bleuzé") << "Fatih Arslan";
    return list;
}
QStringList itkProcessRegistrationILogDemonsPlugin::types(void) const
{
    return QStringList() << "itkProcessRegistrationILogDemons";
}
