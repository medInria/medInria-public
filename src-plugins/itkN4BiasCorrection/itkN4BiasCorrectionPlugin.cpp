/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkN4BiasCorrection.h"
#include "itkN4BiasCorrectionPlugin.h"
#include "itkN4BiasCorrectionToolBox.h"

itkN4BiasCorrectionPlugin::itkN4BiasCorrectionPlugin(QObject *parent) : medPlugin(parent)
{
}

bool itkN4BiasCorrectionPlugin::initialize()
{
    if(!itkN4BiasCorrection::registered())
    {
        dtkWarn() << "Unable to register itkN4BiasCorrection type";
    }

    if ( !itkN4BiasCorrectionToolBox::registered() )
    {
        dtkWarn() << "Unable to register itkN4BiasCorrection toolbox";
    }
    return true;
}

QString itkN4BiasCorrectionPlugin::description() const
{
  QString description = \
          "This plugin implements <a href=\"https://itk.org/Doxygen/html/classitk_1_1N4BiasFieldCorrectionImageFilter.html\" style=\"color: #cc0000\" >itk::N4BiasFieldCorrectionImageFilter</a> \
          from the <a href=\"https://itk.org/\" style=\"color: #cc0000\" >ITK library</a>.<br>  \
          Drop an MRI volume in the INPUT view, choose your parameters and run.";
  return description;
}

QString itkN4BiasCorrectionPlugin::name() const
{
    return "N4 Bias Correction";
}

QString itkN4BiasCorrectionPlugin::version() const
{
    return ITKN4BIASCORRECTIONPLUGIN_VERSION;
}

QStringList itkN4BiasCorrectionPlugin::types() const
{
    return QStringList() << "itkN4BiasCorrection";
}

Q_EXPORT_PLUGIN2(itkN4BiasCorrectionPlugin, itkN4BiasCorrectionPlugin)
