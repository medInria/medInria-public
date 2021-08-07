/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreAPI.h"

#include "medPythonQStringConversion.h"

bool medPythonConvert(const QString& value, PyObject** output)
{
    *output = med::python::PyUnicode_FromString(qUtf8Printable(value));
    return *output;
}

bool medPythonConvert(const PyObject* object, QString* output)
{
    PyObject* stringAsPyBytes = nullptr;

    if (PyBytes_Check(object))
    {
        stringAsPyBytes = const_cast<PyObject*>(object);
        Py_INCREF(stringAsPyBytes);
    }
    else if (PyUnicode_Check(const_cast<PyObject*>(object)))
    {
        stringAsPyBytes = med::python::PyUnicode_AsEncodedString(const_cast<PyObject*>(object), "UTF-8", "strict");
    }

    if (!med::python::PyErr_Occurred())
    {
        if (stringAsPyBytes)
        {
            *output = PyBytes_AS_STRING(stringAsPyBytes);
            Py_CLEAR(stringAsPyBytes);
            return true;
        }
        else
        {
            // The object is not a string, so apply str() and retry
            PyObject* objectAsString = med::python::PyObject_Str(const_cast<PyObject*>(object));

            if (!med::python::PyErr_Occurred())
            {
                bool success = medPythonConvert(objectAsString, output);
                Py_CLEAR(objectAsString);
                return success;
            }
        }
    }

    return false;
}
