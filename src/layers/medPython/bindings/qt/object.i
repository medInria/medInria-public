%feature("unref") QObject "if(!$this->parent()) delete $this;";
%feature("director") QObject;

struct QMetaObject
{
    const char* className() const;
    const QMetaObject* superClass() const;
    bool inherits(const QMetaObject* metaObject) const;
};

%define SIGNAL_NOARGS(cls, signal)

%{
template void connect_noargs(cls*, void (cls ## :: ## *)(), PyObject*);
%}

%extend cls
{
    void connect_noargs_ ## signal(PyObject* receiver)
    {
        connect_noargs($self, & ## cls ## :: ## signal, receiver);
    }
}

%enddef

%define SIGNAL_NOARGS_PRIVATE(cls, signal)

%{
template void connect_noargs_private(cls*, void (cls ## :: ## *)(cls ## :: ## QPrivateSignal), PyObject*);
%}

%extend cls
{
    void connect_noargs_ ## signal(PyObject* receiver)
    {
        connect_noargs_private($self, & ## cls ## :: ## signal, receiver);
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

SIGNAL(QObject, destroyed, QObject*)
