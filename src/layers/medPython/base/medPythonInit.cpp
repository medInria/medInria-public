/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonInit.h"

#include <QApplication>

#include "medPython.h"
#include "medPythonCoreInit.h"

namespace med::python
{

bool initialize()
{
    bool success = true;

    if (!isRunning())
    {
        success = initializeInterpreter(getUserPythonPaths());

        if (success)
        {
            initializeExceptions();
            QApplication::connect(qApp, &QApplication::aboutToQuit, &finalize);
        }
    }

    return success;
}

bool finalize()
{
    bool success = true;

    if (isRunning())
    {
        finalizeExceptions();
        success = finalizeInterpreter();
    }

    return success;
}

bool isRunning()
{
    return isInterpreterInitialized();
}

} // namespace med::python
