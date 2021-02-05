#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include <cstddef>

#include <QStringList>

namespace med::python::core
{

#if defined(Q_OS_WIN)
const char PYTHON_PATH_DELIMITER = ';';
#else
const char PYTHON_PATH_DELIMITER = ':';
#endif

QString wideCharToQString(const wchar_t* input);

/// Note: The returned pointer must be freed using PyMem_RawFree
wchar_t* qStringToWideChar(QString input);

QStringList getPythonPath();

} // namespace med::python::core
