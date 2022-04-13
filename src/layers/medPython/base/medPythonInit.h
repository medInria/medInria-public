#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonExport.h"

namespace med::python
{

/// Launches the Python interpreter and connects the finalize function to the
/// application signal aboutToQuit(). This function does nothing if the
/// interpreter is already running.
///
MEDPYTHON_EXPORT bool initializeCore();

/// Initializes the Python tools module and loads the Python plugins.
///
MEDPYTHON_EXPORT bool initializeToolsAndPlugins();

/// Terminates the Python interpreter and releases all allocated resources. This
/// function does nothing if the interpreter is not running.
///
MEDPYTHON_EXPORT bool finalize();

/// Tests if the Python interpreter is running (i.e. has been initialized and
/// not finalized). If the interpreter is not running, using the Python API will
/// cause a crash.
///
MEDPYTHON_EXPORT bool isRunning();

} // namespace med::python
