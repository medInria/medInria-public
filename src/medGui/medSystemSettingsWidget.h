#ifndef MEDSYSTEMSETTINGSWIDGET_H
#define MEDSYSTEMSETTINGSWIDGET_H

#include <QWidget>
#include "medSettingsWidget.h"

class medSystemSettingsWidgetPrivate;

class medSystemSettingsWidget : public medSettingsWidget
{
    Q_OBJECT
public:
    medSystemSettingsWidget(QWidget *parent = 0);
    medSystemSettingsWidget* createSystemSettingsWidget(QWidget *parent);

signals:

public slots:

private:
  medSystemSettingsWidgetPrivate *d;
};

#endif // MEDSYSTEMSETTINGSWIDGET_H
