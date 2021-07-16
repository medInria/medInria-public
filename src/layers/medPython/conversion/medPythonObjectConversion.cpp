/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonObjectConversion.h"

#include "medPythonObject.h"

bool medPythonConvert(const med::python::AbstractObject& object, PyObject** output)
{
    *output = object.newReference();
    return true;
}

bool medPythonConvert(const PyObject* nativeObject, med::python::Object* output)
{
    (*output).borrow(nativeObject);
    return true;
}
