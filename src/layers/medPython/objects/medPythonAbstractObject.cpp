/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreAPI.h"

#include "medPythonAbstractObject.h"

#include "medPythonCoreFunction.h"
#include "medPythonObject.h"

namespace med::python
{

AbstractObject& AbstractObject::operator=(const AbstractObject& other)
{
    setReference(other.newReference());
    return *this;
}

AbstractObject& AbstractObject::operator=(PyObject* reference)
{
    setReference(reference);
    return *this;
}

void AbstractObject::borrow(const PyObject* reference)
{
    Py_INCREF(reference);
    setReference(const_cast<PyObject*>(reference));
}

PyObject* AbstractObject::operator*() const
{
    return getReference();
}

PyObject* AbstractObject::newReference() const
{
    PyObject* reference = getReference();
    Py_INCREF(reference);
    return reference;
}

Object AbstractObject::type() const
{
    return coreFunction<PyObject*, &PyObject_Type>(**this);
}

QString AbstractObject::typeName() const
{
    return Py_TYPE(**this)->tp_name;
}

bool AbstractObject::hasAttribute(QString name) const
{
    return coreFunction<int, &PyObject_HasAttrString>(**this, qUtf8Printable(name));
}

Object AbstractObject::getAttribute(QString name) const
{
    return coreFunction<PyObject*, &PyObject_GetAttrString>(**this, qUtf8Printable(name));
}

void AbstractObject::setAttribute(QString name, AbstractObject& value)
{
    coreFunction<&PyObject_SetAttrString>(**this, qUtf8Printable(name), *value);
}

QList<QString> AbstractObject::dir() const
{
    Object dir = coreFunction<PyObject*, &PyObject_Dir>(**this);
    return dir.convert<QList<QString> >();
}

AbstractObject::operator bool() const
{
    return coreFunction<bool, &PyObject_IsTrue>(**this);
}

bool AbstractObject::operator!() const
{
    return coreFunction<int, &PyObject_Not>(**this);
}

bool AbstractObject::operator==(const AbstractObject& other) const
{
    return coreFunction<bool, &PyObject_RichCompareBool>(**this, *other, Py_EQ);
}

bool AbstractObject::operator!=(const AbstractObject& other) const
{
    return coreFunction<int, &PyObject_RichCompareBool>(**this, *other, Py_NE);
}

bool AbstractObject::operator<(const AbstractObject& other) const
{
    return coreFunction<int, &PyObject_RichCompareBool>(**this, *other, Py_LT);
}

bool AbstractObject::operator>(const AbstractObject& other) const
{
    return coreFunction<int, &PyObject_RichCompareBool>(**this, *other, Py_GT);
}

bool AbstractObject::operator<=(const AbstractObject& other) const
{
    return coreFunction<int, &PyObject_RichCompareBool>(**this, *other, Py_LE);
}

bool AbstractObject::operator>=(const AbstractObject& other) const
{
    return coreFunction<int, &PyObject_RichCompareBool>(**this, *other, Py_GE);
}

Object AbstractObject::operator+(const AbstractObject& other) const
{
    return coreFunction<PyObject*, &PyNumber_Add>(**this, *other);
}

Object AbstractObject::operator-(const AbstractObject& other) const
{
    return coreFunction<PyObject*, &PyNumber_Subtract>(**this, *other);
}

Object AbstractObject::operator*(const AbstractObject& other) const
{
    return coreFunction<PyObject*, &PyNumber_Multiply>(**this, *other);
}

Object AbstractObject::operator/(const AbstractObject& other) const
{
    return coreFunction<PyObject*, &PyNumber_TrueDivide>(**this, *other);
}

AbstractObject& AbstractObject::operator+=(const AbstractObject& other)
{
    setReference(coreFunction<PyObject*, &PyNumber_InPlaceAdd>(**this, *other));
    return *this;
}

AbstractObject& AbstractObject::operator-=(const AbstractObject& other)
{
    setReference(coreFunction<PyObject*, &PyNumber_InPlaceSubtract>(**this, *other));
    return *this;
}

AbstractObject& AbstractObject::operator*=(const AbstractObject& other)
{
    setReference(coreFunction<PyObject*, &PyNumber_InPlaceMultiply>(**this, *other));
    return *this;
}

AbstractObject& AbstractObject::operator/=(const AbstractObject& other)
{
    setReference(coreFunction<PyObject*, &PyNumber_InPlaceTrueDivide>(**this, *other));
    return *this;
}

ssize_t AbstractObject::length() const
{
    return coreFunction<ssize_t, &PyObject_Size>(**this);
}

bool AbstractObject::contains(const AbstractObject& object) const
{
    if (PyMapping_Check(**this))
    {
        return coreFunction<int, &PyMapping_HasKey>(**this, *object);
    }
    else if (PySequence_Check(**this))
    {
        return coreFunction<int, &PySequence_Contains>(**this, *object);
    }
    else
    {
        unsupportedFunctionError(__func__);
        return false; // never reached
    }
}

void AbstractObject::append(const AbstractObject& item)
{
    if (PyList_Check(**this))
    {
        coreFunction<&PyList_Append>(**this, *item);
    }
    else
    {
        unsupportedFunctionError(__func__);
    }
}

Object AbstractObject::keys() const
{
    return coreFunction<PyObject*, &PyMapping_Keys>(**this);
}

Object AbstractObject::values() const
{
    return coreFunction<PyObject*, &PyMapping_Values>(**this);
}

void AbstractObject::update(const AbstractObject& other)
{
    if (PyDict_Check(**this))
    {
        coreFunction<&PyDict_Merge>(**this, *other, 1);
    }
    else
    {
        unsupportedFunctionError(__func__);
    }
}

void AbstractObject::unsupportedFunctionError(QString functionName) const
{
    throw TypeError(QString("%1 does not support %2").arg(typeName(), functionName));
}

} // namespace med::python
