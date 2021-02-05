#pragma once

#include "QString"

#include "medPythonCoreForward.h"
#include "medPythonExport.h"

#define MEDPYTHON_CODE_LOCATION med::python::SourceCodeLocation{__FILE__, __LINE__, __func__}

namespace med
{
namespace python
{

struct SourceCodeLocation
{
    QString file;
    int line;
    QString function;
};

// Checks if an error occurred during the execution of a CPython function. The
// corresponding exception can be fetched with retrieveCurrentException().
MEDPYTHON_EXPORT bool errorOccured();

// Clears the error indicator. Must be called every time an error is handled or
// else the next CPython function will mistakingly believe there was an error
// (the propagation functions do this automatically).
MEDPYTHON_EXPORT void clearError();

MEDPYTHON_EXPORT PyObject* retrieveCurrentException();

// If there was an error, wrap the corresponding exception and throw it.
MEDPYTHON_EXPORT void propagateErrorIfOccured(const SourceCodeLocation& propagationSite);

// Wrap the current exception and throw it. Must only be called if there was an
// error.
MEDPYTHON_EXPORT void propagateCurrentError(const SourceCodeLocation& propagationSite);

// Wrap an exception and throw it.
MEDPYTHON_EXPORT void propagateError(const SourceCodeLocation& propagationSite, PyObject* nativeException);

namespace internal
{

void initializeErrorHandling();
void finalizeErrorHandling();

PyObject* findRegisteredExceptionClass(PyObject* nativeException);

PyObject* getTracebackModule();

} // namespace internal

} // namespace python
} // namespace med
