#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medPythonExport.h"

namespace med::python
{

// Initializes the Python interpreter. This will also connect the finalize()
// function to the application signal aboutToQuit().
MEDPYTHON_EXPORT void initialize();

// Releases all resources allocated by the Python interpreter. There is no need
// to call this function manually except to restart the interpreter.
MEDPYTHON_EXPORT void finalize();

} // namespace med::python
