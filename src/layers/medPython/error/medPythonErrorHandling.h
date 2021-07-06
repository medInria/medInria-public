#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

/// This is the API to raise, query and propagate Python errors from the wrapper
/// code.
///

#include <QString>

#include "medPythonCoreForward.h"
#include "medPythonExport.h"

namespace med::python
{

/// Raises a Python error. This is equivalent to the "raise" keyword in Python.
///
MEDPYTHON_EXPORT void raiseError(PyObject* nativeExceptionClass, QString message);

/// Raises the Python error corresponding to the C++ EXCEPTION_TYPE class.
/// (EXCEPTION_TYPE must be BaseException or a subclass).
///
template <class EXCEPTION_TYPE>
MEDPYTHON_EXPORT void raiseError(QString message)
{
    raiseError(EXCEPTION_TYPE::nativeClass(), message);
}

/// Checks if a Python error was raised. The corresponding exception can be
/// fetched with retrieveNativeException().
///
MEDPYTHON_EXPORT bool errorOccurred();

/// Clears the error indicator. Must be called every time an error is handled or
/// the next Python function will mistakingly believe there was an error (the
/// propagation functions do this automatically).
///
MEDPYTHON_EXPORT void clearError();

/// Checks if there was an error and throws it as a C++ exception
///
MEDPYTHON_EXPORT void propagateErrorIfOccurred();

/// Throws the current error as a C++ exception. Must only be called if there
/// was an error.
///
MEDPYTHON_EXPORT void propagateCurrentError();

/// Retrieves the Python exception corresponding to the current error.
///
MEDPYTHON_EXPORT PyObject* retrieveNativeException();

} // namespace med::python
