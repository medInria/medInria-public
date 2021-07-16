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
bool resolveFunctionSymbol(QString symbolName, SYMBOL_TYPE* symbolPointer)
{
    return resolveSymbol(symbolName, (QFunctionPointer*)(symbolPointer));
}

template <typename SYMBOL_TYPE>
bool resolveNonFunctionSymbol(QString symbolName, SYMBOL_TYPE* symbolPointer)
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
    return resolveFunctionSymbol("PyStatus_Exception", &PyStatus_Exception)
            & resolveFunctionSymbol("PyConfig_Read", &PyConfig_Read)
            & resolveFunctionSymbol("PyConfig_Clear", &PyConfig_Clear)
            & resolveFunctionSymbol("PyConfig_SetBytesString", &PyConfig_SetBytesString)
            & resolveFunctionSymbol("PyConfig_SetWideStringList", &PyConfig_SetWideStringList)
            & resolveFunctionSymbol("PyPreConfig_InitIsolatedConfig", &PyPreConfig_InitIsolatedConfig)
            & resolveFunctionSymbol("PyConfig_InitIsolatedConfig", &PyConfig_InitIsolatedConfig)
            & resolveFunctionSymbol("Py_PreInitialize", &Py_PreInitialize)
            & resolveFunctionSymbol("Py_InitializeFromConfig", &Py_InitializeFromConfig)
            & resolveFunctionSymbol("Py_IsInitialized", &Py_IsInitialized)
            & resolveFunctionSymbol("Py_FinalizeEx", &Py_FinalizeEx)
            & resolveFunctionSymbol("Py_GetVersion", &Py_GetVersion)
            & resolveFunctionSymbol("Py_SetPythonHome", &Py_SetPythonHome)
            & resolveFunctionSymbol("Py_GetPythonHome", &Py_GetPythonHome)
            & resolveFunctionSymbol("Py_GetPrefix", &Py_GetPrefix)
            & resolveFunctionSymbol("Py_GetExecPrefix", &Py_GetExecPrefix)
            & resolveFunctionSymbol("Py_GetPath", &Py_GetPath)
            & resolveFunctionSymbol("Py_SetPath", &Py_SetPath)
            & resolveFunctionSymbol("PyMem_Free", &PyMem_Free)
            & resolveFunctionSymbol("PyMem_RawMalloc", &PyMem_RawMalloc)
            & resolveFunctionSymbol("PyMem_RawFree", &PyMem_RawFree)
            & resolveFunctionSymbol("Py_EncodeLocale", &Py_EncodeLocale)
            & resolveFunctionSymbol("Py_DecodeLocale", &Py_DecodeLocale)
            & resolveFunctionSymbol("PyUnicode_FromString", &PyUnicode_FromString)
            & resolveFunctionSymbol("PyUnicode_AsEncodedString", &PyUnicode_AsEncodedString)
            & resolveFunctionSymbol("PyErr_SetString", &PyErr_SetString)
            & resolveFunctionSymbol("PyErr_Occurred", &PyErr_Occurred)
            & resolveFunctionSymbol("PyErr_Fetch", &PyErr_Fetch)
            & resolveFunctionSymbol("PyErr_NormalizeException", &PyErr_NormalizeException)
            & resolveFunctionSymbol("PyErr_Clear", &PyErr_Clear)
            & resolveFunctionSymbol("PyException_SetTraceback", &PyException_SetTraceback)
            & resolveFunctionSymbol("PyRun_SimpleStringFlags", &PyRun_SimpleStringFlags)
            & resolveFunctionSymbol("PyImport_Import", &PyImport_Import)
            & resolveFunctionSymbol("PyType_IsSubtype", &PyType_IsSubtype)

            & resolveFunctionSymbol("PyObject_Type", &PyObject_Type)
            & resolveFunctionSymbol("PyObject_Size", &PyObject_Size)
            & resolveFunctionSymbol("PyObject_HasAttrString", &PyObject_HasAttrString)
            & resolveFunctionSymbol("PyObject_GetAttrString", &PyObject_GetAttrString)
            & resolveFunctionSymbol("PyObject_SetAttrString", &PyObject_SetAttrString)
            & resolveFunctionSymbol("PyObject_IsTrue", &PyObject_IsTrue)
            & resolveFunctionSymbol("PyObject_Not", &PyObject_Not)
            & resolveFunctionSymbol("PyObject_RichCompareBool", &PyObject_RichCompareBool)
            & resolveFunctionSymbol("PyObject_Str", &PyObject_Str)
            & resolveFunctionSymbol("PyObject_Dir", &PyObject_Dir)
            & resolveFunctionSymbol("PyObject_GetItem", &PyObject_GetItem)
            & resolveFunctionSymbol("PyObject_SetItem", &PyObject_SetItem)
            & resolveFunctionSymbol("PyObject_Call", &PyObject_Call)
            & resolveFunctionSymbol("PyObject_CallObject", &PyObject_CallObject)
            & resolveFunctionSymbol("PyObject_CallNoArgs", &PyObject_CallNoArgs)
            & resolveNonFunctionSymbol("PyBool_Type", &PyBool_Type)
            & resolveFunctionSymbol("PyBool_FromLong", &PyBool_FromLong)
            & resolveFunctionSymbol("PyLong_FromLong", &PyLong_FromLong)
            & resolveFunctionSymbol("PyLong_AsLong", &PyLong_AsLong)
            & resolveNonFunctionSymbol("PyFloat_Type", &PyFloat_Type)
            & resolveFunctionSymbol("PyFloat_FromDouble", &PyFloat_FromDouble)
            & resolveFunctionSymbol("PyFloat_AsDouble", &PyFloat_AsDouble)
            & resolveFunctionSymbol("PyNumber_Add", &PyNumber_Add)
            & resolveFunctionSymbol("PyNumber_Subtract", &PyNumber_Subtract)
            & resolveFunctionSymbol("PyNumber_Multiply", &PyNumber_Multiply)
            & resolveFunctionSymbol("PyNumber_TrueDivide", &PyNumber_TrueDivide)
            & resolveFunctionSymbol("PyNumber_InPlaceAdd", &PyNumber_InPlaceAdd)
            & resolveFunctionSymbol("PyNumber_InPlaceSubtract", &PyNumber_InPlaceSubtract)
            & resolveFunctionSymbol("PyNumber_InPlaceMultiply", &PyNumber_InPlaceMultiply)
            & resolveFunctionSymbol("PyNumber_InPlaceTrueDivide", &PyNumber_InPlaceTrueDivide)
            & resolveFunctionSymbol("PySequence_Check", &PySequence_Check)
            & resolveFunctionSymbol("PySequence_Size", &PySequence_Size)
            & resolveFunctionSymbol("PySequence_Contains", &PySequence_Contains)
            & resolveFunctionSymbol("PySequence_GetItem", &PySequence_GetItem)
            & resolveFunctionSymbol("PySequence_Tuple", &PySequence_Tuple)
            & resolveFunctionSymbol("PySequence_List", &PySequence_List)
            & resolveFunctionSymbol("PyMapping_Check", &PyMapping_Check)
            & resolveFunctionSymbol("PyMapping_HasKey", &PyMapping_HasKey)
            & resolveFunctionSymbol("PyMapping_Keys", &PyMapping_Keys)
            & resolveFunctionSymbol("PyMapping_Values", &PyMapping_Values)
            & resolveFunctionSymbol("PyTuple_New", &PyTuple_New)
            & resolveFunctionSymbol("PyList_New", &PyList_New)
            & resolveFunctionSymbol("PyList_Append", &PyList_Append)
            & resolveFunctionSymbol("PyDict_New", &PyDict_New)
            & resolveFunctionSymbol("PyDict_SetItem", &PyDict_SetItem)
            & resolveFunctionSymbol("PyDict_Merge", &PyDict_Merge)
            & resolveNonFunctionSymbol("_Py_NoneStruct", &_Py_NoneStruct)
            & resolveNonFunctionSymbol("PyExc_BaseException", &PyExc_BaseException)
            & resolveNonFunctionSymbol("PyExc_Exception", &PyExc_Exception)
            & resolveNonFunctionSymbol("PyExc_GeneratorExit", &PyExc_GeneratorExit)
            & resolveNonFunctionSymbol("PyExc_KeyboardInterrupt", &PyExc_KeyboardInterrupt)
            & resolveNonFunctionSymbol("PyExc_SystemExit", &PyExc_SystemExit)
            & resolveNonFunctionSymbol("PyExc_ArithmeticError", &PyExc_ArithmeticError)
            & resolveNonFunctionSymbol("PyExc_FloatingPointError", &PyExc_FloatingPointError)
            & resolveNonFunctionSymbol("PyExc_OverflowError", &PyExc_OverflowError)
            & resolveNonFunctionSymbol("PyExc_ZeroDivisionError", &PyExc_ZeroDivisionError)
            & resolveNonFunctionSymbol("PyExc_ImportError", &PyExc_ImportError)
            & resolveNonFunctionSymbol("PyExc_ModuleNotFoundError", &PyExc_ModuleNotFoundError)
            & resolveNonFunctionSymbol("PyExc_LookupError", &PyExc_LookupError)
            & resolveNonFunctionSymbol("PyExc_IndexError", &PyExc_IndexError)
            & resolveNonFunctionSymbol("PyExc_KeyError", &PyExc_KeyError)
            & resolveNonFunctionSymbol("PyExc_NameError", &PyExc_NameError)
            & resolveNonFunctionSymbol("PyExc_UnboundLocalError", &PyExc_UnboundLocalError)
            & resolveNonFunctionSymbol("PyExc_OSError", &PyExc_OSError)
            & resolveNonFunctionSymbol("PyExc_BlockingIOError", &PyExc_BlockingIOError)
            & resolveNonFunctionSymbol("PyExc_ChildProcessError", &PyExc_ChildProcessError)
            & resolveNonFunctionSymbol("PyExc_ConnectionError", &PyExc_ConnectionError)
            & resolveNonFunctionSymbol("PyExc_BrokenPipeError", &PyExc_BrokenPipeError)
            & resolveNonFunctionSymbol("PyExc_ConnectionAbortedError", &PyExc_ConnectionAbortedError)
            & resolveNonFunctionSymbol("PyExc_ConnectionRefusedError", &PyExc_ConnectionRefusedError)
            & resolveNonFunctionSymbol("PyExc_ConnectionResetError", &PyExc_ConnectionResetError)
            & resolveNonFunctionSymbol("PyExc_FileExistsError", &PyExc_FileExistsError)
            & resolveNonFunctionSymbol("PyExc_FileNotFoundError", &PyExc_FileNotFoundError)
            & resolveNonFunctionSymbol("PyExc_InterruptedError", &PyExc_InterruptedError)
            & resolveNonFunctionSymbol("PyExc_IsADirectoryError", &PyExc_IsADirectoryError)
            & resolveNonFunctionSymbol("PyExc_NotADirectoryError", &PyExc_NotADirectoryError)
            & resolveNonFunctionSymbol("PyExc_PermissionError", &PyExc_PermissionError)
            & resolveNonFunctionSymbol("PyExc_ProcessLookupError", &PyExc_ProcessLookupError)
            & resolveNonFunctionSymbol("PyExc_TimeoutError", &PyExc_TimeoutError)
            & resolveNonFunctionSymbol("PyExc_RuntimeError", &PyExc_RuntimeError)
            & resolveNonFunctionSymbol("PyExc_NotImplementedError", &PyExc_NotImplementedError)
            & resolveNonFunctionSymbol("PyExc_RecursionError", &PyExc_RecursionError)
            & resolveNonFunctionSymbol("PyExc_SyntaxError", &PyExc_SyntaxError)
            & resolveNonFunctionSymbol("PyExc_IndentationError", &PyExc_IndentationError)
            & resolveNonFunctionSymbol("PyExc_TabError", &PyExc_TabError)
            & resolveNonFunctionSymbol("PyExc_ValueError", &PyExc_ValueError)
            & resolveNonFunctionSymbol("PyExc_UnicodeError", &PyExc_UnicodeError)
            & resolveNonFunctionSymbol("PyExc_UnicodeDecodeError", &PyExc_UnicodeDecodeError)
            & resolveNonFunctionSymbol("PyExc_UnicodeEncodeError", &PyExc_UnicodeEncodeError)
            & resolveNonFunctionSymbol("PyExc_UnicodeTranslateError", &PyExc_UnicodeTranslateError)
            & resolveNonFunctionSymbol("PyExc_AssertionError", &PyExc_AssertionError)
            & resolveNonFunctionSymbol("PyExc_AttributeError", &PyExc_AttributeError)
            & resolveNonFunctionSymbol("PyExc_BufferError", &PyExc_BufferError)
            & resolveNonFunctionSymbol("PyExc_EOFError", &PyExc_EOFError)
            & resolveNonFunctionSymbol("PyExc_MemoryError", &PyExc_MemoryError)
            & resolveNonFunctionSymbol("PyExc_ReferenceError", &PyExc_ReferenceError)
            & resolveNonFunctionSymbol("PyExc_StopAsyncIteration", &PyExc_StopAsyncIteration)
            & resolveNonFunctionSymbol("PyExc_StopIteration", &PyExc_StopIteration)
            & resolveNonFunctionSymbol("PyExc_SystemError", &PyExc_SystemError)
            & resolveNonFunctionSymbol("PyExc_TypeError", &PyExc_TypeError)
            & resolveFunctionSymbol("_Py_VaBuildValue_SizeT", &_Py_VaBuildValue_SizeT)
            & resolveFunctionSymbol("_Py_Dealloc", &_Py_Dealloc);
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
