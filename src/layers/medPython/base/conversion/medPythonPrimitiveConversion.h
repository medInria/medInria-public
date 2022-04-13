#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonCoreForward.h"
#include "medPythonExport.h"

MEDPYTHON_EXPORT bool medPythonConvert(bool value, PyObject** output);
MEDPYTHON_EXPORT bool medPythonConvert(const PyObject* object, bool* output);

MEDPYTHON_EXPORT bool medPythonConvert(int value, PyObject** output);
MEDPYTHON_EXPORT bool medPythonConvert(const PyObject* object, long* output);

MEDPYTHON_EXPORT bool medPythonConvert(double value, PyObject** output);
MEDPYTHON_EXPORT bool medPythonConvert(const PyObject* object, double* output);
