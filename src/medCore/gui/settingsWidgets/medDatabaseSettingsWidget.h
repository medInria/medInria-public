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
#include <medSettingsWidget.h>
#include <QWidget>

class medDatabaseSettingsWidgetPrivate;

class MEDCORE_EXPORT medDatabaseSettingsWidget : public medSettingsWidget
{
    Q_OBJECT
    MED_SETTINGS_INTERFACE("Database","Database Settings")
public:
    medDatabaseSettingsWidget(QWidget *parent = 0);

signals:

public slots:
    void read();

private slots:
    void selectDbDirectory();

protected:
    /**
     * @brief Performs the validation of each control inside the widget.
     *
     * @param void
     * @return true is the validation is successful, false otherwise.
    */
    bool validate();

    bool write();
private:
    medDatabaseSettingsWidgetPrivate *d;

};
