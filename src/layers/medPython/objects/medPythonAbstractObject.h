#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

/// This is the main class for the Python wrapping API. The goal of the API is
/// to automate reference counting and error checking, and to provide a
/// simplified way to use Python from C++. Most of the functions in this class
/// are direct wrappers of identically-named Python functions, and it is best to
/// consult the Python documentation for their use. Any Python function not
/// wrapped here can be called using callMethod.
///
/// IMPORTANT: Instances of this class (and derived) are intended to be handled
/// by value.
///

#include <QList>
#include <QString>

#include "medPythonConversion.h"
#include "medPythonCore.h"
#include "medPythonError.h"
#include "medPythonExport.h"

namespace med::python
{

class ContainerAccessor;
class FunctionCall;
class Object;

class MEDPYTHON_EXPORT AbstractObject
{
public:
    /// Replaces the wrapped object with the one wrapped by 'other'.
    ///
    virtual AbstractObject& operator=(const AbstractObject& other);

    /// Replaces the wrapped object with 'reference'. This function takes
    /// ownership of 'reference' (it does not increase the reference count). If
    /// 'reference' is null then the Python 'None' object is wrapped instead.
    ///
    virtual AbstractObject& operator=(PyObject* reference);

    /// Replaces the wrapped object with 'reference'. This function does not
    /// take ownership of 'reference' (it increases the reference count). If
    /// 'reference' is null then the Python 'None' object is wrapped instead.
    ///
    void borrow(const PyObject* reference);

    /// Returns the wrapped Python reference without increasing the reference
    /// count.
    ///
    PyObject* operator*() const;

    /// Returns the wrapped Python reference with an increased reference count
    /// (the caller takes ownership of the returned reference).
    ///
    PyObject* newReference() const;

    Object type() const;
    QString typeName() const;

    bool hasAttribute(QString name) const;
    Object getAttribute(QString name) const;
    void setAttribute(QString name, AbstractObject& value);

    /// Equivalent to the Python expression 'dir(o)'
    ///
    QList<QString> dir() const;

    operator bool() const;
    bool operator!() const;

    bool operator==(const AbstractObject& other) const;
    bool operator!=(const AbstractObject& other) const;
    bool operator<(const AbstractObject& other) const;
    bool operator>(const AbstractObject& other) const;
    bool operator<=(const AbstractObject& other) const;
    bool operator>=(const AbstractObject& other) const;

    Object operator+(const AbstractObject& other) const;
    Object operator-(const AbstractObject& other) const;
    Object operator*(const AbstractObject& other) const;
    Object operator/(const AbstractObject& other) const;

    AbstractObject& operator+=(const AbstractObject& other);
    AbstractObject& operator-=(const AbstractObject& other);
    AbstractObject& operator*=(const AbstractObject& other);
    AbstractObject& operator/=(const AbstractObject& other);

    /// Equivalent to the Python expression 'len(o)'
    ///
    ssize_t length() const;

    /// Equivalent to the Python expression 'o1 in o2'
    ///
    bool contains(const AbstractObject& object) const;

    void append(const AbstractObject& item);

    Object keys() const;
    Object values() const;

    void update(const AbstractObject& other);

    /// Converts the Python object to a C++ type using the associated overload
    /// of medPythonConvert. The caller owns the returned object.
    ///
    template <class TYPE>
    TYPE convert();

    // Due to ciruclar dependency issues, the following three template functions
    // are defined in the header file of their respective return types.

    template <class ARG>
    ContainerAccessor operator[](const ARG& key);

    template <class... ARGS>
    FunctionCall operator()(ARGS... args);

    template <class... ARGS>
    FunctionCall callMethod(QString name, ARGS... args);

protected:
    virtual PyObject* getReference() const = 0;
    virtual void setReference(PyObject* reference) = 0;

    void unsupportedFunctionError(QString functionName) const;
};

template <class TYPE>
TYPE AbstractObject::convert()
{
    TYPE result;

    if (!medPythonConvert(**this, &result))
    {
        propagateCurrentError();
    }

    return result;
}

} // namespace med::python
