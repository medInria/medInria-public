#ifndef MEDDATABASESETTINGSWIDGET_H
#define MEDDATABASESETTINGSWIDGET_H

#include <medGui/medGuiExport.h>
#include <QWidget>
#include "medSettingsWidget.h"

class medDatabaseSettingsWidgetPrivate;

class MEDGUI_EXPORT medDatabaseSettingsWidget : public medSettingsWidget
{
    Q_OBJECT
public:
    medDatabaseSettingsWidget(QWidget *parent = 0);

signals:

public slots:
    void read();

private slots:
	void selectDbDirectory(void);

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
    medDatabaseSettingsWidgetPrivate *d;

};

MEDGUI_EXPORT medSettingsWidget* createDatabaseSettingsWidget(QWidget *parent);
#endif // MEDDATABASESETTINGSWIDGET_H
