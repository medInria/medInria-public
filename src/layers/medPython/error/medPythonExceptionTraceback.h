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

#include <medPythonCoreAPI.h>

namespace med::python
{

/// Retrieves the traceback of an exception and converts it to a user-readable
/// string.
///
QString formatExceptionTraceback(PyObject* nativeException);

} // namespace med::python
