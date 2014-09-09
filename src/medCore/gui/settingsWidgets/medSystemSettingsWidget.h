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

class medSystemSettingsWidgetPrivate;

class MEDCORE_EXPORT medSystemSettingsWidget : public medSettingsWidget
{
    Q_OBJECT
    MED_SETTINGS_INTERFACE("System","System settings")

public:
    medSystemSettingsWidget(QWidget *parent = 0);

signals:

public slots:
    void read();

protected:
    bool validate();
    virtual bool write();

private:
  medSystemSettingsWidgetPrivate *d;
};
