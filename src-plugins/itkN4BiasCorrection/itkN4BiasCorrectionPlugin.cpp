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

QString itkN4BiasCorrectionPlugin::name() const
{
    return "N4 Bias Correction";
}

QString itkN4BiasCorrectionPlugin::version() const
{
    return ITKN4BIASCORRECTIONPLUGIN_VERSION;
}

QString itkN4BiasCorrectionPlugin::identifier() const
{
    return "itkN4BiasCorrection";
}

QStringList itkN4BiasCorrectionPlugin::types() const
{
    return QStringList() << "itkN4BiasCorrection";
}

Q_EXPORT_PLUGIN2(itkN4BiasCorrectionPlugin, itkN4BiasCorrectionPlugin)
