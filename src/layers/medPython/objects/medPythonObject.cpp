/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonObject.h"

#include "medPythonCoreAPI.h"

namespace med::python
{

struct ObjectPrivate
{
    PyObject* reference;
};

Object Object::borrowed(const PyObject* reference)
{
    Py_INCREF(reference);
    return const_cast<PyObject*>(reference);
}

Object::Object(PyObject* reference) :
    d(new ObjectPrivate)
{
    internalSetReference(reference);
}

Object::Object(const Object& other) :
    d(new ObjectPrivate)
{
    internalSetReference(other.newReference());
}

Object::Object(const AbstractObject& other) :
    d(new ObjectPrivate)
{
    internalSetReference(other.newReference());
}

Object::Object(int value) :
    d(new ObjectPrivate)
{
    PyObject* reference;

    if (!medPythonConvert(value, &reference))
    {
        propagateCurrentError();
    }

    internalSetReference(reference);
}

Object::Object(double value) :
    d(new ObjectPrivate)
{
    PyObject* reference;

    if (!medPythonConvert(value, &reference))
    {
        propagateCurrentError();
    }

    internalSetReference(reference);
}

Object::Object(QString value) :
    d(new ObjectPrivate)
{
    PyObject* reference;

    if (!medPythonConvert(value, &reference))
    {
        propagateCurrentError();
    }

    internalSetReference(reference);
}

Object::~Object()
{
    if (Py_IsInitialized())
    {
        Py_CLEAR(d->reference);
    }

    delete d;
}

PyObject* Object::getReference() const
{
    return d->reference;
}

void Object::setReference(PyObject* reference)
{
    PyObject* oldReference = d->reference;
    internalSetReference(reference);
    Py_CLEAR(oldReference);
}

void Object::internalSetReference(PyObject* reference)
{
    if (reference)
    {
        d->reference = reference;
    }
    else
    {
        d->reference = Py_None;
        Py_INCREF(d->reference);
    }
}

} // namespace med::python
