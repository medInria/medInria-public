#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

/// The base class for propagating Python related exceptions. Direct instances
/// of this class are considered system failures and must only be caught by top
/// level code. Other functions should catch Exception (or a subclass) instead.
///

#include <stdexcept>

#include <QString>

#include "medPythonCoreForward.h"
#include "medPythonExceptionThrower.h"
#include "medPythonExport.h"

namespace med::python
{

struct BaseExceptionPrivate;

class MEDPYTHON_EXPORT BaseException : public std::exception
{
public:
    /// Returns the corresponding Python exception class;
    ///
    static PyObject* nativeClass();

    /// Temporarily creates a native Python exception and uses it to construct a
    /// C++ exception. This is intended for the wrapper code to throw exceptions
    /// as if they originated from the Python interpreter.
    ///
    BaseException(QString message);

    ~BaseException() override;

    const char* what() const throw() override;

protected:
    static PyObject* createNativeException(PyObject* nativeClass, QString message);

    BaseException(PyObject* nativeException);

    void initializeFromNativeException(PyObject* nativeException);

private:
    BaseExceptionPrivate* const d;

    // These functions use the private constructor and so must be friends
    template <class EXCEPTION>
    friend void registerExceptionThrower();
    friend void throwException(PyObject* nativeException);
};

} // namespace med::python
