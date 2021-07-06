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

/// Launches the Python interpreter. This will also connect the teardown
/// function to the application signal aboutToQuit().
///
MEDPYTHON_EXPORT bool setup();

/// Terminates the Python interpreter and releases all allocated resources.
MEDPYTHON_EXPORT bool teardown();

} // namespace med::python
