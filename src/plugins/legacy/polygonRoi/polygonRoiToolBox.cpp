/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "polygonRoiToolBox.h"

#include <medAbstractProcessLegacy.h>
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
#include <polygonEventFilter.h>


const char *polygonRoiToolBox::generateBinaryImageButtonName = "generateBinaryImageButton";


polygonRoiToolBox::polygonRoiToolBox(QWidget *parent ) :
    medAbstractSelectableToolBox(parent), viewEventFilter(nullptr)
{
    QWidget *displayWidget = new QWidget(this);
    this->addWidget(displayWidget);

    QVBoxLayout *layout = new QVBoxLayout();
    displayWidget->setLayout(layout);

    addNewCurve = new QPushButton(tr("Activate Toolbox"));
    addNewCurve->setToolTip(tr("Activate closed polygon mode"));
    addNewCurve->setCheckable(true);
    addNewCurve->setObjectName("closedPolygonButton");
    connect(addNewCurve,SIGNAL(toggled(bool)),this,SLOT(clickClosePolygon(bool)));

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

    QHBoxLayout *ButtonLayout0 = new QHBoxLayout();
    layout->addLayout( ButtonLayout0 );
    ButtonLayout0->addWidget(addNewCurve);

    QHBoxLayout *ButtonLayout1 = new QHBoxLayout();
    layout->addLayout( ButtonLayout1 );
    ButtonLayout1->addWidget(interpolate);
    ButtonLayout1->addWidget(repulsorTool);

    tableViewChooser = new medTableWidgetChooser(this, 1, 3, 50);
    // Mandatory : Qt bug ? : Without the lines below, the size of the table View is not as expected
    QSize size = tableViewChooser->sizeHint();
    tableViewChooser->setFixedHeight(size.height()-1);
    tableViewChooser->setFixedWidth(size.width()-1);
    tableViewChooser->setIconSize(QSize(size.height()-1,size.height()-1));
    connect(tableViewChooser, SIGNAL(selected(unsigned int,unsigned int)), this, SLOT(updateTableWidgetView(unsigned int,unsigned int)));

    QHBoxLayout *ButtonLayout2 = new QHBoxLayout();
    layout->addLayout( ButtonLayout2 );
    ButtonLayout2->addWidget(tableViewChooser);

    QLabel *saveLabel = new QLabel("Save segmentations as:");
    QVBoxLayout *ButtonSaveLayout = new QVBoxLayout();
    saveBinaryMaskButton = new QPushButton(tr("Mask(s)"));
    saveBinaryMaskButton->setToolTip("Import the current mask to the non persistent database");
    saveBinaryMaskButton->setObjectName(generateBinaryImageButtonName);
    connect(saveBinaryMaskButton,SIGNAL(clicked()),this,SLOT(saveBinaryImage()));
    ButtonSaveLayout->addWidget(saveBinaryMaskButton);

    saveContourButton = new QPushButton("Contour(s)");
    saveContourButton->setToolTip("Export these contours as an .ctrb file loadable only in medInria.");
    saveContourButton->setMinimumSize(150, 20);
    saveContourButton->setMaximumSize(150, 20);
    saveContourButton->setObjectName("saveContoursButton");
    connect(saveContourButton, SIGNAL(clicked()), this, SLOT(saveContours()));
    ButtonSaveLayout->addWidget(saveContourButton);

    QHBoxLayout *ButtonLayout3 = new QHBoxLayout();
    layout->addLayout( ButtonLayout3);
    ButtonLayout3->addWidget(saveLabel);
    ButtonLayout3->addLayout(ButtonSaveLayout);

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

void polygonRoiToolBox::loadContoursIfPresent(medAbstractImageView *v, unsigned int layer)
{
    medAbstractData *data = v->layerData(layer);
    if (data->identifier().contains("medContours") && v==currentView)
    {
        if (addNewCurve->isChecked()==false)
        {
            addNewCurve->setChecked(true);
        }
        viewEventFilter->loadContours(data);
        if (viewEventFilter->isContourInSlice())
        {
            repulsorTool->setEnabled(true);
            if (repulsorTool->isChecked())
            {
                    viewEventFilter->activateRepulsor(true);
            }
        }

        v->removeLayer(layer);
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
        QList<medAbstractView*> viewsInTabSelected = containers->viewsInTab(containers->currentIndex());

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

        QList<medViewContainer*> containersInTab = containers->containersInTab(containers->currentIndex());
        if (viewsInTabSelected.size() > 1)
        {
            for (unsigned int i=0; i<v->layersCount(); ++i)
            {
                loadContoursIfPresent(v, i);
            }
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
                addNewCurve->setEnabled(true);

                if (currentView != v)
                {
                    currentView = v;
                }

                loadContoursIfPresent(v, i);

                updateTableWidgetItems();
                connect(currentView, SIGNAL(closed()), this, SLOT(onViewClosed()), Qt::UniqueConnection);
                connect(currentView, SIGNAL(layerRemoved(uint)), this, SLOT(onLayerClosed(uint)), Qt::UniqueConnection);
                connect(view, SIGNAL(orientationChanged()), this, SLOT(updateTableWidgetItems()), Qt::UniqueConnection);
                connect(view, SIGNAL(orientationChanged()), this, SLOT(manageTick()), Qt::UniqueConnection);
                connect(view, SIGNAL(orientationChanged()), this, SLOT(manageRoisVisibility()), Qt::UniqueConnection);

            }
        }
    }
}

void polygonRoiToolBox::onViewClosed()
{
    medAbstractView *viewClosed = qobject_cast<medAbstractView*>(QObject::sender());
    medTabbedViewContainers *containers = this->getWorkspace()->tabbedViewContainers();
    if (containers)
    {
        QList<medAbstractView*> viewsInTabSelected = containers->viewsInTab(containers->currentIndex());
        QList<medViewContainer*> containersInTab = containers->containersInTab(containers->currentIndex());

        if (viewEventFilter)
        {
            if (viewClosed == viewEventFilter->getView() )
            {
                viewEventFilter->reset();
            }
        }
        if (viewsInTabSelected.size()==0)
        {
            if (viewClosed == currentView)
            {
                currentView = nullptr;
            }
            disableButtons();
            clear();
        }
    }
}

void polygonRoiToolBox::onLayerClosed(uint index)
{
    medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();
    medAbstractImageView *v = qobject_cast<medAbstractImageView*>(view);
    if (!v || (v && v->layersCount()==0))
    {
        if (viewEventFilter)
        {
            viewEventFilter->reset();
        }
        this->clear();
    }
}

void polygonRoiToolBox::clickClosePolygon(bool state)
{
    if (!currentView)
    {
        addNewCurve->setChecked(false);
        qDebug()<<metaObject()->className()<<":: clickClosePolygon - no view in container.";
        return;
    }

    saveBinaryMaskButton->setEnabled(state);
    saveContourButton->setEnabled(state);
    enableTableViewChooser(state);
    if (state)
    {
        if (!viewEventFilter)
        {
            viewEventFilter = new polygonEventFilter(currentView);
            connect(viewEventFilter, SIGNAL(enableRepulsor(bool)), repulsorTool, SLOT(setEnabled(bool)), Qt::UniqueConnection);
            connect(viewEventFilter, SIGNAL(enableGenerateMask(bool)), saveBinaryMaskButton, SLOT(setEnabled(bool)), Qt::UniqueConnection);
            connect(viewEventFilter, SIGNAL(enableViewChooser(bool)), this, SLOT(enableTableViewChooser(bool)), Qt::UniqueConnection);
            connect(viewEventFilter, SIGNAL(toggleRepulsorButton(bool)), this, SLOT(activateRepulsor(bool)), Qt::UniqueConnection);
        }

        viewEventFilter->updateView(currentView);
        viewEventFilter->setEnableInterpolation(interpolate->isChecked());
        viewEventFilter->On();
        viewEventFilter->installOnView(currentView);
        viewEventFilter->addObserver();

        if ( viewEventFilter->isContourInSlice() )
        {
            repulsorTool->setEnabled(state);
            if (repulsorTool->isChecked())
            {
                viewEventFilter->activateRepulsor(state);
            }
        }
        connect(viewEventFilter, SIGNAL(clearLastAlternativeView()), this, SLOT(resetToolboxBehaviour()));
    }
    else
    {
        repulsorTool->setEnabled(state);
        viewEventFilter->Off();
        viewEventFilter->removeFromAllViews();
    }
}

void polygonRoiToolBox::activateRepulsor(bool state)
{
    if (currentView && viewEventFilter)
    {
        repulsorTool->setChecked(state);
        viewEventFilter->activateRepulsor(state);
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
    enableTableViewChooser(addNewCurve->isChecked());
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

    medTabbedViewContainers *containers = this->getWorkspace()->tabbedViewContainers();
    QList<medViewContainer*> containersInTabSelected = containers->containersInTab(containers->currentIndex());
    if (containersInTabSelected.size() != 1)
    {

        return;
    }
    medViewContainer* mainContainer = containersInTabSelected.at(0);
    medAbstractImageView* mainView = dynamic_cast<medAbstractImageView *> (mainContainer->view());
    if (!mainView)
    {
        return;
    }
    medAbstractData* data = mainView->layerData(0);

    medViewContainer *previousContainer;
    medViewContainer* container;
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
        container->addData(data);

        medAbstractImageView* view = static_cast<medAbstractImageView *> (container->view());
        viewEventFilter->installOnView(view);
        viewEventFilter->clearCopiedContours();
        connect(view, &medAbstractView::closed, [=](){
            mainContainer->setSelected(true);
        });
        connect(container, &medViewContainer::containerSelected, [=](){
            viewEventFilter->Off();
            repulsorTool->setEnabled(false);
        });
        medTableWidgetItem * item = static_cast<medTableWidgetItem*>(tableViewChooser->selectedItems().at(nbItem));
        connect(view, SIGNAL(closed()), viewEventFilter, SLOT(removeView()));

        view->setOrientation(dynamic_cast<medTableWidgetItem*>(item)->orientation());
        viewEventFilter->addAlternativeViews(view);
    }
    connect(mainContainer, &medViewContainer::containerSelected, [=](){
        if (addNewCurve->isChecked())
        {
            viewEventFilter->On();
            repulsorTool->setEnabled(true);
            if (repulsorTool->isChecked())
            {
                activateRepulsor(true);
            }
        }
    });
    mainContainer->setClosingMode(medViewContainer::CLOSE_BUTTON_HIDDEN);
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
    addNewCurve->setEnabled(false);
    addNewCurve->setChecked(false);
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
    medToolBox::clear();

    if(currentView)
    {
        currentView = nullptr;
    }
    disableButtons();
}
