/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreAPI.h"

#include "medPythonUtils.h"

#include <medSettingsManager.h>

#include "medPythonCoreFunction.h"
#include "medPythonStandardObjects.h"

namespace med::python
{

namespace
{

const char* USER_PATHS_SETTINGS_ID = "user_paths";

void tryLoadPlugins()
{
    Module pkgutil = import("pkgutil");
    Object moduleIterator = pkgutil.callMethod("iter_modules");
    Object moduleInfo = coreFunction(PyIter_Next, *moduleIterator);
    Module medInriaModule = import(PROJECT_NAME);

    while (moduleInfo)
    {
        // moduleInfo[2] is True if the module is a package
        if (moduleInfo[2])
        {
            QString moduleName = moduleInfo[1].convert<QString>();

            if (moduleName.startsWith(PYTHON_PLUGIN_PREFIX))
            {
                try
                {
                    Object plugin = Module::import(moduleName);
                    medInriaModule.attribute("loaded_plugins")[moduleName] = plugin;
                }
                catch (Exception& e)
                {
                    medInriaModule.attribute("failed_plugins")[moduleName] = Object(e.what());
                    qCritical() << QString("Error while loading Python plugin %1: %2")
                                   .arg(moduleName, e.what());
                }
            }
        }

        moduleInfo = coreFunction(PyIter_Next, *moduleIterator);
    }
}

} // namespace

void addPythonPath(QString path)
{
    Module sys = import("sys");
    Object pythonPath = sys.attribute("path");
    path = QDir::toNativeSeparators(path);

    if (!pythonPath.contains(Object(path)))
    {
        pythonPath.append(Object(path));
    }
}

void setUserPythonPaths(QStringList paths)
{
    medSettingsManager::instance()->setValue(PYTHON_SETTINGS_ID, USER_PATHS_SETTINGS_ID, paths);
}

QStringList getUserPythonPaths()
{
    return medSettingsManager::instance()->value(PYTHON_SETTINGS_ID, USER_PATHS_SETTINGS_ID).toStringList();
}

void loadPythonPlugins()
{
    try
    {
        tryLoadPlugins();
        Object failed_plugins = import(PROJECT_NAME).attribute("failed_plugins");
    }
    catch (Exception& e)
    {
        qCritical() << QString("Error while loading Python plugins: %1").arg(e.what());
    }
}

Object runSourceCode(QString sourceCode)
{
    Object globals = dict();
    coreFunction(PyRun_String, qUtf8Printable(sourceCode), Py_file_input, *globals, nullptr);
    return globals;
}

void print(QString text)
{
    PySys_FormatStdout(qUtf8Printable(text));
    propagateErrorIfOccurred();
}

} // namespace med::python
