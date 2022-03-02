%{
#include "medPythonError.h"
#include "medPythonQStringConversion.h"
%}

%typemap(typecheck) QString = char *;

%medPythonTypemaps(QString);

%apply QString { const QString };

%typemap(typecheck) const QString& = char *;

%typemap(in) const QString& (QString temp)
{
    medPythonConvert($input, &temp);
    med::python::propagateErrorIfOccurred();
    $1 = &temp;
}

%typemap(directorout) const QString& (QString temp)
{
    medPythonConvert($input, &temp);
    med::python::propagateErrorIfOccurred();
    $result = temp;
}

%typemap(out) const QString&
{
    medPythonConvert(*$1, &$result);
    med::python::propagateErrorIfOccurred();
}

%typemap(directorin) const QString& (PyObject* temp)
{
    medPythonConvert($1, &temp);
    med::python::propagateErrorIfOccurred();
    $input = temp;
}
