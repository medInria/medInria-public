/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPipelineBindings.h"

#include <medExternalResources.h>
#include <medPython.h>

namespace med::pipeline
{

void initializeBindings()
{
    python::addPythonPath(getExternalResourcesDirectory(TARGET_NAME));
    python::addPythonPath(getExternalResourcePath(QString(TARGET_NAME) + ".zip", TARGET_NAME));
}

} // namespace med::pipeline
