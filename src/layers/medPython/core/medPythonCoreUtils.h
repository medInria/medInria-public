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

namespace med::python
{

#if defined(Q_OS_WIN)
const char PYTHON_PATH_DELIMITER = ';';
#else
const char PYTHON_PATH_DELIMITER = ':';
#endif

/// Do not use before Py_PreInitialize.
///
QString wideCharToQString(const wchar_t* input);

/// Do not use before Py_PreInitialize. The returned pointer must be freed using
/// freeWideChar.
///
wchar_t* qStringToWideChar(QString input);

void freeWideChar(wchar_t* input);

/// Do not use before Py_PreInitialize. The returned pointer must be freed using
/// freeWideCharList (it is up to the caller to retain the size of the list).
///
wchar_t** qStringListToWideChar(QStringList input);

void freeWideCharList(wchar_t** input, size_t numItems);

QStringList getPythonPath();

} // namespace med::python
