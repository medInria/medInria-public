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

// The Python header is included only for the structs, marcos etc. The pointers
// declared below are used instead of the functions and global objects defined
// in the header.
#include <Python.h>

#undef slots
#define slots Q_SLOTS

#include <cstddef>

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

extern int (*PyRun_SimpleString)(const char*);

} // namespace med::python::core
