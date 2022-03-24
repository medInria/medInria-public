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

bool initializeModulePaths()
{
    bool success = false;
    QString pythonToolsDirectory = getExternalResourcesDirectory(".", TARGET_NAME);

    if (pythonToolsDirectory.isEmpty())
    {
        qCritical() << QString("Cannot find the resources directory for %1.")
                       .arg(TARGET_NAME);
    }
    else
    {
        QString zipName = QString("%1.zip").arg(TARGET_NAME);
        QString pythonToolsZip = getExternalResourcePath(zipName, TARGET_NAME);

        if (pythonToolsZip.isEmpty())
        {
            qCritical() << QString("Cannot find %1 in %2 resources.")
                           .arg(zipName, TARGET_NAME);
        }
        else
        {
            try
            {
                addPythonPath(pythonToolsDirectory);
                addPythonPath(pythonToolsZip);
                success = true;
            }
            catch (Exception& e)
            {
                qCritical() << QString("Error while adding Python paths for %1: %2")
                               .arg(TARGET_NAME, e.what());
            }
        }
    }

    return success;
}

bool importToolsModule()
{
    bool success = false;

    try
    {
        import(PYTHON_PACKAGE_NAME);
    }
    catch (Exception& e)
    {
        qCritical() << QString("Error while importing %1: %2")
                       .arg(PYTHON_PACKAGE_NAME, e.what());
    }

    return success;
}

bool initializeSettingsWidget()
{
    bool success = medSettingsWidgetFactory::instance()->registerSettingsWidget<PythonSettingsWidget>();

    if (!success)
    {
        qCritical() << QString("Cannot register %1.").arg(PythonSettingsWidget::staticName());
    }

    return success;
}

} // namespace

bool initializeTools()
{
    bool success = initializeModulePaths() && importToolsModule();

    success &= initializeSettingsWidget();
    // bitwise AND intended here because we always want the settings widget even
    // if the tools module was not successfully imported.

    return success;
}

void startConsole()
{
    try
    {
        Module toolsModule = import(PYTHON_PACKAGE_NAME);

        Object console = toolsModule.attribute("Console").callMethod("createInstance")
                         .kwarg("title", QString(CONSOLE_TITLE))
                         .kwarg("size", list<int>({CONSOLE_WIDTH, CONSOLE_HEIGHT}));
        console.callMethod("setShortcut", QString(CONSOLE_SHORTCUT));
        console.callMethod("run");
        qInfo() << "The Python console can be accessed with " << CONSOLE_SHORTCUT;

        Object failedPlugins = toolsModule.attribute("failed_plugins");

        if (failedPlugins)
        {
            print("** Some medInria plugins failed to load. Type \"med.plugins()\" for details. **\n\n");
        }
    }
    catch (Exception& e)
    {
        qCritical() << QString("Error while starting the Python console: %1")
                       .arg(e.what());
    }
}

} // namespace med::python
