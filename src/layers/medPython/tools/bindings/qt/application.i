%{
#include <QApplication>
%}

%ignore QApplication::fontMetrics;
%ignore QCoreApplication::aboutToQuit;
%ignore qt_sendSpontaneousEvent;

%include "qcoreapplication.h"
%include "qguiapplication.h"

%ignore QApplication::palette;

%include "qapplication.h"

%inline
%{
    QApplication* qApplication()
    {
        return qApp;
    }
%}

%pythoncode
%{
    qApp = qApplication
%}
