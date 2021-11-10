#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonExport.h"
#include "medPythonObject.h"

namespace med::python
{

class MEDPYTHON_EXPORT Module : public Object
{
public:
    static Module main();

    static Module import(QString name);

    Module(PyObject* reference);

    Module(const AbstractObject& other);

    QString name();

    Object dict();

private:
    static PyObject* ensureModuleObject(PyObject* reference);
    static PyObject* getModuleObject(QString name);
};

Module import(QString name);

} // namespace med::python
