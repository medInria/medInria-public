#ifndef MEDSTARTUPSETTINGSWIDGET_H
#define MEDSTARTUPSETTINGSWIDGET_H

#include <medGui/medGuiExport.h>
#include <QWidget>
#include "medSettingsWidget.h"

class medStartupSettingsWidgetPrivate;

class MEDGUI_EXPORT medStartupSettingsWidget : public medSettingsWidget
{
    Q_OBJECT
public:
    medStartupSettingsWidget(QWidget *parent = 0);

signals:

public slots:
    void read();

protected:
    /**
     * @brief Performs the validation of each control inside the widget.
     *
     * @param void
     * @return true is the validation is successful, false otherwise.
    */
    bool validate(void);

    bool write();
private:
    medStartupSettingsWidgetPrivate *d;

};

MEDGUI_EXPORT medSettingsWidget* createStartupSettingsWidget(QWidget *parent);
#endif // MEDSTARTUPSETTINGSWIDGET_H
