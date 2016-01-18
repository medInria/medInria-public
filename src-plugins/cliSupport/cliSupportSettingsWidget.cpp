/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <cliSupportSettingsWidget.h>

#include <QFormLayout>
#include <QLineEdit>

#include <medSettingsManager.h>


struct cliSupportSettingsWidgetPrivate {
  QLineEdit* modulesPathField;
};


cliSupportSettingsWidget::cliSupportSettingsWidget(QWidget *parent)
    : medSettingsWidget(parent)
    , d(new cliSupportSettingsWidgetPrivate())
{
    setTabName(tr("CLI Modules"));
    d->modulesPathField = new QLineEdit(this);
    QFormLayout* layout = new QFormLayout;
    layout->addRow(tr("Modules Path"),d->modulesPathField);
    this->setLayout(layout);
}


bool cliSupportSettingsWidget::validate()
{
    return true;
}


void cliSupportSettingsWidget::read()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    d->modulesPathField->setText(mnger->value("cli_modules","modules_path").toString());
}


bool cliSupportSettingsWidget::write()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    mnger->setValue("cli-support","modules_path",d->modulesPathField->text());
    return true;
}


QString cliSupportSettingsWidget::identifier() const
{
    return "CLI Modules";
}


QString cliSupportSettingsWidget::description() const
{
    return "CLI Modules";
}
