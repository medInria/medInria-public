/*=========================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medPythonExceptionTypes.h"

#include "medPythonCore.h"
#include "medPythonExceptionThrower.h"

namespace med::python
{

PyObject* const Exception::nativeClass = PyExc_Exception;
PyObject* const GeneratorExit::nativeClass = PyExc_GeneratorExit;
PyObject* const KeyboardInterrupt::nativeClass = PyExc_KeyboardInterrupt;
PyObject* const SystemExit::nativeClass = PyExc_SystemExit;
PyObject* const ArithmeticError::nativeClass = PyExc_ArithmeticError;
PyObject* const FloatingPointError::nativeClass = PyExc_FloatingPointError;
PyObject* const OverflowError::nativeClass = PyExc_OverflowError;
PyObject* const ZeroDivisionError::nativeClass = PyExc_ZeroDivisionError;
PyObject* const ImportError::nativeClass = PyExc_ImportError;
PyObject* const ModuleNotFoundError::nativeClass = PyExc_ModuleNotFoundError;
PyObject* const LookupError::nativeClass = PyExc_LookupError;
PyObject* const IndexError::nativeClass = PyExc_IndexError;
PyObject* const KeyError::nativeClass = PyExc_KeyError;
PyObject* const NameError::nativeClass = PyExc_NameError;
PyObject* const UnboundLocalError::nativeClass = PyExc_UnboundLocalError;
PyObject* const OSError::nativeClass = PyExc_OSError;
PyObject* const BlockingIOError::nativeClass = PyExc_BlockingIOError;
PyObject* const ChildProcessError::nativeClass = PyExc_ChildProcessError;
PyObject* const ConnectionError::nativeClass = PyExc_ConnectionError;
PyObject* const BrokenPipeError::nativeClass = PyExc_BrokenPipeError;
PyObject* const ConnectionAbortedError::nativeClass = PyExc_ConnectionAbortedError;
PyObject* const ConnectionRefusedError::nativeClass = PyExc_ConnectionRefusedError;
PyObject* const ConnectionResetError::nativeClass = PyExc_ConnectionResetError;
PyObject* const FileExistsError::nativeClass = PyExc_FileExistsError;
PyObject* const FileNotFoundError::nativeClass = PyExc_FileNotFoundError;
PyObject* const InterruptedError::nativeClass = PyExc_InterruptedError;
PyObject* const IsADirectoryError::nativeClass = PyExc_IsADirectoryError;
PyObject* const NotADirectoryError::nativeClass = PyExc_NotADirectoryError;
PyObject* const PermissionError::nativeClass = PyExc_PermissionError;
PyObject* const ProcessLookupError::nativeClass = PyExc_ProcessLookupError;
PyObject* const TimeoutError::nativeClass = PyExc_TimeoutError;
PyObject* const RuntimeError::nativeClass = PyExc_RuntimeError;
PyObject* const NotImplementedError::nativeClass = PyExc_NotImplementedError;
PyObject* const RecursionError::nativeClass = PyExc_RecursionError;
PyObject* const SyntaxError::nativeClass = PyExc_SyntaxError;
PyObject* const IndentationError::nativeClass = PyExc_IndentationError;
PyObject* const TabError::nativeClass = PyExc_TabError;
PyObject* const ValueError::nativeClass = PyExc_ValueError;
PyObject* const UnicodeError::nativeClass = PyExc_UnicodeError;
PyObject* const UnicodeDecodeError::nativeClass = PyExc_UnicodeDecodeError;
PyObject* const UnicodeEncodeError::nativeClass = PyExc_UnicodeEncodeError;
PyObject* const UnicodeTranslateError::nativeClass = PyExc_UnicodeTranslateError;
PyObject* const AssertionError::nativeClass = PyExc_AssertionError;
PyObject* const AttributeError::nativeClass = PyExc_AttributeError;
PyObject* const BufferError::nativeClass = PyExc_BufferError;
PyObject* const EOFError::nativeClass = PyExc_EOFError;
PyObject* const MemoryError::nativeClass = PyExc_MemoryError;
PyObject* const ReferenceError::nativeClass = PyExc_ReferenceError;
PyObject* const StopAsyncIteration::nativeClass = PyExc_StopAsyncIteration;
PyObject* const StopIteration::nativeClass = PyExc_StopIteration;
PyObject* const SystemError::nativeClass = PyExc_SystemError;
PyObject* const TypeError::nativeClass = PyExc_TypeError;

namespace exception_types
{

void initialize()
{
    exception_thrower::registerThrower<BaseException>();
    exception_thrower::registerThrower<Exception>();
    exception_thrower::registerThrower<GeneratorExit>();
    exception_thrower::registerThrower<KeyboardInterrupt>();
    exception_thrower::registerThrower<SystemExit>();
    exception_thrower::registerThrower<ArithmeticError>();
    exception_thrower::registerThrower<FloatingPointError>();
    exception_thrower::registerThrower<OverflowError>();
    exception_thrower::registerThrower<ZeroDivisionError>();
    exception_thrower::registerThrower<ImportError>();
    exception_thrower::registerThrower<ModuleNotFoundError>();
    exception_thrower::registerThrower<LookupError>();
    exception_thrower::registerThrower<IndexError>();
    exception_thrower::registerThrower<KeyError>();
    exception_thrower::registerThrower<NameError>();
    exception_thrower::registerThrower<UnboundLocalError>();
    exception_thrower::registerThrower<OSError>();
    exception_thrower::registerThrower<BlockingIOError>();
    exception_thrower::registerThrower<ChildProcessError>();
    exception_thrower::registerThrower<ConnectionError>();
    exception_thrower::registerThrower<BrokenPipeError>();
    exception_thrower::registerThrower<ConnectionAbortedError>();
    exception_thrower::registerThrower<ConnectionRefusedError>();
    exception_thrower::registerThrower<ConnectionResetError>();
    exception_thrower::registerThrower<FileExistsError>();
    exception_thrower::registerThrower<FileNotFoundError>();
    exception_thrower::registerThrower<InterruptedError>();
    exception_thrower::registerThrower<IsADirectoryError>();
    exception_thrower::registerThrower<NotADirectoryError>();
    exception_thrower::registerThrower<PermissionError>();
    exception_thrower::registerThrower<TimeoutError>();
    exception_thrower::registerThrower<RuntimeError>();
    exception_thrower::registerThrower<NotImplementedError>();
    exception_thrower::registerThrower<RecursionError>();
    exception_thrower::registerThrower<SyntaxError>();
    exception_thrower::registerThrower<IndentationError>();
    exception_thrower::registerThrower<TabError>();
    exception_thrower::registerThrower<ValueError>();
    exception_thrower::registerThrower<UnicodeError>();
    exception_thrower::registerThrower<UnicodeDecodeError>();
    exception_thrower::registerThrower<UnicodeEncodeError>();
    exception_thrower::registerThrower<UnicodeTranslateError>();
    exception_thrower::registerThrower<AssertionError>();
    exception_thrower::registerThrower<AttributeError>();
    exception_thrower::registerThrower<BufferError>();
    exception_thrower::registerThrower<EOFError>();
    exception_thrower::registerThrower<MemoryError>();
    exception_thrower::registerThrower<ReferenceError>();
    exception_thrower::registerThrower<StopAsyncIteration>();
    exception_thrower::registerThrower<StopIteration>();
    exception_thrower::registerThrower<SystemError>();
    exception_thrower::registerThrower<TypeError>();
}

void finalize()
{
    exception_thrower::clearThrowers();
}

} // namespace exception_types
} // namespace med::python
