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

bool medPythonConvert(const QVariant& value, PyObject** output)
{
    switch (static_cast<int>(value.type()))
    {
    case QMetaType::Bool:
        return medPythonConvert(value.toBool(), output);
    case QMetaType::Int:
        return medPythonConvert(value.toInt(), output);
    case QMetaType::Float:
    case QMetaType::Double:
        return medPythonConvert(value.toDouble(), output);
    case QMetaType::QString:
        return medPythonConvert(value.toString(), output);
    case QMetaType::QVariantList:
        return medPythonConvert(value.toList(), output);
    case QMetaType::QStringList:
        return medPythonConvert(value.toStringList(), output);
    case QMetaType::QVariantHash:
        return medPythonConvert(value.toHash(), output);
    default:
        QString message = QString("Cannot convert QVariant of type %1 to Python")
                          .arg(value.typeName());
        med::python::raiseError<med::python::TypeError>(message);
        return false;
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
    if (PyBool_Check(object))
    {
        return medPythonConvert<bool>(object, output);
    }
    else if (PyLong_Check(object))
    {
        return medPythonConvert<long>(object, output);
    }
    else if (PyFloat_Check(object))
    {
        return medPythonConvert<double>(object, output);
    }
    else if (PyUnicode_Check(object))
    {
        return medPythonConvert<QString>(object, output);
    }
    else if (PyList_Check(object))
    {
        return medPythonConvert<QList<QVariant> >(object, output);
    }
    else if (PyDict_Check(object))
    {
        return medPythonConvert<QHash<QString, QVariant> >(object, output);
    }
    else
    {
        QString message = QString("Cannot convert Python object of type %1 to"
                                  " QVariant").arg(Py_TYPE(object)->tp_name);
        med::python::raiseError<med::python::TypeError>(message);
        return false;
    }
}
