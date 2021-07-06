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

namespace med::python
{

bool setupCore()
{
    qInfo() << "Setting up Python...";
    return setupCoreLibrary() && initializePython();
}

bool teardownCore()
{
    // Bitwise AND is used to prevent short-circuit evaluation, so that the
    // library is unloaded even if finalization failed.
    //
    return finalizePython() & teardownCoreLibrary();
}

} // namespace med::python
