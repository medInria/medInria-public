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
#include <QDebug>

#include "medPythonCore.h"
#include "medPythonError.h"

namespace med::python
{

namespace
{

bool isRunning = false;

} // namespace

bool ensurePythonSetup()
{
    if (!isRunning)
    {
        qInfo() << "Setting up Python...";
        isRunning = setupCore();

        if (isRunning)
        {
            initializeExceptions();
            QApplication::connect(qApp, &QApplication::aboutToQuit, &ensurePythonTeardown);
        }
    }

    return isRunning;
}

bool ensurePythonTeardown()
{
    bool success = true;

    if (isRunning)
    {
        isRunning = false;
        finalizeExceptions();
        success = teardownCore();
    }

    return success;
}

} // namespace med::python
