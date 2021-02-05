#pragma once

#include <stdexcept>

#include "medPythonCoreForward.h"
#include "medPythonError.h"
#include "medPythonExport.h"

namespace med
{
namespace python
{

class ExceptionBasePrivate;

class MEDPYTHON_EXPORT ExceptionBase : public std::exception
{
public:
    static PyObject* nativeType;

    ExceptionBase(const SourceCodeLocation& throwSite, QString message);
    ExceptionBase(const SourceCodeLocation& throwSite, PyObject* nativeException);
    ~ExceptionBase();

    const char* what() const throw() override;

    QString throwSite();

private:
    ExceptionBasePrivate* const d;

    QString formatMessage(PyObject* nativeException);
    QString formatMessageWithTracebackModule(PyObject* nativeException);
    PyObject* getErrorMessageList(PyObject* nativeException);
    QString concatenateErrorMessages(PyObject* errorMessageList);
    QString getMessageItem(PyObject* errorMessageList, ssize_t index);
};

} // namespace python
} // namespace med
