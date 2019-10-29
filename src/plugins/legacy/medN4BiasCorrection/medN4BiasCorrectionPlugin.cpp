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
  QString description = \
          "This plugin implements <a href=\"https://itk.org/Doxygen/html/classitk_1_1N4BiasFieldCorrectionImageFilter.html\" style=\"color: #cc0000\" >itk::N4BiasFieldCorrectionImageFilter</a> \
          from the <a href=\"https://itk.org/\" style=\"color: #cc0000\" >ITK library</a>.<br>  \
          Drop an MRI volume in the INPUT view, choose your parameters and run.";
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
