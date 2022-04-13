#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

/// These functions are used to propagate Python exceptions. Each Python
/// exception class is registered with a function that converts instances to a
/// corresponding C++ class and throws them.
///

#include "medPythonCoreForward.h"

namespace med::python
{

typedef void (*ExceptionThrower)(PyObject* nativeException);

void registerExceptionThrower(PyObject* nativeClass, ExceptionThrower thrower);

template <class EXCEPTION>
void registerExceptionThrower()
{
    ExceptionThrower thrower = [](PyObject* nativeException)
    {
        throw EXCEPTION(nativeException);
    };

    registerExceptionThrower(EXCEPTION::nativeClass(), thrower);
}

void clearExceptionThrowers();

/// Converts a native Python exception to an equivalent C++ exception and throws
/// it. (The input reference is stolen)
//
void throwException(PyObject* nativeException);

} // namespace med::python
