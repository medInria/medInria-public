#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#undef slots
#define slots _slots

#undef _LARGEFILE_SOURCE

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#undef slots
#define slots Q_SLOTS
