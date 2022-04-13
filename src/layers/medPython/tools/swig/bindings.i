%module(package="medPythonTools") bindings

%include "medPython.i"

%{
    #include "medPythonSettingsWidget.h"
%}

%include "medPythonToolsExport.h"

%rename(SettingsWidget) med::python::PythonSettingsWidget;

namespace med::python
{

class PythonSettingsWidget : public medSettingsWidget
{
public:
    static bool registerWidget();
};

%pythoncode
%{

    def registerSettingsWidget():
        SettingsWidget.registerWidget()

%}

} // namespace med::python
