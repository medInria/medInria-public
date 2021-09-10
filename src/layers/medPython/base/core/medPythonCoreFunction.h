#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

/// These templates are used to call the Python functions with automatic
/// propagation of errors and acquiring of the global interpreter lock (second
/// part not yet implemented).
///

#include "medPythonError.h"

namespace med::python
{

template <class FUNCTION_TYPE, class... ARGS>
inline std::invoke_result_t<FUNCTION_TYPE, ARGS...> coreFunction(FUNCTION_TYPE& function, ARGS... args)
{
    std::invoke_result_t<FUNCTION_TYPE, ARGS...> result = function(std::forward<ARGS>(args)...);
    propagateErrorIfOccurred();
    return result;
}

template <class FUNCTION_TYPE, class... ARGS, typename = std::enable_if_t<std::is_void_v<std::invoke_result_t<FUNCTION_TYPE, ARGS...> > > >
inline void coreFunction(FUNCTION_TYPE& function, ARGS... args)
{
    function(std::forward<ARGS>(args)...);
    propagateErrorIfOccurred();
}

} // namespace med::python
