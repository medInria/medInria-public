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
MEDPYTHON_EXPORT bool initialize();

/// Terminates the Python interpreter and releases all allocated resources. This
/// function does nothing if the interpreter is not running.
///
MEDPYTHON_EXPORT bool finalize();

} // namespace med::python
