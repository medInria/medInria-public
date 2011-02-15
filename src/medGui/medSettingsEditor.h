#ifndef MEDSETTINGSEDITOR_H
#define MEDSETTINGSEDITOR_H

#include <QWidget>

class medSettingsEditorPrivate;

class medSettingsEditor : public QWidget
{
    Q_OBJECT
public:
    medSettingsEditor(QWidget *parent = 0);

signals:

public slots:
    void onSaveClicked();
    void onCancelClicked();
    void onResetClicked();
private:
  medSettingsEditorPrivate *d;
  void createSystemSettingsWidget();
};

#endif // MEDSETTINGSEDITOR_H
