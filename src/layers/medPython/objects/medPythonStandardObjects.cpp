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
    ensurePythonSetup();
    Py_RETURN_NONE;
}

Object list()
{
    ensurePythonSetup();
    return coreFunction(PyList_New, 0);
}

Object list(const Object& object)
{
    ensurePythonSetup();
    return coreFunction(PySequence_List, *object);
}

Object tuple()
{
    ensurePythonSetup();
    return coreFunction(PyTuple_New, 0);
}

Object tuple(const Object& object)
{
    ensurePythonSetup();
    return coreFunction(PySequence_Tuple, *object);
}

Object dict()
{
    ensurePythonSetup();
    return coreFunction(PyDict_New);
}

} // namespace med::python
