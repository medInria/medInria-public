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

#include "medPythonAttributeAccessor.h"
#include "medPythonConversionUtils.h"
#include "medPythonCoreFunction.h"
#include "medPythonObject.h"
#include "medPythonInit.h"
#include "medPythonSWIGCore.h"

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
    return coreFunction(PyObject_Type, **this);
}

QString AbstractObject::typeName() const
{
    return Py_TYPE(**this)->tp_name;
}

bool AbstractObject::hasAttribute(QString name) const
{
    return coreFunction(PyObject_HasAttrString, **this, qUtf8Printable(name));
}

AttributeAccessor AbstractObject::attribute(QString name)
{
    return AttributeAccessor(*this, name);
}

QList<QString> AbstractObject::dir() const
{
    Object dir = coreFunction(PyObject_Dir, **this);
    return dir.convert<QList<QString> >();
}

AbstractObject::operator bool() const
{
    return coreFunction(PyObject_IsTrue, **this);
}

bool AbstractObject::operator!() const
{
    return coreFunction(PyObject_Not, **this);
}

bool AbstractObject::operator==(const AbstractObject& other) const
{
    return coreFunction(PyObject_RichCompareBool, **this, *other, Py_EQ);
}

bool AbstractObject::operator!=(const AbstractObject& other) const
{
    return coreFunction(PyObject_RichCompareBool, **this, *other, Py_NE);
}

bool AbstractObject::operator<(const AbstractObject& other) const
{
    return coreFunction(PyObject_RichCompareBool, **this, *other, Py_LT);
}

bool AbstractObject::operator>(const AbstractObject& other) const
{
    return coreFunction(PyObject_RichCompareBool, **this, *other, Py_GT);
}

bool AbstractObject::operator<=(const AbstractObject& other) const
{
    return coreFunction(PyObject_RichCompareBool, **this, *other, Py_LE);
}

bool AbstractObject::operator>=(const AbstractObject& other) const
{
    return coreFunction(PyObject_RichCompareBool, **this, *other, Py_GE);
}

Object AbstractObject::operator+(const AbstractObject& other) const
{
    return coreFunction(PyNumber_Add, **this, *other);
}

Object AbstractObject::operator-(const AbstractObject& other) const
{
    return coreFunction(PyNumber_Subtract, **this, *other);
}

Object AbstractObject::operator*(const AbstractObject& other) const
{
    return coreFunction(PyNumber_Multiply, **this, *other);
}

Object AbstractObject::operator/(const AbstractObject& other) const
{
    return coreFunction(PyNumber_TrueDivide, **this, *other);
}

AbstractObject& AbstractObject::operator+=(const AbstractObject& other)
{
    setReference(coreFunction(PyNumber_InPlaceAdd, **this, *other));
    return *this;
}

AbstractObject& AbstractObject::operator-=(const AbstractObject& other)
{
    setReference(coreFunction(PyNumber_InPlaceSubtract, **this, *other));
    return *this;
}

AbstractObject& AbstractObject::operator*=(const AbstractObject& other)
{
    setReference(coreFunction(PyNumber_InPlaceMultiply, **this, *other));
    return *this;
}

AbstractObject& AbstractObject::operator/=(const AbstractObject& other)
{
    setReference(coreFunction(PyNumber_InPlaceTrueDivide, **this, *other));
    return *this;
}

size_t AbstractObject::length() const
{
    // The cast is safe because only positive values will be returned by this
    // function (an exception will be thrown if there is an error during the
    // call).
    return static_cast<size_t>(coreFunction(PyObject_Size, **this));
}

bool AbstractObject::contains(const AbstractObject& object) const
{
    if (coreFunction(PyMapping_Check, **this))
    {
        return coreFunction(PyMapping_HasKey, **this, *object);
    }
    else if (coreFunction(PySequence_Check, **this))
    {
        return coreFunction(PySequence_Contains, **this, *object);
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
        coreFunction(PyList_Append, **this, *item);
    }
    else
    {
        unsupportedFunctionError(__func__);
    }
}

Object AbstractObject::keys() const
{
    return coreFunction(PyMapping_Keys, **this);
}

Object AbstractObject::values() const
{
    return coreFunction(PyMapping_Values, **this);
}

void AbstractObject::update(const AbstractObject& other)
{
    if (PyDict_Check(**this))
    {
        coreFunction(PyDict_Merge, **this, *other, 1);
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

void* AbstractObject::cast(QString cppTypeName) const
{
    void* result = nullptr;

    if (isSWIGWrappedObject(**this, cppTypeName))
    {
        result = extractSWIGWrappedObject(**this);
    }

    propagateErrorIfOccurred();
    return result;
}

} // namespace med::python
