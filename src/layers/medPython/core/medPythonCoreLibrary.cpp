/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

// Must be included first (see the header notes for details)
#include "medPythonCoreAPI.h"

#include "medPythonCoreLibrary.h"

#include <QDebug>
#include <QLibrary>

#include <medExternalResources.h>

namespace med::python::core
{

namespace
{

QLibrary coreLibrary;

QString getPythonLibraryPath()
{
    return med::getExternalResourcePath(PYTHON_MAIN_LIBRARY, TARGET_NAME);
}

bool loadCoreLibrary()
{
    coreLibrary.setFileName(getPythonLibraryPath());
    bool success = coreLibrary.load();

    if (success)
    {
        qInfo() << "Successfully loaded the Python library: " << coreLibrary.fileName();
    }
    else
    {
        qWarning() << "Error loading the Python library: " << coreLibrary.errorString();
    }

    return success;
}

bool unloadCoreLibrary()
{
    bool success = true;

    if (coreLibrary.isLoaded())
    {
        success = coreLibrary.unload();
    }

    return success;
}

bool resolveSymbol(QString symbolName, QFunctionPointer* symbolPointer)
{
    bool success = true;
    *symbolPointer = coreLibrary.resolve(qUtf8Printable(symbolName));

    if (*symbolPointer == nullptr)
    {
        qWarning() << "Could not resolve symbol: " << symbolName;
        success = false;
    }

    return success;
}

template <typename SYMBOL_TYPE>
bool resolveSymbol(QString symbolName, SYMBOL_TYPE* symbolPointer)
{
    return resolveSymbol(symbolName, (QFunctionPointer*)(symbolPointer));
}

bool resolveSymbols()
{
    qInfo() << "Resolving CPython symbols";

    // Bitwise AND is used to prevent short-circuit evaluation, so that all the
    // failed symbols are printed instead of just the first one.
    //
    return resolveSymbol("PyStatus_Exception", &PyStatus_Exception)
            & resolveSymbol("PyConfig_Read", &PyConfig_Read)
            & resolveSymbol("PyConfig_Clear", &PyConfig_Clear)
            & resolveSymbol("PyConfig_SetBytesString", &PyConfig_SetBytesString)
            & resolveSymbol("PyConfig_SetWideStringList", &PyConfig_SetWideStringList)
            & resolveSymbol("PyPreConfig_InitIsolatedConfig", &PyPreConfig_InitIsolatedConfig)
            & resolveSymbol("PyConfig_InitIsolatedConfig", &PyConfig_InitIsolatedConfig)
            & resolveSymbol("Py_PreInitialize", &Py_PreInitialize)
            & resolveSymbol("Py_InitializeFromConfig", &Py_InitializeFromConfig)
            & resolveSymbol("Py_IsInitialized", &Py_IsInitialized)
            & resolveSymbol("Py_FinalizeEx", &Py_FinalizeEx)
            & resolveSymbol("Py_GetVersion", &Py_GetVersion)
            & resolveSymbol("Py_SetPythonHome", &Py_SetPythonHome)
            & resolveSymbol("Py_GetPythonHome", &Py_GetPythonHome)
            & resolveSymbol("Py_GetPrefix", &Py_GetPrefix)
            & resolveSymbol("Py_GetExecPrefix", &Py_GetExecPrefix)
            & resolveSymbol("Py_GetPath", &Py_GetPath)
            & resolveSymbol("Py_SetPath", &Py_SetPath)
            & resolveSymbol("PyMem_Free", &PyMem_Free)
            & resolveSymbol("PyMem_RawMalloc", &PyMem_RawMalloc)
            & resolveSymbol("PyMem_RawFree", &PyMem_RawFree)
            & resolveSymbol("Py_EncodeLocale", &Py_EncodeLocale)
            & resolveSymbol("Py_DecodeLocale", &Py_DecodeLocale)
            & resolveSymbol("PyRun_SimpleString", &PyRun_SimpleString);
}

} // namespace

bool setupCoreLibrary()
{
    return loadCoreLibrary() && resolveSymbols();
}

bool teardownCoreLibrary()
{
    return unloadCoreLibrary();
}

} // namespace med::python::core
