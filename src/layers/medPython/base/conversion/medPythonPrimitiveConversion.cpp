/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreAPI.h"

#include "medPythonPrimitiveConversion.h"

bool medPythonConvert(bool value, PyObject** output)
{
    *output = PyBool_FromLong(value);
    return *output;
}

bool medPythonConvert(const PyObject* object, bool* output)
{
    *output = PyObject_IsTrue(const_cast<PyObject*>(object));
    return !PyErr_Occurred();
}

bool medPythonConvert(int value, PyObject** output)
{
    *output = PyLong_FromLong(static_cast<long>(value));
    return *output;
}

bool medPythonConvert(const PyObject* object, long* output)
{
    *output = PyLong_AsLong(const_cast<PyObject*>(object));
    return !PyErr_Occurred();
}

bool medPythonConvert(double value, PyObject** output)
{
    *output = PyFloat_FromDouble(value);
    return *output;
}

bool medPythonConvert(const PyObject* object, double* output)
{
    *output = PyFloat_AsDouble(const_cast<PyObject*>(object));
    return !PyErr_Occurred();
}
