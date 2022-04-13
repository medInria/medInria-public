#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include <QVariant>

#include "medPythonCoreForward.h"
#include "medPythonExport.h"

MEDPYTHON_EXPORT bool medPythonConvert(const QVariant& value, PyObject** output);
MEDPYTHON_EXPORT bool medPythonConvert(const PyObject* object, QVariant* output);
