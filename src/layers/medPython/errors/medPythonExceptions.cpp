#include "medPythonExceptions.h"

#include "medPythonCore.h"

namespace med
{
namespace python
{

void internal::initialiazeExceptionClasses()
{
    Exception::nativeType = PyExc_Exception;
    GeneratorExit::nativeType = PyExc_GeneratorExit;
    KeyboardInterrupt::nativeType = PyExc_KeyboardInterrupt;
    SystemExit::nativeType = PyExc_SystemExit;
    ArithmeticError::nativeType = PyExc_ArithmeticError;
    FloatingPointError::nativeType = PyExc_FloatingPointError;
    OverflowError::nativeType = PyExc_OverflowError;
    ZeroDivisionError::nativeType = PyExc_ZeroDivisionError;
    ImportError::nativeType = PyExc_ImportError;
    ModuleNotFoundError::nativeType = PyExc_ModuleNotFoundError;
    LookupError::nativeType = PyExc_LookupError;
    IndexError::nativeType = PyExc_IndexError;
    KeyError::nativeType = PyExc_KeyError;
    NameError::nativeType = PyExc_NameError;
    UnboundLocalError::nativeType = PyExc_UnboundLocalError;
    OSError::nativeType = PyExc_OSError;
    BlockingIOError::nativeType = PyExc_BlockingIOError;
    ChildProcessError::nativeType = PyExc_ChildProcessError;
    ConnectionError::nativeType = PyExc_ConnectionError;
    BrokenPipeError::nativeType = PyExc_BrokenPipeError;
    ConnectionAbortedError::nativeType = PyExc_ConnectionAbortedError;
    ConnectionRefusedError::nativeType = PyExc_ConnectionRefusedError;
    ConnectionResetError::nativeType = PyExc_ConnectionResetError;
    FileExistsError::nativeType = PyExc_FileExistsError;
    FileNotFoundError::nativeType = PyExc_FileNotFoundError;
    InterruptedError::nativeType = PyExc_InterruptedError;
    IsADirectoryError::nativeType = PyExc_IsADirectoryError;
    NotADirectoryError::nativeType = PyExc_NotADirectoryError;
    PermissionError::nativeType = PyExc_PermissionError;
    ProcessLookupError::nativeType = PyExc_ProcessLookupError;
    TimeoutError::nativeType = PyExc_TimeoutError;
    RuntimeError::nativeType = PyExc_RuntimeError;
    NotImplementedError::nativeType = PyExc_NotImplementedError;
    RecursionError::nativeType = PyExc_RecursionError;
    SyntaxError::nativeType = PyExc_SyntaxError;
    IndentationError::nativeType = PyExc_IndentationError;
    TabError::nativeType = PyExc_TabError;
    ValueError::nativeType = PyExc_ValueError;
    UnicodeError::nativeType = PyExc_UnicodeError;
    UnicodeDecodeError::nativeType = PyExc_UnicodeDecodeError;
    UnicodeEncodeError::nativeType = PyExc_UnicodeEncodeError;
    UnicodeTranslateError::nativeType = PyExc_UnicodeTranslateError;
    AssertionError::nativeType = PyExc_AssertionError;
    AttributeError::nativeType = PyExc_AttributeError;
    BufferError::nativeType = PyExc_BufferError;
    EOFError::nativeType = PyExc_EOFError;
    MemoryError::nativeType = PyExc_MemoryError;
    ReferenceError::nativeType = PyExc_ReferenceError;
    StopAsyncIteration::nativeType = PyExc_StopAsyncIteration;
    StopIteration::nativeType = PyExc_StopIteration;
    SystemError::nativeType = PyExc_SystemError;
    TypeError::nativeType = PyExc_TypeError;
}

} // namespace python
} // namespace med
