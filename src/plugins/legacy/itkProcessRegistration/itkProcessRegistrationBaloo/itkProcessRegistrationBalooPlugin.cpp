#include "itkProcessRegistrationBaloo.h"
#include "itkProcessRegistrationBalooPlugin.h"
#include "itkProcessRegistrationBalooToolBox.h"

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// itkProcessRegistrationBalooPlugin
// /////////////////////////////////////////////////////////////////

itkProcessRegistrationBalooPlugin::itkProcessRegistrationBalooPlugin(QObject *parent) : dtkPlugin(parent)
{

}

itkProcessRegistrationBalooPlugin::~itkProcessRegistrationBalooPlugin(void)
{
}

bool itkProcessRegistrationBalooPlugin::initialize(void)
{
    if(!itkProcessRegistrationBaloo::registered())
    {
        dtkWarn() << "Unable to register itkProcessRegistrationBaloo type";
    }
    if(!itkProcessRegistrationBalooToolBox::registered())
    {
        dtkWarn() << "Unable to register itkProcessRegistrationBaloo toolbox";
    }
    
    return true;
}

bool itkProcessRegistrationBalooPlugin::uninitialize(void)
{
    return true;
}

QString itkProcessRegistrationBalooPlugin::name(void) const
{
    return "itkProcessRegistrationBalooPlugin";
}

QString itkProcessRegistrationBalooPlugin::contact(void) const
{
    return QString::fromUtf8("benoit.bleuze@inria.fr");
}

QString itkProcessRegistrationBalooPlugin::description(void) const
{
    return tr("Baloo linear registration method. \n<br/>"
       "This registration method is based on a "
       "pyramidal block-matching algorithm. Blocks are built in the fixed image. "
       "<br/> For each block, the most similar block is searched in the moving image "
       "(exploration). Each paring of blocks defines a displacement vector. The"
       "set of displacement vector is finally used to estimate a linear "
       "transformation from the fixed to the moving image. \n"
       "<br/>"
       "<a href=\"http://www-sop.inria.fr/asclepios/Publications/Gregoire.Malandain/ourselin-miccai-2000.pdf\">"
       "S. Ourselin, A. Roche, S. Prima, and N. Ayache. Block Matching: "
       "A General Framework to Improve Robustness of Rigid Registration of "
       "Medical Images. In A.M. DiGioia and S. Delp, editors, "
       "Third International Conference on Medical Robotics, Imaging And "
       "Computer Assisted Surgery (MICCAI 2000), volume 1935 of Lectures Notes "
       "in Computer Science, Pittsburgh, Penn, USA, pages 557-566, "
       "octobre 11-14 2000. Springer.</a>");
}

QString itkProcessRegistrationBalooPlugin::version(void) const
{
    return ITKPROCESSREGISTRATIONBALOOPLUGIN_VERSION;
}

QStringList itkProcessRegistrationBalooPlugin::tags(void) const
{
    return QStringList();
}

QStringList itkProcessRegistrationBalooPlugin::authors(void) const
{
    QStringList list;
    list << "Olivier Commowick";
    return list;
}

QStringList itkProcessRegistrationBalooPlugin::contributors(void) const
{
    QStringList list;
    list<< "Vincent Garcia" << QString::fromUtf8("Benoît Bleuzé");
    return list;
}


QStringList itkProcessRegistrationBalooPlugin::types(void) const
{
    return QStringList() << "itkProcessRegistrationBaloo";
}
