/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreAPI.h"

#include "medPythonQHashConversion.h"

bool medPythonConvert(const QHash<PyObject*, PyObject*>& qHash, PyObject** output)
{
    bool success = true;
    *output = med::python::PyDict_New();

    if (*output)
    {
        foreach (PyObject* key, qHash.keys())
        {
            PyObject* value = qHash.value(key);

            if (med::python::PyDict_SetItem(*output, key, value) == -1)
            {
                Py_CLEAR(*output);
                success = false;
                break;
            }
        }
    }
    else
    {
        success = false;
    }

    return success;
}

bool medPythonConvert(const PyObject* object, QHash<PyObject*, PyObject*>* output)
{
    bool success = true;
    PyObject* keys = med::python::PyMapping_Keys(const_cast<PyObject*>(object));

    if (keys)
    {
        ssize_t numItems = med::python::PySequence_Size(keys);

        if (numItems != -1)
        {
            for (ssize_t i = 0; i < numItems; i++)
            {
                PyObject* key = med::python::PySequence_GetItem(keys, i);

                if (key)
                {
                    PyObject* value = med::python::PyObject_GetItem(const_cast<PyObject*>(object), key);

                    if (value)
                    {
                        (*output)[key] = value;
                        continue;
                    }

                    Py_CLEAR(key);
                }

                success = false;
                break;
            }
        }
        else
        {
            success = false;
        }

        Py_CLEAR(keys);
    }
    else
    {
        success = false;
    }

    return success;
}

