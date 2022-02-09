/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medN4BiasCorrection.h>
#include <medN4BiasCorrectionPlugin.h>
#include <medN4BiasCorrectionToolBox.h>

medN4BiasCorrectionPlugin::medN4BiasCorrectionPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool medN4BiasCorrectionPlugin::initialize()
{
    if(!medN4BiasCorrection::registered())
    {
        qWarning() << "Unable to register itkN4BiasCorrection type";
    }

    if ( !medN4BiasCorrectionToolBox::registered() )
    {
        qWarning() << "Unable to register itkN4BiasCorrection toolbox";
    }
    return true;
}

QString medN4BiasCorrectionPlugin::description() const
{
    QString description;
    description += "<h1>N4 Bias Correction Tutorial</h1>";
    description += "This toolbox allows to correct nonuniformity associated with MR images.";
    description += "<ul>";
    description += "<li>Drop an MR volume in the INPUT view.</li>";
    description += "<li>Choose your parameters.</li>";
    description += "<li>Click on 'Run'.</li>";
    description += "</ul>";
    description += "This plugin uses the itk::N4BiasFieldCorrectionImageFilter filter from ITK library: https://itk.org";
    return description;
}

QString medN4BiasCorrectionPlugin::name() const
{
    return "N4 Bias Correction";
}

QString medN4BiasCorrectionPlugin::version() const
{
    return MEDN4BIASCORRECTIONPLUGIN_VERSION;
}

QStringList medN4BiasCorrectionPlugin::types() const
{
    return QStringList() << "medN4BiasCorrection";
}
