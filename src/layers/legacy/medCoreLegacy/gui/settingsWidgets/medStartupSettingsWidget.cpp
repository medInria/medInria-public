/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include "medStartupSettingsWidget.h"

#include <medWorkspaceFactory.h>
#include <medStyleSheetParser.h>

#include <QFormLayout>

medStartupSettingsWidget::medStartupSettingsWidget(QWidget * parent) :
    QDialog(parent)
{
    setWindowTitle(tr("Startup"));

    QVBoxLayout *layout = new QVBoxLayout();

    startInFullScreen = new QCheckBox(this);
    startInFullScreen->setToolTip(tr("Start application in full screen mode?"));

    defaultStartingArea = new QComboBox(this);
    defaultStartingArea->setItemData(0, 0, Qt::UserRole);
    defaultStartingArea->addItem(tr("Homepage"));
    defaultStartingArea->addItem(tr("Import/export files"));
    defaultStartingArea->addItem(tr("Composer"));
    QList<medWorkspaceFactory::Details*> workspaceDetails = medWorkspaceFactory::instance()->workspaceDetailsSortedByName(true);
    for(medWorkspaceFactory::Details* detail : workspaceDetails)
    {
        defaultStartingArea->addItem(detail->name);
    }
    defaultStartingArea->setToolTip(tr("Displayed workspace at start-up"));

    // Themes
    theme = new QComboBox(this);
    theme->addItem(tr("Dark"));
    theme->addItem(tr("Grey"));
    theme->addItem(tr("Light"));
    theme->setToolTip(tr("Choose a theme displayed at start-up"));

    QFormLayout *layoutForm = new QFormLayout;
    layoutForm->addRow(tr("Fullscreen"),    startInFullScreen);
    layoutForm->addRow(tr("Starting area"), defaultStartingArea);
    layoutForm->addRow(tr("Theme"),         theme);
    layout->addLayout(layoutForm);

    QLabel *explanation = new QLabel("Restart the application to apply the changes.");
    explanation->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    explanation->setWordWrap(true);
    explanation->setStyleSheet("font: italic");
    layout->addWidget(explanation);

    // Display the current settings
    read();

    connect(startInFullScreen,   SIGNAL(stateChanged(int)),        this, SLOT(write()));
    connect(defaultStartingArea, SIGNAL(currentIndexChanged(int)), this, SLOT(write()));
    connect(theme,               SIGNAL(currentIndexChanged(int)), this, SLOT(write()));
    
    setLayout(layout);
}

void medStartupSettingsWidget::read()
{
    medSettingsManager *manager = medSettingsManager::instance();

    readFullscreenSettings(manager);
    readDefaultStartingArea(manager);
    readDefaultTheme(manager);
}

/**
 * @brief Get to know if the application needs to start in fullscreen or not
 * 
 */
void medStartupSettingsWidget::readFullscreenSettings(medSettingsManager *manager)
{
    startInFullScreen->setChecked(manager->value("startup", "fullscreen").toBool());
}

/**
 * @brief If nothing is configured then Homepage is the default area
 * 
 */
void medStartupSettingsWidget::readDefaultStartingArea(medSettingsManager *manager)
{
    QString osDefaultStartingAreaName = manager->value("startup", "default_starting_area", "Homepage").toString();

    int i = 0;
    bool bFind = false;
    while (!bFind && i<defaultStartingArea->count())
    {
        bFind = osDefaultStartingAreaName == defaultStartingArea->itemText(i);
        if (!bFind)
        {
            ++i;
        }
    }

    if (bFind)
    {
        defaultStartingArea->setCurrentIndex(i);
    }
    else
    {
        defaultStartingArea->setCurrentIndex(0);
    }
}

/**
 * @brief Set theme settings
 * 
 */
void medStartupSettingsWidget::readDefaultTheme(medSettingsManager *manager)
{    
    int indexTheme = manager->value("startup", "theme", 0).toInt();
    if (indexTheme < 0)
    {
        indexTheme = 0;
    }
    if (indexTheme > theme->count() -1)
    {
        indexTheme = theme->count() -1;
    }
    theme->setCurrentIndex(indexTheme);
}

void medStartupSettingsWidget::write()
{
    medSettingsManager *manager = medSettingsManager::instance();
    manager->setValue("startup", "fullscreen",            startInFullScreen->isChecked());
    manager->setValue("startup", "default_starting_area", defaultStartingArea->currentText());
    manager->setValue("startup", "theme",                 theme->currentIndex());
}
