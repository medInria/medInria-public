/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreInit.h"

#include <QDebug>

#include <medExternalResources.h>

#include "medPythonCoreAPI.h"
#include "medPythonCoreUtils.h"

namespace med::python::core
{

namespace
{

bool checkStatus(PyStatus status)
{
    bool success = !PyStatus_Exception(status);

    if (!success)
    {
        qWarning() << "Error during initialization of Python: " << status.err_msg;
    }

    return success;
}

bool preInitialize()
{
    PyPreConfig preConfig;
    PyPreConfig_InitIsolatedConfig(&preConfig);
    preConfig.utf8_mode = 1;
    return checkStatus(Py_PreInitialize(&preConfig));
}

bool getEmbeddedPaths(QString* modulePath, QString* libraryPath)
{
    bool success = true;
    *modulePath = med::getExternalResourcePath(STANDARD_PYTHON_MODULES_ZIP, TARGET_NAME);
    *libraryPath = med::getExternalResourcesDirectory(TARGET_NAME);

    if (modulePath->isEmpty())
    {
        qWarning() << "Cannot find the Python modules archive named "
                   << STANDARD_PYTHON_MODULES_ZIP << " in " << TARGET_NAME;
        success = false;
    }

    if (libraryPath->isEmpty())
    {
        qWarning() << "Cannot find the resources directory for " << TARGET_NAME;
        success = false;
    }

    return success;
}

bool prepareConfig(PyConfig* config)
{
    QString modulePath;
    QString libraryPath;
    bool success = getEmbeddedPaths(&modulePath, &libraryPath);

    if (success)
    {
        wchar_t** moduleSearchPaths = (wchar_t**)PyMem_RawMalloc(2 * sizeof(wchar_t*));
        moduleSearchPaths[0] = qStringToWideChar(modulePath);
        moduleSearchPaths[1] = qStringToWideChar(libraryPath);

        PyConfig_InitIsolatedConfig(config);

        success = checkStatus(PyConfig_SetBytesString(config, &config->program_name, TARGET_NAME))
                  && checkStatus(PyConfig_SetBytesString(config, &config->home, ""))
                  && checkStatus(PyConfig_SetBytesString(config, &config->prefix, ""))
                  && checkStatus(PyConfig_SetBytesString(config, &config->exec_prefix, ""))
          #if PYTHON_VERSION_MINOR >= 9
                  && checkStatus(PyConfig_SetBytesString(config, &config->platlibdir, qUtf8Printable(libraryPath)))
          #endif
                  && checkStatus(PyConfig_Read(config)) // required to avoid recalculation of module_search_paths during initialization
                  && checkStatus(PyConfig_SetWideStringList(config, &config->module_search_paths, 2, moduleSearchPaths));

        for (int i = 0; i < 2; i++)
        {
            PyMem_RawFree(moduleSearchPaths[i]);
        }

        PyMem_RawFree(moduleSearchPaths);
    }

    return success;
}

bool initializeFromConfig(PyConfig* config)
{
    return checkStatus(Py_InitializeFromConfig(config));
}

} // namespace

bool initializePython()
{
    PyConfig config;
    bool success = preInitialize()
                   && prepareConfig(&config)
                   && initializeFromConfig(&config);

    if (success)
    {
        qInfo() << "Python is initialized: " << Py_GetVersion();
        qInfo() << getPythonPath();
    }

    PyConfig_Clear(&config);
    return success;
}

bool finalizePython()
{
    bool success = true;

    // The Python setup might have failed at the symbol resolving step, so we
    // must make sure the function pointers are valid before using them.
    //
    if (Py_IsInitialized && Py_FinalizeEx && Py_IsInitialized())
    {
        success = (Py_FinalizeEx() == 0);
    }

    return success;
}

} // namespace med::python::core
