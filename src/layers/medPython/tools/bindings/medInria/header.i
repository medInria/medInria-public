#undef DTKCORESUPPORT_EXPORT
#define DTKCORESUPPORT_EXPORT

%feature("director");

%pythoncode
%{

    import sys

%}

%{
template <class SENDER_TYPE, class... ARGS>
void connect(SENDER_TYPE* sender, void (SENDER_TYPE::*signal)(ARGS...), PyObject* receiver)
{
    med::python::Object object = med::python::Object::borrowed(receiver);
    sender->connect(sender, signal, [=](ARGS... args) { object(args...); });
}
%}

#define slots
#define signals private

%include <medCoreLegacyExport.h>

%medPythonTypemaps(med::python::Object)
