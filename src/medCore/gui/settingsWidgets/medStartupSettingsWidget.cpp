/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medStartupSettingsWidget.h>
#include <QWidget>
#include <QtGui>
#include <QtCore>

#include <medSettingsManager.h>

class medStartupSettingsWidgetPrivate {

public:
  QWidget* parent;
  QCheckBox* startInFullScreen;
  QComboBox* defaultStartingArea;
  QComboBox* defaultContainerLayout;

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

    d->defaultStartingArea = new QComboBox(this);
    d->defaultStartingArea->addItem(tr("Homepage"));
    d->defaultStartingArea->addItem(tr("Browser"));
    d->defaultStartingArea->addItem(tr("Viewer"));

    d->defaultContainerLayout = new QComboBox(this);
    d->defaultContainerLayout->addItem(tr("Single"),"Single");
    d->defaultContainerLayout->addItem(tr("Multi"),"Multi");
    //TODO: add a setting per specific-layout.
    d->defaultContainerLayout->addItem(tr("Custom"),"Custom");

    QFormLayout* layout = new QFormLayout;
    layout->addRow(tr("Fullscreen"),d->startInFullScreen);
    layout->addRow(tr("Starting area"), d->defaultStartingArea);
    layout->addRow(tr("Default Container layout"), d->defaultContainerLayout);
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

    //if nothing is configured then Browser is the default area
    int index = mnger->value("startup", "default_starting_area", 0).toInt();

    // clamp range
    if (index < 0)
        index = 0;
    if (index > d->defaultStartingArea->count() -1)
        index = d->defaultStartingArea->count() -1;

    d->defaultStartingArea->setCurrentIndex(index);

    //If nothing is set, default is multi
    QString layout = mnger->value("startup",
                                  "default_container_layout",
                                  "Multi").toString();
    index = d->defaultContainerLayout->findData(layout);
    //Clamp value to the default one in case the index is not found.
    if ( index == -1) index = 1;
    d->defaultContainerLayout->setCurrentIndex(index);
}

bool medStartupSettingsWidget::write()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    mnger->setValue("startup","fullscreen", d->startInFullScreen->isChecked());
    mnger->setValue("startup","default_starting_area",
                    d->defaultStartingArea->currentIndex());
    QString layout = d->defaultContainerLayout->itemData(
                d->defaultContainerLayout->currentIndex()).toString();
    mnger->setValue("startup","default_container_layout",
                    layout);
    return true;
}
