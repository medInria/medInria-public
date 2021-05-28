#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

/// This file declares all the CPython functions and global variables that are
/// actually used. They will be linked explicitly when the Python library is
/// loaded at runtime.
///
/// NOTE: CPython defines pre-processor macros that affect the standard headers.
/// Since this file includes the Python header, it must therefore always be
/// included before any standard headers. To make sure this is the case, the
/// include directive should be placed at the top of any file that directly
/// or indirectly includes this one.
///

// One of Python's structs has a member named slots, and this causes conflicts
// with the the Qt keyword of the same name. This (ugly) hack avoids that.
#undef slots
#define slots _slots

// Python recommends to always define this macro before including its header.
#define PY_SSIZE_T_CLEAN

// The Python header is included only for the structs, macros etc. The pointers
// declared below are used instead of the functions and global variables defined
// in the header.
#include <Python.h>

#undef slots
#define slots Q_SLOTS

#include <cstddef>

namespace med::python
{

extern int (*PyStatus_Exception)(PyStatus);

extern PyStatus (*PyConfig_Read)(PyConfig*);
extern void (*PyConfig_Clear)(PyConfig*);
extern PyStatus (*PyConfig_SetBytesString)(PyConfig*, wchar_t* const*, const char*);
extern PyStatus (*PyConfig_SetWideStringList)(PyConfig*, PyWideStringList*, Py_ssize_t, wchar_t**);

extern void (*PyPreConfig_InitIsolatedConfig)(PyPreConfig*);
extern void (*PyConfig_InitIsolatedConfig)(PyConfig*);
extern PyStatus (*Py_PreInitialize)(const PyPreConfig*);
extern PyStatus (*Py_InitializeFromConfig)(const PyConfig*);

extern int (*Py_IsInitialized)();
extern int (*Py_FinalizeEx)();

extern const char* (*Py_GetVersion)();
extern void (*Py_SetPythonHome)(const wchar_t *);
extern wchar_t* (*Py_GetPythonHome)();
extern wchar_t* (*Py_GetPrefix)();
extern wchar_t* (*Py_GetExecPrefix)();
extern wchar_t* (*Py_GetPath)();
extern void (*Py_SetPath)(const wchar_t*);

extern void (*PyMem_Free)(void*);
extern void* (*PyMem_RawMalloc)(size_t);
extern void (*PyMem_RawFree)(void*);

extern char* (*Py_EncodeLocale)(const wchar_t*, size_t*);
extern wchar_t* (*Py_DecodeLocale)(const char*, size_t*);

extern PyObject* (*PyUnicode_FromString)(const char*);
extern PyObject* (*PyUnicode_AsEncodedString)(PyObject*, const char*, const char*);

extern void (*PyErr_SetString)(PyObject*, const char*);
extern PyObject* (*PyErr_Occurred)();
extern void (*PyErr_Fetch)(PyObject**, PyObject**, PyObject**);
extern void (*PyErr_NormalizeException)(PyObject**, PyObject**, PyObject**);
extern void (*PyErr_Clear)();
extern int (*PyException_SetTraceback)(PyObject*, PyObject*);

extern int (*PyRun_SimpleString)(const char*);

extern PyObject* (*PyImport_Import)(PyObject*);

extern Py_ssize_t (*PyObject_Length)(PyObject*);
extern PyObject* (*PyObject_GetAttrString)(PyObject*, const char*);

extern PyObject* (*PyObject_CallObject)(PyObject*, PyObject*);

extern PyObject* (*PySequence_GetItem)(PyObject*, Py_ssize_t);

extern PyObject* (*_Py_VaBuildValue_SizeT)(const char *, va_list);
extern void (*_Py_Dealloc)(PyObject*);

extern PyObject** PyExc_BaseException;
extern PyObject** PyExc_Exception;
extern PyObject** PyExc_GeneratorExit;
extern PyObject** PyExc_KeyboardInterrupt;
extern PyObject** PyExc_SystemExit;
extern PyObject** PyExc_ArithmeticError;
extern PyObject** PyExc_FloatingPointError;
extern PyObject** PyExc_OverflowError;
extern PyObject** PyExc_ZeroDivisionError;
extern PyObject** PyExc_ImportError;
extern PyObject** PyExc_ModuleNotFoundError;
extern PyObject** PyExc_LookupError;
extern PyObject** PyExc_IndexError;
extern PyObject** PyExc_KeyError;
extern PyObject** PyExc_NameError;
extern PyObject** PyExc_UnboundLocalError;
extern PyObject** PyExc_OSError;
extern PyObject** PyExc_BlockingIOError;
extern PyObject** PyExc_ChildProcessError;
extern PyObject** PyExc_ConnectionError;
extern PyObject** PyExc_BrokenPipeError;
extern PyObject** PyExc_ConnectionAbortedError;
extern PyObject** PyExc_ConnectionRefusedError;
extern PyObject** PyExc_ConnectionResetError;
extern PyObject** PyExc_FileExistsError;
extern PyObject** PyExc_FileNotFoundError;
extern PyObject** PyExc_InterruptedError;
extern PyObject** PyExc_IsADirectoryError;
extern PyObject** PyExc_NotADirectoryError;
extern PyObject** PyExc_PermissionError;
extern PyObject** PyExc_ProcessLookupError;
extern PyObject** PyExc_TimeoutError;
extern PyObject** PyExc_RuntimeError;
extern PyObject** PyExc_NotImplementedError;
extern PyObject** PyExc_RecursionError;
extern PyObject** PyExc_SyntaxError;
extern PyObject** PyExc_IndentationError;
extern PyObject** PyExc_TabError;
extern PyObject** PyExc_ValueError;
extern PyObject** PyExc_UnicodeError;
extern PyObject** PyExc_UnicodeDecodeError;
extern PyObject** PyExc_UnicodeEncodeError;
extern PyObject** PyExc_UnicodeTranslateError;
extern PyObject** PyExc_AssertionError;
extern PyObject** PyExc_AttributeError;
extern PyObject** PyExc_BufferError;
extern PyObject** PyExc_EOFError;
extern PyObject** PyExc_MemoryError;
extern PyObject** PyExc_ReferenceError;
extern PyObject** PyExc_StopAsyncIteration;
extern PyObject** PyExc_StopIteration;
extern PyObject** PyExc_SystemError;
extern PyObject** PyExc_TypeError;

} // namespace med::python
