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

const char* STARTUP_PATHS_SETTINGS_ID = "startup_paths";

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

void setStartupPythonPaths(QStringList paths)
{
    medSettingsManager::instance()->setValue(PYTHON_SETTINGS_ID, STARTUP_PATHS_SETTINGS_ID, paths);
}

QStringList getStartupPythonPaths()
{
    return medSettingsManager::instance()->value(PYTHON_SETTINGS_ID, STARTUP_PATHS_SETTINGS_ID).toStringList();
}

void loadPythonPlugins()
{
    Module pkgutil = import("pkgutil");
    Object moduleIterator = pkgutil.callMethod("iter_modules");
    Object moduleInfo = coreFunction(PyIter_Next, *moduleIterator);

    while (moduleInfo)
    {
        QString moduleName = moduleInfo[1].convert<QString>();

        if (moduleName.startsWith(PYTHON_PLUGIN_PREFIX))
        {
            Module::import(moduleName);
        }

        moduleInfo = coreFunction(PyIter_Next, *moduleIterator);
    }
}

Object runSourceCode(QString sourceCode)
{
    Object globals = dict();
    coreFunction(PyRun_String, qUtf8Printable(sourceCode), Py_file_input, *globals, nullptr);
    return globals;
}

} // namespace med::python
