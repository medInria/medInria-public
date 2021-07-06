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

namespace med::python
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
            & resolveSymbol("PyUnicode_FromString", &PyUnicode_FromString)
            & resolveSymbol("PyUnicode_AsEncodedString", &PyUnicode_AsEncodedString)
            & resolveSymbol("PyErr_NormalizeException", &PyErr_NormalizeException)
            & resolveSymbol("PyErr_Clear", &PyErr_Clear)
            & resolveSymbol("PyException_SetTraceback", &PyException_SetTraceback)
            & resolveSymbol("PyRun_SimpleString", &PyRun_SimpleString)
            & resolveSymbol("PyImport_Import", &PyImport_Import)
            & resolveSymbol("PyObject_Length", &PyObject_Length)
            & resolveSymbol("PyObject_GetAttrString", &PyObject_GetAttrString)
            & resolveSymbol("PyObject_CallObject", &PyObject_CallObject)
            & resolveSymbol("PySequence_GetItem", &PySequence_GetItem)
            & resolveSymbol("PyExc_BaseException", &PyExc_BaseException)
            & resolveSymbol("PyExc_Exception", &PyExc_Exception)
            & resolveSymbol("PyExc_GeneratorExit", &PyExc_GeneratorExit)
            & resolveSymbol("PyExc_KeyboardInterrupt", &PyExc_KeyboardInterrupt)
            & resolveSymbol("PyExc_SystemExit", &PyExc_SystemExit)
            & resolveSymbol("PyExc_ArithmeticError", &PyExc_ArithmeticError)
            & resolveSymbol("PyExc_FloatingPointError", &PyExc_FloatingPointError)
            & resolveSymbol("PyExc_OverflowError", &PyExc_OverflowError)
            & resolveSymbol("PyExc_ZeroDivisionError", &PyExc_ZeroDivisionError)
            & resolveSymbol("PyExc_ImportError", &PyExc_ImportError)
            & resolveSymbol("PyExc_ModuleNotFoundError", &PyExc_ModuleNotFoundError)
            & resolveSymbol("PyExc_LookupError", &PyExc_LookupError)
            & resolveSymbol("PyExc_IndexError", &PyExc_IndexError)
            & resolveSymbol("PyExc_KeyError", &PyExc_KeyError)
            & resolveSymbol("PyExc_NameError", &PyExc_NameError)
            & resolveSymbol("PyExc_UnboundLocalError", &PyExc_UnboundLocalError)
            & resolveSymbol("PyExc_OSError", &PyExc_OSError)
            & resolveSymbol("PyExc_BlockingIOError", &PyExc_BlockingIOError)
            & resolveSymbol("PyExc_ChildProcessError", &PyExc_ChildProcessError)
            & resolveSymbol("PyExc_ConnectionError", &PyExc_ConnectionError)
            & resolveSymbol("PyExc_BrokenPipeError", &PyExc_BrokenPipeError)
            & resolveSymbol("PyExc_ConnectionAbortedError", &PyExc_ConnectionAbortedError)
            & resolveSymbol("PyExc_ConnectionRefusedError", &PyExc_ConnectionRefusedError)
            & resolveSymbol("PyExc_ConnectionResetError", &PyExc_ConnectionResetError)
            & resolveSymbol("PyExc_FileExistsError", &PyExc_FileExistsError)
            & resolveSymbol("PyExc_FileNotFoundError", &PyExc_FileNotFoundError)
            & resolveSymbol("PyExc_InterruptedError", &PyExc_InterruptedError)
            & resolveSymbol("PyExc_IsADirectoryError", &PyExc_IsADirectoryError)
            & resolveSymbol("PyExc_NotADirectoryError", &PyExc_NotADirectoryError)
            & resolveSymbol("PyExc_PermissionError", &PyExc_PermissionError)
            & resolveSymbol("PyExc_ProcessLookupError", &PyExc_ProcessLookupError)
            & resolveSymbol("PyExc_TimeoutError", &PyExc_TimeoutError)
            & resolveSymbol("PyExc_RuntimeError", &PyExc_RuntimeError)
            & resolveSymbol("PyExc_NotImplementedError", &PyExc_NotImplementedError)
            & resolveSymbol("PyExc_RecursionError", &PyExc_RecursionError)
            & resolveSymbol("PyExc_SyntaxError", &PyExc_SyntaxError)
            & resolveSymbol("PyExc_IndentationError", &PyExc_IndentationError)
            & resolveSymbol("PyExc_TabError", &PyExc_TabError)
            & resolveSymbol("PyExc_ValueError", &PyExc_ValueError)
            & resolveSymbol("PyExc_UnicodeError", &PyExc_UnicodeError)
            & resolveSymbol("PyExc_UnicodeDecodeError", &PyExc_UnicodeDecodeError)
            & resolveSymbol("PyExc_UnicodeEncodeError", &PyExc_UnicodeEncodeError)
            & resolveSymbol("PyExc_UnicodeTranslateError", &PyExc_UnicodeTranslateError)
            & resolveSymbol("PyExc_AssertionError", &PyExc_AssertionError)
            & resolveSymbol("PyExc_AttributeError", &PyExc_AttributeError)
            & resolveSymbol("PyExc_BufferError", &PyExc_BufferError)
            & resolveSymbol("PyExc_EOFError", &PyExc_EOFError)
            & resolveSymbol("PyExc_MemoryError", &PyExc_MemoryError)
            & resolveSymbol("PyExc_ReferenceError", &PyExc_ReferenceError)
            & resolveSymbol("PyExc_StopAsyncIteration", &PyExc_StopAsyncIteration)
            & resolveSymbol("PyExc_StopIteration", &PyExc_StopIteration)
            & resolveSymbol("PyExc_SystemError", &PyExc_SystemError)
            & resolveSymbol("PyExc_TypeError", &PyExc_TypeError)
            & resolveSymbol("_Py_VaBuildValue_SizeT", &_Py_VaBuildValue_SizeT)
            & resolveSymbol("_Py_Dealloc", &_Py_Dealloc);
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

} // namespace med::python
