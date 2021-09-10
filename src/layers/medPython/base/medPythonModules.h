#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include <QString>

#include "medPythonExport.h"

namespace med::python
{

MEDPYTHON_EXPORT void registerModulePath(QString path);

MEDPYTHON_EXPORT void loadPlugins();

} // namespace med::python
