/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include <medStartupSettingsWidget.h>

#include <medSettingsManager.h>
#include <medWorkspaceFactory.h>

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>

class medStartupSettingsWidgetPrivate
{
public:
    QCheckBox *startInFullScreen;
    QComboBox *defaultStartingArea;
    QComboBox *defaultSegmentationSpeciality;
    QComboBox* theme;
    int defaultThemeIndex;
};

medStartupSettingsWidget::medStartupSettingsWidget(QWidget *parent) :
    QDialog(parent), d(new medStartupSettingsWidgetPrivate)
{
    setWindowTitle(tr("Startup"));

    auto *layout = new QVBoxLayout();

    // Fullscreen
    d->startInFullScreen = new QCheckBox(this);
    d->startInFullScreen->setToolTip(tr("Start application in full screen mode?"));

    // Workspace at start-up
    QList<medWorkspaceFactory::Details*> workspaceDetails = medWorkspaceFactory::instance()->workspaceDetailsSortedByName(true);
    d->defaultStartingArea = new QComboBox(this);
    d->defaultStartingArea->setItemData(0, 0, Qt::UserRole);
    d->defaultStartingArea->addItem(tr("Homepage"));
    d->defaultStartingArea->addItem(tr("Browse data"));
    for(medWorkspaceFactory::Details* detail : workspaceDetails)
    {
        d->defaultStartingArea->addItem(detail->name);
    }
    d->defaultStartingArea->setToolTip(tr("Displayed workspace at start-up"));

    // Themes
    d->defaultThemeIndex = 0;
    d->theme = new QComboBox(this);
    d->theme->addItem(tr("Dark"));
    d->theme->addItem(tr("Grey"));
    d->theme->addItem(tr("Light"));
    d->theme->setCurrentIndex(d->defaultThemeIndex);
    d->theme->setToolTip(tr("Choose a theme displayed at start-up"));

    // Speciality
    d->defaultSegmentationSpeciality = new QComboBox(this);
    d->defaultSegmentationSpeciality->addItem(tr("Default"));
    d->defaultSegmentationSpeciality->addItem(tr("Urology"));

    QFormLayout *layoutForm = new QFormLayout;
    layoutForm->addRow(tr("Fullscreen"),    d->startInFullScreen);
    layoutForm->addRow(tr("Starting area"), d->defaultStartingArea);
    layoutForm->addRow(tr("Theme"),         d->theme);
    layoutForm->addRow(tr("Segmentation speciality"), d->defaultSegmentationSpeciality);
    layout->addLayout(layoutForm);

    QLabel *explanation = new QLabel("Restart the application to apply the changes.");
    explanation->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    explanation->setWordWrap(true);
    explanation->setStyleSheet("font: italic");
    layout->addWidget(explanation);

    // Display the current settings
    read();

    connect(d->startInFullScreen,   SIGNAL(stateChanged(int)),        this, SLOT(write()));
    connect(d->defaultStartingArea, SIGNAL(currentIndexChanged(int)), this, SLOT(write()));
    connect(d->defaultSegmentationSpeciality, SIGNAL(currentIndexChanged(int)), this, SLOT(write()));
    connect(d->theme,               SIGNAL(currentIndexChanged(int)), this, SLOT(write()));
    
    setLayout(layout);
}

void medStartupSettingsWidget::read()
{
    medSettingsManager *manager = &medSettingsManager::instance();

    readFullscreenSettings(manager);
    readDefaultStartingArea(manager);
    readDefaultSegmentationSpeciality(manager);
    readDefaultTheme(manager);
}

/**
 * @brief Get to know if the application needs to start in fullscreen or not
 * 
 */
void medStartupSettingsWidget::readFullscreenSettings(medSettingsManager *manager)
{
    d->startInFullScreen->setChecked(manager->value("startup", "fullscreen").toBool());
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
    while (!bFind && i<d->defaultStartingArea->count())
    {
        bFind = osDefaultStartingAreaName == d->defaultStartingArea->itemText(i);
        if (!bFind)
        {
            ++i;
        }
    }

    if (bFind)
    {
        d->defaultStartingArea->setCurrentIndex(i);
    }
    else
    {
        d->defaultStartingArea->setCurrentIndex(0);
    }
}

/**
 * @brief The segmentation speciality gives flavor to some segmentation toolbox, as PolygonRoi
 * 
 */
void medStartupSettingsWidget::readDefaultSegmentationSpeciality(medSettingsManager *manager)
{
    QString osDefaultSegmentationSpecialityName = manager->value("startup", "default_segmentation_speciality", "Default").toString();

    int i = 0;
    bool bFind = false;
    while (!bFind && i<d->defaultSegmentationSpeciality->count())
    {
        bFind = osDefaultSegmentationSpecialityName == d->defaultSegmentationSpeciality->itemText(i);
        if (!bFind) ++i;
    }

    if (bFind)
    {
        d->defaultSegmentationSpeciality->setCurrentIndex(i);
    }
    else
    {
        d->defaultSegmentationSpeciality->setCurrentIndex(0);
    }
}

/**
 * @brief Switch the theme setting to the saved or default index.
 * 
 */
void medStartupSettingsWidget::readDefaultTheme(medSettingsManager *manager)
{
    int indexTheme = manager->value("startup", "theme", d->defaultThemeIndex).toInt();
    if (indexTheme < 0)
    {
        indexTheme = d->defaultThemeIndex;
    }
    if (indexTheme > d->theme->count() -1)
    {
        indexTheme = d->theme->count() -1;
    }
    d->theme->setCurrentIndex(indexTheme);
}

void medStartupSettingsWidget::write()
{
    medSettingsManager &manager = medSettingsManager::instance();
    manager.setValue("startup", "fullscreen",            d->startInFullScreen->isChecked());
    manager.setValue("startup", "default_starting_area", d->defaultStartingArea->currentText());
    manager.setValue("startup", "default_segmentation_speciality", d->defaultSegmentationSpeciality->currentText());
    manager.setValue("startup", "theme",                 d->theme->currentIndex());
}
