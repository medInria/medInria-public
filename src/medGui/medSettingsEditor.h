#ifndef MEDSETTINGSEDITOR_H
#define MEDSETTINGSEDITOR_H

#include <medGui/medGuiExport.h>
#include <QWidget>

class medSettingsEditorPrivate;

class MEDGUI_EXPORT medSettingsEditor : public QWidget
{
    Q_OBJECT
public:
    medSettingsEditor(QWidget *parent = 0);

signals:
    /**
     * @brief Emits an error message for the medMessageController to display.
     *
     * @param sender Should be the current Object itself.
     * @param text The error message.
     * @param timeout The timeout before the message disapears.
    */
    void showError(QObject *sender, const QString& text,unsigned int timeout=0);

public slots:
    void onSaveClicked();
    void onCancelClicked();
    void onResetClicked();
private:
  medSettingsEditorPrivate *d;
  void createSystemSettingsWidget();
};

#endif // MEDSETTINGSEDITOR_H
