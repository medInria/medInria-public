#ifndef MEDSETTINGSWIDGET_H
#define MEDSETTINGSWIDGET_H

#include <QWidget>

class medSettingsWidgetPrivate;

class medSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit medSettingsWidget(QWidget *parent = 0);

signals:

public slots:

private:
  medSettingsWidgetPrivate *d;
  void createSystemWidget();
};

#endif // MEDSETTINGSWIDGET_H
