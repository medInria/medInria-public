#include "itkProcessRegistrationSuperBaloo.h"
#include "itkProcessRegistrationSuperBalooPlugin.h"
#include "itkProcessRegistrationSuperBalooToolBox.h"

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// itkProcessRegistrationSuperBalooPlugin
// /////////////////////////////////////////////////////////////////

itkProcessRegistrationSuperBalooPlugin::itkProcessRegistrationSuperBalooPlugin(QObject *parent) : dtkPlugin(parent)
{

}

itkProcessRegistrationSuperBalooPlugin::~itkProcessRegistrationSuperBalooPlugin(void)
{
}

bool itkProcessRegistrationSuperBalooPlugin::initialize(void)
{
    if(!itkProcessRegistrationSuperBaloo::registered())
    {
        dtkWarn() << "Unable to register itkProcessRegistrationSuperBaloo type";
    }
    
    if(!itkProcessRegistrationSuperBalooToolBox::registered())
    {
        dtkWarn() << "Unable to register itkProcessRegistrationSuperBaloo toolbox";
    }
    
    return true;
}

bool itkProcessRegistrationSuperBalooPlugin::uninitialize(void)
{
    return true;
}

QString itkProcessRegistrationSuperBalooPlugin::name(void) const
{
    return "itkProcessRegistrationSuperBalooPlugin";
}

QString itkProcessRegistrationSuperBalooPlugin::contact(void) const
{
    return QString::fromUtf8("Benoît Bleuzé benoit.bleuze@inria.fr");
}

QString itkProcessRegistrationSuperBalooPlugin::description(void) const
{
    return QString::fromUtf8("Super Baloo registration methods. "
                             "\n<br/>"
                             "This registration method is "
            "based on a pyramidal block-matching algorithm. Blocks are built "
            "in the fixed image. For each block, the most similar block "
            "is searched in the moving image (exploration). Each paring of "
            "blocks defines a displacement vector. The set of displacement "
            "vectors is finally interpolated to define a dense "
            "displacement field from the fixed to the moving image. "
            "<br/><br/> "
            "The algorithm is described in this article:<br/>"
            "Vincent Garcia, Olivier Commowick, and Grégoire Malandain. "
            "<a href=\"http://www-sop.inria.fr/asclepios/Publications/"
            "Vincent.Garcia/Garcia_2010_EMPIRE10_2.pdf\" >"
            "A Robust and Efficient Block Matching Framework for Non Linear "
            "Registration of Thoracic CT Images.</a>");
}

QString itkProcessRegistrationSuperBalooPlugin::version(void) const
{
    return ITKPROCESSREGISTRATIONSUPERBALOOPLUGIN_VERSION;
}

QStringList itkProcessRegistrationSuperBalooPlugin::tags(void) const
{
    return QStringList();
}

QStringList itkProcessRegistrationSuperBalooPlugin::authors(void) const
{
    QStringList list;
    list << "Olivier Commowick";
    return list;
}

QStringList itkProcessRegistrationSuperBalooPlugin::contributors(void) const
{
    QStringList list;
    list<< "Vincent Garcia" << QString::fromUtf8("Benoît Bleuzé");
    return list;
}


QStringList itkProcessRegistrationSuperBalooPlugin::types(void) const
{
    return QStringList() << "itkProcessRegistrationSuperBaloo";
}
