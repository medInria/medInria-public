/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "polygonRoiToolBox.h"

#include <itkBinaryContourImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkImageRegionIterator.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkSliceBySliceImageFilter.h>
#include <vtkPolyDataMapper.h>

#include <medAbstractDataFactory.h>
#include <medAbstractImageData.h>
#include <medAbstractImageView.h>
#include <medAbstractProcessLegacy.h>
#include <medAbstractRoi.h>
#include <medDataManager.h>
#include <medPluginManager.h>
#include <medTabbedViewContainers.h>
#include <medTransform.h>
#include <medToolBox.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>
#include <medUtilities.h>
#include <medVtkViewBackend.h>
#include <vtkCleanPolyData.h>
#include <vtkPointData.h>

#include <medViewContainer.h>
#include <medViewContainerManager.h>
#include <medViewContainerSplitter.h>
#include <medViewFactory.h>
#include <polygonEventFilter.h>
#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkCellArray.h>
#include <vtkContourRepresentation.h>
#include <vtkContourWidget.h>
#include <vtkFillHolesFilter.h>
#include <vtkImageActor.h>
#include <vtkImageView2D.h>
#include <vtkMatrix4x4.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkParametricSpline.h>
#include <vtkPolyData.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPolyLine.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <medTableWidgetChooser.h>
#include <vtkWidgetEvent.h>
#include <vtkContourWidget.h>
#include <medIntParameterL.h>

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
    connect(repulsorTool,SIGNAL(toggled(bool)),this,SLOT(activateRepulsor(bool)));

    generateBinaryImage_button = new QPushButton(tr("Save Mask"));
    generateBinaryImage_button->setToolTip("Import the current mask to the non persistent database");
    generateBinaryImage_button->setObjectName(generateBinaryImageButtonName);
    connect(generateBinaryImage_button,SIGNAL(clicked()),this,SLOT(generateAndSaveBinaryImage()));

    currentView = nullptr;

    QHBoxLayout *ButtonLayout0 = new QHBoxLayout();
    layout->addLayout( ButtonLayout0 );
    ButtonLayout0->addWidget(addNewCurve);

    QHBoxLayout *ButtonLayout1 = new QHBoxLayout();
    layout->addLayout( ButtonLayout1 );
    ButtonLayout1->addWidget(interpolate);
    ButtonLayout1->addWidget(repulsorTool);
    ButtonLayout1->addWidget(generateBinaryImage_button);

    tableViewChooser = new medTableWidgetChooser(this, 1, 3, 50);
    // Mandatory : Qt bug ? : Without the lines below, the size of the table View is not as expected
    QSize size = tableViewChooser->sizeHint();
    tableViewChooser->setFixedHeight(size.height()-1);
    tableViewChooser->setFixedWidth(size.width()-1);
    tableViewChooser->setIconSize(QSize(size.height()-1,size.height()-1));

    connect(tableViewChooser, SIGNAL(selected(unsigned int,unsigned int)), this, SLOT(updateTableWidgetView(unsigned int,unsigned int)));
    layout->addWidget(tableViewChooser);


    // How to use
    QLabel *explanation = new QLabel(tr("Define a ROI: choose 'Closed Polygon' and click on the data set.\n")
                                     + tr("Remove a landmark: put the cursor on it and right-click.\n")
                                     );
    explanation->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    explanation->setWordWrap(true);
    layout->addWidget(explanation);

    // buttons initialisation: view has no data
    disableButtons();

}

polygonRoiToolBox::~polygonRoiToolBox()
{
    qDebug()<<"delete tb";
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

void polygonRoiToolBox::updateView()
{
    medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();
    medAbstractImageView *v = qobject_cast<medAbstractImageView*>(view);
    if (view)
    {
        // Toolbox does not work with meshes or vector images
        for (unsigned int i=0; i<v->layersCount(); ++i)
        {
            medAbstractData *data = v->layerData(i);
            if(!data || data->identifier().contains("vtkDataMesh")
                    || data->identifier().contains("itkDataImageVector"))
            {
                handleDisplayError(medAbstractProcessLegacy::DIMENSION_3D);
                return;
            }
        }

        if (currentView != v)
        {
            currentView = v;
            if (viewEventFilter)
                viewEventFilter->updateView(currentView);

            addNewCurve->setEnabled(true);
            connect(currentView, SIGNAL(closed()), this, SLOT(onViewClosed()), Qt::UniqueConnection);
            connect(currentView, SIGNAL(layerRemoved(unsigned int)), this, SLOT(onLayerClosed()), Qt::UniqueConnection);
            updateTableWidgetItems();
            connect(view, SIGNAL(orientationChanged()), this, SLOT(updateTableWidgetItems()), Qt::UniqueConnection);
            connect(view, SIGNAL(orientationChanged()), this, SLOT(manageTick()), Qt::UniqueConnection);
            connect(view, SIGNAL(orientationChanged()), this, SLOT(manageRoisVisibility()), Qt::UniqueConnection);
        }
    }
}

void polygonRoiToolBox::onViewClosed()
{
    medTabbedViewContainers *containers = this->getWorkspace()->tabbedViewContainers();
    if (containers)
    {
        QList<medViewContainer*> containersInTabSelected = containers->containersInTab(containers->currentIndex());
        if ( containersInTabSelected.size() == 0 )
        {
            medAbstractView *viewClosed = qobject_cast<medAbstractView*>(QObject::sender());
            if (viewClosed == currentView)
            {
                currentView = nullptr;
            }
            disableButtons();
        }
    }
}

void polygonRoiToolBox::onLayerClosed()
{
    medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();
    medAbstractImageView *v = qobject_cast<medAbstractImageView*>(view);
    // We enter here only if onLayerClosed has not been called during a view removal
    addNewCurve->blockSignals(true);
    addNewCurve->setChecked(false);
    addNewCurve->blockSignals(false);
    if (v && (v->layersCount() > 0))
    {
        currentView = v;
        if (viewEventFilter)
        {
            viewEventFilter->removeManagers();
        }
        this->clear();
    }

}

void polygonRoiToolBox::clickClosePolygon(bool state)
{
    if (!currentView)
    {
        qDebug()<<"no view in container";
        return;
    }
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;

    enableRepulsorButton(state);
    enableGenerateMaskButton(state);
    enableTableViewChooser(state);
    if (state)
    {

        if (!viewEventFilter)
        {
            viewEventFilter = new polygonEventFilter(currentView);
            connect(viewEventFilter, SIGNAL(enableRepulsor(bool)), this, SLOT(enableRepulsorButton(bool)), Qt::UniqueConnection);
            connect(viewEventFilter, SIGNAL(enableGenerateMask(bool)), this, SLOT(enableGenerateMaskButton(bool)), Qt::UniqueConnection);
            connect(viewEventFilter, SIGNAL(enableViewChooser(bool)), this, SLOT(enableTableViewChooser(bool)), Qt::UniqueConnection);
        }
        viewEventFilter->On();
        medViewContainer* container = nullptr;
        foreach (QUuid uuid, this->getWorkspace()->tabbedViewContainers()->containersSelected())
        {
            container = medViewContainerManager::instance()->container(uuid);
            if(container)
            {
                viewEventFilter->installOnView(container->view());
            }
        }
    }
    else
    {
        viewEventFilter->Off();
    }
}

void polygonRoiToolBox::activateRepulsor(bool state)
{
    if (currentView && viewEventFilter)
        viewEventFilter->activateRepulsor(state);
}

void polygonRoiToolBox::manageTick()
{
 if (viewEventFilter && currentView)
     viewEventFilter->manageTick();
}

void polygonRoiToolBox::manageRoisVisibility()
{
    if (viewEventFilter && currentView)
        viewEventFilter->manageRoisVisibility();
}

void polygonRoiToolBox::updateTableWidgetView(unsigned int row, unsigned int col)
{
    medTabbedViewContainers *containers = this->getWorkspace()->tabbedViewContainers();
    QList<medViewContainer*> containersInTabSelected = containers->containersInTab(containers->currentIndex());
    if (containersInTabSelected.size() != 1)
    {

        qDebug()<<"Operation not permitted";
        return;
    }
    medViewContainer* mainContainer = containersInTabSelected.at(0);
    medAbstractImageView* mainView = dynamic_cast<medAbstractImageView *> (mainContainer->view());
    medAbstractData* data = mainView->layerData(0);
    if (mainView != currentView)
    {
        qDebug()<<"????";
    }


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
            qDebug()<<"ERROR";
            return;
        }
        container->addData(data);
        medAbstractImageView* view = dynamic_cast<medAbstractImageView *> (container->view());

        medTableWidgetItem * item = dynamic_cast<medTableWidgetItem*>(tableViewChooser->selectedItems().at(nbItem));
        view->setOrientation(dynamic_cast<medTableWidgetItem*>(item)->orientation());

        if ( viewEventFilter)
        {
            viewEventFilter->updateAlternativeViews(view, item);
        }
    }
}


void polygonRoiToolBox::updateTableWidgetItems()
{
    medTabbedViewContainers *containers = this->getWorkspace()->tabbedViewContainers();
    if (!containers)
    {
        qDebug()<<"No container.";
        return;
    }
    QList<medViewContainer*> containersInTabSelected = containers->containersInTab(containers->currentIndex());
    if ( containersInTabSelected.size() != 1 )
    {
        //qDebug()<<"Too many container.";
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
            qDebug()<<"Unexpected Case";
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


void polygonRoiToolBox::enableRepulsorButton(bool state)
{
    repulsorTool->setEnabled(state);
}

void polygonRoiToolBox::enableGenerateMaskButton(bool state)
{
    generateBinaryImage_button->setEnabled(state);
}

void polygonRoiToolBox::enableTableViewChooser(bool state)
{
    tableViewChooser->setEnabled(state);
}

void polygonRoiToolBox::interpolateCurve(bool state)
{
    if (!viewEventFilter)
        return;
    viewEventFilter->setEnableInterpolation(state);
}

void polygonRoiToolBox::generateAndSaveBinaryImage()
{
    if (!viewEventFilter)
        return;
    viewEventFilter->generateMask();
}

void polygonRoiToolBox::disableButtons()
{
    repulsorTool->setEnabled(false);
    repulsorTool->setChecked(false);
    generateBinaryImage_button->setEnabled(false);
    tableViewChooser->setEnabled(false);
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
