#pragma once

#include "medPythonExceptionBase.h"

namespace med
{
namespace python
{

template <class PARENT>
class MEDPYTHON_EXPORT ExceptionWrapper : public PARENT
{
public:
    static inline PyObject* nativeType = nullptr;

    using PARENT::PARENT;
};

// BASE EXCEPTIONS
typedef ExceptionWrapper<ExceptionBase> Exception;
typedef ExceptionWrapper<ExceptionBase> GeneratorExit;
typedef ExceptionWrapper<ExceptionBase> KeyboardInterrupt;
typedef ExceptionWrapper<ExceptionBase> SystemExit;

// ARITHMETIC ERRORS
typedef ExceptionWrapper<Exception> ArithmeticError;
typedef ExceptionWrapper<ArithmeticError> FloatingPointError;
typedef ExceptionWrapper<ArithmeticError> OverflowError;
typedef ExceptionWrapper<ArithmeticError> ZeroDivisionError;

// IMPORT ERRORS
typedef ExceptionWrapper<Exception> ImportError;
typedef ExceptionWrapper<ImportError> ModuleNotFoundError;

// LOOKUP ERRORS
typedef ExceptionWrapper<Exception> LookupError;
typedef ExceptionWrapper<LookupError> IndexError;
typedef ExceptionWrapper<LookupError> KeyError;

// NAME ERRORS
typedef ExceptionWrapper<Exception> NameError;
typedef ExceptionWrapper<NameError> UnboundLocalError;

// OS ERRORS
typedef ExceptionWrapper<Exception> OSError;
typedef ExceptionWrapper<OSError> BlockingIOError;
typedef ExceptionWrapper<OSError> ChildProcessError;
typedef ExceptionWrapper<OSError> ConnectionError;
typedef ExceptionWrapper<ConnectionError> BrokenPipeError;
typedef ExceptionWrapper<ConnectionError> ConnectionAbortedError;
typedef ExceptionWrapper<ConnectionError> ConnectionRefusedError;
typedef ExceptionWrapper<OSError> ConnectionResetError;
typedef ExceptionWrapper<OSError> FileExistsError;
typedef ExceptionWrapper<OSError> FileNotFoundError;
typedef ExceptionWrapper<OSError> InterruptedError;
typedef ExceptionWrapper<OSError> IsADirectoryError;
typedef ExceptionWrapper<OSError> NotADirectoryError;
typedef ExceptionWrapper<OSError> PermissionError;
typedef ExceptionWrapper<OSError> ProcessLookupError;
typedef ExceptionWrapper<OSError> TimeoutError;

// RUNTIME ERRORS
typedef ExceptionWrapper<Exception> RuntimeError;
typedef ExceptionWrapper<RuntimeError> NotImplementedError;
typedef ExceptionWrapper<RuntimeError> RecursionError;

// SYNTAX ERRORS
typedef ExceptionWrapper<Exception> SyntaxError;
typedef ExceptionWrapper<SyntaxError> IndentationError;
typedef ExceptionWrapper<IndentationError> TabError;

// VALUE ERRORS
typedef ExceptionWrapper<Exception> ValueError;
typedef ExceptionWrapper<ValueError> UnicodeError;
typedef ExceptionWrapper<UnicodeError> UnicodeDecodeError;
typedef ExceptionWrapper<UnicodeError> UnicodeEncodeError;
typedef ExceptionWrapper<UnicodeError> UnicodeTranslateError;

// OTHER ERRORS
typedef ExceptionWrapper<Exception> AssertionError;
typedef ExceptionWrapper<Exception> AttributeError;
typedef ExceptionWrapper<Exception> BufferError;
typedef ExceptionWrapper<Exception> EOFError;
typedef ExceptionWrapper<Exception> MemoryError;
typedef ExceptionWrapper<Exception> ReferenceError;
typedef ExceptionWrapper<Exception> StopAsyncIteration;
typedef ExceptionWrapper<Exception> StopIteration;
typedef ExceptionWrapper<Exception> SystemError;
typedef ExceptionWrapper<Exception> TypeError;

namespace internal
{

void initialiazeExceptionClasses();

} // namespace internal

} // namespace python
} // namespace med
