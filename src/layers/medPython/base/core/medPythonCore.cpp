/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCore.h"

#include "medPythonCoreInit.h"

namespace med::python
{

bool setupCore()
{
    bool success = initializePython();

    if (!success)
    {
        teardownCore();
    }

    return success;
}

bool teardownCore()
{
    return finalizePython();
}

} // namespace med::python
