/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medGuiExport.h"
#include <QWidget>

class medSettingsWidgetPrivate;

/**
 * @brief AbstractClass for all Settings widgets.
 *
 * All the pages in the medSettingsEditor must inherit from this class.
 *
 *
*/
class MEDGUI_EXPORT medSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Should not be used directly, the factory will instanciate all the settingsWidgets.
     *
     * @param parent Parent widget. Should be the medSettingsEditor
    */
    explicit medSettingsWidget(QWidget *parent = 0);


    /**
    * description - mandatory method to describe the plug-in
    * Should be used as the section name (QSettings) when using the setttingsmanager
    * @return   QString
    */
    virtual QString description() const = 0;


    /**
     * @brief Gets the name as it should appear in the settingsEditor.
     * @return QString
    */
    QString tabName() const;

    /**
     * @brief Save the form into the QSettings.
     * First calls validate() and if validate() fails, emit a showError signal.
     *
     * @param void
     * @return bool true if saving worked, false otherwise.
    */
    virtual bool write()=0;

public slots:
    /**
     * @brief Reads its data from the QSettings.
     *
    */
    virtual void read()=0;

    /**
     * @brief Validates the form before saving the data.
     *
     * @param void
     * @return bool true is succeeded, false otherwise.
    */
    virtual bool validate();

protected:

    /**
     * @brief Sets the name of the tab appearing in the settingsEditor.
     * MUST be used by inherited classes.
     *
     * @param section
    */
    void setTabName(QString section);

private:
    medSettingsWidgetPrivate *d; /**< TODO */
};


