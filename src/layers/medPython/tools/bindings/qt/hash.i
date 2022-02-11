%{
#include "medPythonError.h"
#include "medPythonQHashConversion.h"
%}

%define %qHashTypemaps(KEY_TYPE, VALUE_TYPE, PRECEDENCE)

    %feature("novaluewrapper") QHash<KEY_TYPE, VALUE_TYPE >;
    class QHash<KEY_TYPE, VALUE_TYPE >;

    %typecheck(PRECEDENCE) QHash<KEY_TYPE, VALUE_TYPE >
    {
        $1 = PyMapping_Check($input) ? 1 : 0;
    }

    %typemap(in) QHash<KEY_TYPE, VALUE_TYPE >
    {
        medPythonConvert<KEY_TYPE, VALUE_TYPE >($input, &$1);
        med::python::propagateErrorIfOccurred();
    }

    %typemap(directorout) QHash<KEY_TYPE, VALUE_TYPE >
    {
        medPythonConvert<KEY_TYPE, VALUE_TYPE >($input, &$result);
        med::python::propagateErrorIfOccurred();
    }

    %typemap(out) QHash<KEY_TYPE, VALUE_TYPE >
    {
        medPythonConvert<KEY_TYPE, VALUE_TYPE >($1, &$result);
        med::python::propagateErrorIfOccurred();
    }

    %typemap(directorin) QHash<KEY_TYPE, VALUE_TYPE >
    {
        medPythonConvert<KEY_TYPE, VALUE_TYPE >($1, $input);
        med::python::propagateErrorIfOccurred();
    }

    %typemap(argout) QHash<KEY_TYPE, VALUE_TYPE >* OUTPUT
    {
        PyObject* output;
        medPythonConvert<KEY_TYPE, VALUE_TYPE >(*$1, &output);
        $result = SWIG_Python_AppendOutput($result, output);
    }

%enddef

%qHashTypemaps(QString, QString, SWIG_TYPECHECK_MAP)
