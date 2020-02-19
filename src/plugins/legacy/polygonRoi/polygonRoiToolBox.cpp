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
    contourObserver = nullptr;
//    cursorState = CURSORSTATE::FIRST_CLICK;
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


//    extractRoiButton = new QPushButton("Extract ROI",this);
//    extractRoiButton->setToolTip("Extract polygon ROI from a current mask in the view.\nBeware, do not use it with a non-binary dataset");
//    extractRoiButton->setObjectName("extractRoiButton");
//    connect(extractRoiButton,SIGNAL(clicked()),this,SLOT(extractROI()));

    repulsorTool = new QPushButton(tr("Repulsor"));
    repulsorTool->setToolTip(tr("Activate repulsor"));
    repulsorTool->setObjectName("repulsorTool");
    repulsorTool->setCheckable(true);
    connect(repulsorTool,SIGNAL(toggled(bool)),this,SLOT(activateRepulsor(bool)));

//    generateBinaryImage_button = new QPushButton(tr("Save Mask"));
//    generateBinaryImage_button->setToolTip("Import the current mask to the non persistent database");
//    generateBinaryImage_button->setObjectName(generateBinaryImageButtonName);
//    connect(generateBinaryImage_button,SIGNAL(clicked()),this,SLOT(generateAndSaveBinaryImage()));

    currentView = nullptr;

    QHBoxLayout *ButtonLayout0 = new QHBoxLayout();
    layout->addLayout( ButtonLayout0 );
    ButtonLayout0->addWidget(addNewCurve);
    ButtonLayout0->addWidget(interpolate);
    ButtonLayout0->addWidget(repulsorTool);

//    QHBoxLayout *ButtonLayout1 = new QHBoxLayout();
//    layout->addLayout( ButtonLayout1 );
//    ButtonLayout1->addWidget(autoClose);
//    ButtonLayout1->addWidget(generateBinaryImage_button);

    tableViewChooser = new medTableWidgetChooser(this, 1, 3, 50);
    // Mandatory : Qt bug ? : Without the lines below, the size of the table View is not as expected
    QSize size = tableViewChooser->sizeHint();
    tableViewChooser->setFixedHeight(size.height()-1);
    tableViewChooser->setFixedWidth(size.width()-1);
    tableViewChooser->setIconSize(QSize(size.height()-1,size.height()-1));

    connect(tableViewChooser, SIGNAL(selected(unsigned int,unsigned int)), this, SLOT(updateTableWidgetView(unsigned int,unsigned int)));
    layout->addWidget(tableViewChooser);

    hashViewObserver = new QHash<medAbstractView*,contourWidgetListener*>();

    // How to use
    QLabel *explanation = new QLabel(tr("Define a ROI: choose 'Closed Polygon' and click on the data set.\n")
                                     + tr("Remove a landmark: put the cursor on it and SUPPR.\n")
                                     + tr("Undo action: CTRL/CMD + z.\n")
                                     + tr("Redo action: CTRL/CMD + y.\n")
                                     + tr("Copy ROIs in current slice: CTRL/CMD + c.\n")
                                     + tr("Paste ROIs: CTRL/CMD + v.\n")
                                     );
    explanation->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    explanation->setWordWrap(true);
    layout->addWidget(explanation);

    //shortcuts
    undo_shortcut  = new QShortcut(QKeySequence(tr("Ctrl+z", "Undo polygon manipulation")),this);
    redo_shortcut  = new QShortcut(QKeySequence(tr("Ctrl+y", "Redo polygon manipulation")),this);
    copy_shortcut  = new QShortcut(QKeySequence(tr("Ctrl+c", "Copy polygons in current slice")),this);
    paste_shortcut = new QShortcut(QKeySequence(tr("Ctrl+v", "Paste polygons")),this);

    //Shortcuts
    connect(undo_shortcut,  SIGNAL(activated()), this, SLOT(undo()));
    connect(redo_shortcut,  SIGNAL(activated()), this, SLOT(redo()));
    connect(copy_shortcut,  SIGNAL(activated()), this, SLOT(copyContours()));
    connect(paste_shortcut, SIGNAL(activated()), this, SLOT(pasteContours()));

    // buttons initialisation: view has no data
    disableButtons();

    // Add Roi Management Toolbox
//    roiManagementToolBox = medToolBoxFactory::instance()->createToolBox("medRoiManagementToolBox");
//    roiManagementToolBox->header()->hide();
//    this->addWidget(roiManagementToolBox);

}

polygonRoiToolBox::~polygonRoiToolBox()
{
    delete hashViewObserver;
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
    if (!m_maskData)
    {
        generateBinaryImage();
    }
    return m_maskData; // return output data
}

void polygonRoiToolBox::updateView()
{
    qDebug()<<"polygonRoiToolBox::updateView ";
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

//            if(interactorStyleRepulsor != nullptr)
//            {
//                interactorStyleRepulsor->SetCurrentView(currentView);
//            }


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
    medAbstractView *viewClosed = qobject_cast<medAbstractView*>(QObject::sender());
    hashViewObserver->remove(viewClosed);

    if (viewClosed == currentView)
    {
        currentView = nullptr;
    }
    m_maskData = nullptr;

    disableButtons();

    //interactorStyleRepulsor = nullptr;
}

void polygonRoiToolBox::onLayerClosed()
{
    medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();
    medAbstractImageView *v = qobject_cast<medAbstractImageView*>(view);

    // We enter here only if onLayerClosed has not been called during a view removal
    if (v && (v->layersCount() > 0))
    {
        currentView = v;
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

    if (state)
    {

        if (!viewEventFilter)
        {
            viewEventFilter = new polygonEventFilter(currentView);
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
//        setCursorState(cursorState);

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

QList<medSeriesOfRoi*> * polygonRoiToolBox::getListOfView(medAbstractView *view)
{
//    return medRoiManager::instance()->getSeriesOfRoi()->value(view);
}

void polygonRoiToolBox::copyContours()
{
//    medRoiManager * manager = medRoiManager::instance();
//    manager->setCurrentView(currentView);
//    manager->copy();
}
void polygonRoiToolBox::pasteContours()
{
//    medRoiManager * manager = medRoiManager::instance();
//    manager->setCurrentView(currentView);
//    manager->paste();
}

void polygonRoiToolBox::undo()
{
    if (!currentView)
    {
        return;
    }
//    medRoiManager::instance()->setCurrentView(currentView);
//    medRoiManager::instance()->undo();
//    currentView->render();
}

void polygonRoiToolBox::redo()
{
    if (!currentView)
    {
        return;
    }
//    medRoiManager::instance()->setCurrentView(currentView);
//    medRoiManager::instance()->redo();
    //    currentView->render();
}

void polygonRoiToolBox::manageTick()
{
 if (viewEventFilter)
     viewEventFilter->manageTick();
}

void polygonRoiToolBox::manageRoisVisibility()
{
    if (viewEventFilter)
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

void polygonRoiToolBox::interpolateCurve(bool state)
{
    if (!viewEventFilter)
        return;
    viewEventFilter->setEnableInterpolation(state);
}

void polygonRoiToolBox::generateBinaryImage()
{
//    if (currentView)
//    {
//        vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;

//        QList<medSeriesOfRoi*> *listROI = getListOfView(currentView);
//        if (!listROI || listROI->isEmpty())
//        {
//            return; // check if ROI or not
//        }

//        ListRois *list = listROI->at(0)->getListOfRois();

//        QList<QPair<vtkPolyData *,PlaneIndexSlicePair> > listPolyData = QList<QPair<vtkPolyData *,PlaneIndexSlicePair> >();

//        int orientation = view2d->GetViewOrientation();

//        for(int i=0; i<list->size(); i++)
//        {
//            polygonRoi *polyRoi = dynamic_cast<polygonRoi*>(list->at(i));  // TODO : need to test if the cast goes well we cannot be sure that the Rois are polygonRoi
//            vtkContourWidget *contour =  polyRoi->getContour();
//            unsigned int orientationOfRoi = polyRoi->getOrientation();
//            unsigned int idSlice = polyRoi->getIdSlice();
//            if (viewsPlaneIndex.empty())
//            {
//                return;
//            }
//            unsigned char planeIndex = viewsPlaneIndex.value(currentView)->at(orientationOfRoi);
//            vtkContourRepresentation *contourRep = contour->GetContourRepresentation();
//            view2d->SetViewOrientation(orientationOfRoi); // we set the view Orientation to the orientation of the ROI, to retreive the polyData with world coordinates based on the display from the orientation.
//            listPolyData.append(QPair<vtkPolyData*,PlaneIndexSlicePair>(contourRep->GetContourRepresentationAsPolyData(),PlaneIndexSlicePair(idSlice,planeIndex)));
//        }
//        view2d->SetViewOrientation(orientation);

//        QList<QPair<vtkPolygon*,PlaneIndexSlicePair> > listPolygon = createImagePolygons(listPolyData);
//        binaryImageFromPolygon(listPolygon);
//    }
}

void polygonRoiToolBox::generateAndSaveBinaryImage()
{
    generateBinaryImage();
    medDataManager::instance()->importData(m_maskData, false);
}

QList<QPair<vtkPolygon*,polygonRoiToolBox::PlaneIndexSlicePair> > polygonRoiToolBox::createImagePolygons(QList<QPair<vtkPolyData*,PlaneIndexSlicePair> > &listPoly)
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;
    QList<QPair<vtkPolygon*,PlaneIndexSlicePair> > listPolygon = QList<QPair<vtkPolygon*,PlaneIndexSlicePair> >();

    for(int i=0; i<listPoly.size(); i++)
    {
        vtkPolygon *polygon = vtkPolygon::New();
        vtkPoints *points = vtkPoints::New();

        const int nb = listPoly.at(i).first->GetNumberOfPoints();

        vtkIdType ids[1000];
        double imagePreviousPoint[3] ={0,0,0};
        unsigned int nbPoint = 0;

        unsigned int x1=0, y1=0, z1=0;
        switch (listPoly.at(i).second.second)
        {
            case 0 :
            {
                x1=1;
                y1=2;
                z1=0;
                break;
            }
            case 1 :
            {
                x1=0;
                y1=2;
                z1=1;
                break;
            }
            case 2 :
            {
                x1=0;
                y1=1;
                z1=2;
                break;
            }
        }

        for(int j=0; j<nb; j++)
        {
            double *point = listPoly.at(i).first->GetPoint(j);

            int imagePoint[3];
            double imagePointDouble[3];

            view2d->GetImageCoordinatesFromWorldCoordinates(point,imagePoint);

            imagePointDouble[x1]= (double)imagePoint[x1];
            imagePointDouble[y1]= (double)imagePoint[y1];

            imagePointDouble[z1]= (double)listPoly.at(i).second.first;

            if (imagePointDouble[x1] == imagePreviousPoint[x1] && imagePointDouble[y1] == imagePreviousPoint[y1] && imagePointDouble[z1] == imagePreviousPoint[z1])
            {
                continue;
            }

            points->InsertNextPoint(imagePointDouble);
            ids[nbPoint]=nbPoint;
            nbPoint++;

            imagePreviousPoint[x1] = imagePointDouble[x1];
            imagePreviousPoint[y1] = imagePointDouble[y1];
            imagePreviousPoint[z1] = imagePointDouble[z1];
        }

        polygon->Initialize(points->GetNumberOfPoints(), ids, points);

        listPolygon.append(QPair<vtkPolygon*,PlaneIndexSlicePair>(polygon,listPoly.at(i).second));
    }

    return listPolygon;
}

void polygonRoiToolBox::binaryImageFromPolygon(QList<QPair<vtkPolygon*,PlaneIndexSlicePair> > polys)
{
    m_maskData = medAbstractDataFactory::instance()->create( "itkDataImageUChar3" );
    medAbstractImageData *inputData = qobject_cast<medAbstractImageData*>(currentView->layerData(currentView->currentLayer()));// TODO : currentlayer looks dangerous
    if (!inputData)
    {
        qDebug()<<"polygonRoiToolBox::binaryImageFromPolygon data error.";
        return;
    }
    initializeMaskData(inputData,m_maskData);
    UChar3ImageType::Pointer m_itkMask = dynamic_cast<UChar3ImageType*>( reinterpret_cast<itk::Object*>(m_maskData->data()) );

    for(int k=0; k<polys.size(); k++)
    {
        double bounds[6];
        polys[k].first->GetBounds(bounds);
        unsigned int x=0, y=0, z=0;
        unsigned int bx=0, by=0;
        unsigned int dimX=0, dimY=0;
        switch (polys[k].second.second)
        {
            case 0 :
            {
                dimX = inputData->yDimension();
                dimY = inputData->zDimension();
                x = 1;
                y = 2;
                z = 0;
                bx = 2;
                by = 4;
                break;
            }
            case 1 :
            {
                dimX = inputData->xDimension();
                dimY = inputData->zDimension();
                x = 0;
                y = 2;
                z = 1;
                bx = 0;
                by = 4;
                break;
            }
            case 2 :
            {
                dimX = inputData->xDimension();
                dimY = inputData->yDimension();
                x = 0;
                y = 1;
                z = 2;
                bx = 0;
                by = 2;
                break;
            }
        }

        // Qt rasterization
        int nbPoints = polys[k].first->GetPoints()->GetNumberOfPoints();
        vtkPoints *pointsArray = polys[k].first->GetPoints();
        QPolygonF polygon;
        for(int i=0; i<nbPoints; i++)
        {
            polygon << QPointF(pointsArray->GetPoint(i)[x], pointsArray->GetPoint(i)[y]);
        }

        QPainterPath path;
        path.addPolygon(polygon);

        QBrush brush;
        brush.setColor(Qt::white);
        brush.setStyle(Qt::SolidPattern);

        QImage imagePolygon(dimX,dimY,QImage::Format_RGB16);
        imagePolygon.fill(Qt::black);
        QPainter painter;
        painter.begin(&imagePolygon);
        painter.fillPath(path,brush);

        for(int i=bounds[bx]; i<=bounds[bx+1]; i++)
        {
            for(int j=bounds[by]; j<=bounds[by+1]; j++)
            {
                QRgb val = imagePolygon.pixel(i,j);

                if (val==qRgb(255,255,255))
                {
                    UChar3ImageType::IndexType index;
                    index[x]=i;
                    index[y]=j;
                    index[z]=polys[k].second.first;
                    m_itkMask->SetPixel(index,1);
                }
            }
        }
    }

    medUtilities::setDerivedMetaData(m_maskData, inputData, "polygon roi");
}

void polygonRoiToolBox::initializeMaskData( medAbstractData *imageData, medAbstractData *maskData )
{
    UChar3ImageType::Pointer mask = UChar3ImageType::New();

    Q_ASSERT(mask->GetImageDimension() == 3);

    medAbstractImageData *mImage = qobject_cast<medAbstractImageData*>(imageData);
    Q_ASSERT(mImage);

    UChar3ImageType::RegionType region;
    region.SetSize(0, ( mImage->Dimension() > 0 ? mImage->xDimension() : 1 ) );
    region.SetSize(1, ( mImage->Dimension() > 1 ? mImage->yDimension() : 1 ) );
    region.SetSize(2, ( mImage->Dimension() > 2 ? mImage->zDimension() : 1 ) );

    UChar3ImageType::DirectionType direction;
    UChar3ImageType::SpacingType spacing;
    UChar3ImageType::PointType origin;

    direction.Fill(0);
    spacing.Fill(0);
    origin.Fill(0);
    for (unsigned int i = 0; i < mask->GetImageDimension(); ++i)
    {
        direction(i,i) = 1;
    }

    unsigned int maxIndex = std::min<unsigned int>(mask->GetImageDimension(),mImage->Dimension());

    switch (mImage->Dimension())
    {
        case 2:
        {
            itk::ImageBase <2> * imageDataOb = dynamic_cast<itk::ImageBase <2> *>( reinterpret_cast<itk::Object*>(imageData->data()) );

            for (unsigned int i = 0; i < maxIndex; ++i)
            {
                for (unsigned int j = 0; j < maxIndex; ++j)
                {
                    direction(i,j) = imageDataOb->GetDirection()(i,j);
                }

                spacing[i] = imageDataOb->GetSpacing()[i];
                origin[i] = imageDataOb->GetOrigin()[i];
            }

            break;
        }

        case 4:
        {
            itk::ImageBase <4> * imageDataOb = dynamic_cast<itk::ImageBase <4> *>( reinterpret_cast<itk::Object*>(imageData->data()) );

            for (unsigned int i = 0; i < maxIndex; ++i)
            {
                for (unsigned int j = 0; j < maxIndex; ++j)
                {
                    direction(i,j) = imageDataOb->GetDirection()(i,j);
                }

                spacing[i] = imageDataOb->GetSpacing()[i];
                origin[i] = imageDataOb->GetOrigin()[i];
            }

            break;
        }

        case 3:
        default:
        {
            itk::ImageBase <3> *imageDataOb = dynamic_cast<itk::ImageBase <3> *>( reinterpret_cast<itk::Object*>(imageData->data()) );

            for (unsigned int i = 0; i < maxIndex; ++i)
            {
                for (unsigned int j = 0; j < maxIndex; ++j)
                {
                    direction(i,j) = imageDataOb->GetDirection()(i,j);
                }

                spacing[i] = imageDataOb->GetSpacing()[i];
                origin[i] = imageDataOb->GetOrigin()[i];
            }

            break;
        }
    }

    mask->SetOrigin(origin);
    mask->SetDirection(direction);
    mask->SetSpacing(spacing);
    mask->SetLargestPossibleRegion(region);
    mask->SetBufferedRegion(region);
    mask->Allocate();
    mask->FillBuffer(0);

    maskData->setData((QObject*)(mask.GetPointer()));
}

int polygonRoiToolBox::computePlaneIndex()
{
    typedef  UChar3ImageType::DirectionType::InternalMatrixType::element_type ElemType;
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;
    int planeIndex = -1;

    if (currentView->is2D())
    {
        const QVector3D vpn = currentView->viewPlaneNormal();

        vnl_vector_fixed<ElemType, 3> vecVpn(vpn.x(), vpn.y(), vpn.z() );
        vtkMatrix4x4 *direction = view2d->GetOrientationMatrix();
        double absDotProductMax = 0;
        planeIndex = 0;

        for (unsigned int i = 0; i < 3; ++i)
        {
            double dotProduct = 0;
            for (unsigned int j = 0; j < 3; ++j)
            {
                dotProduct += direction->GetElement(j,i) * vecVpn[j];
            }

            if (fabs(dotProduct) > absDotProductMax)
            {
                planeIndex = i;
                absDotProductMax = fabs(dotProduct);
            }
        }
    }
    return planeIndex;
}

void polygonRoiToolBox::disableButtons()
{
    //addNewCurve->setEnabled(false);
    //addNewCurve->setChecked(false);
//    repulsorTool->setEnabled(false);
//    repulsorTool->setChecked(false);
//    generateBinaryImage_button->setEnabled(false);
//    interpolate->setEnabled(false);
//    extractRoiButton->setEnabled(false);
    //tableViewChooser->setEnabled(false);
    //splitViewButton->setEnabled(false);
}

void polygonRoiToolBox::clear()
{
    medToolBox::clear();

    if(currentView)
    {
        // We need to update the medSliderL
        roiManagementToolBox->updateView();

        ListRois *list;
//        QList<medSeriesOfRoi*> *listSeries = medRoiManager::instance()->getSeriesOfRoi()->value(currentView);

//        if(listSeries != nullptr
//                && listSeries->count() > 0
//                && listSeries->at(0)->getListOfRois()->size() != 0)
//        {
//            // Get list of Rois
//            list = listSeries->at(0)->getListOfRois();
//            DeleteSeveralRoisCommand * deleteCommand = new DeleteSeveralRoisCommand(currentView,*list,"Polygon rois","deleteSeveralRois");
//            medRoiManager::instance()->addToUndoRedoStack(deleteCommand);
//        }

        currentView = nullptr;
    }

    m_maskData = nullptr;
    disableButtons();
}

void polygonRoiToolBox::buttonsStateWhenROI()
{
    addNewCurve->setEnabled(true);
//    extractRoiButton->setEnabled(true);
//    repulsorTool->setEnabled(true);
    interpolate->setEnabled(true);
//    splitViewButton->setEnabled(true);
//    generateBinaryImage_button->setEnabled(true);
}

template <class ImageType> UChar3ImageType::Pointer cast(medAbstractData *input)
{
    typedef itk::CastImageFilter< ImageType, UChar3ImageType > CasterType;
    typename CasterType::Pointer caster = CasterType::New();
    caster->SetInput(dynamic_cast< ImageType*>((itk::Object*)(input->data())));
    caster->Update();
    return caster->GetOutput();
}

UChar3ImageType::Pointer extract2dContour(UChar3ImageType::Pointer input)
{
    typedef itk::Image <unsigned char, 2> Mask2dType;

    typedef itk::MinimumMaximumImageCalculator <UChar3ImageType> ImageCalculatorFilterType;
    ImageCalculatorFilterType::Pointer imageCalculatorFilter
            = ImageCalculatorFilterType::New ();
    imageCalculatorFilter->SetImage(input);
    imageCalculatorFilter->Compute();

    // Check if the dataset is a mask
    itk::ImageRegionIterator<UChar3ImageType> it(input, input->GetRequestedRegion() );
    while(!it.IsAtEnd())
    {
        if ((it.Get() != imageCalculatorFilter->GetMinimum())
                && (it.Get() != imageCalculatorFilter->GetMaximum()))
        {
            // Odd values have been found. The dataset is not a mask
            return nullptr;
        }
        ++it;
    }

    typedef itk::BinaryContourImageFilter <Mask2dType, Mask2dType> binaryContourImageFilterType;
    binaryContourImageFilterType::Pointer contourFilter = binaryContourImageFilterType::New ();
    contourFilter->SetBackgroundValue(imageCalculatorFilter->GetMinimum());
    contourFilter->SetForegroundValue(imageCalculatorFilter->GetMaximum());

    typedef itk::SliceBySliceImageFilter< UChar3ImageType, UChar3ImageType> SbSFilterType;
    SbSFilterType::Pointer sliceBySliceFilter = SbSFilterType::New();
    sliceBySliceFilter->SetFilter( contourFilter );
    sliceBySliceFilter->SetInput( input );
    sliceBySliceFilter->Update();
    return sliceBySliceFilter->GetOutput();
}

UChar3ImageType::Pointer polygonRoiToolBox::castToUChar3(medAbstractImageData *input)
{
    if (!input)
    {
        displayMessageError("No input found!");
        return nullptr;
    }

    QString id = input->identifier();

    UChar3ImageType::Pointer img = UChar3ImageType::New();

    if ( id == "itkDataImageChar3" )
    {
        img = cast<itk::Image <char,3> >(input);
    }
    else if ( id == "itkDataImageUChar3" )
    {
        img = dynamic_cast< UChar3ImageType*>((itk::Object*)(input->data()));
    }
    else if ( id == "itkDataImageShort3" )
    {
        img = cast<itk::Image <short,3> >(input);
    }
    else if ( id == "itkDataImageUShort3" )
    {
        img = cast<itk::Image <unsigned short,3> >(input);
    }
    else if ( id == "itkDataImageInt3" )
    {
        img = cast<itk::Image <int,3> >(input);
    }
    else if ( id == "itkDataImageUInt3" )
    {
        img = cast<itk::Image <unsigned int,3> >(input);
    }
    else if ( id == "itkDataImageLong3" )
    {
        img = cast<itk::Image <long,3> >(input);
    }
    else if ( id == "itkDataImageULong3" )
    {
        img = cast<itk::Image <unsigned long,3> >(input);
    }
    else if ( id == "itkDataImageFloat3" )
    {
        img = cast<itk::Image <float,3> >(input);
    }
    else if ( id == "itkDataImageDouble3" )
    {
        img = cast<itk::Image <double,3> >(input);
    }
    else
    {
        displayMessageError("Pixel type not yet implemented");
        img =  nullptr;
    }

    return img;
}

void polygonRoiToolBox::showExtractROIWarningPopUp()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Extract ROI error");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("An error occured, please check that your current layer is a mask.");
    msgBox.exec();
}

void polygonRoiToolBox::extractROI()
{
//    this->setToolBoxOnWaitStatusForNonRunnableProcess();

//    medAbstractImageData *input = qobject_cast<medAbstractImageData*>(currentView->layerData(currentView->currentLayer()));

//    UChar3ImageType::Pointer img = castToUChar3(input);

//    if (img)
//    {
//        UChar3ImageType::Pointer contourImage = extract2dContour(img);

//        if (contourImage)
//        {
//            UChar3ImageType::SizeType size = contourImage->GetLargestPossibleRegion().GetSize();
//            UChar3ImageType::IndexType px;

//            vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;

//            vtkSmartPointer<vtkPoints>  bufferPoints = vtkPoints::New();
//            for (unsigned int sl=0 ; sl<size[2] ; sl++) // go through slices
//            {
//                bufferPoints->Reset();
//                bool isEmpty = true;

//                for (unsigned int y=0 ; y<size[1] ; y++)
//                {
//                    for (unsigned int x=0 ; x<size[0] ; x++)
//                    {
//                        px[0] = x;
//                        px[1] = y;
//                        px[2] = sl;
//                        if (contourImage->GetPixel(px) != 0)
//                        {
//                            addPointToBuffer(view2d, bufferPoints, px);
//                            isEmpty = false;
//                            break;
//                        }
//                    }
//                    if (!isEmpty)
//                    {
//                        break;
//                    }
//                }
//                if (!isEmpty)
//                {
//                    // Contour indices are added into bufferPoints
//                    getContourIndices(view2d, contourImage, px, bufferPoints);

//                    vtkPoints *polyPoints = vtkPoints::New();
//                    polyPoints->DeepCopy(bufferPoints);

//                    // Create a cell array to connect the points into meaningful geometry
//                    int numPts = polyPoints->GetNumberOfPoints();
//                    vtkIdType* vertexIndices = new vtkIdType[numPts+1];
//                    for (int i = 0; i < numPts; i++)
//                    {
//                        vertexIndices[i] = static_cast<vtkIdType>(i);
//                    }
//                    // Set the last vertex to 0; thus closing the circle.
//                    vertexIndices[numPts] = 0;
//                    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
//                    lines->InsertNextCell(numPts+1, vertexIndices);

//                    vtkPolyData *poly = vtkPolyData::New();
//                    poly->SetPoints(polyPoints);
//                    poly->SetLines(lines);

//                    // Points decimation
//                    int nbPointsPoly = poly->GetNumberOfPoints();
//                    vtkPoints *points = poly->GetPoints();
//                    vtkPoints *decimatedPoints = vtkPoints::New();

//                    for (int k = nbPointsPoly-1; k >= 0; k--)
//                    {
//                        if (k % HANDLE_PERIOD == 0) //one handle every HANDLE_PERIOD points
//                        {
//                            decimatedPoints->InsertNextPoint(points->GetPoint(k));
//                        }
//                    }
//                    poly->SetPoints(decimatedPoints);

//                    polygonRoi *polyRoi = new polygonRoi(view2d);
//                    vtkContourWidget *contour = polyRoi->getContour();
//                    contour->Initialize(poly);

//                    vtkContourRepresentation *contourRep = contour->GetContourRepresentation();
//                    contourRep->SetClosedLoop(1);

//                    if (!viewsPlaneIndex.contains(currentView))
//                    {
//                        QList<int> *planeIndexes = new QList<int>();
//                        for (int i = 0; i < 3; i++)
//                        {
//                            planeIndexes->append(0); // fill the list with 0
//                        }
//                        viewsPlaneIndex.insert(currentView,planeIndexes);
//                    }

//                    polyRoi->setIdSlice(sl);
//                    polyRoi->setOrientation(view2d->GetViewOrientation());
//                    polyRoi->setMasterRoi(false);
//                    polyRoi->forceVisibilityOff();

//                    // Save PlaneIndex for this view and orientation
//                    AddRoiCommand *command = new AddRoiCommand(currentView, polyRoi,"Polygon rois","NewRois");
//                    medRoiManager::instance()->addToUndoRedoStack(command);
//                    QList<int> *planeIndexes= viewsPlaneIndex.value(currentView);
//                    planeIndexes->replace(view2d->GetViewOrientation(),computePlaneIndex());

//                    buttonsStateWhenROI();
//                }
//            }
//        }
//        else
//        {
//            showExtractROIWarningPopUp();
//        }
//    }

//    this->setToolBoxOnReadyToUse();
}

void polygonRoiToolBox::addPointToBuffer(vtkImageView2D *view2d, vtkSmartPointer<vtkPoints> bufferPoints, itk::ImageBase<3>::IndexType point)
{
    double worldPoint[3];
    int displayPoint[3];
    displayPoint[0] = point[0];
    displayPoint[1] = point[1];
    displayPoint[2] = point[2];
    view2d->GetWorldCoordinatesFromImageCoordinates(displayPoint, worldPoint);
    bufferPoints->InsertNextPoint(worldPoint[0], worldPoint[1], worldPoint[2]);
}

void polygonRoiToolBox::getContourIndices(vtkImageView2D *view2d, UChar3ImageType::Pointer contourImage, UChar3ImageType::IndexType px, vtkSmartPointer<vtkPoints> bufferPoints)
{
    UChar3ImageType::IndexType px2 = px;
    do
    {
        UChar3ImageType::IndexType pxLeft, pxBottomLeft, pxBottom, pxBottomRight, pxRight, pxTopRight, pxTop, pxTopLeft;
        pxLeft = pxBottomLeft = pxBottom = pxBottomRight = pxRight = pxTopRight = pxTop = pxTopLeft = px2;
        pxLeft[0] = px2[0]-1;

        pxBottomLeft[0] = px2[0]-1;
        pxBottomLeft[1] = px2[1]+1;

        pxBottom[1] = px2[1]+1;

        pxBottomRight[0] = px2[0]+1;
        pxBottomRight[1] = px2[1]+1;

        pxRight[0] = px2[0]+1;

        pxTopRight[0] = px2[0]+1;
        pxTopRight[1] = px2[1]-1;

        pxTop[1] = px2[1]-1;

        pxTopLeft[0] = px2[0]-1;
        pxTopLeft[1] = px2[1]-1;

        if (contourImage->GetPixel(pxLeft) != 0)
        {
            px2 = pxLeft;
        }
        else if (contourImage->GetPixel(pxBottomLeft) != 0)
        {
            px2 = pxBottomLeft;
        }
        else if (contourImage->GetPixel(pxBottom) != 0)
        {
            px2 = pxBottom;
        }
        else if (contourImage->GetPixel(pxBottomRight) != 0)
        {
            px2 = pxBottomRight;
        }
        else if (contourImage->GetPixel(pxRight) != 0)
        {
            px2 = pxRight;
        }
        else if (contourImage->GetPixel(pxTopRight) != 0)
        {
            px2 = pxTopRight;
        }
        else if (contourImage->GetPixel(pxTop) != 0)
        {
            px2 = pxTop;
        }
        else if (contourImage->GetPixel(pxTopLeft) != 0)
        {
            px2 = pxTopLeft;
        }
        else
            break;

        addPointToBuffer(view2d, bufferPoints, px2);
        contourImage->SetPixel(px2, 0);
    }
    while (px2 != px);
}
