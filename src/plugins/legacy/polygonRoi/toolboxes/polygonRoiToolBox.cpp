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
//#include <medVtkViewBackend.h>

const char *polygonRoiToolBox::generateBinaryImageButtonName = "generateBinaryImageButton";

polygonRoiToolBox::polygonRoiToolBox(QWidget *parent ) :
    medAbstractSelectableToolBox(parent), activeDataIndex()
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

    activateTBButton = new QPushButton(tr("Activate Toolbox"));
    activateTBButton->setToolTip(tr("Activate closed polygon mode. You should only have one view."));
    activateTBButton->setCheckable(true);
    activateTBButton->setObjectName("closedPolygonButton");
    connect(activateTBButton,SIGNAL(toggled(bool)),this,SLOT(clickClosePolygon(bool)));

    interpolate = new QCheckBox(tr("Interpolate between contours"));
    interpolate->setToolTip("Interpolate between master ROIs");
    interpolate->setObjectName("interpolateButton");
    interpolate->setChecked(true);
    connect(interpolate,SIGNAL(clicked(bool)) ,this,SLOT(interpolateCurve(bool)));

    auto repulsorLayout = new QHBoxLayout();
    auto repulsorLabel = new QLabel("Correct contours");
    repulsorLayout->addWidget(repulsorLabel);

    repulsorTool = new QPushButton(tr("Repulsor"));
    repulsorTool->setToolTip(tr("Activate Repulsor"));
    repulsorTool->setObjectName("repulsorTool");
    repulsorTool->setCheckable(true);
    connect(repulsorTool,SIGNAL(clicked(bool)),this,SLOT(activateRepulsor(bool)));
    repulsorLayout->addWidget(repulsorTool);

    auto activateTBLayout = new QHBoxLayout();
    layout->addLayout(activateTBLayout );
    activateTBLayout->addWidget(activateTBButton);

    // Add Contour Management Toolbox
    QString identifier = speciality.toLower() + QString("LabelToolBox");
    pMedToolBox = medToolBoxFactory::instance()->createToolBox(identifier);

    pMedToolBox->header()->hide();
    pMedToolBox->hide();

    layout->addWidget(pMedToolBox);
    connect(activateTBButton, SIGNAL(toggled(bool)), pMedToolBox, SLOT(setEnabled(bool)), Qt::UniqueConnection);

    auto contoursActionLayout = new QVBoxLayout();
    layout->addLayout(contoursActionLayout );
    contoursActionLayout->addWidget(interpolate);
    contoursActionLayout->addLayout(repulsorLayout);

    auto saveLabel = new QLabel("Save segmentations as:");
    auto saveButtonsLayout = new QHBoxLayout();
    saveBinaryMaskButton = new QPushButton(tr("Mask(s)"));
    saveBinaryMaskButton->setToolTip("Import the current mask to the non persistent database");
    saveBinaryMaskButton->setObjectName(generateBinaryImageButtonName);
    connect(saveBinaryMaskButton,SIGNAL(clicked()),this,SLOT(saveBinaryImage()));
    saveButtonsLayout->addWidget(saveBinaryMaskButton);

    saveContourButton = new QPushButton("Contour(s)");
    saveContourButton->setToolTip("Export these contours as an .ctrb file loadable only in medInria.");
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
    dataDestButton = new QRadioButton("Save in source of original data",this);
    dataDestButton->setToolTip("Generated data can be saved in origin data source or default working source");
    dataDestButton->setObjectName("dstButton");
    dataDestButton->setChecked(true);
    helpButton = new QPushButton("Help");
    helpButton->setToolTip("show help related to this toolbox.");
    helpButton->setFixedSize(150, 20);
    helpButton->setObjectName("helpButton");
    connect(helpButton, SIGNAL(clicked()), this, SLOT(showHelp()));
    helpLayout->addWidget(dataDestButton);
    helpLayout->addWidget(helpButton);

    // buttons initialisation: view has no data
    disableButtons();
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
    return nullptr;
}

void polygonRoiToolBox::updateView()
{
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
            if (viewEventHash.contains(data->dataIndex()))
            {
                baseViewEvent *event = viewEventHash[data->dataIndex()];
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
    if (viewEventHash.contains(data->dataIndex()))
    {
        baseViewEvent *event = viewEventHash[data->dataIndex()];
        if (event->getCurrentView() != view)
        {
            event->removeViewFromList(view);
        }
    }
}

void polygonRoiToolBox::createAndConnectEventFilter(const medAbstractData *data, medAbstractImageView *imageView)
{
    QString toolBoxName = data->metaDataValues(medMetaDataKeys::key("StudyDescription"))[0] +
                            " - " +
                            data->metaDataValues(medMetaDataKeys::key("SeriesDescription"))[0];

    if (specialityPreference==1)
    {
        auto eventFilter = new urologyViewEvent(imageView, this);
        eventFilter->initialize(pMedToolBox, toolBoxName);
        viewEventHash[data->dataIndex()] = eventFilter;
    }
    else
    {
        auto eventFilter = new defaultViewEvent(imageView, this);
        eventFilter->initialize(pMedToolBox, toolBoxName);
        viewEventHash[data->dataIndex()] = eventFilter;
    }

    connect(imageView, SIGNAL(selectedRequest(bool)), this, SLOT(onDataIndexActivated()), Qt::UniqueConnection);
    connect(imageView, SIGNAL(selectedRequest(bool)), activateTBButton, SLOT(setEnabled(bool)), Qt::UniqueConnection);
    activateTBButton->setEnabled(true);

    connect(imageView, SIGNAL(layerRemoved(medAbstractData *)), this, SLOT(onLayerRemoved(medAbstractData *)), Qt::UniqueConnection);

}

void polygonRoiToolBox::onLayerRemoved(medAbstractData *data)
{
    if (viewEventHash.contains(data->dataIndex()))
    {
        baseViewEvent *pViewEvent = viewEventHash[data->dataIndex()];
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

        viewEventHash.remove(data->dataIndex());
        if (activeDataIndex == data->dataIndex())
        {
            activeDataIndex = medDataIndex();
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
            if (data->dataIndex() == activeDataIndex)
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
            if (viewEventHash.contains(data->dataIndex()))
            {
                baseViewEvent *event = viewEventHash.value(data->dataIndex());

                event->onSelectContainer();
                event->activateRepulsor(repulsorTool->isChecked());
                event->setEnableInterpolation(interpolate->isChecked());
                activeDataIndex = data->dataIndex();
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
        pMedToolBox->hide();
        for (baseViewEvent *event : viewEventHash.values())
        {
            event->removeViewInteractor();
            disconnect(event->getCurrentView(), SIGNAL(selectedRequest(bool)), this, SLOT(onDataIndexActivated()));
            disconnect(event->getCurrentView(), SIGNAL(layerRemoved(medAbstractData *)), this, SLOT(onLayerRemoved(medAbstractData *)));
        }
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
        if (activeDataIndex.isValid() && viewEventHash[activeDataIndex])
        {
            viewEventHash[activeDataIndex]->onSelectContainer();
        }
    }
    saveBinaryMaskButton->setEnabled(state);
    saveContourButton->setEnabled(state);
    dataDestButton->setEnabled(state);
    interpolate->setEnabled(state);
    repulsorTool->setEnabled(state);
}

void polygonRoiToolBox::activateRepulsor(bool state)
{
    if (viewEventHash.contains(activeDataIndex))
    {
        viewEventHash.value(activeDataIndex)->activateRepulsor(state);
    }
}

void polygonRoiToolBox::interpolateCurve(bool state)
{
    if (viewEventHash.contains(activeDataIndex))
    {
        viewEventHash.value(activeDataIndex)->setEnableInterpolation(state);
    }
}

void polygonRoiToolBox::saveBinaryImage()
{
    for (baseViewEvent *event1 : viewEventHash.values())
    {
        event1->saveMask();
    }
}

void polygonRoiToolBox::disableButtons()
{
    activateTBButton->setEnabled(false);
    activateTBButton->setChecked(false);
    repulsorTool->setEnabled(false);
    repulsorTool->setChecked(false);
    saveBinaryMaskButton->setEnabled(false);
    dataDestButton->setEnabled(false);
    saveContourButton->setEnabled(false);
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
    activeDataIndex = medDataIndex();
    // Switching to a new toolbox, we can clean the main container behavior

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
        container->setSelected(iView==pView);
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


    QString underlineStyle = "<span>&#8226; %1</span>";
    QString titleStyle = "<span style=\" font-size : 14px;text-decoration: underline;\"><center>%1</center></span><br>";

    QString main = QString(QString(titleStyle).arg("Main features")
                                 + QString(underlineStyle).arg("Draw Contour:") + " Activate the toolbox, then shift+click on the data set<br><br>"
                                 + QString(underlineStyle).arg("Define new Label:") + " Select a label then shift+click on the data set<br><br>"
                                 + QString(underlineStyle).arg("Add New Label:") + " Click on Plus Button<br><br>"
                                 + QString(underlineStyle).arg("Remove label:") + " Click on Minus Button<br><br>");

    QString shortcut = QString(QString(titleStyle).arg("Shortcut")
                                     + QString(underlineStyle).arg("A:") + " Select contour (work only closed to a contour)<br><br>"
                                     + QString(underlineStyle).arg("C:") + " Copy contour (work only closed to a contour)<br><br>"
                                     + QString(underlineStyle).arg("V:") + " Paste contour(s)<br><br>"
                                     + QString(underlineStyle).arg("Up/Down:") + " Move to previous/next slice<br><br>"
                                     + QString(underlineStyle).arg("BackSpace:") + " Delete node (work only closed to a contour)<br><br>"
                                     + QString(underlineStyle).arg("Alt + Click:") + " Draw cross on mouse click 2D position in all views<br><br>"
                                     + QString(underlineStyle).arg("D:") + " Draw cross on current 2D position in all views<br><br>"
                                     + QString(underlineStyle).arg("E:") + " Erase cross in all views<br><br>"
                                     + QString(underlineStyle).arg("H:") + " show this help<br><br>");
    if (specialityPreference==1)
    {
        shortcut.append(QString(underlineStyle).arg("S:") + " Switch color between target and score<br><br>");
    }

    QString contextual = QString(QString(titleStyle).arg("Contextual menu")
                                       + QString(underlineStyle).arg("Remove node/contour/label<br><br>")
                                       + QString(underlineStyle).arg("Save segmentation contour/mask<br><br>")
                                       + QString(underlineStyle).arg("Change current label with another existing label<br><br>")
                                       + QString(underlineStyle).arg("Copy<br><br>"));

    if (specialityPreference==1)
    {
        contextual.append(QString(underlineStyle).arg("Attach a pirad score to a target<br><br>"));
    }

    const QString explanation = main + shortcut + contextual;
    msgBox.setText(explanation);
    msgBox.exec();
}