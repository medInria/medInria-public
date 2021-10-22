%feature("unref") QObject "if(!$this->parent()) delete $this;";
%feature("director") QObject;

%{
#include <QObject>
#include <QMetaObject>
%}

#undef Q_OBJECT
#define Q_OBJECT \
    public: \
        static const QMetaObject staticMetaObject; \
        virtual const QMetaObject *metaObject() const;

struct QMetaObject
{
    const char* className() const;
    const QMetaObject* superClass() const;
    bool inherits(const QMetaObject* metaObject) const;
};

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
    void connect_noargs_ ## signal(PyObject* receiver)
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
Q_OBJECT

public:
    virtual const QMetaObject* metaObject() const;
    void deleteLater();
};

%medPythonTypemaps(QObject*);

%extend QObject
{
    PyObject* _parent()
    {
        PyObject* result = med::python::wrapObjectWithSWIG($self->parent());
        med::python::propagateErrorIfOccurred();
        return result;
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
                connect = getattr(self, f'connect_{signal}', None)
                if not connect:
                    connect = getattr(self, f'connect_noargs_{signal}')
                connect(receiver)

        def parent(self):
            return self._parent()
    %}
}

SIGNAL_NOARGS(QObject, destroyed)
