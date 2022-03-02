/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreAPI.h"

#include <QDebug>

#include "medPythonTools.h"

#include <medExternalResources.h>
#include <medPython.h>
#include <medSettingsWidgetFactory.h>

#include "medPythonSettingsWidget.h"

namespace med::python
{

namespace
{

void initializeModulePaths()
{
    addPythonPath(getExternalResourcesDirectory(TARGET_NAME));
    addPythonPath(getExternalResourcePath(QString(TARGET_NAME) + ".zip", TARGET_NAME));
}

void importModulesInMainNamespace()
{
    QString command = QString("from %1 import *").arg(PYTHON_PACKAGE_NAME);
    coreFunction(PyRun_SimpleString, qUtf8Printable(command));
}

void initializeSettingsWidget()
{
    medSettingsWidgetFactory::instance()->registerSettingsWidget<PythonSettingsWidget>();
}

} // namespace

void initializeTools()
{
    initializeModulePaths();
    importModulesInMainNamespace();
    initializeSettingsWidget();
}

void startConsole()
{
    Module toolsModule = import(PYTHON_PACKAGE_NAME);
    Object console = toolsModule.attribute("Console").callMethod("createInstance")
                     .kwarg("title", QString(CONSOLE_TITLE))
                     .kwarg("size", list<int>({CONSOLE_WIDTH, CONSOLE_HEIGHT}));
    console.callMethod("setShortcut", QString(CONSOLE_SHORTCUT));
    console.callMethod("run");
    qInfo() << "The Python console can be accessed with " << CONSOLE_SHORTCUT;
}

} // namespace med::python
