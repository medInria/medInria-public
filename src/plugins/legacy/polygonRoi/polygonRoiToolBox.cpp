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
    colorsList = QList<QColor>({
        Qt::green,
        Qt::blue,
        QColor(224,255,255),
        QColor(64,224,208),
        QColor(46,139,87),
        Qt::darkMagenta,
        Qt::gray,
        Qt::white,
        Qt::black
    });
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

    specialities = new QComboBox();

    plusButton = new QPushButton(QIcon(":/pixmaps/plus.png"), "");
    plusButton->setMaximumSize(QSize(20,20));
    connect(plusButton, SIGNAL(pressed()), this, SLOT(addLabelNameInList()));
    plusButton->hide();

    minusButton = new QPushButton(QIcon(":/pixmaps/minus.png"), "");
    minusButton->setMaximumSize(QSize(20,20));
    connect(minusButton, SIGNAL(pressed()), this, SLOT(removeLabelNameInList()));
    minusButton->hide();

    QStringList spe;
    spe << "default" << "prostate";
    specialities->addItems(spe);
    connect(specialities, SIGNAL(currentIndexChanged(int)), this, SLOT(showWidgetListForIndex(int)));
    // The lines below are temp code
    // Will be replaced soon by values parsed in json configuration file
    QListWidget *defaultNameList = new QListWidget;
    defaultNameList->show();
    structuresList.append(defaultNameList);
    QListWidget *prostateNameList = new QListWidget;
    prostateNameList->hide();
    structuresList.append(prostateNameList);
    // End of temp code

    currentView = nullptr;

    QHBoxLayout *activateTBLayout = new QHBoxLayout();
    layout->addLayout( activateTBLayout );
    activateTBLayout->addWidget(activateTBButton);

    listNamesLayout = new QVBoxLayout();
    layout->addLayout(listNamesLayout);
    listNamesLayout->addWidget(specialities);
    for (QListWidget *labelNamesList : structuresList)
    {
        listNamesLayout->addWidget(labelNamesList);
    }
    QHBoxLayout *plusMinusLayout = new QHBoxLayout;
    plusMinusLayout->addWidget(minusButton, Qt::AlignLeft);
    plusMinusLayout->addWidget(plusButton, Qt::AlignLeft);
    plusMinusLayout->addStretch();
    listNamesLayout->addLayout(plusMinusLayout);

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
    for (QListWidget *widget : structuresList)
    {
        delete widget;
    }
    structuresList.clear();
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

bool polygonRoiToolBox::loadContoursIfPresent(medAbstractImageView *v, unsigned int layer)
{
    bool retVal = false;
    medAbstractData *data = v->layerData(layer);
    if (data->identifier().contains("medContours") && v==currentView)
    {
        if (activateTBButton->isChecked()==false)
        {
            activateTBButton->setChecked(true);
        }

        medContours *contours = dynamic_cast<medContours*>(data);
        QVector<medTagContours> &tagContoursSet = contours->getTagContoursSet();
        QListWidget *widget = structuresList.at(specialities->currentIndex());
        for (medTagContours &tagContours : tagContoursSet)
        {
            if (widget->count()<colorsList.size())
            {
                QColor color = findAvailableColor(widget);
                bool itemFound = false;
                for (int row = 0; row < widget->count(); row++)
                {
                    QListWidgetItem *item = widget->item(row);
                    if (item->text()==tagContours.getLabelName())
                    {
                        itemFound = true;
                        color = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
                    }
                }
                if (!itemFound)
                {
                    QListWidgetItem *item = createWidgetItem(tagContours.getLabelName(),
                                                             color);
                    widget->addItem(item);
                    widget->setMaximumHeight((20*widget->count())+5);
                }
                if (color!=QColor::Invalid)
                {
                    viewEventFilter->loadContours(tagContours, color);
                }
            }
            else
            {
                errorMessage("Too many label already imported");
            }
        }

        if (viewEventFilter->isContourInSlice())
        {
            repulsorTool->setEnabled(true);
            if (repulsorTool->isChecked())
            {
                viewEventFilter->activateRepulsor(true);
            }
        }

        v->removeLayer(layer);
        retVal = true;
    }
    return retVal;
}

QColor polygonRoiToolBox::findAvailableColor(QListWidget *widget)
{
    QColor color;
    QList<QColor> colors = colorsList;
    for (int row = 0; row < widget->count(); row++)
    {
        QListWidgetItem *item = widget->item(row);
        color = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
        if (colors.contains(color))
        {
            colors.removeOne(color);
        }
    }
    if (colors.empty())
    {
        color = QColor::Invalid;
    }
    else
    {
        color = colors.at(0);
    }
    return color;
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
                activateTBButton->setEnabled(true);

                // The lines below are temp code
                // Will be replaced soon by values parsed in json configuration file
                QListWidget *defaultWidget = structuresList.at(0);
                if ( defaultWidget->count()==0)
                {
                    QStringList defaultStructures;
                    defaultStructures << "Label-0";
                    initStructureNames(defaultWidget,defaultStructures);
                }
                QListWidget *prostateWidget = structuresList.at(1);
                if ( prostateWidget->count()==0)
                {
                    QStringList prostateStructures;
                    prostateStructures << "WG (whole gland)" << "TZ (transitional zone)" << "Target 1";
                    initStructureNames(prostateWidget,prostateStructures, true);
                }
                // End of temp code

                if (currentView != v)
                {
                    currentView = v;
                }
                loadContoursIfPresent(v, i);
                showWidgetListForIndex(specialities->currentIndex());

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

void polygonRoiToolBox::clearStructuresList()
{
    for (int row = 0; row < structuresList.size(); row++)
    {
        QListWidget *widget = structuresList.at(row);
        if (row < 2)
        {
            widget->clear();
            widget->hide();
        }
        else
        {
            listNamesLayout->removeWidget(widget);
            structuresList.removeOne(widget);
            delete widget;
        }
    }
    const QSignalBlocker blocker(specialities);
    int count = specialities->count();
    for (int row = 0; row < count; row++)
    {
        if (row > 1)
        {
            specialities->removeItem(row);
        }
    }
    specialities->setCurrentIndex(0);
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
                clearStructuresList();
            }
        }
        currentView = nullptr;
        clear();
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
            viewEventFilter->removeFromAllViews();
            clearStructuresList();
        }
        this->clear();
        medTabbedViewContainers *containers = this->getWorkspace()->tabbedViewContainers();
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
    specialities->setEnabled(state);
    for (QListWidget *structureWidget : structuresList)
    {
        structureWidget->setEnabled(state);
    }
    plusButton->setEnabled(state);
    minusButton->setEnabled(state);

    if (state)
    {
        if (!viewEventFilter)
        {
            viewEventFilter = new polygonEventFilter(currentView, colorsList);
            connect(viewEventFilter, SIGNAL(enableRepulsor(bool)), repulsorTool, SLOT(setEnabled(bool)), Qt::UniqueConnection);
            connect(viewEventFilter, SIGNAL(enableGenerateMask(bool)), saveBinaryMaskButton, SLOT(setEnabled(bool)), Qt::UniqueConnection);
            connect(viewEventFilter, SIGNAL(enableViewChooser(bool)), this, SLOT(enableTableViewChooser(bool)), Qt::UniqueConnection);
            connect(viewEventFilter, SIGNAL(toggleRepulsorButton(bool)), this, SLOT(activateRepulsor(bool)), Qt::UniqueConnection);
            connect(viewEventFilter, SIGNAL(sendErrorMessage(QString)), this, SLOT(errorMessage(QString)), Qt::UniqueConnection);
            connect(viewEventFilter, SIGNAL(sendContourInfoToListWidget(medContourInfo&)), this, SLOT(receiveContoursDatasFromView(medContourInfo&)), Qt::UniqueConnection);
        }

        viewEventFilter->updateView(currentView);
        viewEventFilter->setEnableInterpolation(interpolate->isChecked());
        viewEventFilter->On();
        viewEventFilter->installOnView(currentView);
        viewEventFilter->addObserver();
        this->currentView->viewWidget()->setFocus();
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
        container->addData(data);

        medAbstractImageView* view = static_cast<medAbstractImageView *> (container->view());
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
//        connect(view, SIGNAL(closed()), viewEventFilter, SLOT(removeView()));
//        connect(view, &medAbstractImageView::closed, [=](){
//            if (viewEventFilter)
//            {

//            }
//        });
        view->setOrientation(dynamic_cast<medTableWidgetItem*>(item)->orientation());
        viewEventFilter->addAlternativeViews(view);

        viewGroup->addImpactedView(view);

    }
    viewGroup->setLinkAllParameters(true);
    viewGroup->removeParameter("Slicing");
    viewGroup->removeParameter("Orientation");

    medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();
    medAbstractImageView *v = qobject_cast<medAbstractImageView*>(view);
    for (unsigned int i = 0;i < v->layersCount();++i)
    {
        QString linkLayerName = linkGroupBaseName + QString::number(linkGroupNumber) + " Layer " + QString::number(i+1);
        medLayerParameterGroupL* layerGroup = new medLayerParameterGroupL(linkLayerName, this);
        layerGroup->setLinkAllParameters(true);
        layerGroup->removeParameter("Slicing");
        medTabbedViewContainers *containers = this->getWorkspace()->tabbedViewContainers();
        if (containers)
        {
            QList<medViewContainer*> containersInTab = containers->containersInTab(containers->currentIndex());
            for (int idx = 0; idx<containersInTab.size(); idx++)
            {
                medAbstractImageView* v = dynamic_cast<medAbstractImageView *> (containersInTab[idx]->view());
                layerGroup->addImpactedlayer(v, v->layerData(i));
            }
        }
    }

    foreach(medAbstractParameterL* param, v->linkableParameters())
    {
        param->trigger();
    }

    for (unsigned int i = 0;i < v->layersCount();++i)
    {
        foreach(medAbstractParameterL* param, v->linkableParameters(i))
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

QListWidgetItem * polygonRoiToolBox::createWidgetItem(QString name, QColor col)
{
    QPixmap pixmap(20,20);
    pixmap.fill(col);
    QListWidgetItem *item =  new QListWidgetItem(pixmap, name);
    item->setSizeHint(QSize(20,20));
    QFont font;
    font.setBold(true);
    item->setFont(font);
    item->setFlags(item->flags() | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);

    return item;
}

void polygonRoiToolBox::receiveContoursDatasFromView(medContourInfo& info)
{
    QListWidget *labels = structuresList.at(specialities->currentIndex());
    for (int row = 0; row < labels->count(); row++)
    {
        QListWidgetItem *item = labels->item(row);
        QString itemName = item->text();
        QColor itemColor = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
        if ( info.nameUpdated() && itemColor==info.getColor())
        {
            item->setText(info.getName());
            item->setSelected(true);
        }
        else if (itemName==info.getName() && itemColor==info.getColor())
        {
            item->setSelected(info.isSelected());
            if (info.scoreState())
            {
                QColor secondColor = info.getAdditionalColor();
                QColor color = (item->foreground().color()==secondColor)?Qt::white:secondColor;
                item->setForeground(color);
                if ( item->foreground().color()==secondColor )
                {
                    QString style = "QListView::item:selected { background: rgb(%1, %2, %3); }";
                    labels->setStyleSheet(style.arg(secondColor.red()).arg(secondColor.green()).arg(secondColor.blue()));
                }
                else
                {
                    labels->setStyleSheet("QListView::item:selected { background: palette(Highlight) }");
                }
            }
        }
        else
        {
            item->setSelected(false);
        }
    }
}

void polygonRoiToolBox::addLabelNameInList()
{
    QListWidget *structureWidget = structuresList.at(specialities->currentIndex());
    if ( structureWidget->count()==colorsList.size())
    {
        displayMessageError("Unable to create more label. Maximum size achieved");
        return;
    }
    QColor color = findAvailableColor(structureWidget);
    QListWidgetItem *item = createWidgetItem(QString("Label-%1").arg(QString::number(structureWidget->count())),
                                             color);
    structureWidget->addItem(item);
    item->setSelected(true);
    structureWidget->setMaximumHeight((20*structureWidget->count())+5);

    emit structureWidget->itemClicked(item);
}

void polygonRoiToolBox::removeLabelNameInList()
{
    QListWidget *widget = structuresList.at(specialities->currentIndex());
    for (int row = 0; row<widget->count(); row++)
    {
        QListWidgetItem *item = widget->item(row);
        if (item->isSelected())
        {
            QString name = item->text();
            QColor col = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
            medContourInfo info = medContourInfo(item->text(), col);
            if (viewEventFilter)
            {
                viewEventFilter->deleteLabel(info);
            }
            widget->takeItem(row);
        }
    }

    if (widget->count()==0)
    {
        if (viewEventFilter)
        {
            viewEventFilter->updateContourState(QString(), QColor::Invalid);
        }
        if (specialities->currentIndex() > 1)
        {
            listNamesLayout->removeWidget(widget);
            structuresList.removeOne(widget);
            delete widget;
            specialities->removeItem(specialities->currentIndex());
        }
    }
    else
    {
        widget->setMaximumHeight((20*widget->count())+5);
    }
}

void polygonRoiToolBox::showWidgetListForIndex(int index)
{
    for (QListWidget *widget : structuresList)
    {
        widget->hide();
    }
    QListWidget *structuresWidget = structuresList.at(index);

    updateLabelNamesOnContours(structuresWidget);

    if (structuresWidget->count()>0)
    {
        structuresWidget->item(0)->setSelected(true);
        emit structuresWidget->itemClicked(structuresWidget->item(0));
    }

    structuresWidget->show();
    plusButton->show();
    minusButton->show();
}

void polygonRoiToolBox::updateLabelNamesOnContours(QListWidget *structuresWidget)
{
    QList<medContourInfo> infos;
    for (int row = 0; row < structuresWidget->count(); row++)
    {
        QListWidgetItem *item = structuresWidget->item(row);
        QColor col = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
        medContourInfo info = medContourInfo(item->text(), col);
        infos.append(info);
    }

    if ( viewEventFilter)
    {
        viewEventFilter->updateManagerInfos(infos);
    }

}

void polygonRoiToolBox::initStructureNames(QListWidget *structuresWidget,
                                        QStringList names, bool isProstate)
{
    connect(structuresWidget, &QListWidget::itemClicked, [this, structuresWidget](QListWidgetItem *item){
       structuresWidget->setStyleSheet("QListView::item:selected { background: palette(Highlight) }");
       if ( viewEventFilter )
       {
            if (currentView)
            {
               currentView->viewWidget()->setFocus();
            }
            QColor color = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
            QString name = item->text();
            bool score = (item->checkState()==Qt::Checked)?true:false;
            if (!score)
            {
                item->setForeground(Qt::white);
            }
            if ( viewEventFilter)
            {
                viewEventFilter->updateContourState(name, color, score);
            }
       }
    });

    connect(structuresWidget, &QListWidget::itemChanged, [this](QListWidgetItem *item){
        if ( viewEventFilter )
        {
            if (currentView)
            {
                currentView->viewWidget()->setFocus();
            }
            QColor color = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
            QString name = item->text();
            if ( viewEventFilter)
            {
                viewEventFilter->changeContourName(name, color);
            }
        }
    });

    connect(structuresWidget->model(), &QAbstractItemModel::rowsMoved, [this, structuresWidget](const QModelIndex &, int, int, const QModelIndex &, int){
//        QListWidgetItem *item = structuresWidget->currentItem();
//        if ( viewEventFilter )
//        {
//            if (currentView)
//            {
//                currentView->viewWidget()->setFocus();
//            }
//            QColor color = item->icon().pixmap(QSize(20,20)).toImage().pixelColor(0,0);
//            QString name = item->text();
//            if ( viewEventFilter)
//            {
//                viewEventFilter->updateContourState(name, color);
//            }
//        }
    });
    structuresWidget->setContentsMargins(0,0,0,0);

    int idx = 0;
    for (QString name : names)
    {
        if (colorsList.size()<=idx)
        {
            errorMessage("Unable to add new contour in list : No more color available");
            break;
        }
        QColor col = colorsList.at(idx);
        idx++;
        QListWidgetItem *item = createWidgetItem(name,
                                                 col);

        structuresWidget->addItem(item);
        if (isProstate && col!=Qt::green && col!=Qt::blue)
        {
            item->setCheckState(Qt::Checked);
        }
    }
    structuresWidget->setDragDropMode(QAbstractItemView::InternalMove);
    structuresWidget->setDragEnabled(true);
    structuresWidget->setAcceptDrops(true);
    structuresWidget->setDropIndicatorShown(true);
    structuresWidget->setMaximumHeight((20*structuresWidget->count())+5);
    structuresWidget->hide();
    return;
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
    specialities->setDisabled(true);
    for (QListWidget *widget : structuresList)
    {
        widget->setDisabled(true);
    }
    plusButton->setDisabled(true);
    minusButton->setDisabled(true);
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
