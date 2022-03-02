/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonQObjectConversion.h"

#include "medPythonConversionUtils.h"
#include "medPythonError.h"

bool medPythonConvert(const QObject* object, PyObject** output)
{
    *output = med::python::wrapObjectWithSWIG(object);
    return !med::python::errorOccurred();
}

bool medPythonConvert(const PyObject* nativeObject, QObject** output)
{
    *output = (QObject*)med::python::extractSWIGWrappedObject(nativeObject);
    return !med::python::errorOccurred();
}
