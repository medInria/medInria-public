#include "medPythonExceptionBase.h"

#include "medPythonCore.h"

namespace med
{
namespace python
{

class ExceptionBasePrivate
{
public:
    SourceCodeLocation throwSite;
    QString message;
};

PyObject* ExceptionBase::nativeType = PyExc_BaseException;

ExceptionBase::ExceptionBase(const SourceCodeLocation& throwSite, QString message) :
    d(new ExceptionBasePrivate)
{
    d->throwSite = throwSite;
    PyObject* args = Py_BuildValue("s", qUtf8Printable(message));
    PyObject* nativeException = PyObject_CallObject(nativeType, args);
    PyErr_Clear();
    d->message = formatMessage(nativeException);
    Py_CLEAR(args);
    Py_CLEAR(nativeException);
}

ExceptionBase::ExceptionBase(const SourceCodeLocation& throwSite, PyObject* nativeException) :
    d(new ExceptionBasePrivate)
{
    d->throwSite = throwSite;
    d->message = formatMessage(nativeException);
    Py_CLEAR(nativeException);
}

ExceptionBase::~ExceptionBase()
{
    delete d;
}

const char* ExceptionBase::what() const throw()
{
    return qUtf8Printable(d->message);
}

QString ExceptionBase::throwSite()
{
    return QString("%1 (%2): %3")
            .arg(d->throwSite.file)
            .arg(d->throwSite.line)
            .arg(d->throwSite.function);
}

QString ExceptionBase::formatMessage(PyObject* nativeException)
{
    QString result;

    if (internal::getTracebackModule())
    {
        result = formatMessageWithTracebackModule(nativeException);
    }

    return result;
}

QString ExceptionBase::formatMessageWithTracebackModule(PyObject* nativeException)
{
    QString result;
    PyObject* errorMessageList = getErrorMessageList(nativeException);
    result = concatenateErrorMessages(errorMessageList);
    return result;
}

PyObject* ExceptionBase::getErrorMessageList(PyObject* nativeException)
{
    PyObject* traceback = PyObject_GetAttrString(nativeException, "__traceback__");
    QString formatMethodName = traceback ? "format_exception" : "format_exception_only";
    PyObject* formatMethod = PyObject_GetAttrString(internal::getTracebackModule(), qUtf8Printable(formatMethodName));
    PyObject* formatArgs;
    PyObject* errorMessageList = nullptr;

    if (traceback)
    {
        formatArgs = Py_BuildValue("NNN", Py_TYPE(nativeException), nativeException, traceback);
    }
    else
    {
        formatArgs = Py_BuildValue("NN", Py_TYPE(nativeException), nativeException);
    }

    if (formatMethod && formatArgs)
    {
        errorMessageList = PyObject_CallObject(formatMethod, formatArgs);
    }

    PyErr_Clear();
    Py_CLEAR(traceback);
    Py_CLEAR(formatMethod);
    Py_CLEAR(formatArgs);
    return errorMessageList;
}

QString ExceptionBase::concatenateErrorMessages(PyObject* errorMessageList)
{
    QString result;
    ssize_t numErrorMessages = PyObject_Length(errorMessageList);

    for (ssize_t i = 0; i < numErrorMessages; i++)
    {
        result.append(getMessageItem(errorMessageList, i));
    }

    PyErr_Clear();
    return result;
}

QString ExceptionBase::getMessageItem(PyObject* errorMessageList, ssize_t index)
{
    QString result;
    PyObject* messageItem = PySequence_GetItem(errorMessageList, index);

    if (messageItem)
    {
        PyObject* encodedMessageItem = PyUnicode_AsEncodedString(messageItem, "UTF-8", "strict");
        Py_CLEAR(messageItem);

        if (encodedMessageItem)
        {
            result = strdup(PyBytes_AS_STRING(encodedMessageItem));
            Py_CLEAR(encodedMessageItem);
         }
    }

    PyErr_Clear();
    return result;
}

} // namespace python
} // namespace med
