/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkAndOperator.h"
#include "itkNotOperator.h"
#include "itkOrOperator.h"
#include "itkXorOperator.h"
#include "medBinaryOperationPlugin.h"
#include "medBinaryOperationToolBox.h"

medBinaryOperationPlugin::medBinaryOperationPlugin(QObject *parent) : medPluginLegacy(parent)
{
}

bool medBinaryOperationPlugin::initialize()
{
    if(!itkAndOperator::registered())
    {
        qWarning() << "Unable to register itkAndOperator type";
    }

    if (!itkNotOperator::registered())
    {
        qWarning() << "Unable to register itkNotOperator type";
    }

    if(!itkOrOperator::registered())
    {
        qWarning() << "Unable to register itkOrOperator type";
    }
    
    if(!itkXorOperator::registered())
    {
        qWarning() << "Unable to register itkXorOperator type";
    }

    if (!medBinaryOperationToolBox::registered())
    {
        qWarning() << "Unable to register medBinaryOperation toolbox";
    }

    return true;
}

QString medBinaryOperationPlugin::name() const
{
    return "Binary Operation";
}

QString medBinaryOperationPlugin::description() const
{
    QString description;
    description += "<h1>Binary Operation Tutorial</h1>";
    description += "Apply binary operations on masks.";
    description += "<ul>";
    description += "<li>Drop a mask in the INPUT view.</li>";
    description += "<li>Drop a second mask in the toolbox drop area, except to use 'NOT'.</li>";
    description += "<li>Choose the type of binary computation that you want to process.</li>";
    description += "<li>Click on 'Run'.</li>";
    description += "</ul>";
    description += "This plugin uses the ITK library: https://itk.org";
    return description;
}

QString medBinaryOperationPlugin::version() const
{
    return MEDBINARYOPERATIONPLUGIN_VERSION;
}

QStringList medBinaryOperationPlugin::types() const
{
    return QStringList() << "medBinaryOperation";
}
