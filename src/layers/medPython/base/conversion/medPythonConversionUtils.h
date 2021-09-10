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
#include "medPythonExport.h"

struct swig_type_info;

namespace med::python
{

MEDPYTHON_EXPORT const swig_type_info* getSWIGTypeInfo(QString typeName, bool raisePythonError = true);

MEDPYTHON_EXPORT const swig_type_info* getSWIGTypeInfo(const PyObject* object, bool raisePythonError = true);

MEDPYTHON_EXPORT bool isSWIGWrappedObject(const PyObject* object, QString typeName = "");

MEDPYTHON_EXPORT void* extractSWIGWrappedObject(const PyObject* object);

MEDPYTHON_EXPORT PyObject* wrapObjectWithSWIG(void* object, QString typeName, bool takeOwnership = false);

MEDPYTHON_EXPORT PyObject* wrapObjectWithSWIG(QObject* object, bool takeOwnership = false);

} // namespace med::python
