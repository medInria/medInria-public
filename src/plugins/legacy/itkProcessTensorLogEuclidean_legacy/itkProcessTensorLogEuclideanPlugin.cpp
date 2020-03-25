/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkProcessTensorLogEuclideanPlugin.h"

#include "itkProcessTensorEstimationToolBox.h"
#include "itkProcessTensorScalarMapsToolBox.h"
#include "itkProcessTensorTractographyToolBox.h"
#include "itkProcessTensorEstimation.h"
#include "itkProcessTensorScalarMaps.h"
#include "itkProcessTensorTractography.h"

#include "itkProcessTensorLogEuclideanEstimation.h"
#include "itkProcessTensorLogEuclideanRemoveNonPositive.h"
#include "itkProcessTensorDTIBrainMaskCalculator.h"
#include "itkProcessTensorLogEuclideanAnisotropicSmoothing.h"
#include "itkProcessTensorLogEuclideanTractography.h"


#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// itkProcessTensorLogEuclideanPluginPrivate
// /////////////////////////////////////////////////////////////////

class itkProcessTensorLogEuclideanPluginPrivate
{
public:
    // Class variables go here.
    static const char *s_Name;
};

const char * itkProcessTensorLogEuclideanPluginPrivate::s_Name = "itkProcessTensorLogEuclideanPlugin";

// /////////////////////////////////////////////////////////////////
// itkProcessTensorLogEuclideanPlugin
// /////////////////////////////////////////////////////////////////

itkProcessTensorLogEuclideanPlugin::itkProcessTensorLogEuclideanPlugin(QObject *parent) : dtkPlugin(parent), d(new itkProcessTensorLogEuclideanPluginPrivate)
{
    
}

itkProcessTensorLogEuclideanPlugin::~itkProcessTensorLogEuclideanPlugin()
{
    delete d;
    
    d = NULL;
}

bool itkProcessTensorLogEuclideanPlugin::initialize()
{
    if(!itkProcessTensorEstimation::registered()) {dtkWarn() << "Unable to register itkProcessTensorEstimation type";}
    if(!itkProcessTensorScalarMaps::registered()) {dtkWarn() << "Unable to register itkProcessTensorScalarMaps type";}
    if(!itkProcessTensorTractography::registered()) {dtkWarn() << "Unable to register itkProcessTensorTractography type";}
    
    if(!itkProcessTensorEstimationToolBox::registered()) {dtkWarn() << "Unable to register itkProcessTensorEstimationToolBox type";}
    if(!itkProcessTensorScalarMapsToolBox::registered()) {dtkWarn() << "Unable to register itkProcessTensorScalarMapsToolBox type";}
    if(!itkProcessTensorTractographyToolBox::registered()) {dtkWarn() << "Unable to register itkProcessTensorTractographyToolBox type";}
    
    if(!itkProcessTensorLogEuclideanEstimation::registered()) {dtkWarn() << "Unable to register itkProcessTensorLogEuclideanEstimation type";}
    if(!itkProcessTensorLogEuclideanRemoveNonPositive::registered()) {dtkWarn() << "Unable to register itkProcessTensorLogEuclideanRemoveNonPositive type";}
    if(!itkProcessTensorDTIBrainMaskCalculator::registered()) {dtkWarn() << "Unable to register itkProcessTensorDTIBrainMaskCalculator type";}
    if(!itkProcessTensorLogEuclideanAnisotropicSmoothing::registered()) {dtkWarn() << "Unable to register itkProcessTensorLogEuclideanAnisotropicSmoothing type";}
    if(!itkProcessTensorLogEuclideanTractography::registered()) {dtkWarn() << "Unable to register itkProcessTensorLogEuclideanTractography type";}
    
    return true;
}

bool itkProcessTensorLogEuclideanPlugin::uninitialize()
{
    return true;
}

QString itkProcessTensorLogEuclideanPlugin::name() const
{
    return d->s_Name;
}

QString itkProcessTensorLogEuclideanPlugin::description() const
{
    return "Diffusion MR processing and tractography \n<br/>"
    "with an ITK implementation of Log-Euclidean metrics.<br/>"
    "See: <a href=\"http://www.inria.fr/sophia/asclepios/Publications/Pierre.Fillard/Fillard.TMI.2007.pdf\">"
    "http://www.inria.fr/sophia/asclepios/Publications/Pierre.Fillard/Fillard.TMI.2007.pdf</a>";
}

QString itkProcessTensorLogEuclideanPlugin::version() const
{
    return ITKPROCESSTENSORLOGEUCLIDEANPLUGIN_VERSION;
}

QString itkProcessTensorLogEuclideanPlugin::contact() const
{
    return "Pierre Fillard (pierre.fillard@inria.fr)";
}

QStringList itkProcessTensorLogEuclideanPlugin::authors() const
{
    return QStringList() << "Pierre Fillard"
                         << "Sergio Medina";
}

QStringList itkProcessTensorLogEuclideanPlugin::contributors() const
{
    return QStringList() << "Nicolas Toussaint" << "Olivier Commowick";
}

QString itkProcessTensorLogEuclideanPlugin::identifier() const
{
    return itkProcessTensorLogEuclideanPluginPrivate::s_Name;
}


QStringList itkProcessTensorLogEuclideanPlugin::tags() const
{
    return QStringList() << "itk" << "process" << "tensor" << "log-euclidean";
}

QStringList itkProcessTensorLogEuclideanPlugin::types() const
{
    return QStringList() << "itkProcessTensorLogEuclideanTractography"
    << "itkProcessTensorLogEuclideanEstimation"
    << "itkProcessTensorLogEuclideanRemoveNonPositive"
    << "itkProcessTensorLogEuclideanCoefficientCalculator"
    << "itkProcessTensorDTIBrainMaskCalculator"
    << "itkProcessTensorLogEuclideanAnisotropicSmoothing"
    << "itkProcessTensorEstimationToolBox"
    << "itkProcessTensorScalarMapsToolBox"
    << "itkProcessTensorTractographyToolBox"
    << "itkProcessTensorEstimation"
    << "itkProcessTensorScalarMaps"
    << "itkProcessTensorTractography";
}

QStringList itkProcessTensorLogEuclideanPlugin::dependencies() const
{
    return QStringList();
}

Q_EXPORT_PLUGIN2(itkProcessTensorLogEuclideanPlugin, itkProcessTensorLogEuclideanPlugin)
