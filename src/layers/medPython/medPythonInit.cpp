/*=========================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medPythonInit.h"

#include <QApplication>
#include <QDebug>

#include "medPythonCore.h"
#include "medPythonExceptionTypes.h"

namespace med::python
{

void initialize()
{
    if (!Py_IsInitialized())
    {
        Py_Initialize();
        exception_types::initialize();
        QApplication::connect(qApp, &QApplication::aboutToQuit, &finalize);
        qInfo() << "Python initialized: " << Py_GetVersion();
    }
}

void finalize()
{
    if (Py_IsInitialized())
    {
        exception_types::finalize();
        Py_Finalize();
    }
}

} // namespace med::python
