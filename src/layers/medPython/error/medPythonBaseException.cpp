/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonBaseException.h"

#include "medPythonCoreAPI.h"
#include "medPythonExceptionTraceback.h"

namespace med::python
{

struct BaseExceptionPrivate
{
    QString message;
};

PyObject* BaseException::nativeClass()
{
    return PyExc_BaseException;
}

BaseException::BaseException(QString message) :
    d(new BaseExceptionPrivate)
{
    PyObject* nativeException = createNativeException(nativeClass(), message);
    initializeFromNativeException(nativeException);
}

BaseException::BaseException(PyObject* nativeException) :
    d(new BaseExceptionPrivate)
{
    initializeFromNativeException(nativeException);
}

BaseException::~BaseException()
{
    delete d;
}

const char* BaseException::what() const throw()
{
    return qUtf8Printable(d->message);
}

PyObject* BaseException::createNativeException(PyObject* nativeClass, QString message)
{
    PyObject* args = Py_BuildValue("(s)", qUtf8Printable(message));
    PyObject* nativeException = PyObject_CallObject(nativeClass, args);
    PyErr_Clear();
    Py_CLEAR(args);
    return nativeException;
}

void BaseException::initializeFromNativeException(PyObject* nativeException)
{
    if (nativeException)
    {
        d->message = formatExceptionTraceback(nativeException);
        Py_CLEAR(nativeException);
    }
    else
    {
        d->message = "Undefined error";
    }
}

} // namespace med::python
