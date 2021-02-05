#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

// These wrappers are intended to call the CPython functions with automatic
// wrapping and propagation of Python exceptions.

#include "medPythonCore.h"
#include "medPythonError.h"

namespace med
{
namespace python
{
namespace core
{

template <class RETURN_TYPE, auto FUNCTION, class... ARGS>
inline RETURN_TYPE wrappedFunction(ARGS... args)
{
    auto result = FUNCTION(args...);
    propagateErrorIfOccured(MEDPYTHON_CODE_LOCATION);
    return result;
}

template <auto FUNCTION, class... ARGS>
inline void wrappedFunctionNoReturn(ARGS... args)
{
    FUNCTION(args...);
    propagateErrorIfOccured(MEDPYTHON_CODE_LOCATION);
}

} // namespace core
} // namespace python
} // namespace med
