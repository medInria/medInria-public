/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreAPI.h"

#include "medPythonFunctionCall.h"

#include "medPythonContainerAccessor.h"
#include "medPythonCoreFunction.h"
#include "medPythonError.h"

namespace med::python
{

struct FunctionCallPrivate
{
    Object callable;
    Object args;
    Object kwargs;
    Object result;
    bool evaluated;
};

FunctionCall::FunctionCall(const AbstractObject& callable, const AbstractObject& args) :
    d(new FunctionCallPrivate)
{
    d->callable = callable;
    d->args = args;
    d->kwargs = coreFunction<PyObject*, &PyDict_New>();
    d->evaluated = false;
}

FunctionCall::~FunctionCall()
{
    if (!d->evaluated)
    {
        PyObject* result = evaluate();
        Py_CLEAR(result);
        propagateCurrentError();
    }
}

FunctionCall& FunctionCall::kwarg(QString name, const AbstractObject& value)
{
    d->kwargs[name] = value;
    return *this;
}

PyObject* FunctionCall::getReference() const
{
    if (!d->evaluated)
    {
        PyObject* result = evaluate();
        d->evaluated = true;

        if (result)
        {
            d->result = result;
        }
        else
        {
            propagateCurrentError();
        }
    }

    return *d->result;
}

void FunctionCall::setReference(PyObject* Object)
{
    Py_CLEAR(Object);
    throw SyntaxError("Cannot assign to a function call");
}

PyObject* FunctionCall::evaluate() const
{
    PyObject* result = nullptr;
    ssize_t numArgs = PyObject_Size(*d->args);

    if (!PyErr_Occurred())
    {
        ssize_t numKwargs = PyObject_Size(*d->kwargs);

        if (!PyErr_Occurred())
        {
            if (numKwargs > 0)
            {
                result = PyObject_Call(*d->callable, *d->args, *d->kwargs);
            }
            else
            {
                if (numArgs > 0)
                {
                    result = PyObject_CallObject(*d->callable, *d->args);
                }
                else
                {
                    result = PyObject_CallNoArgs(*d->callable);
                }
            }
        }
    }

    return result;
}

} // namespace med::python
