/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>
#include <QWidget>
#include <QTabWidget>

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
class MEDCORE_EXPORT medSettingsEditor : public QWidget
{
    Q_OBJECT

public:

    medSettingsEditor(QWidget *parent = 0, bool useAdvancedWidget = false);
    ~medSettingsEditor();

    virtual void initialize();
    virtual void queryWidgets();

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
    void showError(const QString& text,unsigned int timeout=0);

    /**
     * @brief Emits an info message for the medMessageController to display.
     *
     * Typically here, it is emitted when the settings are successfully saved.
     * @param sender Should be the current Object itself.
     * @param text The error message.
     * @param timeout The timeout before the message disapears.
    */
    void showInfo(const QString& text,unsigned int timeout=0);

    /**
    * Emitted if the dialog should disappear
    */
    void finished();

public slots:
    virtual void onAdvancedClicked();
    virtual void onSaveClicked();
    virtual void onCancelClicked();
    virtual void onResetClicked();
    virtual void setTabPosition(QTabWidget::TabPosition position);
    virtual QTabWidget::TabPosition tabPosition();

protected:
    virtual bool save();

private:
  medSettingsEditorPrivate *d; /**< TODO */

};


