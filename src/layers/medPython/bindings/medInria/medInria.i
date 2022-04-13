%module(directors="1") medInria

%include "medPython.i"
%import "qt.i"

%feature("director");

%include <medCoreLegacyExport.h>

%medPythonTypemaps(med::python::Object)

%include "data.i"
%include "gui.i"

%{
    #include <QDebug>
%}

%inline
%{
    void _qDebug(QString message)
    {
        qDebug() << message;
    }

    void _qInfo(QString message)
    {
        qInfo() << message;
    }

    void _qWarning(QString message)
    {
        qWarning() << message;
    }

    void _qCritical(QString message)
    {
        qCritical() << message;
    }
%}

%pythoncode
%{
    def logDebug(message):
        _qDebug(message)

    def logInfo(message):
        _qInfo(message)

    def logWarning(message):
        _qWarning(message)

    def logCritical(message):
        _qCritical(message)
%}

%pythoncode
%{
    import sys
%}
