#pragma once

#include "medGuiExport.h"
#include "medSettingsWidget.h"
#include <QWidget>

class medDatabaseSettingsWidgetPrivate;

class MEDGUI_EXPORT medDatabaseSettingsWidget : public medSettingsWidget
{
    Q_OBJECT
public:
    medDatabaseSettingsWidget(QWidget *parent = 0);

    QString identifier()  const;
    QString description() const;

signals:

public slots:
    void read();

private slots:
    void selectDbDirectory();

protected:
    /**
     * @brief Performs the validation of each control inside the widget.
     *
     * @param void
     * @return true is the validation is successful, false otherwise.
    */
    bool validate();

    bool write();
private:
    medDatabaseSettingsWidgetPrivate *d;

};

MEDGUI_EXPORT medSettingsWidget* createDatabaseSettingsWidget(QWidget *parent);

