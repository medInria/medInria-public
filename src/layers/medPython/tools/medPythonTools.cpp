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

namespace med::python
{

void initializeTools()
{
    registerModulePath(getExternalResourcesDirectory(TARGET_NAME));
    registerModulePath(getExternalResourcePath(QString(TARGET_NAME) + ".zip", TARGET_NAME));
    QString command = QString("from %1 import *").arg(PYTHON_PACKAGE_NAME);
    coreFunction(PyRun_SimpleString, qUtf8Printable(command));
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
