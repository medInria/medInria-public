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

#include "medPythonToolsExport.h"

namespace med::python
{

inline const char* CONSOLE_TITLE = "Python console";
inline const char* CONSOLE_SHORTCUT = "Ctrl+Shift+P";
inline const int CONSOLE_WIDTH = 800;
inline const int CONSOLE_HEIGHT = 600;

MEDPYTHONTOOLS_EXPORT void initializeTools();

MEDPYTHONTOOLS_EXPORT void startConsole();

} // namespace med::python
