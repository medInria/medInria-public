#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

// These templates are used to call the Python functions with automatic
// propagation of Python exceptions to C++.

#include "medPythonCore.h"
#include "medPythonError.h"

namespace med::python
{

template <class RETURN_TYPE, auto FUNCTION, class... ARGS>
inline RETURN_TYPE coreFunction(ARGS... args)
{
    auto result = FUNCTION(args...);
    propagateErrorIfOccured();
    return result;
}

template <auto FUNCTION, class... ARGS>
inline void coreFunctionNoReturn(ARGS... args)
{
    FUNCTION(args...);
    propagateErrorIfOccured();
}

} // namespace med::python
