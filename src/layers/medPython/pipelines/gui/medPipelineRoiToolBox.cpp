/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medPipelineRoiToolBox.h"

#include <QtGui>

#include <medAbstractLayeredView.h>
#include <medAbstractSelectableToolBox.h>
#include <medComboBox.h>
#include <medDataManager.h>
#include <medJobManagerL.h>
#include <medPluginManager.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>
#include <medToolBoxTab.h>
#include <medUtilities.h>
#include <medViewContainer.h>

namespace med::pipeline
{

RoiToolBox::RoiToolBox(QWidget *parent)
    : medAbstractSelectableToolBox(parent)
{
    currentView = nullptr;
    currentRoiTool= nullptr;

    roiComboBox = new medComboBox;
    roiComboBox->addItem("Choose method to perform segmentation");
    roiComboBox->setToolTip(tr("Browse through the list of available ROI toolboxes"));

    medToolBoxFactory* toolBoxFactory = medToolBoxFactory::instance();

    QString toolboxName[] = {"med::AlgorithmPaintToolBox", "polygonRoiToolBox"};
    int index = 1;
    for(const QString &name : toolboxName)
    {
        medToolBoxDetails* details = toolBoxFactory->toolBoxDetailsFromId(name);
        roiComboBox->addItem(details->name, name);
        roiComboBox->setItemData(index, details->description, Qt::ToolTipRole);
        index++;
    }

    connect(roiComboBox, SIGNAL(activated(int)), this, SLOT(changeCurrentRoiTool(int)));

    QWidget* mainWidget = new QWidget;
    mainLayout = new QVBoxLayout(mainWidget);

    roiComboBox->adjustSize();
    mainLayout->addWidget(roiComboBox);
    mainWidget->setLayout(mainLayout);

    this->addWidget(mainWidget);
    this->setTitle("ROI Tool");
}

bool RoiToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<RoiToolBox>();
}

void RoiToolBox::updateView()
{
    medAbstractView* view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();
    medAbstractLayeredView * medView = dynamic_cast<medAbstractLayeredView*>(view);

    if (medView)
    {
         currentView = medView;
    }
}

void RoiToolBox::initializeToolbox()
{
    if ( currentRoiTool->identifier() == "med::AlgorithmPaintToolBox" )
    {
        currentRoiTool->findChild<QPushButton*>("clearMaskButton")->click();
        currentRoiTool->findChild<QPushButton*>("paintButton")->click();
        currentRoiTool->findChild<QPushButton*>("Magic Wand")->hide();
        currentRoiTool->findChild<QPushButton*>("saveButton")->setFixedWidth(0);
    }
    else if ( currentRoiTool->identifier() == "polygonRoiToolBox" )
    {
        currentRoiTool->clear();
        currentRoiTool->findChild<QAbstractButton*>("generateBinaryImageButton")->hide();
        currentRoiTool->findChild<QPushButton*>("closedPolygonButton")->click();
    }
}

void RoiToolBox::changeCurrentRoiTool(int index)
{
    // clear previous toolbox if present
    if (currentRoiTool)
    {
        currentRoiTool->clear();
    }

    medAbstractSelectableToolBox* toolBox = nullptr;
    QString identifier = roiComboBox->itemData(index).toString();
    if (roiTools.contains(identifier))
    {
        toolBox = roiTools[identifier];
    }
    else
    {
        medToolBox* t = medToolBoxFactory::instance()->createToolBox(identifier, this);
        toolBox = qobject_cast<medAbstractSelectableToolBox*>(t);

        if (toolBox)
        {
            medAbstractWorkspaceLegacy* workspace = this->getWorkspace();
            if (workspace)
            {
                toolBox->setWorkspace(workspace);
            }

            toolBox->setStyleSheet("medToolBoxBody {border:none}");
            roiTools[identifier] = toolBox;

        }
    }

    // check if there was a previous generator
    if(currentRoiTool)
    {
        // Remove previous tlbx from current tlbx
        currentRoiTool->hide();
        mainLayout->removeWidget(currentRoiTool);

        currentRoiTool = nullptr;
    }

    if (!toolBox)
    {
        this->setAboutPluginVisibility(false);
        return;
    }

    currentRoiTool = toolBox;
    currentRoiTool->header()->hide();

    dtkPlugin* plugin = currentRoiTool->plugin();
    this->setAboutPluginButton(plugin);
    this->setAboutPluginVisibility(true);

    currentRoiTool->show();
    mainLayout->addWidget(currentRoiTool);

    initializeToolbox();

    connect (currentRoiTool, SIGNAL(success()),
             this, SIGNAL(success()),
             Qt::UniqueConnection);
}

medAbstractData* RoiToolBox::processOutput()
{
    if (currentRoiTool)
    {
        return currentRoiTool->processOutput();
    }

    return nullptr;
}

} // namespace med::pipeline
