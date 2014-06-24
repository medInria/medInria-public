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
    /**
     * @brief Instantiate the widget.
     *
     * @param parent
    */
    medSettingsEditor(QWidget *parent = 0, bool useAdvancedWidget = false);
    ~medSettingsEditor();

    /**
     * @brief initialize layout
     */
    virtual void initialize();

    /**
     * @brief add settings widgets from factory
     */
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
    /**
     * @brief Switches from the advanced to the default mode, and vice versa.
     *
    */
    virtual void onAdvancedClicked();

    /**
     * @brief Performs validation tests on each section and tires to save.
     *
     * If any section fails, an error message is emitted for it, and the other sections are still saved.
     *
     * The widget closes if the save action is successful.
     *
    */
    virtual void onSaveClicked();

    /**
     * @brief Cancel any non saved change and closes the widget.
     *
    */
    virtual void onCancelClicked();

    /**
     * @brief reset the display to the last saved values.
     *
    */
    virtual void onResetClicked();

    /**
     * @brief Sets the tabs position around the tab widget.
     *
     * @param position
     */
    virtual void setTabPosition(QTabWidget::TabPosition position);

    /**
     * @brief Gets the position of the tabs around the tab widget.
     *
     */
    virtual QTabWidget::TabPosition tabPosition();

protected:

    /**
     * Call save on all child widgets and return the status
     */
    virtual bool save();

private:
  medSettingsEditorPrivate *d; /**< TODO */

};


