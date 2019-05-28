/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <ITKProcessExample.h>
#include <ITKProcessExamplePlugin.h>
#include <ITKProcessExampleToolBox.h>
#include <ITKProcessExampleDefinitions.h>

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// ITKProcessExamplePluginPrivate
// /////////////////////////////////////////////////////////////////

class ITKProcessExamplePluginPrivate
{
public:
    // Class variables go here.
};

// /////////////////////////////////////////////////////////////////
// ITKProcessExamplePlugin
// /////////////////////////////////////////////////////////////////

ITKProcessExamplePlugin::ITKProcessExamplePlugin(QObject *parent) : dtkPlugin(parent), d(new ITKProcessExamplePluginPrivate)
{

}

ITKProcessExamplePlugin::~ITKProcessExamplePlugin()
{
    delete d;

    d = nullptr;
}

bool ITKProcessExamplePlugin::initialize()
{
    if(!ITKProcessExample::registered()) {
        dtkWarn() << "Unable to register ITKProcessExample type";
    }

    if(!ITKProcessExampleToolBox::registered()) {
        dtkWarn() << "Unable to register ITKProcessExample toolbox";
    }

    return true;
}

bool ITKProcessExamplePlugin::uninitialize()
{
    return true;
}

QString ITKProcessExamplePlugin::name() const
{
    return "ITKProcessExamplePlugin";
}

QString ITKProcessExamplePlugin::contact() const
{
    return QString::fromUtf8("Clément Philipot clement.philipot@inria.fr");
}

QString ITKProcessExamplePlugin::version() const
{
    return PLUGIN_VERSION;
}

QString ITKProcessExamplePlugin::description() const
{
    return "ITK GaussianBlur Filter Example Plugin";
}

QStringList ITKProcessExamplePlugin::tags() const
{
    return QStringList() << "ITK" << "process" << "gaussianblur" << "filerexample" << "preprocessing";
}

QStringList ITKProcessExamplePlugin::types() const
{
    return QStringList() << "ITKProcessExample";
}
kk
