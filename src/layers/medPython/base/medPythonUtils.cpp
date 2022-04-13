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
