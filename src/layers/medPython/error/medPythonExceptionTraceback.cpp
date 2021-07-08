/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonExceptionTraceback.h"

#include "medPythonCoreUtils.h"

namespace med::python
{

namespace
{

PyObject* tracebackModule = nullptr;

PyObject* getTracebackModule()
{
    if (!tracebackModule)
    {
        PyObject* tracebackString = PyUnicode_FromString("traceback");
        tracebackModule = PyImport_Import(tracebackString);
        Py_CLEAR(tracebackString);
        PyErr_Clear();
    }

    return tracebackModule;
}

QString getTracebackItem(PyObject* tracebackList, ssize_t index)
{
    QString result;
    PyObject* tracebackItem = PySequence_GetItem(tracebackList, index);

    if (tracebackItem)
    {
        PyObject* encodedTracebackItem = PyUnicode_AsEncodedString(tracebackItem, "UTF-8", "strict");
        Py_CLEAR(tracebackItem);

        if (encodedTracebackItem)
        {
            result = PyBytes_AS_STRING(encodedTracebackItem);
            Py_CLEAR(encodedTracebackItem);
         }
    }

    PyErr_Clear();
    return result;
}

QString concatenateTracebackList(PyObject* tracebackList)
{
    QString result;
    ssize_t numTracebackItems = PyObject_Length(tracebackList);

    for (ssize_t i = 0; i < numTracebackItems; i++)
    {
        result.append(getTracebackItem(tracebackList, i));
    }

    PyErr_Clear();
    return result;
}

PyObject* getTracebackList(PyObject* nativeException)
{
    PyObject* traceback = PyObject_GetAttrString(nativeException, "__traceback__");
    QString formatMethodName = traceback ? "format_exception" : "format_exception_only";
    PyObject* formatMethod = PyObject_GetAttrString(getTracebackModule(), qUtf8Printable(formatMethodName));
    PyObject* formatArgs;
    PyObject* tracebackList = nullptr;

    if (traceback)
    {
        formatArgs = Py_BuildValue("NNN", Py_TYPE(nativeException), nativeException, traceback);
    }
    else
    {
        formatArgs = Py_BuildValue("NN", Py_TYPE(nativeException), nativeException);
    }

    if (formatMethod && formatArgs)
    {
        tracebackList = PyObject_CallObject(formatMethod, formatArgs);
    }

    PyErr_Clear();
    Py_CLEAR(traceback);
    Py_CLEAR(formatMethod);
    Py_CLEAR(formatArgs);
    return tracebackList;
}

QString formatWithTracebackModule(PyObject* nativeException)
{
    PyObject* tracebackList = getTracebackList(nativeException);
    QString result = concatenateTracebackList(tracebackList);
    Py_CLEAR(tracebackList);
    return result;
}

} // namespace

QString formatExceptionTraceback(PyObject* nativeException)
{
    QString result;

    if (getTracebackModule())
    {
        result = formatWithTracebackModule(nativeException);
    }

    return result;
}

} // namespace med::python
