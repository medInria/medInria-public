%{
#include "medPythonError.h"
#include "medPythonQVariantConversion.h"
%}

class QVariant
{
};

%typemap(in) QVariant
{
    medPythonConvert($input, &$1);
    med::python::propagateErrorIfOccurred();
}

%typemap(directorout) QVariant
{
    medPythonConvert($input, &$result);
    med::python::propagateErrorIfOccurred();
}

%typemap(out) QVariant
{
    medPythonConvert($1, &$result);
    med::python::propagateErrorIfOccurred();
}

%typemap(directorin) QVariant
{
    medPythonConvert($1, $input);
    med::python::propagateErrorIfOccurred();
}
