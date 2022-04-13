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
#include "medPythonCoreAPI.h"
#include "medPythonCoreInit.h"

namespace med::python
{

bool initializeCore()
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

bool initializeToolsAndPlugins()
{
    bool success = false;

    if (isRunning())
    {
        try
        {
            import("medPythonTools").callMethod("initialize");

            // Allow user to query info on successful and failed plugins.
            QString command = "from medPythonTools import pluginsInfo";
            coreFunction(PyRun_SimpleString, qUtf8Printable(command));

            success = true;
        }
        catch (Exception& e)
        {
            qCritical() << QString("Error during initialization of the Python components: %1").arg(e.what());
        }
    }

    return success;
}

bool finalize()
{
    bool success = true;

    if (isRunning())
    {
        try
        {
            import("medPythonTools").callMethod("finalize");
        }
        catch (Exception& e)
        {
            qCritical() << QString("Error during finalization of the Python components: %1").arg(e.what());
            success = false;
        }

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
