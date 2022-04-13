#pragma SWIG nowarn=503,509,602,325,312,314,317,362,361

%ignore operator=;
%ignore operator>>;
%ignore operator<<;
%ignore operator==;
%ignore operator[];
%ignore operator!=;
%ignore operator*=;
%ignore operator/=;
%ignore operator bool;
%ignore operator int;
%ignore operator float;
%ignore operator double;
%ignore operator double*;

#define decltype(x) int

#undef DTKCORESUPPORT_EXPORT
#define DTKCORESUPPORT_EXPORT

#undef QDOC_PROPERTY
#define QDOC_PROPERTY(x)

#undef QT_WARNING_DISABLE_GCC
#define QT_WARNING_DISABLE_GCC(x)

#undef QT_WARNING_POP
#define QT_WARNING_POP

#undef QT_WARNING_PUSH
#define QT_WARNING_PUSH

#undef Q_ALLOC_SIZE
#define Q_ALLOC_SIZE(x)

#undef Q_DECLARE_METATYPE
#define Q_DECLARE_METATYPE(x)

#undef Q_DECLARE_SHARED
#define Q_DECLARE_SHARED(x)

#undef Q_DECL_COLD_FUNCTION
#define Q_DECL_COLD_FUNCTION

#undef Q_DECL_CONSTEXPR
#define Q_DECL_CONSTEXPR

#undef Q_DECL_CONST_FUNCTION
#define Q_DECL_CONST_FUNCTION

#undef Q_DECL_EQ_DELETE
#define Q_DECL_EQ_DELETE

#undef Q_DECL_IMPORT
#define Q_DECL_IMPORT

#undef Q_DECL_NOEXCEPT
#define Q_DECL_NOEXCEPT

#undef Q_DECL_NOTHROW
#define Q_DECL_NOTHROW

#undef Q_DECL_OVERRIDE
#define Q_DECL_OVERRIDE

#undef Q_DECL_PURE_FUNCTION
#define Q_DECL_PURE_FUNCTION

#undef Q_DECL_RELAXED_CONSTEXPR
#define Q_DECL_RELAXED_CONSTEXPR

#undef Q_DECL_NOEXCEPT_EXPR
#define Q_DECL_NOEXCEPT_EXPR(x)

#undef Q_DECL_UNUSED
#define Q_DECL_UNUSED

#undef Q_ENUM
#define Q_ENUM(x)

#undef Q_FLAG
#define Q_FLAG(x)

#undef Q_GADGET
#define Q_GADGET

#undef Q_INVOKABLE
#define Q_INVOKABLE

#undef Q_NORETURN
#define Q_NORETURN

#undef Q_OBJECT
#define Q_OBJECT \
    public: \
        static const QMetaObject staticMetaObject; \
        virtual const QMetaObject *metaObject() const;

#undef Q_PRIVATE_SLOT
#define Q_PRIVATE_SLOT(...)

#undef Q_PROPERTY
#define Q_PROPERTY(...)

#undef Q_REQUIRED_RESULT
#define Q_REQUIRED_RESULT

#undef Q_SIGNALS
#define Q_SIGNALS public

#undef Q_SLOTS
#define Q_SLOTS

#define slots
#define signals private

%{
    #include <QObject>
    #include <QMetaObject>
    #include "medPython.h"

    template <class SENDER_TYPE>
    void connect_noargs(SENDER_TYPE* sender, void (SENDER_TYPE::*signal)(), PyObject* receiver)
    {
        med::python::Object object = med::python::Object::borrowed(receiver);
        sender->connect(sender, signal, [=]() { object();});
    }

    template <class SENDER_TYPE, class PRIVATE_TAG>
    void connect_noargs_private(SENDER_TYPE* sender, void (SENDER_TYPE::*signal)(PRIVATE_TAG), PyObject* receiver)
    {
        med::python::Object object = med::python::Object::borrowed(receiver);
        sender->connect(sender, signal, [=]() { object(); });
    }

    template <class SENDER_TYPE, class... ARGS>
    void connect(SENDER_TYPE* sender, void (SENDER_TYPE::*signal)(ARGS...), PyObject* receiver)
    {
        med::python::Object object = med::python::Object::borrowed(receiver);
        sender->connect(sender, signal, [=](ARGS... args) { object(args...); });
    }
%}

%feature("director:except")
{
    if ($error != nullptr)
    {
        med::python::propagateCurrentError();
    }
}

%exception
{
    try
    {
        $action
    }
    catch (med::python::Exception& e)
    {
        med::python::raiseError(e.nativeClass(), e.what());
        SWIG_fail;
    }
}

%define %medPythonTypemaps(TYPE)

    %typemap(in) TYPE
    {
        medPythonConvert($input, &$1);
        med::python::propagateErrorIfOccurred();
    }

    %typemap(directorout) TYPE
    {
        medPythonConvert($input, &$result);
        med::python::propagateErrorIfOccurred();
    }

    %typemap(out) TYPE
    {
        medPythonConvert($1, &$result);
        med::python::propagateErrorIfOccurred();
    }

    %typemap(directorin) TYPE (PyObject* temp)
    {
        medPythonConvert($1, &temp);
        med::python::propagateErrorIfOccurred();
        $input = temp;
    }

    %typemap(in, numinputs = 0) TYPE* OUTPUT (TYPE temp)
    {
        $1 = &temp;
    }

    %typemap(argout) TYPE* OUTPUT
    {
        PyObject* output;
        medPythonConvert(*$1, &output);
        $result = SWIG_Python_AppendOutput($result, output);
    }

%enddef
