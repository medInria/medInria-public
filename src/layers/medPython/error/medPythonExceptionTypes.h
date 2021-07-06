#pragma once
/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

/// Each Python exception class is associated with a corresponding C++ class.
/// (Refer to the Python documentation for more information on the exception
/// types)
///

#include "medPythonBaseException.h"

#define MEDPYTHON_EXCEPTION_CLASS(name, parent) \
    class name : public parent \
    { \
    public: \
        static PyObject* nativeClass();  \
        name(QString message) : parent(createNativeException(nativeClass(), message)) {} \
    protected: \
        name(PyObject* nativeException) : parent(nativeException) {} \
        template <class EXCEPTION> \
        friend void registerExceptionThrower(); \
    }

namespace med::python
{

// BASE EXCEPTIONS
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(Exception, BaseException);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(GeneratorExit, BaseException);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(KeyboardInterrupt, BaseException);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(SystemExit, BaseException);

// ARITHMETIC ERRORS
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(ArithmeticError, Exception);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(FloatingPointError, ArithmeticError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(OverflowError, ArithmeticError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(ZeroDivisionError, ArithmeticError);

// IMPORT ERRORS
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(ImportError, Exception);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(ModuleNotFoundError, ImportError);

// LOOKUP ERRORS
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(LookupError, Exception);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(IndexError, LookupError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(KeyError, LookupError);

// NAME ERRORS
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(NameError, Exception);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(UnboundLocalError, NameError);

// OS ERRORS
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(OSError, Exception);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(BlockingIOError, OSError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(ChildProcessError, OSError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(ConnectionError, OSError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(BrokenPipeError, ConnectionError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(ConnectionAbortedError, ConnectionError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(ConnectionRefusedError, ConnectionError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(ConnectionResetError, OSError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(FileExistsError, OSError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(FileNotFoundError, OSError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(InterruptedError, OSError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(IsADirectoryError, OSError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(NotADirectoryError, OSError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(PermissionError, OSError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(ProcessLookupError, OSError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(TimeoutError, OSError);

// RUNTIME ERRORS
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(RuntimeError, Exception);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(NotImplementedError, RuntimeError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(RecursionError, RuntimeError);

// SYNTAX ERRORS
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(SyntaxError, Exception);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(IndentationError, SyntaxError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(TabError, IndentationError);

// VALUE ERRORS
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(ValueError, Exception);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(UnicodeError, ValueError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(UnicodeDecodeError, UnicodeError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(UnicodeEncodeError, UnicodeError);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(UnicodeTranslateError, UnicodeError);

// OTHER ERRORS
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(AssertionError, Exception);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(AttributeError, Exception);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(BufferError, Exception);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(EOFError, Exception);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(MemoryError, Exception);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(ReferenceError, Exception);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(StopAsyncIteration, Exception);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(StopIteration, Exception);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(SystemError, Exception);
MEDPYTHON_EXPORT MEDPYTHON_EXCEPTION_CLASS(TypeError, Exception);

void initializeExceptions();

void finalizeExceptions();

} // namespace med::python
