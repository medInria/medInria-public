#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include <QHash>

#include "medPythonCoreForward.h"
#include "medPythonExport.h"
#include "medPythonQListConversion.h"

namespace med::python
{

template <class KEY_TYPE, class VALUE_TYPE>
void clearHash(QHash<KEY_TYPE, VALUE_TYPE> hash)
{
    QList<KEY_TYPE> keys = hash.keys();
    QList<VALUE_TYPE> values = hash.values();
    hash.clear();
    clearList(keys);
    clearList(values);
}

} // namespace med::python

MEDPYTHON_EXPORT bool medPythonConvert(const QHash<PyObject*, PyObject*>& qHash, PyObject** output);

template <class KEY_TYPE, class VALUE_TYPE>
MEDPYTHON_EXPORT bool medPythonConvert(const QHash<KEY_TYPE, VALUE_TYPE>& qHash, PyObject** output)
{
    bool success = true;
    QHash<PyObject*, PyObject*> objectHash;

    foreach (KEY_TYPE key, qHash.keys())
    {
        PyObject* pythonKey;

        if (medPythonConvert(key, &pythonKey))
        {
            PyObject* pythonValue;

            if (medPythonConvert(qHash.value(key), &pythonValue))
            {
                objectHash[pythonKey] = pythonValue;
                continue;
            }

            med::python::decref(pythonKey);
        }

        success = false;
        break;
    }

    success = success && medPythonConvert(objectHash, output);
    med::python::clearHash(objectHash);
    return success;
}

MEDPYTHON_EXPORT bool medPythonConvert(const PyObject* object, QHash<PyObject*, PyObject*>* output);

template <class KEY_TYPE, class VALUE_TYPE>
MEDPYTHON_EXPORT bool medPythonConvert(const PyObject* object, QHash<KEY_TYPE, VALUE_TYPE>* output)
{
    bool success = true;
    QHash<PyObject*, PyObject*> objectHash;

    if (medPythonConvert(object, &objectHash))
    {
        output->clear();
        QHash<PyObject*, PyObject*>::iterator hashItem;

        for (hashItem = objectHash.begin(); hashItem != objectHash.end(); hashItem++)
        {
            KEY_TYPE key;

            if (medPythonConvert(hashItem.key(), &key))
            {
                VALUE_TYPE value;

                if (medPythonConvert(hashItem.value(), &value))
                {
                    (*output)[key] = value;
                    continue;
                }
                else
                {
                    // add the key with a default value to the hash so that
                    // clearHash takes care of deleting it properly.
                    (*output)[key];
                }
            }

            success = false;
            break;
        }

        med::python::clearHash(objectHash);

        if (!success)
        {
            med::python::clearHash(*output);
        }
    }

    return success;
}
