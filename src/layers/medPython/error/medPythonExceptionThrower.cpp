/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonExceptionThrower.h"

#include <QHash>

#include "medPythonBaseException.h"
#include "medPythonCoreAPI.h"

namespace med::python
{

namespace
{

QHash<PyObject*, ExceptionThrower> exceptionThrowers;

// A native exception might be a subclass instance of one of the registered
// exception classes. To convert it we must go up the hierarchy to find the
// actual registered class.
//
PyObject* findRegisteredBaseClass(PyObject* nativeClass)
{
    PyObject* result = nullptr;
    PyObject* typeLineage = PyObject_GetAttrString(nativeClass, "__mro__"); // method resolution order

    if (!PyErr_Occurred() && PyTuple_Check(typeLineage))
    {
        ssize_t lineageSize = PyTuple_GET_SIZE(typeLineage);

        for (int i = 1; i < lineageSize; i++)
        {
            result = PyTuple_GET_ITEM(typeLineage, i);

            if (exceptionThrowers.contains(result))
            {
                break;
            }
        }
    }

    Py_CLEAR(typeLineage);
    return result;
}

} // namespace

void registerExceptionThrower(PyObject* nativeClass, ExceptionThrower thrower)
{
    exceptionThrowers[nativeClass] = thrower;
}

void clearExceptionThrowers()
{
    exceptionThrowers.clear();
}

void throwException(PyObject* nativeException)
{
    PyObject* nativeClass = reinterpret_cast<PyObject*>(Py_TYPE(nativeException));

    if (!exceptionThrowers.contains(nativeClass))
    {
        // The exception class is not registered so we search if a parent class
        // is registered.
        nativeClass = findRegisteredBaseClass(nativeClass);
    }

    if (exceptionThrowers.contains(nativeClass))
    {
        exceptionThrowers[nativeClass](nativeException);
    }
    else
    {
        // This exception is not a registered type, so throw it as a base
        // exception.
        throw BaseException(nativeException);
    }
}

} // namespace med::python
