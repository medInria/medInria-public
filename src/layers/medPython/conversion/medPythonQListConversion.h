#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include <QList>

#include "medPythonCoreForward.h"
#include "medPythonExport.h"

namespace med::python
{

template <class TYPE>
void clearList(QList<TYPE> list)
{
    list.clear();
}

template <class TYPE>
void clearList(QList<TYPE*> list)
{
    while (!list.isEmpty())
    {
        delete list.takeFirst();
    }
}

void clearList(QList<PyObject*> list);

} // namespace med::python

MEDPYTHON_EXPORT bool medPythonConvert(const QList<PyObject*>& qList, PyObject** output);

template <class TYPE>
MEDPYTHON_EXPORT bool medPythonConvert(const QList<TYPE>& qList, PyObject** output)
{
    bool success = true;
    QList<PyObject*> objectList;

    foreach (TYPE item, qList)
    {
        PyObject* pythonItem;

        if (medPythonConvert(item, &pythonItem))
        {
            objectList.append(pythonItem);
        }
        else
        {
            success = false;
            break;
        }
    }

    success = success && medPythonConvert(objectList, output);
    med::python::clearList(objectList);
    return success;
}

MEDPYTHON_EXPORT bool medPythonConvert(const PyObject* object, QList<PyObject*>* output);

template <class TYPE>
MEDPYTHON_EXPORT bool medPythonConvert(const PyObject* object, QList<TYPE>* output)
{
    bool success = true;
    QList<PyObject*> objectList;

    if (medPythonConvert(object, &objectList))
    {
        output->clear();

        foreach (PyObject* pythonItem, objectList)
        {
            TYPE item;

            if (medPythonConvert(pythonItem, &item))
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

        med::python::clearList(objectList);
    }

    return success;
}
