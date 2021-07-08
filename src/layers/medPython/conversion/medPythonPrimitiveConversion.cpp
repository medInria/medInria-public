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
    *output = med::python::PyBool_FromLong(value);
    return *output;
}

bool medPythonConvert(const PyObject* object, bool* output)
{
    *output = med::python::PyObject_IsTrue(const_cast<PyObject*>(object));
    return !med::python::PyErr_Occurred();
}

bool medPythonConvert(int value, PyObject** output)
{
    *output = med::python::PyLong_FromLong(static_cast<long>(value));
    return *output;
}

bool medPythonConvert(const PyObject* object, long* output)
{
    *output = med::python::PyLong_AsLong(const_cast<PyObject*>(object));
    return !med::python::PyErr_Occurred();
}

bool medPythonConvert(double value, PyObject** output)
{
    *output = med::python::PyFloat_FromDouble(value);
    return *output;
}

bool medPythonConvert(const PyObject* object, double* output)
{
    *output = med::python::PyFloat_AsDouble(const_cast<PyObject*>(object));
    return !med::python::PyErr_Occurred();
}
