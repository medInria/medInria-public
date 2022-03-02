/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreAPI.h"

#include "medPythonModule.h"

#include "medPythonCoreFunction.h"
#include "medPythonError.h"
#include "medPythonStandardObjects.h"

namespace med::python
{

Module Module::main()
{
    return getModuleObject("__main__");
}

Module Module::import(QString name)
{
    return getModuleObject(name);
}

Module::Module(PyObject* reference) :
    Object(ensureModuleObject(reference))
{
}

Module::Module(const AbstractObject& other) :
    Object(ensureModuleObject(other.newReference()))
{
}

QString Module::name()
{
    return coreFunction(PyModule_GetName, **this);
}

Object Module::dict()
{
    return Object::borrowed(coreFunction(PyModule_GetDict, **this));
}

PyObject* Module::ensureModuleObject(PyObject* reference)
{
    if (!PyModule_Check(reference))
    {
        QString typeName = Py_TYPE(reference)->tp_name;
        Py_CLEAR(reference);

        throw TypeError(QString("'%1' is not a module").arg(typeName));
    }

    return reference;
}

PyObject* Module::getModuleObject(QString name)
{
    Object pythonName = name;
    PyObject* moduleObject = coreFunction(PyImport_GetModule, *pythonName);

    if (!moduleObject)
    {
        moduleObject = coreFunction(PyImport_ImportModuleLevel, qUtf8Printable(name), nullptr, nullptr, *list(), 0);
    }

    return moduleObject;
}

Module import(QString name)
{
    return Module::import(name);
}

} // namespace med::python

