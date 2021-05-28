/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonInit.h"

#include <QApplication>

#include "medPythonCore.h"
#include "medPythonError.h"

namespace med::python
{

bool setup()
{
    bool success = setupCore();

    if (success)
    {
        initializeExceptions();
        QApplication::connect(qApp, &QApplication::aboutToQuit, &teardown);
    }
    else
    {
        teardownCore();
    }

    return success;
}

bool teardown()
{
    finalizeExceptions();
    return teardownCore();
}

} // namespace med::python
