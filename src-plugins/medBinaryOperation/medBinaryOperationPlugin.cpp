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

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// medBinaryOperationPluginPrivate
// /////////////////////////////////////////////////////////////////

class medBinaryOperationPluginPrivate
{
public:
    // Class variables go here.
    static const char *s_Name;
};

const char * medBinaryOperationPluginPrivate::s_Name = "medBinaryOperation";

// /////////////////////////////////////////////////////////////////
// medBinaryOperationPlugin
// /////////////////////////////////////////////////////////////////

medBinaryOperationPlugin::medBinaryOperationPlugin(QObject *parent) : dtkPlugin(parent), d(new medBinaryOperationPluginPrivate)
{
    
}

medBinaryOperationPlugin::~medBinaryOperationPlugin()
{
    delete d;
    
    d = NULL;
}

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

bool medBinaryOperationPlugin::uninitialize()
{
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

QString medBinaryOperationPlugin::contact() const
{
    return "For any feedback <music-userfeedback@inria.fr>";
}

QStringList medBinaryOperationPlugin::authors() const
{
    return QStringList() << "Mathilde Merle";
}

QStringList medBinaryOperationPlugin::contributors() const
{
    QStringList list;
    return list;
}

QString medBinaryOperationPlugin::identifier() const
{
    return medBinaryOperationPluginPrivate::s_Name;
}


QStringList medBinaryOperationPlugin::tags() const
{
    return QStringList();
}

QStringList medBinaryOperationPlugin::types() const
{
    return QStringList() << "medBinaryOperation";
}
QStringList medBinaryOperationPlugin::dependencies() const
{
    return QStringList();
}
Q_EXPORT_PLUGIN2(medBinaryOperationPlugin, medBinaryOperationPlugin)
