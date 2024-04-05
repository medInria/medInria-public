/*==============================================================================

 medInria

 Copyright (c) INRIA 2024. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

==============================================================================*/

#include "medDevelopmentSettingsWidget.h"

#include <medSettingsManager.h>

#include <QCheckBox>
#include <QLabel>
#include <QFormLayout>
#include <QVBoxLayout>

class medDevelopmentSettingsWidgetPrivate
{
public:
    QCheckBox* developerModeCheckBox;
};

medDevelopmentSettingsWidget::medDevelopmentSettingsWidget(QWidget *parent) :
    medSettingsWidget(parent), d(new medDevelopmentSettingsWidgetPrivate())
{
    setTabName("Development");

    QFormLayout* formLayout = new QFormLayout();
    formLayout->setSizeConstraint(QLayout::SetFixedSize);
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(formLayout);

    d->developerModeCheckBox = new QCheckBox("Enable developer mode");
    formLayout->addRow(d->developerModeCheckBox);

    QLabel* label = new QLabel("<em>Enabling developer mode will make various "
                               "development-specific options visible throughout "
                               "the application.</em>");
    formLayout->addRow(label);

    setLayout(mainLayout);
}

bool medDevelopmentSettingsWidget::write()
{
    medSettingsManager::instance().setValue("development", "developer_mode", d->developerModeCheckBox->isChecked());
    return true;
}

void medDevelopmentSettingsWidget::read()
{
    d->developerModeCheckBox->setChecked(medSettingsManager::instance().value("development", "developer_mode").toBool());
}

bool medDevelopmentSettingsWidget::validate()
{
    return true;
}
