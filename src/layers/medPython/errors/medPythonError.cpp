#include "medPythonError.h"

#include <QHash>

#include "medPythonCore.h"
#include "medPythonExceptions.h"

namespace med
{
namespace python
{

namespace
{

PyObject* tracebackModule;

typedef void (*PropagationFunction)(const SourceCodeLocation&, PyObject*);

// Maps a Python exception class to a function that wraps and throws instances
// of that exception.
QHash<const PyObject*, PropagationFunction> propagationFunctions;

template <class EXCEPTION_WRAPPER>
void initializeErrorPropagation()
{
    PropagationFunction propagate = [](const SourceCodeLocation& propagationSite, PyObject* nativeException)
    {
        throw EXCEPTION_WRAPPER(propagationSite, nativeException);
    };

    propagationFunctions[EXCEPTION_WRAPPER::nativeType] = propagate;
}

void initializeErrorPropagation()
{
    if (propagationFunctions.isEmpty())
    {
        initializeErrorPropagation<ExceptionBase>();
        initializeErrorPropagation<Exception>();
        initializeErrorPropagation<GeneratorExit>();
        initializeErrorPropagation<KeyboardInterrupt>();
        initializeErrorPropagation<SystemExit>();
        initializeErrorPropagation<ArithmeticError>();
        initializeErrorPropagation<FloatingPointError>();
        initializeErrorPropagation<OverflowError>();
        initializeErrorPropagation<ZeroDivisionError>();
        initializeErrorPropagation<ImportError>();
        initializeErrorPropagation<ModuleNotFoundError>();
        initializeErrorPropagation<LookupError>();
        initializeErrorPropagation<IndexError>();
        initializeErrorPropagation<KeyError>();
        initializeErrorPropagation<NameError>();
        initializeErrorPropagation<UnboundLocalError>();
        initializeErrorPropagation<OSError>();
        initializeErrorPropagation<BlockingIOError>();
        initializeErrorPropagation<ChildProcessError>();
        initializeErrorPropagation<ConnectionError>();
        initializeErrorPropagation<BrokenPipeError>();
        initializeErrorPropagation<ConnectionAbortedError>();
        initializeErrorPropagation<ConnectionRefusedError>();
        initializeErrorPropagation<ConnectionResetError>();
        initializeErrorPropagation<FileExistsError>();
        initializeErrorPropagation<FileNotFoundError>();
        initializeErrorPropagation<InterruptedError>();
        initializeErrorPropagation<IsADirectoryError>();
        initializeErrorPropagation<NotADirectoryError>();
        initializeErrorPropagation<PermissionError>();
        initializeErrorPropagation<TimeoutError>();
        initializeErrorPropagation<RuntimeError>();
        initializeErrorPropagation<NotImplementedError>();
        initializeErrorPropagation<RecursionError>();
        initializeErrorPropagation<SyntaxError>();
        initializeErrorPropagation<IndentationError>();
        initializeErrorPropagation<TabError>();
        initializeErrorPropagation<ValueError>();
        initializeErrorPropagation<UnicodeError>();
        initializeErrorPropagation<UnicodeDecodeError>();
        initializeErrorPropagation<UnicodeEncodeError>();
        initializeErrorPropagation<UnicodeTranslateError>();
        initializeErrorPropagation<AssertionError>();
        initializeErrorPropagation<AttributeError>();
        initializeErrorPropagation<BufferError>();
        initializeErrorPropagation<EOFError>();
        initializeErrorPropagation<MemoryError>();
        initializeErrorPropagation<ReferenceError>();
        initializeErrorPropagation<StopAsyncIteration>();
        initializeErrorPropagation<StopIteration>();
        initializeErrorPropagation<SystemError>();
        initializeErrorPropagation<TypeError>();
    }
}

} // namespace

void internal::initializeErrorHandling()
{
    PyObject* tracebackString = PyUnicode_FromString("traceback");
    tracebackModule = PyImport_Import(tracebackString);
    PyErr_Clear();
    initialiazeExceptionClasses();
    initializeErrorPropagation();
}

void internal::finalizeErrorHandling()
{
    Py_CLEAR(tracebackModule);
    propagationFunctions.clear();
}

bool errorOccured()
{
    return PyErr_Occurred();
}

void clearError()
{
    PyErr_Clear();
}

PyObject* retrieveCurrentException()
{
    PyObject* exceptionType;
    PyObject* exceptionInstance;
    PyObject* traceback;

    PyErr_Fetch(&exceptionType, &exceptionInstance, &traceback);
    PyErr_NormalizeException(&exceptionType, &exceptionInstance, &traceback);

    if (traceback)
    {
        PyException_SetTraceback(exceptionInstance, traceback);
    }

    Py_CLEAR(exceptionType);
    Py_CLEAR(traceback);

    return exceptionInstance;
}

void propagateErrorIfOccured(const SourceCodeLocation& propagationSite)
{
    if (PyErr_Occurred())
    {
        propagateCurrentError(propagationSite);
    }
}

void propagateCurrentError(const SourceCodeLocation& propagationSite)
{
    PyObject* nativeException = retrieveCurrentException();
    PyErr_Clear();
    propagateError(propagationSite, nativeException);
}

// A native exception might be a subclass instance of one of the registered
// exception classes. To wrap it we must go up the hierarchy to find the actual
// registered class.
void propagateError(const SourceCodeLocation& propagationSite, PyObject* nativeException)
{
    PyObject* registeredExceptionClass = internal::findRegisteredExceptionClass(nativeException);

    if (registeredExceptionClass)
    {
        PropagationFunction propagate = propagationFunctions[registeredExceptionClass];
        Py_CLEAR(registeredExceptionClass);
        propagate(propagationSite, nativeException);
    }
    else
    {
        // We failed to find a propagation function, so we throw as a base
        // exception.
        throw ExceptionBase(propagationSite, nativeException);
    }
}

PyObject* internal::findRegisteredExceptionClass(PyObject* nativeException)
{
    PyObject* exceptionClass = PyObject_Type(nativeException);
    PyObject* registeredClass = nullptr;

    if (propagationFunctions.contains(exceptionClass))
    {
        registeredClass = exceptionClass;
    }
    else
    {
        PyObject* typeLineage = PyObject_GetAttrString(exceptionClass, "__mro__"); // method resolution order
        Py_CLEAR(exceptionClass);

        if (!PyErr_Occurred() && PyTuple_Check(typeLineage))
        {
            ssize_t lineageSize = PyTuple_GET_SIZE(typeLineage);

            for (int i = 0; i < lineageSize; i++)
            {
                PyObject* baseClass = PyTuple_GET_ITEM(typeLineage, i);

                if (propagationFunctions.contains(baseClass))
                {
                    Py_INCREF(baseClass);
                    registeredClass = baseClass;
                    break;
                }
            }
        }

        Py_CLEAR(typeLineage);
    }

    PyErr_Clear();
    return registeredClass;
}

PyObject* internal::getTracebackModule()
{
    return tracebackModule;
}

} // namespace python
} // namespace med
