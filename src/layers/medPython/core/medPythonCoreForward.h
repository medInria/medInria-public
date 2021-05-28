#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

/// This header forwards and wraps some elements of the CPython library to avoid
/// having to include the Python headers outside of the module.
///

#include "medPythonExport.h"

typedef struct _object PyObject;

namespace med::python
{

/// Increases the reference count of a Python object if it is non-null
///
MEDPYTHON_EXPORT void incref(PyObject* object);

/// Decreases the reference count of a Python object if it is non-null
///
MEDPYTHON_EXPORT void decref(PyObject* object);

} // namespace med::python
