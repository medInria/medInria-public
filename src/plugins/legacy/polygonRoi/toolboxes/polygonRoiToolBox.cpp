/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "polygonRoiToolBox.h"

#include <contoursManagementToolBox.h>
#include <medAbstractProcessLegacy.h>
#include <medParameterGroupManagerL.h>
#include <medPluginManager.h>
#include <medTabbedViewContainers.h>
#include <medTableWidgetChooser.h>
#include <medToolBox.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>
#include <medUtilities.h>
#include <medViewContainer.h>
#include <medViewContainerManager.h>
#include <medViewContainerSplitter.h>
#include <medViewFactory.h>
#include <medViewParameterGroupL.h>
#include <polygonEventFilter.h>
#include <QPair>
#include <medContours.h>
#include <medLayerParameterGroupL.h>
#include <medAbstractParameterL.h>

const char *polygonRoiToolBox::generateBinaryImageButtonName = "generateBinaryImageButton";

polygonRoiToolBox::polygonRoiToolBox(QWidget *parent ) :
    medAbstractSelectableToolBox(parent), viewEventFilter(nullptr)
{
    QWidget *displayWidget = new QWidget(this);
    this->addWidget(displayWidget);

    QVBoxLayout *layout = new QVBoxLayout();
    displayWidget->setLayout(layout);

    activateTBButton = new QPushButton(tr("Activate Toolbox"));
    activateTBButton->setToolTip(tr("Activate closed polygon mode"));
    activateTBButton->setCheckable(true);
    activateTBButton->setObjectName("closedPolygonButton");
    connect(activateTBButton,SIGNAL(toggled(bool)),this,SLOT(clickClosePolygon(bool)));

    interpolate = new QCheckBox(tr("interpolate"));
    interpolate->setToolTip("Interpolate between master ROIs");
    interpolate->setObjectName("interpolateButton");
    interpolate->setChecked(true);
    connect(interpolate,SIGNAL(clicked(bool)) ,this,SLOT(interpolateCurve(bool)));

    repulsorTool = new QPushButton(tr("Repulsor"));
    repulsorTool->setToolTip(tr("Activate repulsor"));
    repulsorTool->setObjectName("repulsorTool");
    repulsorTool->setCheckable(true);
    connect(repulsorTool,SIGNAL(clicked(bool)),this,SLOT(activateRepulsor(bool)));

    currentView = nullptr;

    QHBoxLayout *activateTBLayout = new QHBoxLayout();
    layout->addLayout( activateTBLayout );
    activateTBLayout->addWidget(activateTBButton);

    // Add Contour Management Toolbox
    managementToolBox = medToolBoxFactory::instance()->createToolBox("contoursManagementToolBox");
    managementToolBox->header()->hide();
    layout->addWidget(managementToolBox);
    connect(activateTBButton,SIGNAL(toggled(bool)),managementToolBox,SLOT(clickActivationButton(bool)), Qt::UniqueConnection);
    connect(this, SIGNAL(currentLabelsDisplayed()), managementToolBox, SLOT(showCurrentLabels()), Qt::UniqueConnection);

    QHBoxLayout *contoursActionLayout = new QHBoxLayout();
    layout->addLayout( contoursActionLayout );
    contoursActionLayout->addWidget(interpolate);
    contoursActionLayout->addWidget(repulsorTool);

    tableViewChooser = new medTableWidgetChooser(this, 1, 3, 50);
    // Mandatory : Qt bug ? : Without the lines below, the size of the table View is not as expected
    QSize size = tableViewChooser->sizeHint();
    tableViewChooser->setFixedHeight(size.height()-1);
    tableViewChooser->setFixedWidth(size.width()-1);
    tableViewChooser->setIconSize(QSize(size.height()-1,size.height()-1));
    connect(tableViewChooser, SIGNAL(selected(unsigned int,unsigned int)), this, SLOT(updateTableWidgetView(unsigned int,unsigned int)));

    QHBoxLayout *tableViewLayout = new QHBoxLayout();
    layout->addLayout( tableViewLayout );
    tableViewLayout->addWidget(tableViewChooser);

    QLabel *saveLabel = new QLabel("Save segmentations as:");
    QVBoxLayout *saveButtonsLayout = new QVBoxLayout();
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

    QHBoxLayout *saveLayout = new QHBoxLayout();
    layout->addLayout( saveLayout);
    saveLayout->addWidget(saveLabel);
    saveLayout->addLayout(saveButtonsLayout);

    // How to use
    QString underlineStyle = "<br><br><span style=\" text-decoration: underline;\">%1</span>";
    QLabel *explanation = new QLabel(QString(underlineStyle).arg("Define a Contour: ") + " Activate the toolbox, then click on the data set."
                                     + QString(underlineStyle).arg("Define new Label: ") + " Right-click on the image then choose color"
                                     + QString(underlineStyle).arg("Rename a Label: ") + " Put the cursor on a node then right-click and set a new label name"
                                     + QString(underlineStyle).arg("Remove node/contour/label: ") + " BackSpace or put the cursor on a node then right-click and choose menu\"Remove ...\"."
                                     + QString(underlineStyle).arg("Save segmentation: ") + " Put the cursor on a node then right-click and choose menu \"Save ...\"."
                                     + QString(underlineStyle).arg("Copy ROIs in current slice: ") + " CTRL/CMD + c. or put the cursor on a node then right-click and choose menu\"Copy ...\""
                                     + QString(underlineStyle).arg("Paste ROIs:") + " CTRL/CMD + v."
                                     + QString(underlineStyle).arg("Change current label:") + " Put the cursor on a node then right-click and choose menu \"Change label ...\".");

    explanation->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    explanation->setWordWrap(true);
    layout->addWidget(explanation);

    // buttons initialisation: view has no data
    disableButtons();
}

polygonRoiToolBox::~polygonRoiToolBox()
{
    delete viewEventFilter;
}

bool polygonRoiToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<polygonRoiToolBox>();
}

dtkPlugin* polygonRoiToolBox::plugin()
{
    medPluginManager *pm = medPluginManager::instance();
    dtkPlugin *plugin = pm->plugin ( "Polygon ROI" );
    return plugin;
}

medAbstractData *polygonRoiToolBox::processOutput()
{
//    if (!m_maskData)
//    {
//        //generateBinaryImage();
//    }
//    return m_maskData; // return output data
    return nullptr;
}

void polygonRoiToolBox::checkRepulsor()
{
    if (viewEventFilter && viewEventFilter->isContourInSlice())
    {
        repulsorTool->setEnabled(true);
        if (repulsorTool->isChecked())
        {
            viewEventFilter->activateRepulsor(true);
        }
    }
}

void polygonRoiToolBox::updateView()
{
    medTabbedViewContainers *containers = this->getWorkspace()->tabbedViewContainers();
    if (containers)
    {
        if ( containers->currentIndex() == -1 )
        {
            return;
        }
        if ( containers->viewsInTab(containers->currentIndex()).size() > 1)
        {
            return;
        }

        medAbstractView *view = containers->getFirstSelectedContainerView();
        if (!view)
        {
            return;
        }
        medAbstractImageView *v = qobject_cast<medAbstractImageView*>(view);
        if (!v)
        {
            return;
        }

        if (v->layersCount()==1 && viewEventFilter)
        {
            medAbstractData *data = v->layerData(0);
            if (data->identifier().contains("medContours"))
            {
                return;
            }
        }

        for (unsigned int i=0; i<v->layersCount(); ++i)
        {
            medAbstractData *data = v->layerData(i);
            if(!data || data->identifier().contains("vtkDataMesh")
                                    || data->identifier().contains("itkDataImageVector"))
            {
                handleDisplayError(medAbstractProcessLegacy::DIMENSION_3D);
                return;
            }
            else
            {
                currentView = v;
                if (viewEventFilter)
                {
                    viewEventFilter->updateView(currentView);
                }
                activateTBButton->setEnabled(true);

                if (data->identifier().contains("medContours"))
                {
                    if (activateTBButton->isChecked()==false)
                    {
                        activateTBButton->setChecked(true);
                    }
                    managementToolBox->setWorkspace(getWorkspace());
                    managementToolBox->updateView();
                }

                emit currentLabelsDisplayed();

                updateTableWidgetItems();
                connect(currentView, SIGNAL(layerRemoved(uint)), this, SLOT(onLayerClosed(uint)), Qt::UniqueConnection);
                connect(view, SIGNAL(orientationChanged()), this, SLOT(updateTableWidgetItems()), Qt::UniqueConnection);
                connect(view, SIGNAL(orientationChanged()), this, SLOT(manageTick()), Qt::UniqueConnection);
                connect(view, SIGNAL(orientationChanged()), this, SLOT(manageRoisVisibility()), Qt::UniqueConnection);
            }
        }
    }
}


void polygonRoiToolBox::onLayerClosed(uint index)
{
    medAbstractImageView *view = static_cast<medAbstractImageView*>(QObject::sender());
    if (view->layersCount()==0)
    {
        if (viewEventFilter)
        {
            viewEventFilter->removeFromAllViews();
            viewEventFilter->reset();
        }
        clear();
        medTabbedViewContainers *tabs = this->getWorkspace()->tabbedViewContainers();
        if (tabs)
        {
            QList<medViewContainer*> containersInTab = tabs->containersInTab(tabs->currentIndex());
            if (containersInTab.size()>=1)
            {
                for(medViewContainer *container : containersInTab)
                {
                     if(container->uuid()!=mainContainerUUID)
                    {
                        container->close();
                    }
                }
            }
        }
    }
}

void polygonRoiToolBox::clickClosePolygon(bool state)
{
    if (!currentView)
    {
        activateTBButton->setChecked(false);
        qDebug()<<metaObject()->className()<<":: clickClosePolygon - no view in container.";
        return;
    }

    saveBinaryMaskButton->setEnabled(state);
    saveContourButton->setEnabled(state);
    enableTableViewChooser(state);
    interpolate->setEnabled(state);

    if (state)
    {
        if (!viewEventFilter)
        {
            viewEventFilter = new polygonEventFilter(currentView);
            connect(viewEventFilter, SIGNAL(enableRepulsor(bool)), repulsorTool, SLOT(setEnabled(bool)), Qt::UniqueConnection);
            connect(viewEventFilter, SIGNAL(enableGenerateMask(bool)), saveBinaryMaskButton, SLOT(setEnabled(bool)), Qt::UniqueConnection);
            connect(viewEventFilter, SIGNAL(enableViewChooser(bool)), this, SLOT(enableTableViewChooser(bool)), Qt::UniqueConnection);
            connect(viewEventFilter, SIGNAL(toggleRepulsorButton(bool)), this, SLOT(activateRepulsor(bool)), Qt::UniqueConnection);
            connect(viewEventFilter, SIGNAL(sendErrorMessage(QString)), this, SLOT(errorMessage(QString)), Qt::UniqueConnection);
            connect(this, SIGNAL(deactivateContours()), managementToolBox, SLOT(unselectAll()), Qt::UniqueConnection);
            connect(managementToolBox, SIGNAL(repulsorState()), this, SLOT(checkRepulsor()), Qt::UniqueConnection);
            connect(managementToolBox, SIGNAL(sendDatasToView(QList<medContourSharedInfo>)), viewEventFilter, SLOT(receiveDatasFromToolbox(QList<medContourSharedInfo>)), Qt::UniqueConnection);
            connect(managementToolBox, SIGNAL(sendContourState(medContourSharedInfo)), viewEventFilter, SLOT(receiveContourState(medContourSharedInfo)), Qt::UniqueConnection);
            connect(managementToolBox, SIGNAL(sendContourName(medContourSharedInfo)), viewEventFilter, SLOT(receiveContourName(medContourSharedInfo)), Qt::UniqueConnection);
            connect(managementToolBox, SIGNAL(sendActivationState(medContourSharedInfo)), viewEventFilter, SLOT(receiveActivationState(medContourSharedInfo)), Qt::UniqueConnection);
            connect(managementToolBox, SIGNAL(labelToDelete(medContourSharedInfo)), viewEventFilter, SLOT(deleteLabel(medContourSharedInfo)), Qt::UniqueConnection);
            connect(viewEventFilter, SIGNAL(sendContourInfoToListWidget(medContourSharedInfo&)), managementToolBox, SLOT(receiveContoursDatasFromView(medContourSharedInfo&)), Qt::UniqueConnection);
            connect(managementToolBox, SIGNAL(contoursToLoad(medTagContours, QColor)), viewEventFilter, SLOT(loadContours(medTagContours, QColor)), Qt::UniqueConnection);
            connect(viewEventFilter, SIGNAL(saveContours(medAbstractImageView*,vtkMetaDataSet*,QVector<medTagContours>)), managementToolBox, SLOT(onContoursSaved(medAbstractImageView*,vtkMetaDataSet*,QVector<medTagContours>)), Qt::UniqueConnection);
        }
        viewEventFilter->updateView(currentView);
        viewEventFilter->setEnableInterpolation(interpolate->isChecked());
        viewEventFilter->On();
        viewEventFilter->installOnView(currentView);
        viewEventFilter->addObserver();
        this->currentView->viewWidget()->setFocus();
        repulsorTool->setEnabled(state);
        if (repulsorTool->isChecked())
        {
            viewEventFilter->activateRepulsor(state);
        }
        connect(viewEventFilter, SIGNAL(clearLastAlternativeView()), this, SLOT(resetToolboxBehaviour()));
    }
    else
    {
        viewEventFilter->removeFromView(currentView);
        repulsorTool->setEnabled(state);
        emit deactivateContours();
        viewEventFilter->Off();

    }
}

void polygonRoiToolBox::activateRepulsor(bool state)
{
    if (currentView && viewEventFilter)
    {
        repulsorTool->setChecked(state);
        viewEventFilter->activateRepulsor(state);
        currentView->viewWidget()->setFocus();
    }
}

void polygonRoiToolBox::resetToolboxBehaviour()
{
    medTabbedViewContainers *containers = this->getWorkspace()->tabbedViewContainers();
    QList<medViewContainer*> containersInTabSelected = containers->containersInTab(containers->currentIndex());
    if (containersInTabSelected.size() != 1)
    {
        return;
    }
    containersInTabSelected[0]->setClosingMode(medViewContainer::CLOSE_CONTAINER);
    enableTableViewChooser(activateTBButton->isChecked());
}

void polygonRoiToolBox::errorMessage(QString error)
{
    displayMessageError(error);
}

void polygonRoiToolBox::manageTick()
{
    if (viewEventFilter && currentView)
    {
        viewEventFilter->manageTick();
    }
}

void polygonRoiToolBox::manageRoisVisibility()
{
    if (viewEventFilter && currentView)
    {
        viewEventFilter->manageRoisVisibility();
    }
}

void polygonRoiToolBox::updateTableWidgetView(unsigned int row, unsigned int col)
{
    if (!viewEventFilter)
    {
        return;
    }

    medTabbedViewContainers *tabs = this->getWorkspace()->tabbedViewContainers();
    QList<medViewContainer*> containersInTabSelected = tabs->containersInTab(tabs->currentIndex());
    if (containersInTabSelected.size() != 1)
    {

        return;
    }
    medViewContainer* mainContainer = containersInTabSelected.at(0);
    mainContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    mainContainerUUID = mainContainer->uuid();
    medAbstractImageView* mainView = dynamic_cast<medAbstractImageView *> (mainContainer->view());
    if (!mainView)
    {
        return;
    }

    medViewContainer *previousContainer;
    medViewContainer* container;
    QString linkGroupBaseName = "MPR ";
    unsigned int linkGroupNumber = 1;

    QString linkGroupName = linkGroupBaseName + QString::number(linkGroupNumber);
    while (medParameterGroupManagerL::instance()->viewGroup(linkGroupName))
    {
        linkGroupNumber++;
        linkGroupName = linkGroupBaseName + QString::number(linkGroupNumber);
    }
    medViewParameterGroupL *viewGroup = new medViewParameterGroupL(linkGroupName, mainView);
    viewGroup->addImpactedView(mainView);
    for (int nbItem = 0; nbItem<tableViewChooser->selectedItems().size(); nbItem++)
    {
        if (nbItem == 0)
        {
            container = mainContainer->splitVertically();
            previousContainer = container;
        }
        else if (nbItem == 1)
        {
            container = previousContainer->splitHorizontally();
        }
        else if (nbItem == 2)
        {
            container = mainContainer->splitHorizontally();
        }
        else
        {
            handleDisplayError(medAbstractProcessLegacy::FAILURE);
            return;
        }
        for (int i=0; i<mainView->layersCount(); i++)
        {
            container->addData(mainView->layerData(i));
        }

        medAbstractImageView* view = static_cast<medAbstractImageView *> (container->view());
        connect(view, &medAbstractImageView::closed, [this, mainContainer](){
            if (viewEventFilter)
            {
                viewEventFilter->clearAlternativeView();
                medTabbedViewContainers *tabs = this->getWorkspace()->tabbedViewContainers();
                if (tabs)
                {
                    QList<medViewContainer*> containersInTab = tabs->containersInTab(tabs->currentIndex());
                    if (containersInTab.size()==1 && containersInTab.at(0)->uuid()==mainContainerUUID)
                    {
                        if (activateTBButton->isEnabled())
                        {
                            repulsorTool->setEnabled(true);
                            tableViewChooser->setEnabled(true);
                        }
                    }
                }
                mainContainer->setSelected(true);
            }
        });
        viewEventFilter->installOnView(view);
        viewEventFilter->clearCopiedContours();
        connect(container, &medViewContainer::containerSelected, [=](){
            if (viewEventFilter)
            {
                viewEventFilter->Off();
                repulsorTool->setEnabled(false);
            }
        });

        medTableWidgetItem * item = static_cast<medTableWidgetItem*>(tableViewChooser->selectedItems().at(nbItem));
        view->setOrientation(dynamic_cast<medTableWidgetItem*>(item)->orientation());
        viewEventFilter->addAlternativeViews(view);

        viewGroup->addImpactedView(view);

    }

    viewGroup->setLinkAllParameters(true);
    viewGroup->removeParameter("Slicing");
    viewGroup->removeParameter("Orientation");

    foreach(medAbstractParameterL* param, mainView->linkableParameters())
    {
        param->trigger();
    }

    for (unsigned int i = 0;i < mainView->layersCount();++i)
    {
        foreach(medAbstractParameterL* param, mainView->linkableParameters(i))
        {
            param->trigger();
        }
    }

    connect(mainContainer, &medViewContainer::containerSelected, [=](){
        if (currentView && activateTBButton->isChecked())
        {
            currentView->viewWidget()->setFocus();
            viewEventFilter->On();
            repulsorTool->setEnabled(true);
            if (repulsorTool->isChecked())
            {
                activateRepulsor(true);
            }
        }
    });
    mainContainer->setSelected(true);
    viewEventFilter->enableOtherViewsVisibility(true);

    tableViewChooser->setEnabled(false);
}


void polygonRoiToolBox::updateTableWidgetItems()
{
    if ( tableViewChooser->selectedItems().size() > 0 )
    {
        return;
    }

    medTableWidgetItem *firstOrientation;
    medTableWidgetItem *secondOrientation;
    medTableWidgetItem *thirdOrientation;
    if (currentView)
    {

        switch(currentView->orientation())
        {
        case medImageView::VIEW_ORIENTATION_AXIAL:
            firstOrientation = new medTableWidgetItem(QIcon(":/icons/CoronalIcon.png"),
                                                      QString("Coronal view"),
                                                      medTableWidgetItem::CoronalType);
            secondOrientation = new medTableWidgetItem(QIcon(":/icons/SagittalIcon.png"),
                                                       QString("Sagittal view"),
                                                       medTableWidgetItem::SagittalType);
            thirdOrientation = new medTableWidgetItem(QIcon(":/icons/3DIcon.png"),
                                                      QString("3d view"),
                                                      medTableWidgetItem::ThreeDimType);

        break;

        case medImageView::VIEW_ORIENTATION_CORONAL:
            firstOrientation = new medTableWidgetItem(QIcon(":/icons/AxialIcon.png"),
                                                      QString("Axial view"),
                                                      medTableWidgetItem::AxialType);
            secondOrientation =  new medTableWidgetItem(QIcon(":/icons/SagittalIcon.png"),
                                                        QString("Sagittal view"),
                                                        medTableWidgetItem::SagittalType);
            thirdOrientation = new medTableWidgetItem(QIcon(":/icons/3DIcon.png"),
                                                      QString("3d view"),
                                                      medTableWidgetItem::ThreeDimType);

        break;

        case medImageView::VIEW_ORIENTATION_SAGITTAL:
            firstOrientation = new medTableWidgetItem(QIcon(":/icons/AxialIcon.png"),
                                                      QString("Axial view"),
                                                      medTableWidgetItem::AxialType);
            secondOrientation = new medTableWidgetItem(QIcon(":/icons/CoronalIcon.png"),
                                                       QString("Coronal view"),
                                                       medTableWidgetItem::CoronalType);
            thirdOrientation = new medTableWidgetItem(QIcon(":/icons/3DIcon.png"),
                                                      QString("3d view"),
                                                      medTableWidgetItem::ThreeDimType);

        break;


        case medImageView::VIEW_ORIENTATION_3D:
            firstOrientation = new medTableWidgetItem(QIcon(":/icons/AxialIcon.png"),
                                                      QString("Axial view"),
                                                      medTableWidgetItem::AxialType);
            secondOrientation = new medTableWidgetItem(QIcon(":/icons/CoronalIcon.png"),
                                                       QString("Coronal view"),
                                                       medTableWidgetItem::CoronalType);
            thirdOrientation = new medTableWidgetItem(QIcon(":/icons/SagittalIcon.png"),
                                                      QString("Sagittal view"),
                                                      medTableWidgetItem::SagittalType);

        break;

        case medImageView::VIEW_ALL_ORIENTATION:
        default:
            qDebug()<<metaObject()->className()<<":: updateTableWidgetItems - unknown view.";
            handleDisplayError(medAbstractProcessLegacy::FAILURE);
            return;
        }

    }
    else
    {
        firstOrientation = new medTableWidgetItem(QIcon(":/icons/AxialIcon.png"),
                                                  QString("Axial view"),
                                                  medTableWidgetItem::AxialType);
        secondOrientation = new medTableWidgetItem(QIcon(":/icons/CoronalIcon.png"),
                                                   QString("Coronal view"),
                                                   medTableWidgetItem::CoronalType);
        thirdOrientation = new medTableWidgetItem(QIcon(":/icons/SagittalIcon.png"),
                                                  QString("Sagittal view"),
                                                  medTableWidgetItem::SagittalType);
    }
    tableViewChooser->setItem(0, 0,firstOrientation);
    tableViewChooser->setItem(0, 1,secondOrientation);
    tableViewChooser->setItem(0, 2,thirdOrientation);

    return;

}

void polygonRoiToolBox::enableTableViewChooser(bool state)
{
    if (state)
    {
        medTabbedViewContainers *containers = this->getWorkspace()->tabbedViewContainers();
        QList<medViewContainer*> containersInTabSelected = containers->containersInTab(containers->currentIndex());
        if (containersInTabSelected.size() == 1)
        {
            tableViewChooser->setEnabled(state);
        }
    }
    else
    {
        tableViewChooser->setEnabled(state);
    }
}

void polygonRoiToolBox::interpolateCurve(bool state)
{
    if (!viewEventFilter)
    {
        return;
    }
    viewEventFilter->setEnableInterpolation(state);
    if ( currentView )
    {
        currentView->viewWidget()->setFocus();
    }
}

void polygonRoiToolBox::saveBinaryImage()
{
    if (!viewEventFilter)
    {
        return;
    }
    viewEventFilter->saveMask();
}

void polygonRoiToolBox::disableButtons()
{
    activateTBButton->setEnabled(false);
    activateTBButton->setChecked(false);
    repulsorTool->setEnabled(false);
    repulsorTool->setChecked(false);
    saveBinaryMaskButton->setEnabled(false);
    saveContourButton->setEnabled(false);
    tableViewChooser->setEnabled(false);
    interpolate->setEnabled(false);
    interpolate->setChecked(true);
}

void polygonRoiToolBox::saveContours()
{
    if (viewEventFilter)
    {
        viewEventFilter->saveAllContours();
    }
}

void polygonRoiToolBox::clear()
{
    managementToolBox->clear();
    medToolBox::clear();

    if(currentView)
    {
        currentView = nullptr;
    }
    disableButtons();
}
