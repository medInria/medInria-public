#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

// This is used to retrieve the traceback of an exception and convert it to a
// user-readable string.

#include <QString>

#include "medPythonCoreForward.h"

namespace med::python::exception_traceback
{

QString formatTraceback(PyObject* nativeException);

} // namespace med::python::exception_traceback
