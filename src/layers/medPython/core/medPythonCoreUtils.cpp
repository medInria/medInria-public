/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

// Must be included first (see the header notes for details)
#include "medPythonCoreAPI.h"

#include "medPythonCoreUtils.h"

namespace med::python::core
{

QString wideCharToQString(const wchar_t* input)
{
    QString result;
    char* result_char = Py_EncodeLocale(input, nullptr);
    result = result_char;
    PyMem_Free(result_char);
    return result;
}

wchar_t* qStringToWideChar(QString input)
{
    return Py_DecodeLocale(qUtf8Printable(input), nullptr);
}

QStringList getPythonPath()
{
    return wideCharToQString(Py_GetPath()).split(PYTHON_PATH_DELIMITER);
}

} // namespace med::python::core
