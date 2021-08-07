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

#include "medExternalResources.h"

namespace med::python
{

namespace
{

QLibrary coreLibrary;

QString getPythonLibraryPath()
{
    return med::getExternalResourcePath(PYTHON_MAIN_LIBRARY, TARGET_NAME);
}

bool findCoreLibrary()
{
    QString libraryPath = getPythonLibraryPath();
    bool success = !libraryPath.isEmpty();

    if (success)
    {
        coreLibrary.setFileName(libraryPath);
    }
    else
    {
        qWarning() << QString("Cannot find the Python library named %1 in module %2").arg(PYTHON_MAIN_LIBRARY, TARGET_NAME);
    }

    return success;
}

bool loadCoreLibrary()
{
    coreLibrary.setLoadHints(QLibrary::ExportExternalSymbolsHint);
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

template <typename SYMBOL_TYPE>
bool resolvePointerSymbol(QString symbolName, SYMBOL_TYPE* symbolPointer)
{
    SYMBOL_TYPE* valueAddress;
    bool success = resolveSymbol(symbolName, (QFunctionPointer*)(&valueAddress));

    if (success)
    {
        *symbolPointer = *valueAddress;
    }

    return success;
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
            & resolveSymbol("PyErr_SetString", &PyErr_SetString)
            & resolveSymbol("PyErr_Occurred", &PyErr_Occurred)
            & resolveSymbol("PyErr_Fetch", &PyErr_Fetch)
            & resolveSymbol("PyErr_NormalizeException", &PyErr_NormalizeException)
            & resolveSymbol("PyErr_Clear", &PyErr_Clear)
            & resolveSymbol("PyException_SetTraceback", &PyException_SetTraceback)
            & resolveSymbol("PyRun_SimpleStringFlags", &PyRun_SimpleStringFlags)
            & resolveSymbol("PyType_IsSubtype", &PyType_IsSubtype)
            & resolveSymbol("PyObject_Type", &PyObject_Type)
            & resolveSymbol("PyObject_Size", &PyObject_Size)
            & resolveSymbol("PyObject_HasAttrString", &PyObject_HasAttrString)
            & resolveSymbol("PyObject_GetAttrString", &PyObject_GetAttrString)
            & resolveSymbol("PyObject_SetAttrString", &PyObject_SetAttrString)
            & resolveSymbol("PyObject_IsTrue", &PyObject_IsTrue)
            & resolveSymbol("PyObject_Not", &PyObject_Not)
            & resolveSymbol("PyObject_RichCompareBool", &PyObject_RichCompareBool)
            & resolveSymbol("PyObject_Str", &PyObject_Str)
            & resolveSymbol("PyObject_Dir", &PyObject_Dir)
            & resolveSymbol("PyObject_GetItem", &PyObject_GetItem)
            & resolveSymbol("PyObject_SetItem", &PyObject_SetItem)
            & resolveSymbol("PyObject_Call", &PyObject_Call)
            & resolveSymbol("PyObject_CallObject", &PyObject_CallObject)
            & resolveSymbol("PyObject_CallNoArgs", &PyObject_CallNoArgs)
            & resolveSymbol("_Py_NoneStruct", &Py_None)
            & resolveSymbol("PyBool_Type", &PyModule_Type)
            & resolveSymbol("PyBool_Type", &PyBool_Type)
            & resolveSymbol("PyBool_FromLong", &PyBool_FromLong)
            & resolveSymbol("PyLong_Type", &PyModule_Type)
            & resolveSymbol("PyLong_FromLong", &PyLong_FromLong)
            & resolveSymbol("PyLong_AsLong", &PyLong_AsLong)
            & resolveSymbol("PyFloat_Type", &PyFloat_Type)
            & resolveSymbol("PyFloat_FromDouble", &PyFloat_FromDouble)
            & resolveSymbol("PyFloat_AsDouble", &PyFloat_AsDouble)
            & resolveSymbol("PyNumber_Add", &PyNumber_Add)
            & resolveSymbol("PyNumber_Subtract", &PyNumber_Subtract)
            & resolveSymbol("PyNumber_Multiply", &PyNumber_Multiply)
            & resolveSymbol("PyNumber_TrueDivide", &PyNumber_TrueDivide)
            & resolveSymbol("PyNumber_InPlaceAdd", &PyNumber_InPlaceAdd)
            & resolveSymbol("PyNumber_InPlaceSubtract", &PyNumber_InPlaceSubtract)
            & resolveSymbol("PyNumber_InPlaceMultiply", &PyNumber_InPlaceMultiply)
            & resolveSymbol("PyNumber_InPlaceTrueDivide", &PyNumber_InPlaceTrueDivide)
            & resolveSymbol("PySequence_Check", &PySequence_Check)
            & resolveSymbol("PySequence_Size", &PySequence_Size)
            & resolveSymbol("PySequence_Contains", &PySequence_Contains)
            & resolveSymbol("PySequence_GetItem", &PySequence_GetItem)
            & resolveSymbol("PySequence_Tuple", &PySequence_Tuple)
            & resolveSymbol("PySequence_List", &PySequence_List)
            & resolveSymbol("PyMapping_Check", &PyMapping_Check)
            & resolveSymbol("PyMapping_HasKey", &PyMapping_HasKey)
            & resolveSymbol("PyMapping_Keys", &PyMapping_Keys)
            & resolveSymbol("PyMapping_Values", &PyMapping_Values)
            & resolveSymbol("PyTuple_Type", &PyModule_Type)
            & resolveSymbol("PyTuple_New", &PyTuple_New)
            & resolveSymbol("PyList_Type", &PyModule_Type)
            & resolveSymbol("PyList_New", &PyList_New)
            & resolveSymbol("PyList_Append", &PyList_Append)
            & resolveSymbol("PyDict_Type", &PyModule_Type)
            & resolveSymbol("PyDict_New", &PyDict_New)
            & resolveSymbol("PyDict_SetItem", &PyDict_SetItem)
            & resolveSymbol("PyDict_Merge", &PyDict_Merge)
            & resolveSymbol("PyModule_Type", &PyModule_Type)
            & resolveSymbol("PyModule_GetName", &PyModule_GetName)
            & resolveSymbol("PyModule_GetDict", &PyModule_GetDict)
            & resolveSymbol("PyImport_Import", &PyImport_Import)
            & resolveSymbol("PyImport_ImportModuleLevel", &PyImport_ImportModuleLevel)
            & resolveSymbol("PyImport_AddModule", &PyImport_AddModule)
            & resolveSymbol("PyImport_GetModule", &PyImport_GetModule)
            & resolvePointerSymbol("PyExc_BaseException", &PyExc_BaseException)
            & resolvePointerSymbol("PyExc_Exception", &PyExc_Exception)
            & resolvePointerSymbol("PyExc_GeneratorExit", &PyExc_GeneratorExit)
            & resolvePointerSymbol("PyExc_KeyboardInterrupt", &PyExc_KeyboardInterrupt)
            & resolvePointerSymbol("PyExc_SystemExit", &PyExc_SystemExit)
            & resolvePointerSymbol("PyExc_ArithmeticError", &PyExc_ArithmeticError)
            & resolvePointerSymbol("PyExc_FloatingPointError", &PyExc_FloatingPointError)
            & resolvePointerSymbol("PyExc_OverflowError", &PyExc_OverflowError)
            & resolvePointerSymbol("PyExc_ZeroDivisionError", &PyExc_ZeroDivisionError)
            & resolvePointerSymbol("PyExc_ImportError", &PyExc_ImportError)
            & resolvePointerSymbol("PyExc_ModuleNotFoundError", &PyExc_ModuleNotFoundError)
            & resolvePointerSymbol("PyExc_LookupError", &PyExc_LookupError)
            & resolvePointerSymbol("PyExc_IndexError", &PyExc_IndexError)
            & resolvePointerSymbol("PyExc_KeyError", &PyExc_KeyError)
            & resolvePointerSymbol("PyExc_NameError", &PyExc_NameError)
            & resolvePointerSymbol("PyExc_UnboundLocalError", &PyExc_UnboundLocalError)
            & resolvePointerSymbol("PyExc_OSError", &PyExc_OSError)
            & resolvePointerSymbol("PyExc_BlockingIOError", &PyExc_BlockingIOError)
            & resolvePointerSymbol("PyExc_ChildProcessError", &PyExc_ChildProcessError)
            & resolvePointerSymbol("PyExc_ConnectionError", &PyExc_ConnectionError)
            & resolvePointerSymbol("PyExc_BrokenPipeError", &PyExc_BrokenPipeError)
            & resolvePointerSymbol("PyExc_ConnectionAbortedError", &PyExc_ConnectionAbortedError)
            & resolvePointerSymbol("PyExc_ConnectionRefusedError", &PyExc_ConnectionRefusedError)
            & resolvePointerSymbol("PyExc_ConnectionResetError", &PyExc_ConnectionResetError)
            & resolvePointerSymbol("PyExc_FileExistsError", &PyExc_FileExistsError)
            & resolvePointerSymbol("PyExc_FileNotFoundError", &PyExc_FileNotFoundError)
            & resolvePointerSymbol("PyExc_InterruptedError", &PyExc_InterruptedError)
            & resolvePointerSymbol("PyExc_IsADirectoryError", &PyExc_IsADirectoryError)
            & resolvePointerSymbol("PyExc_NotADirectoryError", &PyExc_NotADirectoryError)
            & resolvePointerSymbol("PyExc_PermissionError", &PyExc_PermissionError)
            & resolvePointerSymbol("PyExc_ProcessLookupError", &PyExc_ProcessLookupError)
            & resolvePointerSymbol("PyExc_TimeoutError", &PyExc_TimeoutError)
            & resolvePointerSymbol("PyExc_RuntimeError", &PyExc_RuntimeError)
            & resolvePointerSymbol("PyExc_NotImplementedError", &PyExc_NotImplementedError)
            & resolvePointerSymbol("PyExc_RecursionError", &PyExc_RecursionError)
            & resolvePointerSymbol("PyExc_SyntaxError", &PyExc_SyntaxError)
            & resolvePointerSymbol("PyExc_IndentationError", &PyExc_IndentationError)
            & resolvePointerSymbol("PyExc_TabError", &PyExc_TabError)
            & resolvePointerSymbol("PyExc_ValueError", &PyExc_ValueError)
            & resolvePointerSymbol("PyExc_UnicodeError", &PyExc_UnicodeError)
            & resolvePointerSymbol("PyExc_UnicodeDecodeError", &PyExc_UnicodeDecodeError)
            & resolvePointerSymbol("PyExc_UnicodeEncodeError", &PyExc_UnicodeEncodeError)
            & resolvePointerSymbol("PyExc_UnicodeTranslateError", &PyExc_UnicodeTranslateError)
            & resolvePointerSymbol("PyExc_AssertionError", &PyExc_AssertionError)
            & resolvePointerSymbol("PyExc_AttributeError", &PyExc_AttributeError)
            & resolvePointerSymbol("PyExc_BufferError", &PyExc_BufferError)
            & resolvePointerSymbol("PyExc_EOFError", &PyExc_EOFError)
            & resolvePointerSymbol("PyExc_MemoryError", &PyExc_MemoryError)
            & resolvePointerSymbol("PyExc_ReferenceError", &PyExc_ReferenceError)
            & resolvePointerSymbol("PyExc_StopAsyncIteration", &PyExc_StopAsyncIteration)
            & resolvePointerSymbol("PyExc_StopIteration", &PyExc_StopIteration)
            & resolvePointerSymbol("PyExc_SystemError", &PyExc_SystemError)
            & resolvePointerSymbol("PyExc_TypeError", &PyExc_TypeError)
#if PYTHON_VERSION_MINOR >= 9
            & resolveSymbol("_Py_Dealloc", &_Py_Dealloc)
#endif
            & resolveSymbol("_Py_BuildValue_SizeT", &_Py_BuildValue_SizeT);
}

} // namespace

bool setupCoreLibrary()
{
    return findCoreLibrary() && loadCoreLibrary() && resolveSymbols();
}

bool teardownCoreLibrary()
{
    return unloadCoreLibrary();
}

} // namespace med::python
