/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>

class medCompositeDataSetImporterSelectorToolBoxPrivate;

class MEDCORE_EXPORT medCompositeDataSetImporterSelectorToolBox : public medToolBox
{
    Q_OBJECT

public:
     medCompositeDataSetImporterSelectorToolBox(QWidget *parent = 0);
    ~medCompositeDataSetImporterSelectorToolBox();

    virtual void initialize();

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


public slots:
    virtual void onImportClicked();
    virtual void onLoadClicked();
    virtual void onResetClicked();
    void onCurrentTypeChanged(const int type);

 protected:
    virtual bool import();


 private:
    medCompositeDataSetImporterSelectorToolBoxPrivate* d;

};


