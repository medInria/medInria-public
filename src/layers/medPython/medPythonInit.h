#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

/// The following functions handle the setup and teardown of the embedded Python
/// interpreter. These functions do not need to be called if one uses only the
/// wwrapping API, as the API ensures lazy loading of Python.
///

#include "medPythonExport.h"

namespace med::python
{

/// Launches the Python interpreter and connects the teardown function to the
/// application signal aboutToQuit(). This function does nothing if the
/// interpreter is already running.
///
MEDPYTHON_EXPORT bool ensurePythonSetup();

/// Terminates the Python interpreter and releases all allocated resources. This
/// function does nothing if the interpreter is not running.
///
MEDPYTHON_EXPORT bool ensurePythonTeardown();

/// Used for lazy loading. Do not call directly.
///
MEDPYTHON_EXPORT void lazyLoadPython();

} // namespace med::python
