/*=========================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medPython.h"

#include <QApplication>
#include <QDebug>

#include "medPythonCore.h"

namespace med
{
namespace python
{

void initialize()
{
    if (!Py_IsInitialized())
    {
        Py_Initialize();
        QApplication::connect(qApp, &QApplication::aboutToQuit, &finalize);
        qInfo() << "Python initialized: " << Py_GetVersion();
    }
}

void finalize()
{
    if (Py_IsInitialized())
    {
        Py_Finalize();
    }
}

} // namespace python
} // namespace med
