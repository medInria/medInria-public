#pragma once

/*==============================================================================

 medInria

 Copyright (c) INRIA 2024. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include <medCoreLegacyExport.h>
#include <medSettingsWidget.h>

class medDevelopmentSettingsWidgetPrivate;

class MEDCORELEGACY_EXPORT medDevelopmentSettingsWidget : public medSettingsWidget
{
    Q_OBJECT
    MED_SETTINGS_INTERFACE("Development", "Development Settings")

public:
    medDevelopmentSettingsWidget(QWidget* parent = nullptr);

    virtual bool write();

public slots:
    virtual void read();
    virtual bool validate();

private:
    medDevelopmentSettingsWidgetPrivate *d;
};
