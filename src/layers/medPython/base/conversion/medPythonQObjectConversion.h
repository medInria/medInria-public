#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include <QObject>

#include "medPythonCore.h"
#include "medPythonExport.h"

MEDPYTHON_EXPORT bool medPythonConvert(const QObject* object, PyObject** output);
MEDPYTHON_EXPORT bool medPythonConvert(const PyObject* nativeObject, QObject** output);
