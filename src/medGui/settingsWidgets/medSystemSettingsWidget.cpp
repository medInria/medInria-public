/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medSystemSettingsWidget.h"

#include <QWidget>
#include <QtGui>
#include <QDir>
#include <QtCore>

#include <medPluginManager.h>
#include <medSettingsManager.h>

class medSystemSettingsWidgetPrivate {

public:
  QWidget* parent;
  QLineEdit* pluginsPathField;
  QLineEdit* modulesPathField;
  QLineEdit* scriptsPathField;
  QCheckBox* clearOnPatientField;

  medSystemSettingsWidgetPrivate();
  ~medSystemSettingsWidgetPrivate();
};

medSystemSettingsWidgetPrivate::medSystemSettingsWidgetPrivate()
{
}

medSystemSettingsWidgetPrivate::~medSystemSettingsWidgetPrivate()
{
}


medSystemSettingsWidget::medSystemSettingsWidget(QWidget *parent) :
        medSettingsWidget(parent),
        d(new medSystemSettingsWidgetPrivate())
{
    setTabName(tr("System"));
    d->pluginsPathField = new QLineEdit(this);
    d->modulesPathField = new QLineEdit(this);
    d->scriptsPathField = new QLineEdit(this);
    d->clearOnPatientField = new QCheckBox(this);
    QFormLayout* layout = new QFormLayout;
    layout->addRow(tr("Plugins Path"),d->pluginsPathField);
    layout->addRow(tr("Modules Path"),d->modulesPathField);
    layout->addRow(tr("Scripts Path"),d->scriptsPathField);
    layout->addRow(tr("Clear on patient change"),d->clearOnPatientField);
    this->setLayout(layout);
}


medSettingsWidget* createSystemSettingsWidget(QWidget *parent){
    return new medSystemSettingsWidget(parent);
}

bool medSystemSettingsWidget::validate()
{
    if (!validatePaths(d->pluginsPathField->text()))
        return false;

    if (!validatePaths(d->modulesPathField->text()))
        return false;

    if (!validatePaths(d->scriptsPathField->text()))
        return false;
    return true;
}

bool medSystemSettingsWidget::validatePaths(QString paths)
{
    //empty paths are allowed, the user hasn't configured them yet
    if (paths.isEmpty())
        return true;

    // splitting on a colon only obviously can not be considered safe on windows! skipping for the moment
    /*
    QStringList splitted = paths.split(":");

    foreach (QString path, splitted) {
        // two consecutive colons won't be allowed
        if (path.isEmpty())
            return false;

        // path is not empty, does it exist in the system?
        if (!QDir(path).exists())
            return false;
    }
    */

    return true;
}

void medSystemSettingsWidget::read()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    d->pluginsPathField->setText(mnger->value("plugins","path").toString());
    d->modulesPathField->setText(mnger->value("scripts","modules_path").toString());
    d->scriptsPathField->setText(mnger->value("scripts","script_path").toString());
    bool clear = mnger->value("system","clearOnPatientChange").toBool();
    d->clearOnPatientField->setCheckState(clear?Qt::Checked:Qt::Unchecked);
}

bool medSystemSettingsWidget::write()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    mnger->setValue("plugins","path",d->pluginsPathField->text());
    mnger->setValue("scripts","modules_path",d->modulesPathField->text());
    mnger->setValue("scripts","script_path",d->scriptsPathField->text());
    mnger->setValue("system","clearOnPatientChange",d->clearOnPatientField->isChecked());
    return true;
}

QString medSystemSettingsWidget::identifier() const {
    return "System";
}

QString medSystemSettingsWidget::description() const {
    return "System";
}
