#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

/// This file declares the CPython functions and global variables that will be
/// linked explicitly when the Python library is loaded at runtime.
///
/// NOTE: CPython defines pre-processor macros that affect the standard headers.
/// Since this file includes the Python header, it must therefore always be
/// included before any standard headers. To make sure this is the case, the
/// include directive should be placed at the top of any file that directly
/// or indirectly includes this one.
///

// One of Python's structs has a member named slots, and this causes conflicts
// with the the Qt keyword of the same name.
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

// CPython uses structs for type definitions. Because of explicit linking we
// have to use pointers to structs, and therefore some macros need to be
// redefined without the dereferencing of the type argument (the other type
// checks work differently).
#undef PyBool_Check
#if PYTHON_VERSION_MINOR < 9
  #define PyBool_Check(x) (Py_TYPE(x) == PyBool_Type)
#else
  #define PyBool_Check(x) Py_IS_TYPE(x, PyBool_Type)
#endif
#undef PyFloat_Check
#define PyFloat_Check(op) PyObject_TypeCheck(op, PyFloat_Type)
#undef PyModule_Check
#define PyModule_Check(op) PyObject_TypeCheck(op, PyModule_Type)

#undef Py_None

#undef _Py_Dealloc

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

extern int (*PyRun_SimpleStringFlags)(const char*, PyCompilerFlags*);

extern int (*PyType_IsSubtype)(PyTypeObject*, PyTypeObject*);

extern PyObject* (*PyObject_Type)(PyObject*);
extern Py_ssize_t (*PyObject_Size)(PyObject*);
extern int (*PyObject_HasAttrString)(PyObject*, const char*);
extern PyObject* (*PyObject_GetAttrString)(PyObject*, const char*);
extern int (*PyObject_SetAttrString)(PyObject*, const char*, PyObject*);
extern int (*PyObject_IsTrue)(PyObject*);
extern int (*PyObject_Not)(PyObject*);
extern int (*PyObject_RichCompareBool)(PyObject*, PyObject*, int);
extern PyObject* (*PyObject_Str)(PyObject*);
extern PyObject* (*PyObject_Dir)(PyObject*);
extern PyObject* (*PyObject_GetItem)(PyObject*, PyObject*);
extern int (*PyObject_SetItem)(PyObject*, PyObject*, PyObject*);
extern PyObject* (*PyObject_Call)(PyObject*, PyObject*, PyObject*);
extern PyObject* (*PyObject_CallObject)(PyObject*, PyObject*);
extern PyObject* (*PyObject_CallNoArgs)(PyObject*);

extern PyObject* Py_None;

extern PyTypeObject* PyBool_Type;
extern PyObject* (*PyBool_FromLong)(long);

extern PyTypeObject* PyLong_Type;
extern PyObject* (*PyLong_FromLong)(long);
extern long (*PyLong_AsLong)(PyObject*);

extern PyTypeObject* PyFloat_Type;
extern PyObject* (*PyFloat_FromDouble)(double);
extern double (*PyFloat_AsDouble)(PyObject*);

extern PyObject* (*PyNumber_Add)(PyObject*, PyObject*);
extern PyObject* (*PyNumber_Subtract)(PyObject*, PyObject*);
extern PyObject* (*PyNumber_Multiply)(PyObject*, PyObject*);
extern PyObject* (*PyNumber_TrueDivide)(PyObject*, PyObject*);
extern PyObject* (*PyNumber_InPlaceAdd)(PyObject*, PyObject*);
extern PyObject* (*PyNumber_InPlaceSubtract)(PyObject*, PyObject*);
extern PyObject* (*PyNumber_InPlaceMultiply)(PyObject*, PyObject*);
extern PyObject* (*PyNumber_InPlaceTrueDivide)(PyObject*, PyObject*);

extern int (*PySequence_Check)(PyObject*);
extern Py_ssize_t (*PySequence_Size)(PyObject*);
extern int (*PySequence_Contains)(PyObject*, PyObject*);
extern PyObject* (*PySequence_GetItem)(PyObject*, Py_ssize_t);
extern PyObject* (*PySequence_Tuple)(PyObject*);
extern PyObject* (*PySequence_List)(PyObject*);

extern int (*PyMapping_Check)(PyObject*);
extern int (*PyMapping_HasKey)(PyObject*, PyObject*);
extern PyObject* (*PyMapping_Keys)(PyObject*);
extern PyObject* (*PyMapping_Values)(PyObject*);

extern PyTypeObject* PyTuple_Type;
extern PyObject* (*PyTuple_New)(Py_ssize_t);

extern PyTypeObject* PyList_Type;
extern PyObject* (*PyList_New)(Py_ssize_t);
extern int (*PyList_Append)(PyObject*, PyObject*);

extern PyTypeObject* PyDict_Type;
extern PyObject* (*PyDict_New)();
extern int (*PyDict_SetItem)(PyObject*, PyObject*, PyObject*);
extern int (*PyDict_Merge)(PyObject*, PyObject*, int);

extern PyTypeObject* PyModule_Type;
extern const char* (*PyModule_GetName)(PyObject*);
extern PyObject* (*PyModule_GetDict)(PyObject*);

extern PyObject* (*PyImport_Import)(PyObject*);
extern PyObject* (*PyImport_ImportModuleLevel)(const char*, PyObject*, PyObject*, PyObject*, int);
extern PyObject* (*PyImport_AddModule)(const char*);
extern PyObject* (*PyImport_GetModule)(PyObject*);

extern PyObject* (*_Py_BuildValue_SizeT)(const char*, ...);

extern void (*_Py_Dealloc)(PyObject*);

} // namespace med::python
