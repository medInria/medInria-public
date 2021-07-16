#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

/// This class represents a function call. The AbstractObject it implements is
/// the return value of the call. The actual call is only triggered when the
/// return value is needed (by any function that uses the value of the
/// object or when assigning this object to another), or, if the return value is
/// not used or assigned, when the instance goes out of scope. The 'kwargs'
/// function can be used to add keyword arguments to the call. For example, the
/// following Python code:
///
///    employee = db.getEmployee("John", "Doe", branch="marketing", category=2)
///
/// would be written in medPython as:
///
///    Object employee = db.callMethod("getEmployee", "John", "Doe")
///                                .kwarg("branch", "marketing")
///                                .kwarg("category", 2);
///

#include "medPythonExport.h"
#include "medPythonObject.h"
#include "medPythonStandardObjects.h"

namespace med::python
{

struct FunctionCallPrivate;

class MEDPYTHON_EXPORT FunctionCall : public AbstractObject
{
public:
    FunctionCall(const AbstractObject& callable, const AbstractObject& args);
    virtual ~FunctionCall();

    /// Add a keyword argument to the call
    ///
    FunctionCall& kwarg(QString name, const AbstractObject& value);

    /// Add a keyword argument to the call. The type must be eligible to the
    /// 'Object::create' function.
    ///
    template <class TYPE>
    FunctionCall& kwarg(QString name, const TYPE& value);

protected:
    PyObject* getReference() const override;
    void setReference(PyObject* reference) override;

private:
    FunctionCallPrivate* const d;

    PyObject* evaluate() const;
};

template <class TYPE>
FunctionCall& FunctionCall::kwarg(QString name, const TYPE& value)
{
    return kwarg(name, Object::create(value));
}

// The following AbstractObject template functions are defined here to avoid
// circular dependency issues:

template <class... ARGS>
FunctionCall AbstractObject::operator()(ARGS... args)
{
    QList<Object> argsList;
    (argsList << ... << Object::create(args)); // C++ fold expression
    return FunctionCall(*this, tuple(argsList));
}

template <class... ARGS>
FunctionCall AbstractObject::callMethod(QString name, ARGS... args)
{
    return getAttribute(name).operator()(args...);
}

} // namespace med::python
