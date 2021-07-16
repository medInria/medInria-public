/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreAPI.h"

namespace med::python
{

int (*PyStatus_Exception)(PyStatus) = nullptr;

PyStatus (*PyConfig_Read)(PyConfig*) = nullptr;
void (*PyConfig_Clear)(PyConfig*) = nullptr;
PyStatus (*PyConfig_SetBytesString)(PyConfig*, wchar_t* const*, const char*) = nullptr;
PyStatus (*PyConfig_SetWideStringList)(PyConfig*, PyWideStringList*, Py_ssize_t, wchar_t**) = nullptr;

void (*PyPreConfig_InitIsolatedConfig)(PyPreConfig*) = nullptr;
void (*PyConfig_InitIsolatedConfig)(PyConfig*) = nullptr;
PyStatus (*Py_PreInitialize)(const PyPreConfig*) = nullptr;
PyStatus (*Py_InitializeFromConfig)(const PyConfig*) = nullptr;

int (*Py_IsInitialized)() = nullptr;
int (*Py_FinalizeEx)() = nullptr;

const char* (*Py_GetVersion)() = nullptr;
void (*Py_SetPythonHome)(const wchar_t *) = nullptr;
wchar_t* (*Py_GetPythonHome)() = nullptr;
wchar_t* (*Py_GetPrefix)() = nullptr;
wchar_t* (*Py_GetExecPrefix)() = nullptr;
wchar_t* (*Py_GetPath)() = nullptr;
void (*Py_SetPath)(const wchar_t*) = nullptr;

void (*PyMem_Free)(void*) = nullptr;
void* (*PyMem_RawMalloc)(size_t) = nullptr;
void (*PyMem_RawFree)(void*) = nullptr;

char* (*Py_EncodeLocale)(const wchar_t*, size_t*) = nullptr;
wchar_t* (*Py_DecodeLocale)(const char*, size_t *) = nullptr;

PyObject* (*PyUnicode_FromString)(const char*) = nullptr;
PyObject* (*PyUnicode_AsEncodedString)(PyObject*, const char*, const char*) = nullptr;

void (*PyErr_SetString)(PyObject*, const char*) = nullptr;
PyObject* (*PyErr_Occurred)() = nullptr;
void (*PyErr_Fetch)(PyObject**, PyObject**, PyObject**) = nullptr;
void (*PyErr_NormalizeException)(PyObject**, PyObject**, PyObject**) = nullptr;
void (*PyErr_Clear)() = nullptr;
int (*PyException_SetTraceback)(PyObject*, PyObject*) = nullptr;

int (*PyRun_SimpleStringFlags)(const char*, PyCompilerFlags*) = nullptr;

PyObject* (*PyImport_Import)(PyObject*) = nullptr;

int (*PyType_IsSubtype)(PyTypeObject*, PyTypeObject*) = nullptr;

PyObject* (*PyObject_Type)(PyObject*) = nullptr;
Py_ssize_t (*PyObject_Size)(PyObject*) = nullptr;
int (*PyObject_HasAttrString)(PyObject*, const char*) = nullptr;
PyObject* (*PyObject_GetAttrString)(PyObject*, const char*) = nullptr;
int (*PyObject_SetAttrString)(PyObject*, const char*, PyObject*) = nullptr;
int (*PyObject_IsTrue)(PyObject*) = nullptr;
int (*PyObject_Not)(PyObject*) = nullptr;
int (*PyObject_RichCompareBool)(PyObject*, PyObject*, int) = nullptr;
PyObject* (*PyObject_Str)(PyObject*) = nullptr;
PyObject* (*PyObject_Dir)(PyObject*) = nullptr;
PyObject* (*PyObject_GetItem)(PyObject*, PyObject*) = nullptr;
int (*PyObject_SetItem)(PyObject*, PyObject*, PyObject*) = nullptr;
PyObject* (*PyObject_Call)(PyObject*, PyObject*, PyObject*) = nullptr;
PyObject* (*PyObject_CallObject)(PyObject*, PyObject*) = nullptr;
PyObject* (*PyObject_CallNoArgs)(PyObject*) = nullptr;

PyObject* (*PyBool_FromLong)(long) = nullptr;

PyObject* (*PyLong_FromLong)(long) = nullptr;
long (*PyLong_AsLong)(PyObject*) = nullptr;

PyObject* (*PyFloat_FromDouble)(double) = nullptr;
double (*PyFloat_AsDouble)(PyObject*) = nullptr;

PyObject* (*PyNumber_Add)(PyObject*, PyObject*) = nullptr;
PyObject* (*PyNumber_Subtract)(PyObject*, PyObject*) = nullptr;
PyObject* (*PyNumber_Multiply)(PyObject*, PyObject*) = nullptr;
PyObject* (*PyNumber_TrueDivide)(PyObject*, PyObject*) = nullptr;
PyObject* (*PyNumber_InPlaceAdd)(PyObject*, PyObject*) = nullptr;
PyObject* (*PyNumber_InPlaceSubtract)(PyObject*, PyObject*) = nullptr;
PyObject* (*PyNumber_InPlaceMultiply)(PyObject*, PyObject*) = nullptr;
PyObject* (*PyNumber_InPlaceTrueDivide)(PyObject*, PyObject*) = nullptr;

int (*PySequence_Check)(PyObject*);
Py_ssize_t (*PySequence_Size)(PyObject*) = nullptr;
int (*PySequence_Contains)(PyObject*, PyObject*) = nullptr;
PyObject* (*PySequence_GetItem)(PyObject*, Py_ssize_t) = nullptr;
PyObject* (*PySequence_Tuple)(PyObject*) = nullptr;
PyObject* (*PySequence_List)(PyObject*) = nullptr;

int (*PyMapping_Check)(PyObject*) = nullptr;
int (*PyMapping_HasKey)(PyObject*, PyObject*) = nullptr;
PyObject* (*PyMapping_Keys)(PyObject*) = nullptr;
PyObject* (*PyMapping_Values)(PyObject*) = nullptr;

PyObject* (*PyTuple_New)(Py_ssize_t) = nullptr;

PyObject* (*PyList_New)(Py_ssize_t) = nullptr;
int (*PyList_Append)(PyObject*, PyObject*) = nullptr;

PyObject* (*PyDict_New)() = nullptr;
int (*PyDict_SetItem)(PyObject*, PyObject*, PyObject*) = nullptr;
int (*PyDict_Merge)(PyObject*, PyObject*, int) = nullptr;

PyObject* (*_Py_VaBuildValue_SizeT)(const char*, va_list) = nullptr;
void (*_Py_Dealloc)(PyObject*) = nullptr;

PyObject* PyExc_BaseException = nullptr;
PyObject* PyExc_Exception = nullptr;
PyObject* PyExc_GeneratorExit = nullptr;
PyObject* PyExc_KeyboardInterrupt = nullptr;
PyObject* PyExc_SystemExit = nullptr;
PyObject* PyExc_ArithmeticError = nullptr;
PyObject* PyExc_FloatingPointError = nullptr;
PyObject* PyExc_OverflowError = nullptr;
PyObject* PyExc_ZeroDivisionError = nullptr;
PyObject* PyExc_ImportError = nullptr;
PyObject* PyExc_ModuleNotFoundError = nullptr;
PyObject* PyExc_LookupError = nullptr;
PyObject* PyExc_IndexError = nullptr;
PyObject* PyExc_KeyError = nullptr;
PyObject* PyExc_NameError = nullptr;
PyObject* PyExc_UnboundLocalError = nullptr;
PyObject* PyExc_OSError = nullptr;
PyObject* PyExc_BlockingIOError = nullptr;
PyObject* PyExc_ChildProcessError = nullptr;
PyObject* PyExc_ConnectionError = nullptr;
PyObject* PyExc_BrokenPipeError = nullptr;
PyObject* PyExc_ConnectionAbortedError = nullptr;
PyObject* PyExc_ConnectionRefusedError = nullptr;
PyObject* PyExc_ConnectionResetError = nullptr;
PyObject* PyExc_FileExistsError = nullptr;
PyObject* PyExc_FileNotFoundError = nullptr;
PyObject* PyExc_InterruptedError = nullptr;
PyObject* PyExc_IsADirectoryError = nullptr;
PyObject* PyExc_NotADirectoryError = nullptr;
PyObject* PyExc_PermissionError = nullptr;
PyObject* PyExc_ProcessLookupError = nullptr;
PyObject* PyExc_TimeoutError = nullptr;
PyObject* PyExc_RuntimeError = nullptr;
PyObject* PyExc_NotImplementedError = nullptr;
PyObject* PyExc_RecursionError = nullptr;
PyObject* PyExc_SyntaxError = nullptr;
PyObject* PyExc_IndentationError = nullptr;
PyObject* PyExc_TabError = nullptr;
PyObject* PyExc_ValueError = nullptr;
PyObject* PyExc_UnicodeError = nullptr;
PyObject* PyExc_UnicodeDecodeError = nullptr;
PyObject* PyExc_UnicodeEncodeError = nullptr;
PyObject* PyExc_UnicodeTranslateError = nullptr;
PyObject* PyExc_AssertionError = nullptr;
PyObject* PyExc_AttributeError = nullptr;
PyObject* PyExc_BufferError = nullptr;
PyObject* PyExc_EOFError = nullptr;
PyObject* PyExc_MemoryError = nullptr;
PyObject* PyExc_ReferenceError = nullptr;
PyObject* PyExc_StopAsyncIteration = nullptr;
PyObject* PyExc_StopIteration = nullptr;
PyObject* PyExc_SystemError = nullptr;
PyObject* PyExc_TypeError = nullptr;

} // namespace med::python

PyObject _Py_NoneStruct = {};
PyTypeObject PyBool_Type = {};
PyTypeObject PyFloat_Type = {};

// The following functions are called by CPython macros. To avoid link issues at
// build time, we define our own version that calls the proper runtime-linked
// one.

int PyType_IsSubtype(PyTypeObject* object1, PyTypeObject* object2)
{
    return med::python::PyType_IsSubtype(object1, object2);
}

void _Py_Dealloc(PyObject* object)
{
    med::python::_Py_Dealloc(object);
}

PyObject* _Py_BuildValue_SizeT(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    PyObject* result = med::python::_Py_VaBuildValue_SizeT(format, args);
    va_end(args);
    return result;
}
