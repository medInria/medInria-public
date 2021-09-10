%feature("unref") QObject "if(!$this->parent()) delete $this;";
%feature("director") QObject;

%{
#include <QObject>
%}

%{
template <class... ARGS, class SENDER_TYPE>
void connect(SENDER_TYPE* sender, void (SENDER_TYPE::*signal)(ARGS...), PyObject* receiver)
{
    med::python::Object object = med::python::Object::borrowed(receiver);
    sender->connect(sender, signal, [=](ARGS... args) { object(args...); });
}
%}

%define SIGNAL_NOARGS(cls, signal)

%{
template void connect(cls*, void (cls ## :: ## *)(), PyObject*);
%}

%extend cls
{
    void connect_ ## signal(PyObject* receiver)
    {
        connect($self, & ## cls ## :: ## signal, receiver);
    }
}

%enddef

%define SIGNAL(cls, signal, ...)

%{
    template void connect(cls*, void (cls ## :: ## *)(__VA_ARGS__), PyObject*);
%}

%extend cls
{
    void connect_ ## signal(PyObject* receiver)
    {
        connect($self, & ## cls ## :: ## signal, receiver);
    }
}

%enddef

%rename(parent) QObject::getParent;

class QObject
{
public:
    void deleteLater();
};

%extend QObject
{
    QObject* getParent()
    {
        return $self->parent();
    }

    QVariant getProperty(QString name)
    {
        return $self->property(qUtf8Printable(name));
    }

    void connectOldStyle(QString signal, QObject* receiver, QString slot)
    {
        $self->connect($self, qUtf8Printable(signal.prepend("2")), receiver, qUtf8Printable(slot.prepend("1")));
    }

    %pythoncode
    %{
        def connect(self, signal, receiver, slot=None):
            if slot:
                self.connectOldStyle(signal, receiver, slot)
            else:
                connect = getattr(self, f'connect_{signal}')
                connect(receiver)
    %}
}

SIGNAL_NOARGS(QObject, destroyed)
