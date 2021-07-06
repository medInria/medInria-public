/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonErrorHandling.h"

#include "medPythonCoreAPI.h"
#include "medPythonExceptionThrower.h"
#include <QDebug>
namespace med::python
{

void raiseError(PyObject* nativeExceptionClass, QString message)
{
    PyErr_SetString(nativeExceptionClass, qUtf8Printable(message));
}

bool errorOccurred()
{
    return PyErr_Occurred();
}

void clearError()
{
    PyErr_Clear();
}

void propagateErrorIfOccurred()
{
    if (PyErr_Occurred())
    {
        propagateCurrentError();
    }
}

void propagateCurrentError()
{
    PyObject* nativeException = retrieveNativeException();
    PyErr_Clear();
    throwException(nativeException);
}

PyObject* retrieveNativeException()
{
    PyObject* exceptionClass;
    PyObject* exceptionInstance;
    PyObject* traceback;

    PyErr_Fetch(&exceptionClass, &exceptionInstance, &traceback);
    PyErr_NormalizeException(&exceptionClass, &exceptionInstance, &traceback);

    if (traceback)
    {
        PyException_SetTraceback(exceptionInstance, traceback);
    }

    Py_CLEAR(exceptionClass);
    Py_CLEAR(traceback);

    return exceptionInstance;
}

} // namespace med::python
