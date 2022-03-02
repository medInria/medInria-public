/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreAPI.h"

#include "medPythonQVariantConversion.h"

#include "medPythonConversion.h"
#include "medPythonError.h"
#include "medPythonSWIGCore.h"

bool medPythonConvert(const QVariant& value, PyObject** output)
{
    switch (static_cast<int>(value.type()))
    {
    case QMetaType::Bool:
    {
        return medPythonConvert(value.toBool(), output);
    }
    case QMetaType::Int:
    {
        return medPythonConvert(value.toInt(), output);
    }
    case QMetaType::Float:
    case QMetaType::Double:
    {
        return medPythonConvert(value.toDouble(), output);
    }
    case QMetaType::QString:
    {
        return medPythonConvert(value.toString(), output);
    }
    case QMetaType::QVariantList:
    {
        return medPythonConvert(value.toList(), output);
    }
    case QMetaType::QStringList:
    {
        return medPythonConvert(value.toStringList(), output);
    }
    case QMetaType::QVariantHash:
    {
        return medPythonConvert(value.toHash(), output);
    }
    case QMetaType::QObjectStar:
    {
        *output = med::python::wrapObjectWithSWIG(value.value<QObject*>());
        return *output;
    }
    default:
        QVariant* valueCopy = new QVariant(value);
        *output = med::python::wrapObjectWithSWIG(valueCopy, "QVariant*", true);
        return *output;
    }
}

template <class TYPE>
bool medPythonConvert(const PyObject* object, QVariant* output)
{
    TYPE value;

    if (medPythonConvert(object, &value))
    {
        *output = QVariant::fromValue<TYPE>(value);
        return true;
    }
    else
    {
        return false;
    }
}

bool medPythonConvert(const PyObject* object, QVariant* output)
{
    bool success = false;

    if (PyBool_Check(object))
    {
        success = medPythonConvert<bool>(object, output);
    }
    else if (PyLong_Check(object))
    {
        success = medPythonConvert<long>(object, output);
    }
    else if (PyFloat_Check(object))
    {
        success = medPythonConvert<double>(object, output);
    }
    else if (PyUnicode_Check(object))
    {
        success = medPythonConvert<QString>(object, output);
    }
    else if (PyList_Check(object))
    {
        success = medPythonConvert<QList<QVariant> >(object, output);
    }
    else if (PyDict_Check(object))
    {
        success = medPythonConvert<QHash<QString, QVariant> >(object, output);
    }
    else if (med::python::isSWIGWrappedObject(object, "QObject*"))
    {
        QObject* value = (QObject*)med::python::extractSWIGWrappedObject(object);

        if (value)
        {
            *output = QVariant::fromValue<QObject*>(value);
            success = true;
        }
    }
    else if (med::python::isSWIGWrappedObject(object, "QVariant*"))
    {
        QVariant* value = (QVariant*)med::python::extractSWIGWrappedObject(object);

        if (value)
        {
            *output = *value;
            success = true;
        }
    }
    else
    {
        QString message = QString("Cannot convert Python object of type %1 to"
                                  " QVariant").arg(Py_TYPE(object)->tp_name);
        med::python::raiseError<med::python::TypeError>(message);
    }

    return success;
}
