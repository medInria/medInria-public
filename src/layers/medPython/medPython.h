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

namespace med
{
namespace python
{

// After initialization, this will connect the finalize() method to the
// application signal aboutToQuit().
MEDPYTHON_EXPORT void initialize();

// Does not need to be called unless the python interpreter must be stopped or
// restarted.
MEDPYTHON_EXPORT void finalize();

} // namespace python
} // namespace med
