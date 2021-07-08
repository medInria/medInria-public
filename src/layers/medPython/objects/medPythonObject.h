#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

/// This is the wrapper class for concrete Python objects. See the
/// AbstractObject header for more information.
///

#include "medPythonAbstractObject.h"
#include "medPythonExport.h"

namespace med::python
{

struct ObjectPrivate;

class MEDPYTHON_EXPORT Object : public AbstractObject
{
public:
    /// Converts a C++ object to Python using the associated overload of the
    /// medPythonConvert function.
    ///
    template <class TYPE>
    static Object create(const TYPE& value);

    /// Wraps a Python object and increase its reference count (the caller keeps
    /// ownership of 'reference').
    ///
    static Object borrowed(const PyObject* reference);

    /// Wraps a Python object without increasing its reference count (the caller
    /// loses ownership of 'reference'). If the reference is null then the
    /// Python None object is wrapped instead.
    ///
    Object(PyObject* reference = nullptr);

    /// Wraps the same Python object as the one wrapped by 'other'.
    ///
    Object(const Object& other);

    /// Wraps the same Python object as the one wrapped by 'other'.
    ///
    Object(const AbstractObject& other);

    /// Creates a wrapped Python integer.
    ///
    Object(int value);

    /// Creates a wrapped Python float.
    ///
    Object(double value);

    /// Creates a wrapped Python string.
    ///
    Object(QString value);

    virtual ~Object();

    using AbstractObject::operator=;

protected:
    ObjectPrivate* const d;

    PyObject* getReference() const override;
    void setReference(PyObject* reference) override;

private:
    void internalSetReference(PyObject* reference);
};

template <class TYPE>
Object Object::create(const TYPE& value)
{
    PyObject* reference;

    if (!medPythonConvert(value, &reference))
    {
        propagateCurrentError();
    }

    return reference;
}

} // namespace med::python
