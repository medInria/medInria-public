#undef DTKCORESUPPORT_EXPORT
#define DTKCORESUPPORT_EXPORT

%pythoncode
%{

    import sys

%}

%{
template <class... ARGS, class SENDER_TYPE>
void connect(SENDER_TYPE* sender, void (SENDER_TYPE::*signal)(ARGS...), PyObject* receiver)
{
    med::python::Object object = med::python::Object::borrowed(receiver);
    sender->connect(sender, signal, [=](ARGS... args) { object(args...); });
}
%}

#define slots
#define signals private

%include <medCoreLegacyExport.h>
