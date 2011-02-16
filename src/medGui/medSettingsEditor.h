#ifndef MEDSETTINGSEDITOR_H
#define MEDSETTINGSEDITOR_H

#include <medGui/medGuiExport.h>
#include <QWidget>

class medSettingsEditorPrivate;

/**
 * @brief Edits the application and plugins settings.
 *
 * Widget (for the moment a tool window) that display the settings
 * for both the whole application and plugins.
 * All medSettingsWidgets registered to the medSettingsWidgetFactory are listed in tabs.
 * There is also an alternative advanced tree listing all the sections and keys from the application qSetting.
 *
*/
class MEDGUI_EXPORT medSettingsEditor : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Instatnciate the widget.
     *
     * @param parent
    */
    medSettingsEditor(QWidget *parent = 0);

signals:
    /**
     * @brief Emits an error message for the medMessageController to display.
     *
     * When a section failed the medSettingsWidget::validate() method, a message with its name is emitted.
     *
     * @param sender Should be the current Object itself.
     * @param text The error message.
     * @param timeout The timeout before the message disapears.
    */
    void showError(QObject *sender, const QString& text,unsigned int timeout=0);

    /**
     * @brief Emits an info message for the medMessageController to display.
     *
     * Typically here, it is emitted when the settings are successfully saved.
     * @param sender Should be the current Object itself.
     * @param text The error message.
     * @param timeout The timeout before the message disapears.
    */
    void showInfo(QObject *sender, const QString& text,unsigned int timeout=0);

public slots:
    /**
     * @brief Switches from the advanced to the default mode, and vice versa.
     *
    */
    void onAdvancedClicked();

    /**
     * @brief Performs validation tests on each section and tires to save.
     *
     * If any section fails, an error message is emitted for it, and the other sections are still saved.
     *
     * The widget closes if the save action is successful.
     *
    */
    void onSaveClicked();

    /**
     * @brief Cancel any non saved change and closes the widget.
     *
    */
    void onCancelClicked();

    /**
     * @brief reset the display to the last saved values.
     *
    */
    void onResetClicked();

private:
  medSettingsEditorPrivate *d; /**< TODO */

};

#endif // MEDSETTINGSEDITOR_H
