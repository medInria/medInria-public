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

class medStartupSettingsWidgetPrivate
{
public:
    QWidget *parent;
    QCheckBox *startInFullScreen;
    QComboBox *defaultStartingArea;
    QComboBox *defaultSegmentationSpeciality;

    medStartupSettingsWidgetPrivate();
    ~medStartupSettingsWidgetPrivate();
};

medStartupSettingsWidgetPrivate::medStartupSettingsWidgetPrivate()
{
}

medStartupSettingsWidget::medStartupSettingsWidget(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("Startup"));

    startInFullScreen = new QCheckBox(this);
    startInFullScreen->setToolTip(tr("Start application in full screen mode?"));

    QList<medWorkspaceFactory::Details*> workspaceDetails = medWorkspaceFactory::instance()->workspaceDetailsSortedByName(true);

    defaultStartingArea = new QComboBox(this);
    defaultStartingArea->setItemData(0, 0, Qt::UserRole);
    defaultStartingArea->addItem(tr("Homepage"));
    defaultStartingArea->addItem(tr("Import/export files"));
    defaultStartingArea->addItem(tr("Composer"));
    for(medWorkspaceFactory::Details* detail : workspaceDetails)
    {
        defaultStartingArea->addItem(detail->name);
    }

    d->defaultSegmentationSpeciality = new QComboBox(this);
    d->defaultSegmentationSpeciality->addItem(tr("Default"));
    d->defaultSegmentationSpeciality->addItem(tr("Urology"));

    QFormLayout *layout = new QFormLayout;
    layout->addRow(tr("Fullscreen"), d->startInFullScreen);
    layout->addRow(tr("Starting area"), d->defaultStartingArea);
    layout->addRow(tr("Segmentation speciality"), d->defaultSegmentationSpeciality);

    this->setLayout(layout);
}

    // Display the current settings
    read();

    connect(startInFullScreen,   SIGNAL(stateChanged(int)),        this, SLOT(write()));
    connect(defaultStartingArea, SIGNAL(currentIndexChanged(int)), this, SLOT(write()));
    
    setLayout(layout);
}

void medStartupSettingsWidget::read()
{
    medSettingsManager *mnger = medSettingsManager::instance();
    startInFullScreen->setChecked(mnger->value("startup", "fullscreen").toBool());

    //if nothing is configured then Homepage is the default area
    QString osDefaultStartingAreaName = mnger->value("startup", "default_starting_area", "Homepage").toString();

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

    QString osDefaultSegmentationSpecialityName = mnger->value("startup", "default_segmentation_speciality", "Default").toString();

    i = 0;
    bFind = false;
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

void medStartupSettingsWidget::write()
{
    medSettingsManager *mnger = medSettingsManager::instance();
    mnger->setValue("startup", "fullscreen", d->startInFullScreen->isChecked());
    mnger->setValue("startup", "default_starting_area", d->defaultStartingArea->currentText());
    mnger->setValue("startup", "default_segmentation_speciality", d->defaultSegmentationSpeciality->currentText());
    return true;
}
