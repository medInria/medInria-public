#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

/// This file declares all the CPython functions that are actually used. They
/// will be linked manually when the Python library is loaded at runtime.
///

#include <cstddef>

// The following definitions avoid compilation errors when including Python.h
#undef slots
#define slots _slots
#undef _LARGEFILE_SOURCE
#define PY_SSIZE_T_CLEAN

// The Python header is included only for the structs. The function pointers
// declared below are used instead of the functions defined in the header.
#include <Python.h>

#undef slots
#define slots Q_SLOTS

namespace med::python::core
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

extern int (*PyRun_SimpleString)(const char*);

} // namespace med::python::core
