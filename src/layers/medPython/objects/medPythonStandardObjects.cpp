/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreAPI.h"

#include "medPythonStandardObjects.h"

#include "medPythonCoreFunction.h"

namespace med::python
{

Object none()
{
    Py_RETURN_NONE;
}

Object list()
{
    return coreFunction<PyObject*, &PyList_New>(0);
}

Object list(const Object& object)
{
    return coreFunction<PyObject*, &PySequence_List>(*object);
}

Object tuple()
{
    return coreFunction<PyObject*, &PyTuple_New>(0);
}

Object tuple(const Object& object)
{
    return coreFunction<PyObject*, &PySequence_Tuple>(*object);
}

Object dict()
{
    return coreFunction<PyObject*, &PyDict_New>();
}

} // namespace med::python
