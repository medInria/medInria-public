/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medSettingsWidget.h>
#include <QWidget>

#include <medCoreLegacyExport.h>

class medStartupSettingsWidgetPrivate;

class MEDCORELEGACY_EXPORT medStartupSettingsWidget : public medSettingsWidget
{
    Q_OBJECT
    MED_SETTINGS_INTERFACE("Start Up","Startup settings")

public:
    medStartupSettingsWidget(QWidget *parent = 0);

signals:

public slots:
    void read();

protected:
    bool validate();
    bool write();

private:
    medStartupSettingsWidgetPrivate *d;

private slots :
    void genericWorkspaceState(int pi_iState);

};
