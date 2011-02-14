#ifndef MEDSETTINGSWIDGET_H
#define MEDSETTINGSWIDGET_H

#include <QWidget>

class medSettingsWidgetPrivate;

class medSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit medSettingsWidget(const QString& section,
                               QWidget *parent = 0);

    virtual bool save(void);

signals:
    void showError(QObject *sender, const QString& text,unsigned int timeout=0);

public slots:

protected:
    virtual bool validate(void);


private:
    medSettingsWidgetPrivate *d;
};

#endif // MEDSETTINGSWIDGET_H
