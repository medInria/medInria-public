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

#include "medPythonCore.h"
#include "medPythonError.h"
#include "medPythonUtils.h"

namespace med::python
{

namespace
{

bool isRunning = false;

} // namespace

bool initialize()
{
    if (!isRunning)
    {
        QStringList startupPaths = getStartupPythonPaths();
        isRunning = initializeInterpreter(startupPaths);

        if (isRunning)
        {
            initializeExceptions();
            QApplication::connect(qApp, &QApplication::aboutToQuit, &finalize);
        }
    }

    return isRunning;
}

bool finalize()
{
    bool success = true;

    if (isRunning)
    {
        isRunning = false;
        finalizeExceptions();
        success = finalizeInterpreter();
    }

    return success;
}

} // namespace med::python
