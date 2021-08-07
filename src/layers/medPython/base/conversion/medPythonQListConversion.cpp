/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreAPI.h"

#include "medPythonQListConversion.h"

namespace med::python
{

void clearList(QList<PyObject*> list)
{
    while (!list.isEmpty())
    {
        PyObject* item = list.takeFirst();
        Py_CLEAR(item);
    }
}

} // namespace med::python

bool medPythonConvert(const QList<PyObject*>& qList, PyObject** output)
{
    *output = med::python::PyList_New(qList.length());

    if (*output)
    {
        for (ssize_t i = 0; i < qList.length(); i++)
        {
            PyObject* item = qList.at(i);
            Py_INCREF(item);
            PyList_SET_ITEM(*output, i, item);
        }

        return true;
    }
    else
    {
        return false;
    }
}

bool medPythonConvert(const PyObject* object, QList<PyObject*>* output)
{
    bool success = true;
    ssize_t numItems = med::python::PySequence_Size(const_cast<PyObject*>(object));

    if (numItems != -1)
    {
        for (ssize_t i = 0; i < numItems; i++)
        {
            PyObject* item = med::python::PySequence_GetItem(const_cast<PyObject*>(object), i);

            if (item)
            {
                output->append(item);
            }
            else
            {
                med::python::clearList(*output);
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
