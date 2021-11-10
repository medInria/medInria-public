#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include <QObject>
#include <QString>

#include "medPythonCore.h"
#include "medPythonError.h"
#include "medPythonExport.h"

struct swig_type_info;

namespace med::python
{

MEDPYTHON_EXPORT const swig_type_info* getSWIGTypeInfo(QString typeName, bool raisePythonError = true);

MEDPYTHON_EXPORT const swig_type_info* getSWIGTypeInfo(const PyObject* object, bool raisePythonError = true);

MEDPYTHON_EXPORT bool isSWIGWrappedObject(const PyObject* object, QString typeName = "");

MEDPYTHON_EXPORT void* extractSWIGWrappedObject(const PyObject* object);

MEDPYTHON_EXPORT PyObject* wrapObjectWithSWIG(const void* object, QString typeName, bool takeOwnership = false);

MEDPYTHON_EXPORT PyObject* wrapObjectWithSWIG(const QObject* object, bool takeOwnership = false);

} // namespace med::python

#define MED_PYTHON_STANDARD_VALUE_CONVERSIONS(TYPE) \
    MEDPYTHON_EXPORT bool medPythonConvert(const TYPE& value, PyObject** output) \
    { \
        TYPE* valueCopy = new TYPE(value); \
        *output = med::python::wrapObjectWithSWIG(valueCopy, QString(#TYPE) + "*", true); \
        return !med::python::errorOccurred(); \
    } \
    MEDPYTHON_EXPORT bool medPythonConvert(const PyObject* object, TYPE* output) \
    { \
        *output = *(TYPE*)med::python::extractSWIGWrappedObject(object); \
        return !med::python::errorOccurred(); \
    }

#define MED_PYTHON_STANDARD_POINTER_CONVERSIONS(TYPE) \
    MEDPYTHON_EXPORT bool medPythonConvert(const TYPE* object, PyObject** output) \
    { \
        *output = med::python::wrapObjectWithSWIG(object, QString(#TYPE) + "*"); \
        return !med::python::errorOccurred(); \
    } \
    MEDPYTHON_EXPORT bool medPythonConvert(const PyObject* object, TYPE** output) \
    { \
        *output = (TYPE*)med::python::extractSWIGWrappedObject(nativeObject); \
        return !med::python::errorOccurred(); \
    }
