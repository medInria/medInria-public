#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

// These functions are used to propagate Python exceptions. Each Python
// exception class is registered with a function that converts instances to a
// corresponding C++ class and throws them.

#include "medPythonCoreForward.h"

namespace med::python::exception_thrower
{

typedef void (*ExceptionThrower)(PyObject* nativeException);

void registerThrower(PyObject* nativeClass, ExceptionThrower thrower);

template <class EXCEPTION>
void registerThrower()
{
    ExceptionThrower thrower = [](PyObject* nativeException)
    {
        throw EXCEPTION(nativeException);
    };

    registerThrower(EXCEPTION::nativeClass, thrower);
}

void clearThrowers();

void throwException(PyObject* nativeException);

} // namespace med::python::exception_thrower
