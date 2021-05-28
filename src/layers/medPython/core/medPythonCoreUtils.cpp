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

namespace med::python
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

void freeWideChar(wchar_t* input)
{
    PyMem_RawFree(input);
}

wchar_t** qStringListToWideChar(QStringList input)
{
    wchar_t** result = (wchar_t**)PyMem_RawMalloc(input.size() * sizeof(wchar_t*));

    for (int i = 0; i < input.size(); i++)
    {
        result[i] = qStringToWideChar(input[i]);
    }

    return result;
}

void freeWideCharList(wchar_t** input, size_t numItems)
{
    for (size_t i = 0; i < numItems; i++)
    {
        freeWideChar(input[i]);
    }

    PyMem_RawFree(input);
}

QStringList getPythonPath()
{
    return wideCharToQString(Py_GetPath()).split(PYTHON_PATH_DELIMITER);
}

} // namespace med::python
