#ifndef MEDSETTINGSWIDGET_H
#define MEDSETTINGSWIDGET_H

#include <QWidget>

class medSettingsWidgetPrivate;

class medSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit medSettingsWidget(QWidget *parent = 0);

    virtual bool save(void);
    const QString& section()const;

signals:
    void showError(QObject *sender, const QString& text,unsigned int timeout=0);

public slots:

protected:
    virtual bool validate(void);
    void setSection(QString section);

private:
    medSettingsWidgetPrivate *d;
};

#endif // MEDSETTINGSWIDGET_H
