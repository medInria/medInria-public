/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCore.h"

#include <QDebug>

#include "medPythonCoreInit.h"
#include "medPythonCoreLibrary.h"

namespace med::python::core
{

bool setup()
{
    qInfo() << "Setting up Python...";
    return setupCoreLibrary() && initializePython();
}

bool teardown()
{
    return finalizePython() & teardownCoreLibrary();
}

} // namespace med::python::core
