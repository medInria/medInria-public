/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "polygonRoiToolBox.h"
#include "urologyViewEvent.h"

#include <medAbstractProcessLegacy.h>
#include <medParameterGroupManagerL.h>
#include <medPluginManager.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>
#include <medUtilities.h>
#include <medViewContainer.h>
#include <medViewContainerSplitter.h>
#include <medViewParameterGroupL.h>
#include <defaultViewEvent.h>
#include <medSettingsManager.h>
#include <medMetaDataKeys.h>
#include <medContours.h>
#include <medDataManager.h>

const char *polygonRoiToolBox::generateBinaryImageButtonName = "generateBinaryImageButton";

polygonRoiToolBox::polygonRoiToolBox(QWidget *parent ) :
    medAbstractSelectableToolBox(parent)
{
    medSettingsManager &manager = medSettingsManager::instance();
    QString speciality = manager.value("startup", "default_segmentation_speciality", "Default").toString();
    if (speciality=="Urology")
    {
        specialityPreference = 1;
    }
    else
    {
        specialityPreference = 0;
    }
    auto displayWidget = new QWidget(this);
    this->addWidget(displayWidget);

    auto layout = new QVBoxLayout();
    displayWidget->setLayout(layout);

    auto *explanation = new QLabel(tr("Drop a data in the view and activate:"));
    explanation->setWordWrap(true);
    explanation->setStyleSheet("font: italic");
    layout->addWidget(explanation );

    // Activation button
    activateTBButton = new QPushButton(tr("Activate Toolbox"));
    activateTBButton->setToolTip(tr("Activate closed polygon mode. You should only have one view."));
    activateTBButton->setCheckable(true);
    activateTBButton->setObjectName("closedPolygonButton");
    connect(activateTBButton,SIGNAL(toggled(bool)),this,SLOT(clickClosePolygon(bool)));
    connect(activateTBButton, &QAbstractButton::toggled, [=] (bool state) { explanation->setVisible(!state); });
    auto activateTBLayout = new QHBoxLayout();
    layout->addLayout(activateTBLayout);
    activateTBLayout->addWidget(activateTBButton);

    // Add label management tool
    QString identifier = speciality.toLower() + QString("LabelToolBox");
    pMedToolBox = medToolBoxFactory::instance()->createToolBox(identifier);
    pMedToolBox->header()->hide();
    pMedToolBox->hide();
    pMedToolBox->setObjectName("labelTool");
    layout->addWidget(pMedToolBox);
    connect(activateTBButton, SIGNAL(toggled(bool)), pMedToolBox, SLOT(setEnabled(bool)), Qt::UniqueConnection);

    // Actions on contours
    interpolate = new QCheckBox(tr("Interpolate between contours"));
    interpolate->setToolTip("Interpolate between master ROIs");
    interpolate->setObjectName("interpolateButton");
    interpolate->setChecked(true);
    connect(interpolate,SIGNAL(clicked(bool)) ,this,SLOT(interpolateCurve(bool)));

    auto repulsorLayout = new QHBoxLayout();
    repulsorLabel = new QLabel("Correct contours:");
    repulsorLayout->addWidget(repulsorLabel);

    repulsorTool = new QPushButton(tr("Repulsor"));
    repulsorTool->setToolTip(tr("Activate Repulsor"));
    repulsorTool->setObjectName("repulsorTool");
    repulsorTool->setCheckable(true);
    connect(repulsorTool,SIGNAL(clicked(bool)),this,SLOT(activateRepulsor(bool)));
    repulsorLayout->addWidget(repulsorTool);

    auto contoursActionLayout = new QVBoxLayout();
    layout->addLayout(contoursActionLayout );
    contoursActionLayout->addWidget(interpolate);
    contoursActionLayout->addLayout(repulsorLayout);

    // Export widgets
    saveLabel = new QLabel("Save segmentations as:");
    saveLabel->setObjectName("saveLabel");
    auto saveButtonsLayout = new QHBoxLayout();
    saveBinaryMaskButton = new QPushButton(tr("Mask(s)"));
    saveBinaryMaskButton->setToolTip("Import the current mask to the non persistent database");
    saveBinaryMaskButton->setObjectName(generateBinaryImageButtonName);
    connect(saveBinaryMaskButton,SIGNAL(clicked()),this,SLOT(saveBinaryImage()));
    saveButtonsLayout->addWidget(saveBinaryMaskButton);

    saveContourButton = new QPushButton("Contour(s)");
    saveContourButton->setToolTip("Export these contours as an .ctrb file loadable only in this application.");
    saveContourButton->setMinimumSize(150, 20);
    saveContourButton->setMaximumSize(150, 20);
    saveContourButton->setObjectName("saveContoursButton");
    connect(saveContourButton, SIGNAL(clicked()), this, SLOT(saveContours()));
    saveButtonsLayout->addWidget(saveContourButton);

    auto saveLayout = new QVBoxLayout();
    layout->addLayout(saveLayout);
    saveLayout->addWidget(saveLabel);
    saveLayout->addLayout(saveButtonsLayout);

    // How to use
    auto helpLayout = new QVBoxLayout();
    layout->addLayout(helpLayout);
    helpLayout->setContentsMargins(0, 10, 0, 0);
    helpButton = new QPushButton("Help");
    helpButton->setToolTip("show help related to this toolbox.");
    helpButton->setMinimumSize(150, 20);
    helpButton->setMaximumSize(150, 20);
    helpButton->setObjectName("helpButton");
    connect(helpButton, SIGNAL(clicked()), this, SLOT(showHelp()));
    helpLayout->addWidget(helpButton);

    // buttons initialisation: view has no data
    disableButtons();

    activeData = nullptr;
}

polygonRoiToolBox::~polygonRoiToolBox()
{
    clear();
    pMedToolBox->deleteLater();
    // If every view of the container has been closed, we need to check if the view needs to be clean
    medTabbedViewContainers *tabs = getWorkspace()->tabbedViewContainers();
    QList<medViewContainer*> containersInTabSelected = tabs->containersInTab(tabs->currentIndex());
    if (containersInTabSelected.size() == 1)
    {
        auto view = containersInTabSelected.at(0)->view();
        if (view && dynamic_cast<medAbstractLayeredView*>(view)->layersCount() == 0)
        {
            containersInTabSelected.at(0)->checkIfStillDeserveToLiveContainer();
        }
    }

}

bool polygonRoiToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<polygonRoiToolBox>();
}

dtkPlugin* polygonRoiToolBox::plugin()
{
    return medPluginManager::instance().plugin("Polygon ROI");
}

medAbstractData *polygonRoiToolBox::processOutput()
{
    return processOutputs().value(0);
}

QList<dtkSmartPointer<medAbstractData> > polygonRoiToolBox::processOutputs()
{
    QList<dtkSmartPointer<medAbstractData> > outputMasks;
    for (baseViewEvent *event1 : viewEventHash.values())
    {
        auto masks = event1->saveMasks();
        outputMasks.append(masks);
    }
    return outputMasks;
}

void polygonRoiToolBox::updateView()
{
    medTabbedViewContainers *tabs = this->getWorkspace()->tabbedViewContainers();
    for (medViewContainer *container : tabs->containersInTab(tabs->currentIndex()))
    {
        medAbstractView *view = container->view();
        if (!view)
        {
            return;
        }
        auto imageView = dynamic_cast<medAbstractImageView *>(view);
        if (!imageView)
        {
            displayMessageError("[FATAL] Unable to get medAbstractImageView from medAbstractView");
            return;
        }
        // work on image data we want to segment
        QList<medAbstractData *> dataList = getITKImageDataInSelectedView(view);
        if (!dataList.empty())
        {
            // attached view event with the first image data we found in view
            medAbstractData *data = dataList[0];
            if (viewEventHash.contains(data))
            {
                baseViewEvent *event = viewEventHash[data];
                // check if we have an event view attached to this data but in another container/view
                if (imageView != event->getCurrentView())
                {
                    connect(imageView, SIGNAL(layerRemoved(medAbstractData * )), this,
                            SLOT(onLayerRemoveOnOrientedViews(medAbstractData * )), Qt::UniqueConnection);
                    connect(imageView, SIGNAL(orientationChanged()), event, SLOT(showOnDifferentOrientation()));

                }
            }
            else
            {
                createAndConnectEventFilter(data, imageView);
            }

            // check if there is contour (.ctr/.ctrb)
            QList<medContours *> contoursList = getContoursInSelectedView(view);
            if (!contoursList.empty())
            {
                for (medContours *contour : contoursList)
                {
                    QVector<medTagContours> &tagContoursSet = contour->getTagContoursSet();
                    qint32 speIndex = tagContoursSet[0].getSpecialityIndex();
                    if (speIndex!=specialityPreference)
                    {
                        displayMessageError("Unable to load contour. Mismatch speciality preference ");
                    }
                    else
                    {
                        for (baseViewEvent *event1 : viewEventHash.values())
                        {
                            if (event1->getCurrentView()==view)
                            {
                                event1->loadContours(tagContoursSet);
                                if (!activateTBButton->isChecked())
                                {
                                    emit activateTBButton->click();
                                }

                                break;
                            }
                        }
                        imageView->removeData(dynamic_cast<medAbstractData *>(contour));
                    }
                }
            }
        }
        else
        {
            // Avoid loading of contours if there is no image data loaded before
            QList<medContours *> contoursList = getContoursInSelectedView(view);
            if (!contoursList.empty())
            {
                for (medContours *contour : contoursList)
                {
                    displayMessageError("[WARNING] - Drop image data in view before contours");
                    imageView->removeData(dynamic_cast<medAbstractData *>(contour));
                    container->removeView();
                }
            }
        }
    }
}

void polygonRoiToolBox::onLayerRemoveOnOrientedViews(medAbstractData *data)
{
    auto view = dynamic_cast<medAbstractImageView *>(sender());
    if (viewEventHash.contains(data))
    {
        baseViewEvent *event = viewEventHash[data];
        if (event->getCurrentView() != view)
        {
            event->removeViewFromList(view);
        }
    }
}

void polygonRoiToolBox::createAndConnectEventFilter(const medAbstractData *data, medAbstractImageView *imageView)
{
    QString dataName = data->metaDataValues(medMetaDataKeys::StudyDescription.key())[0] +
            " - " +
            data->metaDataValues(medMetaDataKeys::SeriesDescription.key())[0];

    if (specialityPreference==1)
    {
        auto eventFilter = new urologyViewEvent(imageView, this);
        eventFilter->initialize(pMedToolBox, dataName);
        viewEventHash[data] = eventFilter;
    }
    else
    {
        auto eventFilter = new defaultViewEvent(imageView, this);
        eventFilter->initialize(pMedToolBox, dataName);
        viewEventHash[data] = eventFilter;
    }

    connect(imageView, SIGNAL(selectedRequest(bool)), this, SLOT(onDataIndexActivated()), Qt::UniqueConnection);
    connect(imageView, SIGNAL(selectedRequest(bool)), activateTBButton, SLOT(setEnabled(bool)), Qt::UniqueConnection);
    activateTBButton->setEnabled(true);

    connect(imageView, SIGNAL(layerRemoved(medAbstractData *)), this, SLOT(onLayerRemoved(medAbstractData *)), Qt::UniqueConnection);

}

void polygonRoiToolBox::onLayerRemoved(medAbstractData *data)
{
    if (viewEventHash.contains(data))
    {
        baseViewEvent *pViewEvent = viewEventHash[data];
        disconnect(pViewEvent->getCurrentView(), SIGNAL(selectedRequest(bool)), this, SLOT(onDataIndexActivated()));

        // check if this data is in another view. If yes, we have to disconnect signals
        medTabbedViewContainers *tabs = this->getWorkspace()->tabbedViewContainers();
        for (medViewContainer *container : tabs->containersInTab(tabs->currentIndex()))
        {
            QUuid uuid = container->uuid();
            medAbstractView *view = container->view();
            if (!view)
            {
                QString msg = "No view in selected container (" + uuid.toString() + ")";
                displayMessageError(msg);
                return;
            }
            QList<medAbstractData *> dataList = getITKImageDataInSelectedView(view);
            auto imageView = dynamic_cast<medAbstractImageView *>(view);
            if (dataList.contains(data) && imageView != pViewEvent->getCurrentView())
            {
                disconnect(view, SIGNAL(orientationChanged()), pViewEvent, SLOT(showOnDifferentOrientation()));
                disconnect(view, SIGNAL(layerRemoved(medAbstractData * )), this,
                           SLOT(onLayerRemoveOnOrientedViews(medAbstractData * )));
            }
        }

        viewEventHash.remove(data);
        if (activeData == data)
        {
            activeData = nullptr;
        }
        delete pViewEvent;

        if (viewEventHash.empty())
        {
            clear();
        }
    }
}

void polygonRoiToolBox::onDataIndexActivated()
{
    if (activateTBButton->isChecked())
    {
        auto view = dynamic_cast<medAbstractImageView *>(sender());
        for (medAbstractData *data : getITKImageDataInSelectedView(view))
        {
            if (data == activeData)
            {
                return;
            }
        }
        for (baseViewEvent *event : viewEventHash)
        {
            event->removeFromAllViews();
        }
        for (medAbstractData *data : getITKImageDataInSelectedView(view))
        {
            if (viewEventHash.contains(data))
            {
                baseViewEvent *event = viewEventHash.value(data);

                event->onSelectContainer();
                event->activateRepulsor(repulsorTool->isChecked());
                event->setEnableInterpolation(interpolate->isChecked());
                activeData = data;
                highLightContainer(event->getCurrentView());
                return;
            }
        }
    }
}

void polygonRoiToolBox::clickClosePolygon(bool state)
{
    if (!state)
    {
        clear();
        updateView();
    }
    else
    {
        pMedToolBox->show();
        for (baseViewEvent *event : viewEventHash.values())
        {
            qDebug()<<"data index "<<viewEventHash.key(event)<<" is connected "<<event->getCurrentView();
            connect(event->getCurrentView(), SIGNAL(selectedRequest(bool)), this, SLOT(onDataIndexActivated()),
                    Qt::UniqueConnection);
            connect(event->getCurrentView(), SIGNAL(layerRemoved(medAbstractData * )), this,
                    SLOT(onLayerRemoved(medAbstractData *)), Qt::UniqueConnection);
        }
        if(activeData && viewEventHash[activeData])
        {
            viewEventHash[activeData]->onSelectContainer();
        }
        else if (!viewEventHash.empty())
        {
            viewEventHash.values().first()->getCurrentView()->selectedRequest(true);
        }
        saveBinaryMaskButton->setEnabled(state);
        saveContourButton->setEnabled(state);
        saveLabel->setEnabled(state);
        interpolate->setEnabled(state);
        repulsorTool->setEnabled(state);
        repulsorLabel->setEnabled(state);
    }
}

void polygonRoiToolBox::activateRepulsor(bool state)
{
    if (viewEventHash.contains(activeData))
    {
        viewEventHash.value(activeData)->activateRepulsor(state);
    }
}

void polygonRoiToolBox::interpolateCurve(bool state)
{
    if (viewEventHash.contains(activeData))
    {
        viewEventHash.value(activeData)->setEnableInterpolation(state);
    }
}

void polygonRoiToolBox::saveBinaryImage()
{
    QList<dtkSmartPointer<medAbstractData>> outputMasks = processOutputs();
    for (auto output : outputMasks)
    {
       medDataManager::instance().importData(output, false);
    }
}

void polygonRoiToolBox::disableButtons()
{
    activateTBButton->setEnabled(false);
    activateTBButton->setChecked(false);
    repulsorTool->setEnabled(false);
    repulsorTool->setChecked(false);
    repulsorLabel->setEnabled(false);
    saveBinaryMaskButton->setEnabled(false);
    saveContourButton->setEnabled(false);
    saveLabel->setEnabled(false);
    interpolate->setEnabled(false);
    interpolate->setChecked(true);
}

void polygonRoiToolBox::saveContours()
{
    for (baseViewEvent *event : viewEventHash.values())
    {
        event->saveAllContours();
    }
}

void polygonRoiToolBox::clear()
{
    disableButtons();
    activateTBButton->setText("Activate Toolbox");
    activeData = nullptr;

    for (baseViewEvent *event : viewEventHash)
    {
        delete event;
    }
    viewEventHash.clear();

    pMedToolBox->clear();
    pMedToolBox->setEnabled(false);
}

QList<medAbstractData *> polygonRoiToolBox::getITKImageDataInSelectedView(medAbstractView *view)
{
    QList<medAbstractData *> dataList;
    if (!view)
    {
        QString msg = "Unable to get data if the view is null";
        displayMessageError(msg);
        qDebug() << msg;
    }
    else
    {
        auto layeredView = dynamic_cast<medAbstractLayeredView *>(view);
        if (!layeredView)
        {
            QString msg = "Unable to get data if the layered view is null";
            displayMessageError(msg);
            qDebug() << msg;
        }
        else
        {
            for (unsigned int layer=0; layer<layeredView->layersCount(); layer++)
            {
                medAbstractData *data = layeredView->layerData(layer);

                if (!data || data->identifier().contains("vtkDataMesh"))
                {
                    handleDisplayError(medAbstractProcessLegacy::NO_MESH);
                }
                else if (data->identifier().contains("itkDataImageVector"))
                {
                    handleDisplayError(medAbstractProcessLegacy::UNDEFINED);
                }
                else if (data->identifier().contains("itkDataImage"))
                {
                    dataList.append(layeredView->layerData(layer));
                }
            }
        }
    }
    return dataList;
}

QList<medContours *> polygonRoiToolBox::getContoursInSelectedView(medAbstractView *view)
{
    QList<medContours *> contoursList;
    if (!view)
    {
        QString msg = "Unable to get data if the view is null";
        displayMessageError(msg);
        qDebug() << msg;
    }
    else
    {
        auto layeredView = dynamic_cast<medAbstractLayeredView *>(view);
        if (!layeredView)
        {
            QString msg = "Unable to get data if the layered view is null";
            displayMessageError(msg);
            qDebug() << msg;
        }
        else
        {
            for (unsigned int layer=0; layer<layeredView->layersCount(); layer++)
            {
                medAbstractData *data = layeredView->layerData(layer);
                if (data && data->identifier().contains("medContours"))
                {
                    contoursList.append(dynamic_cast<medContours*>(data));
                }
            }
        }
    }
    return contoursList;
}

void polygonRoiToolBox::highLightContainer(medAbstractView *pView)
{
    medTabbedViewContainers *tabs = this->getWorkspace()->tabbedViewContainers();
    for (medViewContainer *container : tabs->containersInTab(tabs->currentIndex()))
    {
        auto iView = dynamic_cast<medAbstractImageView *>(container->view());
        if (iView==pView)
        {
            container->highlight("red");
        }
    }
}

void polygonRoiToolBox::drawCross(double *position)
{
    for (auto event : viewEventHash)
    {
        event->drawCross(position);
    }
}

void polygonRoiToolBox::eraseCross()
{
    for (auto event : viewEventHash)
    {
        event->eraseCross();
    }
}

void polygonRoiToolBox::showHelp() const
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Help");
    msgBox.setIcon(QMessageBox::Information);
    
    QString main = QString("<h3>Main features</h3>")
        + QString("<ul>")
        + QString("<li><b>Draw a contour</b>: shift+click on the data to create a contour</li>")
        + QString("<li><b>Add a new label</b>: click on '+' button in the label list</li>")
        + QString("<li><b>Remove a label</b>: click on '-' button in the label list</li>")
        + QString("<li><b>Use a new label</b>: select a label in the list, then shift+click on the data</li>")
        + QString("<li><b>Alt+click</b>: draw a landmark at that position in all views</li>")
        + QString("</ul>");

    QString shortcut = QString("<h3>Shortcuts</h3>")
        + QString("<ul>")
        + QString("<li><kbd>A</kbd> : select nearby contour</li>")
        + QString("<li><kbd>C</kbd> : copy nearby contour</li>")
        + QString("<li><kbd>V</kbd> : paste contour(s)</li>")
        + QString("<li><kbd>Up/Down</kbd> : move to previous/next slice</li>")
        + QString("<li><kbd>Backspace</kbd> : delete nearby node</li>")
        + QString("<li><kbd>E</kbd> : erase landmarks in all views</li>")
        + QString("<li><kbd>H</kbd> : show this help</li>");
    if (specialityPreference==1)
    {
        shortcut.append(QString("<li><kbd>S</kbd> : switch color between target and score</li>"));
    }
    shortcut += QString("</ul>");

    QString contextual = QString("<h3>Contextual menu (right-click on a contour)</h3>")
        + QString("<ul>")
        + QString("<li>Remove</li>")
        + QString("<li>Save segmentation</li>")
        + QString("<li>Rename label</li>")
        + QString("<li>Copy</li>");
    if (specialityPreference==1)
    {
        contextual.append(QString("<li>Attach a pirad score to a target</li>"));
    }
    contextual += QString("</ul>");

    const QString explanation = main + shortcut + contextual;
    msgBox.setText(explanation);
    msgBox.exec();
}
