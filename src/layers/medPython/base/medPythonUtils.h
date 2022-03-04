#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include <QString>

#include "medPythonObjects.h"
#include "medPythonExport.h"

namespace med::python
{

inline const char* PYTHON_SETTINGS_ID = "python";

/// Appends 'path' to sys.path.
/// (this function may throw Python-related exceptions)
///
MEDPYTHON_EXPORT void addPythonPath(QString path);

/// Specifies paths to be appended to sys.path at startup.
/// The paths are saved in the application settings.
///
MEDPYTHON_EXPORT void setStartupPythonPaths(QStringList paths);

/// Retrieves the list of paths that were added to the application settings
/// through the addPythonPath function.
///
MEDPYTHON_EXPORT QStringList getStartupPythonPaths();

/// Loads all top-level packages in sys.path that have the prefix 'medInria_' in
/// their name. All exceptions are handled. Successfully loaded plugins are
/// added to the 'loaded_plugins' dict of the medInria module ('med' in the
/// Python console) and failed plugin errors are added to 'failed_plugins' dict.
///
MEDPYTHON_EXPORT void loadPythonPlugins();

/// Executes Python code and returns a dict containing all global objects
/// resulting from the execution.
/// (this function may throw Python-related exceptions)
///
MEDPYTHON_EXPORT Object runSourceCode(QString sourceCode);

MEDPYTHON_EXPORT void print(QString text);

} // namespace med::python
