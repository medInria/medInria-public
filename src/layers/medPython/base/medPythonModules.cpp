/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreAPI.h"

#include "medPythonModules.h"

#include "medPythonCoreFunction.h"
#include "medPythonObjects.h"

namespace med::python
{

void registerModulePath(QString path)
{
    Module sys = import("sys");
    sys.attribute("path").append(Object(path));
}

void loadPlugins()
{
    Module pkgutil = import("pkgutil");
    Object moduleIterator = pkgutil.callMethod("iter_modules");
    Object moduleInfo = coreFunction(PyIter_Next, *moduleIterator);

    while (moduleInfo)
    {
        QString moduleName = moduleInfo[1].convert<QString>();

        if (moduleName.startsWith(PYTHON_PLUGIN_PREFIX))
        {
            Module::import(moduleName);
        }

        moduleInfo = coreFunction(PyIter_Next, *moduleIterator);
    }
}

} // namespace med::python
