/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSystemSettingsWidget.h>

#include <QWidget>
#include <QtGui>
#include <QDir>
#include <QtCore>

#include <medPluginManager.h>
#include <medSettingsManager.h>

class medSystemSettingsWidgetPrivate {

public:
  QWidget* parent;
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
    d->clearOnPatientField = new QCheckBox(this);
    QFormLayout* layout = new QFormLayout;
    layout->addRow(tr("Clear on patient change"),d->clearOnPatientField);
    this->setLayout(layout);
}

bool medSystemSettingsWidget::validate()
{
    return true;
}

void medSystemSettingsWidget::read()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    bool clear = mnger->value("system","clearOnPatientChange").toBool();
    d->clearOnPatientField->setCheckState(clear?Qt::Checked:Qt::Unchecked);
}

bool medSystemSettingsWidget::write()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    mnger->setValue("system","clearOnPatientChange",d->clearOnPatientField->isChecked());
    return true;
}
