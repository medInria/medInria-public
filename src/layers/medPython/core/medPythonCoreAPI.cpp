/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreAPI.h"

namespace med::python::core
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

int (*PyRun_SimpleString)(const char*) = nullptr;

} // namespace med::python::core
