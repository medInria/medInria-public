#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreForward.h"
#include "medPythonExport.h"

namespace med::python
{

class AbstractObject;
class Object;

} // namespace med::python

MEDPYTHON_EXPORT bool medPythonConvert(const med::python::AbstractObject& object, PyObject** output);
MEDPYTHON_EXPORT bool medPythonConvert(const PyObject* nativeObject, med::python::Object* output);
