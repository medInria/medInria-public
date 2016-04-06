/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medStartupSettingsWidget.h>
#include <QtWidgets>
#include <QtGui>
#include <QtCore>

#include <medSettingsManager.h>
#include <medWorkspaceFactory.h>

class medStartupSettingsWidgetPrivate {

public:
  QWidget* parent;
  QCheckBox* startInFullScreen;
  QCheckBox* genericWorkspaceEnabled;
  QComboBox* defaultStartingArea;

  medStartupSettingsWidgetPrivate();
  ~medStartupSettingsWidgetPrivate();
};

medStartupSettingsWidgetPrivate::medStartupSettingsWidgetPrivate()
{
}

medStartupSettingsWidgetPrivate::~medStartupSettingsWidgetPrivate()
{
}


medStartupSettingsWidget::medStartupSettingsWidget(QWidget *parent) :
        medSettingsWidget(parent),
        d(new medStartupSettingsWidgetPrivate())
{
    setTabName(tr("Startup"));
    d->startInFullScreen = new QCheckBox(this);
    d->startInFullScreen->setToolTip(tr("Start application in full screen mode?"));

    d->genericWorkspaceEnabled = new QCheckBox(this);
    d->genericWorkspaceEnabled->setToolTip(tr("Enable generic workspace?"));

    QList<medWorkspaceFactory::Details*> workspaceDetails = medWorkspaceFactory::instance()->workspaceDetailsSortedByName(true);

    d->defaultStartingArea = new QComboBox(this);
    d->defaultStartingArea->addItem(tr("Homepage"));
    d->defaultStartingArea->addItem(tr("Browser"));
    foreach ( medWorkspaceFactory::Details* detail, workspaceDetails )
        d->defaultStartingArea->addItem(detail->name);

    QFormLayout* layout = new QFormLayout;
    layout->addRow(tr("Fullscreen"),d->startInFullScreen);
    layout->addRow(tr("Generic workspace enabled"),d->genericWorkspaceEnabled);
    layout->addRow(tr("Starting area"), d->defaultStartingArea);
    this->setLayout(layout);
}

/**
 * @brief Performs the validation of each control inside the widget.
 *
 * @param void
 * @return true is the validation is successful, false otherwise.
*/
bool medStartupSettingsWidget::validate()
{
    return true;
}

void medStartupSettingsWidget::read()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    d->startInFullScreen->setChecked(mnger->value("startup", "fullscreen").toBool());
    d->genericWorkspaceEnabled->setChecked(mnger->value("startup", "genericWorkspace", false).toBool());
    //if nothing is configured then Browser is the default area
    int index = mnger->value("startup", "default_starting_area", 0).toInt();

    // clamp range
    if (index < 0)
        index = 0;
    if (index > d->defaultStartingArea->count() -1)
        index = d->defaultStartingArea->count() -1;

    d->defaultStartingArea->setCurrentIndex(index);
}

bool medStartupSettingsWidget::write()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    mnger->setValue("startup","fullscreen", d->startInFullScreen->isChecked());
    mnger->setValue("startup","default_starting_area",
                    d->defaultStartingArea->currentIndex());
    mnger->setValue("startup", "genericWorkspace", d->genericWorkspaceEnabled->isChecked());
    return true;
}
