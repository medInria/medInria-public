%{
#include "medPythonError.h"
#include "medPythonQListConversion.h"
%}

%define %qListTypemaps(CTYPE, PRECEDENCE)

    %feature("novaluewrapper") QList<CTYPE >;
    class QList<CTYPE >;

    %typecheck(PRECEDENCE) QList<CTYPE >
    {
        $1 = PySequence_Check($input) ? 1 : 0;
    }

    %typemap(in) QList<CTYPE >
    {
        medPythonConvert<CTYPE >($input, &$1);
        med::python::propagateErrorIfOccurred();
    }

    %typemap(directorout) QList<CTYPE >
    {
        medPythonConvert<CTYPE >($input, &$result);
        med::python::propagateErrorIfOccurred();
    }

    %typemap(out) QList<CTYPE >
    {
        medPythonConvert<CTYPE >($1, &$result);
        med::python::propagateErrorIfOccurred();
    }

    %typemap(directorin) QList<CTYPE >
    {
        medPythonConvert<CTYPE >($1, $input);
        med::python::propagateErrorIfOccurred();
    }

    %typemap(argout) QList<CTYPE >* OUTPUT
    {
        PyObject* output;
        medPythonConvert<CTYPE >(*$1, &output);
        $result = SWIG_Python_AppendOutput($result, output);
    }

%enddef

%qListTypemaps(bool, SWIG_TYPECHECK_BOOL_ARRAY)
%qListTypemaps(long, SWIG_TYPECHECK_INT64_ARRAY)
%qListTypemaps(double, SWIG_TYPECHECK_DOUBLE_ARRAY)
%qListTypemaps(QString, SWIG_TYPECHECK_STRING_ARRAY)

%apply QList<QString> { QStringList };
