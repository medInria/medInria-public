#pragma once

#include <medSettingsWidget.h>

#include "medPythonToolsExport.h"

namespace med::python
{

class PythonSettingsWidgetPrivate;

class MEDPYTHONTOOLS_EXPORT PythonSettingsWidget : public medSettingsWidget
{
    Q_OBJECT
    MED_SETTINGS_INTERFACE("Python", "Python Settings")

public:
    PythonSettingsWidget(QWidget* parent = 0);

    bool write() override;

public slots:
    void read() override;
    bool validate() override;

private slots:
    void addPath();

private:
    PythonSettingsWidgetPrivate *d;
};

} // namespace med::python
