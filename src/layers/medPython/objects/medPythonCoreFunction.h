#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

/// These templates are used to call the Python functions from the object
/// wrapping API. They allow automatic propagation of errors and proper handling
/// of the global interpreter lock (not yet implemented).
///

#include "medPythonError.h"

namespace med::python
{

template <class RETURN_TYPE, auto FUNCTION, class... ARGS>
inline RETURN_TYPE coreFunction(ARGS... args)
{
    RETURN_TYPE result = (*FUNCTION)(std::forward<ARGS>(args)...);
    propagateErrorIfOccurred();
    return result;
}

template <auto FUNCTION, class... ARGS>
inline void coreFunction(ARGS... args)
{
    (*FUNCTION)(std::forward<ARGS>(args)...);
    propagateErrorIfOccurred();
}

} // namespace med::python
