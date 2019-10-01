/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
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

int retrieveGenericWorkSpace(QList<medWorkspaceFactory::Details*> pi_oListOfWorkspaceDetails)
{
    int iRes = -1;

    bool bMatch = false;
    medWorkspaceFactory::Details *poDetail = nullptr;
    for (int i = 0; i < pi_oListOfWorkspaceDetails.size() && !bMatch; ++i)
    {
        poDetail = pi_oListOfWorkspaceDetails[i];
        bMatch = poDetail->name == "Generic";
        if (bMatch)
        {
            iRes = i;
        }
    }

    return iRes;
}

class medStartupSettingsWidgetPrivate
{
public:
    int m_iGenericWorkspaceIndex;
    QWidget *parent;
    QCheckBox *startInFullScreen;
    QCheckBox *genericWorkspaceEnabled;
    QComboBox *defaultStartingArea;

    medStartupSettingsWidgetPrivate();
    ~medStartupSettingsWidgetPrivate();
};



medStartupSettingsWidgetPrivate::medStartupSettingsWidgetPrivate()
{
}

medStartupSettingsWidgetPrivate::~medStartupSettingsWidgetPrivate()
{
}

medStartupSettingsWidget::medStartupSettingsWidget(QWidget *parent) : medSettingsWidget(parent), d(new medStartupSettingsWidgetPrivate())
{
    setTabName(tr("Startup"));
    d->startInFullScreen = new QCheckBox(this);
    d->startInFullScreen->setToolTip(tr("Start application in full screen mode?"));

    d->genericWorkspaceEnabled = new QCheckBox(this);
    d->genericWorkspaceEnabled->setToolTip(tr("Enable generic workspace?"));

    QList<medWorkspaceFactory::Details*> workspaceDetails = medWorkspaceFactory::instance()->workspaceDetailsSortedByName(true);

    d->m_iGenericWorkspaceIndex = retrieveGenericWorkSpace(workspaceDetails);
    if (d->m_iGenericWorkspaceIndex!=-1)d->m_iGenericWorkspaceIndex += 3;

    d->defaultStartingArea = new QComboBox(this);
    d->defaultStartingArea->addItem(tr("Search"));
    d->defaultStartingArea->setItemData(0, 0, Qt::UserRole - 1); // Search is disabled
    d->defaultStartingArea->addItem(tr("Homepage"));
    d->defaultStartingArea->addItem(tr("Browser"));
    d->defaultStartingArea->addItem(tr("Composer"));
    foreach(medWorkspaceFactory::Details* detail, workspaceDetails)
    {
        d->defaultStartingArea->addItem(detail->name);
    }
    QFormLayout *layout = new QFormLayout;
    layout->addRow(tr("Fullscreen"), d->startInFullScreen);
    layout->addRow(tr("Generic workspace enabled"), d->genericWorkspaceEnabled);
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
    medSettingsManager *mnger = medSettingsManager::instance();
    d->startInFullScreen->setChecked(mnger->value("startup", "fullscreen").toBool());
    d->genericWorkspaceEnabled->setChecked(mnger->value("startup", "genericWorkspace", false).toBool());

    //if nothing is configured then Homepage is the default area
    QString osDefaultStartingAreaName = mnger->value("startup", "default_starting_area", "Homepage").toString();

    int i = 0;
    bool bFind = false;
    while (!bFind && i<d->defaultStartingArea->count())
    {
        bFind = osDefaultStartingAreaName == d->defaultStartingArea->itemText(i);
        if (!bFind) ++i;
    }

    if (bFind)
    {
        d->defaultStartingArea->setCurrentIndex(i);
    }
    else
    {
        d->defaultStartingArea->setCurrentIndex(0);
    }

    connect(d->genericWorkspaceEnabled, SIGNAL(stateChanged(int)), this, SLOT(genericWorkspaceState(int)));
}

bool medStartupSettingsWidget::write()
{
    medSettingsManager *mnger = medSettingsManager::instance();
    mnger->setValue("startup", "fullscreen", d->startInFullScreen->isChecked());
    mnger->setValue("startup", "default_starting_area", d->defaultStartingArea->currentText());
    mnger->setValue("startup", "genericWorkspace", d->genericWorkspaceEnabled->isChecked());

    return true;
}

void medStartupSettingsWidget::genericWorkspaceState(int pi_iState)
{
    if (d->m_iGenericWorkspaceIndex != -1)
    {
        if (pi_iState == Qt::Unchecked)
        {
            if (d->defaultStartingArea->currentText() == "Generic")
            {
                d->defaultStartingArea->setCurrentIndex(0);
            }
            d->defaultStartingArea->removeItem(d->m_iGenericWorkspaceIndex);
        }
        else
        {
            d->defaultStartingArea->insertItem(d->m_iGenericWorkspaceIndex, "Generic");
        }
    }
}
