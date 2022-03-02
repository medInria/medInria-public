#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonConversionUtils.h"
#include "medPythonDataIndexConversion.h"
#include "medPythonObjectConversion.h"
#include "medPythonPrimitiveConversion.h"
#include "medPythonQHashConversion.h"
#include "medPythonQListConversion.h"
#include "medPythonQObjectConversion.h"
#include "medPythonQStringConversion.h"
#include "medPythonQVariantConversion.h"

/// Conversions between C++ and Python are defined by overloading the
/// medPythonConvert function (this function is in the global namespace in order
/// to facilitate argument-dependant lookup):
///
/// The overload to convert a C++ type to Python has the following signature:
///
///     bool medPythonConvert(INPUT_TYPE value, PyObject** output);
///
/// The output must be a new reference to a Python object (meaning the caller
/// owns it). The function must return true if the conversion succeeded, false
/// otherwise. If the conversion failed, a Python error must be raised (but not
/// propagated to C++).
///
/// The overload to convert a Python object to C++ has the following signature:
///
///     bool medPythonConvert(const PyObject* object, OUTPUT_TYPE* output);
///
/// The caller retains ownership of the input object. The function must return
/// true if the conversion succeeded, false otherwise. If the conversion failed,
/// a Python error must be raised (but not propagated to C++).
///
