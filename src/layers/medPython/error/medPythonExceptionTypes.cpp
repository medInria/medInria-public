/*==============================================================================

 medInria

 Copyright (c) INRIA 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medPythonExceptionTypes.h"

#include "medPythonCoreAPI.h"
#include "medPythonExceptionThrower.h"

#define MEDPYTHON_NATIVE_CLASS_METHOD(exceptionClass, nativeClassObject) \
    PyObject* exceptionClass::nativeClass() \
    { \
        return nativeClassObject; \
    }

namespace med::python
{

MEDPYTHON_NATIVE_CLASS_METHOD(Exception, PyExc_Exception)
MEDPYTHON_NATIVE_CLASS_METHOD(GeneratorExit, PyExc_GeneratorExit)
MEDPYTHON_NATIVE_CLASS_METHOD(KeyboardInterrupt, PyExc_KeyboardInterrupt)
MEDPYTHON_NATIVE_CLASS_METHOD(SystemExit, PyExc_SystemExit)
MEDPYTHON_NATIVE_CLASS_METHOD(ArithmeticError, PyExc_ArithmeticError)
MEDPYTHON_NATIVE_CLASS_METHOD(FloatingPointError, PyExc_FloatingPointError)
MEDPYTHON_NATIVE_CLASS_METHOD(OverflowError, PyExc_OverflowError)
MEDPYTHON_NATIVE_CLASS_METHOD(ZeroDivisionError, PyExc_ZeroDivisionError)
MEDPYTHON_NATIVE_CLASS_METHOD(ImportError, PyExc_ImportError)
MEDPYTHON_NATIVE_CLASS_METHOD(ModuleNotFoundError, PyExc_ModuleNotFoundError)
MEDPYTHON_NATIVE_CLASS_METHOD(LookupError, PyExc_LookupError)
MEDPYTHON_NATIVE_CLASS_METHOD(IndexError, PyExc_IndexError)
MEDPYTHON_NATIVE_CLASS_METHOD(KeyError, PyExc_KeyError)
MEDPYTHON_NATIVE_CLASS_METHOD(NameError, PyExc_NameError)
MEDPYTHON_NATIVE_CLASS_METHOD(UnboundLocalError, PyExc_UnboundLocalError)
MEDPYTHON_NATIVE_CLASS_METHOD(OSError, PyExc_OSError)
MEDPYTHON_NATIVE_CLASS_METHOD(BlockingIOError, PyExc_BlockingIOError)
MEDPYTHON_NATIVE_CLASS_METHOD(ChildProcessError, PyExc_ChildProcessError)
MEDPYTHON_NATIVE_CLASS_METHOD(ConnectionError, PyExc_ConnectionError)
MEDPYTHON_NATIVE_CLASS_METHOD(BrokenPipeError, PyExc_BrokenPipeError)
MEDPYTHON_NATIVE_CLASS_METHOD(ConnectionAbortedError, PyExc_ConnectionAbortedError)
MEDPYTHON_NATIVE_CLASS_METHOD(ConnectionRefusedError, PyExc_ConnectionRefusedError)
MEDPYTHON_NATIVE_CLASS_METHOD(ConnectionResetError, PyExc_ConnectionResetError)
MEDPYTHON_NATIVE_CLASS_METHOD(FileExistsError, PyExc_FileExistsError)
MEDPYTHON_NATIVE_CLASS_METHOD(FileNotFoundError, PyExc_FileNotFoundError)
MEDPYTHON_NATIVE_CLASS_METHOD(InterruptedError, PyExc_InterruptedError)
MEDPYTHON_NATIVE_CLASS_METHOD(IsADirectoryError, PyExc_IsADirectoryError)
MEDPYTHON_NATIVE_CLASS_METHOD(NotADirectoryError, PyExc_NotADirectoryError)
MEDPYTHON_NATIVE_CLASS_METHOD(PermissionError, PyExc_PermissionError)
MEDPYTHON_NATIVE_CLASS_METHOD(ProcessLookupError, PyExc_ProcessLookupError)
MEDPYTHON_NATIVE_CLASS_METHOD(TimeoutError, PyExc_TimeoutError)
MEDPYTHON_NATIVE_CLASS_METHOD(RuntimeError, PyExc_RuntimeError)
MEDPYTHON_NATIVE_CLASS_METHOD(NotImplementedError, PyExc_NotImplementedError)
MEDPYTHON_NATIVE_CLASS_METHOD(RecursionError, PyExc_RecursionError)
MEDPYTHON_NATIVE_CLASS_METHOD(SyntaxError, PyExc_SyntaxError)
MEDPYTHON_NATIVE_CLASS_METHOD(IndentationError, PyExc_IndentationError)
MEDPYTHON_NATIVE_CLASS_METHOD(TabError, PyExc_TabError)
MEDPYTHON_NATIVE_CLASS_METHOD(ValueError, PyExc_ValueError)
MEDPYTHON_NATIVE_CLASS_METHOD(UnicodeError, PyExc_UnicodeError)
MEDPYTHON_NATIVE_CLASS_METHOD(UnicodeDecodeError, PyExc_UnicodeDecodeError)
MEDPYTHON_NATIVE_CLASS_METHOD(UnicodeEncodeError, PyExc_UnicodeEncodeError)
MEDPYTHON_NATIVE_CLASS_METHOD(UnicodeTranslateError, PyExc_UnicodeTranslateError)
MEDPYTHON_NATIVE_CLASS_METHOD(AssertionError, PyExc_AssertionError)
MEDPYTHON_NATIVE_CLASS_METHOD(AttributeError, PyExc_AttributeError)
MEDPYTHON_NATIVE_CLASS_METHOD(BufferError, PyExc_BufferError)
MEDPYTHON_NATIVE_CLASS_METHOD(EOFError, PyExc_EOFError)
MEDPYTHON_NATIVE_CLASS_METHOD(MemoryError, PyExc_MemoryError)
MEDPYTHON_NATIVE_CLASS_METHOD(ReferenceError, PyExc_ReferenceError)
MEDPYTHON_NATIVE_CLASS_METHOD(StopAsyncIteration, PyExc_StopAsyncIteration)
MEDPYTHON_NATIVE_CLASS_METHOD(StopIteration, PyExc_StopIteration)
MEDPYTHON_NATIVE_CLASS_METHOD(SystemError, PyExc_SystemError)
MEDPYTHON_NATIVE_CLASS_METHOD(TypeError, PyExc_TypeError)

void initializeExceptions()
{
    registerExceptionThrower<BaseException>();
    registerExceptionThrower<Exception>();
    registerExceptionThrower<GeneratorExit>();
    registerExceptionThrower<KeyboardInterrupt>();
    registerExceptionThrower<SystemExit>();
    registerExceptionThrower<ArithmeticError>();
    registerExceptionThrower<FloatingPointError>();
    registerExceptionThrower<OverflowError>();
    registerExceptionThrower<ZeroDivisionError>();
    registerExceptionThrower<ImportError>();
    registerExceptionThrower<ModuleNotFoundError>();
    registerExceptionThrower<LookupError>();
    registerExceptionThrower<IndexError>();
    registerExceptionThrower<KeyError>();
    registerExceptionThrower<NameError>();
    registerExceptionThrower<UnboundLocalError>();
    registerExceptionThrower<OSError>();
    registerExceptionThrower<BlockingIOError>();
    registerExceptionThrower<ChildProcessError>();
    registerExceptionThrower<ConnectionError>();
    registerExceptionThrower<BrokenPipeError>();
    registerExceptionThrower<ConnectionAbortedError>();
    registerExceptionThrower<ConnectionRefusedError>();
    registerExceptionThrower<ConnectionResetError>();
    registerExceptionThrower<FileExistsError>();
    registerExceptionThrower<FileNotFoundError>();
    registerExceptionThrower<InterruptedError>();
    registerExceptionThrower<IsADirectoryError>();
    registerExceptionThrower<NotADirectoryError>();
    registerExceptionThrower<PermissionError>();
    registerExceptionThrower<TimeoutError>();
    registerExceptionThrower<RuntimeError>();
    registerExceptionThrower<NotImplementedError>();
    registerExceptionThrower<RecursionError>();
    registerExceptionThrower<SyntaxError>();
    registerExceptionThrower<IndentationError>();
    registerExceptionThrower<TabError>();
    registerExceptionThrower<ValueError>();
    registerExceptionThrower<UnicodeError>();
    registerExceptionThrower<UnicodeDecodeError>();
    registerExceptionThrower<UnicodeEncodeError>();
    registerExceptionThrower<UnicodeTranslateError>();
    registerExceptionThrower<AssertionError>();
    registerExceptionThrower<AttributeError>();
    registerExceptionThrower<BufferError>();
    registerExceptionThrower<EOFError>();
    registerExceptionThrower<MemoryError>();
    registerExceptionThrower<ReferenceError>();
    registerExceptionThrower<StopAsyncIteration>();
    registerExceptionThrower<StopIteration>();
    registerExceptionThrower<SystemError>();
    registerExceptionThrower<TypeError>();
}

void finalizeExceptions()
{
    clearExceptionThrowers();
}

} // namespace med::python
