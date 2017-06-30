/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
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


bool medBinaryOperationPlugin::initialize()
{
    if(!itkXorOperator::registered())
    {
        dtkWarn() << "Unable to register itkXorOperator type";
    }

    if(!itkAndOperator::registered())
    {
        dtkWarn() << "Unable to register itkAndOperator type";
    }

    if(!itkOrOperator::registered())
    {
        dtkWarn() << "Unable to register itkOrOperator type";
    }
    
    if (!itkNotOperator::registered())
    {
        dtkWarn() << "Unable to register itkNotOperator toolbox";
    }

    if (!medBinaryOperationToolBox::registered())
    {
        dtkWarn() << "Unable to register medBinaryOperation toolbox";
    }

    return true;
}

QString medBinaryOperationPlugin::name() const
{
    return "medBinaryOperationPlugin";
}

QString medBinaryOperationPlugin::description() const
{
    return tr("Plugin allowing to apply several binary operations on your data.");
}

QString medBinaryOperationPlugin::version() const
{
    return MEDBINARYOPERATIONPLUGIN_VERSION;
}

QString medBinaryOperationPlugin::identifier() const
{
    return "medBinaryOperation";
}

QStringList medBinaryOperationPlugin::types() const
{
    return QStringList() << "medBinaryOperation";
}

Q_EXPORT_PLUGIN2(medBinaryOperationPlugin, medBinaryOperationPlugin)
