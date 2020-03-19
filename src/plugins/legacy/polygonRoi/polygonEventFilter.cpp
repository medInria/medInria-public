/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "polygonEventFilter.h"

#include <float.h>

// medInria
#include <medAbstractImageData.h>
#include <medAbstractDataFactory.h>
#include <medContours.h>
#include <medDataManager.h>
#include <medIntParameterL.h>
#include <medUtilities.h>
#include <medVtkViewBackend.h>
#include <vtkMetaSurfaceMesh.h>

// Qt
#include <QGuiApplication>
#include <QScreen>

// vtk
#include <vtkAppendPolyData.h>
#include <vtkRenderWindowInteractor.h>


polygonEventFilter::polygonEventFilter(medAbstractImageView *view) :
        medViewEventFilter(), currentView(view), cursorState(CURSORSTATE::CS_MOUSE_EVENT), isRepulsorActivated(false)
{
    colorList = QList<QColor>({
        Qt::green,
        Qt::blue,
        Qt::yellow,
        Qt::red,
        Qt::cyan,
        Qt::gray,
        Qt::white,
        Qt::black
    });

    // interactors
    interactorStyleRepulsor = vtkInriaInteractorStylePolygonRepulsor::New();
}

polygonEventFilter::~polygonEventFilter()
{
    if (interactorStyleRepulsor)
        interactorStyleRepulsor->Delete();
    for (medTagRoiManager *manager: managers)
    {
        delete manager;
    }
    managers.clear();
}

void polygonEventFilter::updateView(medAbstractImageView *view)
{
    currentView = view;
}

void polygonEventFilter::reset()
{
    for (medTagRoiManager *manager: managers)
    {
        delete manager;
    }
    managers.clear();
    cursorState = CURSORSTATE::CS_MOUSE_EVENT;
    activateRepulsor(false);
    if (interactorStyleRepulsor)
    {
        interactorStyleRepulsor->Delete();
        interactorStyleRepulsor = nullptr;
    }
}

bool polygonEventFilter::mouseReleaseEvent(medAbstractView *view, QMouseEvent *mouseEvent)
{
    medAbstractImageView *v = dynamic_cast<medAbstractImageView *> (view);
    if ( !currentView )
    {
        return false;
    }
    if (currentView != v)
    {
        return updateMainViewOnChosenSlice(v, mouseEvent);
    }
    if (isRepulsorActivated)
    {
        if (interactorStyleRepulsor != nullptr)
        {
            interactorStyleRepulsor->GetManager()->SetMasterRoi(true);
            interactorStyleRepulsor->GetManager()->interpolateIfNeeded();
        }
    }

    return false;
}

bool polygonEventFilter::mousePressEvent(medAbstractView * view, QMouseEvent *mouseEvent)
{
    medAbstractImageView *v = dynamic_cast<medAbstractImageView *> (view);
    if ( !currentView )
    {
        return false;
    }
    if (currentView != v)
        return false;

    if (mouseEvent->button()==Qt::LeftButton)
    {
        return leftButtonBehaviour(view, mouseEvent);
    }
    else if (mouseEvent->button()==Qt::RightButton)
    {
        return rightButtonBehaviour(view, mouseEvent);
    }
    return false;
}

bool polygonEventFilter::updateMainViewOnChosenSlice(medAbstractImageView *view, QMouseEvent *mouseEvent)
{
    QPointF position;
    position.setX(mouseEvent->x()*QGuiApplication::screenAt(mouseEvent->pos())->devicePixelRatio());
    position.setY(mouseEvent->y()*QGuiApplication::screenAt(mouseEvent->pos())->devicePixelRatio());
    QVector3D clickPosition = view->mapDisplayToWorldCoordinates(position);
    int slice = findClosestSliceFromMouseClick(clickPosition);
    if ( slice == -1 )
        return false;
    vtkImageView2D *view2D =  static_cast<medVtkViewBackend*>(currentView->backend())->view2D;
    view2D->SetSlice(slice);
    view2D->Render();
    return false;
}

bool polygonEventFilter::leftButtonBehaviour(medAbstractView *view, QMouseEvent *mouseEvent)
{
    if (isRepulsorActivated)
    {
        cursorState = CURSORSTATE::CS_REPULSOR;
    }

    if (cursorState != CURSORSTATE::CS_NONE && !managers.empty())
        setToolboxButtonsState(true);

    switch (cursorState)
    {
    case CURSORSTATE::CS_MOUSE_EVENT:
    case CURSORSTATE::CS_SLICE_CHANGED:
    {
        return addPointInContourWithLabel(mouseEvent);
    }
    case CURSORSTATE::CS_CONTINUE:
    {
        for (medTagRoiManager *manager : managers)
        {
            manager->setContourEnabled(true);
            polygonRoi *roi = manager->roiOpenInSlice();
            if ( roi != nullptr)
            {
                roi->getContour()->SetWidgetState(vtkContourWidget::Define);

                roi->getContour()->InvokeEvent(vtkCommand::PlacePointEvent);
                cursorState = CURSORSTATE::CS_NONE;
                return false;
            }
        }
        return addPointInContourWithLabel(mouseEvent);
    }
    case CURSORSTATE::CS_REPULSOR:
    {
        double mousePos[2];
        mousePos[0] = mouseEvent->x()*QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();
        mousePos[1] = (currentView->viewWidget()->height()-mouseEvent->y()-1)*QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();
        medTagRoiManager * manager = closestManagerInSlice(mousePos);
        if (!manager)
        {
            return false;

        }
        if (interactorStyleRepulsor == nullptr)
        {
            interactorStyleRepulsor = vtkInriaInteractorStylePolygonRepulsor::New();
        }
        interactorStyleRepulsor->SetCurrentView(currentView);
        interactorStyleRepulsor->SetManager(manager);
        return false;
    }
    case CURSORSTATE::CS_NONE:
    default:
    {
        return false;
    }
    }
    return false;
}


bool polygonEventFilter::rightButtonBehaviour(medAbstractView *view, QMouseEvent *mouseEvent)
{
    QMenu mainMenu;
    QList<QColor> colorsToExclude;
    medTagRoiManager * closestManager = nullptr;
    double minDist = DBL_MAX;
    double dist = 0.;
    double mousePos[2];
    mousePos[0] = mouseEvent->x()*QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();
    mousePos[1] = (currentView->viewWidget()->height()-mouseEvent->y()-1)*QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();
    for (medTagRoiManager *manager : managers)
    {
        if ( manager->existingRoiInSlice() )
        {
            colorsToExclude.append(manager->getColor());

            dist = manager->getMinimumDistanceFromNodesToEventPosition(mousePos);
            if ( dist < minDist )
            {
                minDist = dist;
                closestManager = manager;
            }
        }
    }

    QList<QColor> colors = updateColors(colorsToExclude);
    QMenu *labelMenu = updateLabelMenu(colors);
    QMenu *roiManagementMenu = new QMenu("Remove selected : ");
    QMenu *saveMenu = new QMenu("Save Segmentation as : ");
    if (closestManager && closestManager->getMinimumDistanceFromNodesToEventPosition(mousePos) < 10 )
    {
        closestManager->select(true);
        mainMenu.addMenu(roiManagementMenu);
        QAction *deleteOneNodeAction = new QAction("Node", roiManagementMenu);
        connect(deleteOneNodeAction, &QAction::triggered, [=](){
            deleteNode(closestManager, mousePos[0], mousePos[1]);
        });
        roiManagementMenu->addAction(deleteOneNodeAction);

        QAction *deleteContourAction = new QAction("Contour", roiManagementMenu);
        connect(deleteContourAction, &QAction::triggered, [=](){
            deleteContour(closestManager);
        });
        roiManagementMenu->addAction(deleteContourAction);

        QAction *deleteLabelAction = new QAction("Label", roiManagementMenu);
        connect(deleteLabelAction, &QAction::triggered, [=](){
           deleteLabel(closestManager);
        });
        roiManagementMenu->addAction(deleteLabelAction);

        QAction *saveMaskAction = new QAction("Mask", &mainMenu);
        connect(saveMaskAction, &QAction::triggered, [=](){
            saveMask(closestManager);
        });
        saveMenu->addAction(saveMaskAction);

        QAction *saveContourAction = new QAction("Contour", &mainMenu);
        connect(saveContourAction, &QAction::triggered, [=](){
            saveContour(closestManager);
        });
        saveMenu->addAction(saveContourAction);

        mainMenu.addMenu(roiManagementMenu);
        mainMenu.addMenu(saveMenu);
    }
    else
    {
        labelMenu->setTitle("Label");
        mainMenu.addMenu(labelMenu);
    }


    mainMenu.exec(mouseEvent->globalPos());

    delete labelMenu;
    delete roiManagementMenu;
    delete saveMenu;

    return true;
}

medTagRoiManager *polygonEventFilter::addManagerToList(int label)
{
    medTagRoiManager * manager = new medTagRoiManager(currentView, this, colorList[label]);
    managers.append(manager);
    connect(manager, SIGNAL(toggleRepulsorButton(bool)), this, SIGNAL(toggleRepulsorButton(bool)), Qt::UniqueConnection);
    connect(manager, SIGNAL(updateRoisInAlternativeViews()), this, SLOT(addRoisInAlternativeViews()), Qt::UniqueConnection);
    return manager;
}

bool polygonEventFilter::addPointInContourWithLabel(QMouseEvent *mouseEvent)
{
    bool res = false;
    if (managers.size()==0)
    {
        addManagerToList(0);
        return res;
    }
    else
    {
        QList<QColor> colorsToExclude;
        for (medTagRoiManager *manager : managers)
        {
            double mousePos[2];
            mousePos[0] = mouseEvent->x()*QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();
            mousePos[1] = (currentView->viewWidget()->height()-mouseEvent->y()-1)*QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();
            if ( manager->mouseIsCloseFromContour(mousePos) )
            {
                cursorState = CURSORSTATE::CS_NONE;
                return res;
            }

            if ( manager->existingRoiInSlice() )
            {
                if ( manager->roiClosedInSlice())
                    colorsToExclude.append(manager->getColor());
                else
                {
                    cursorState = CURSORSTATE::CS_NONE;
                    return res;
                }
            }
            else
            {
                if (!manager->isSameSliceOrientation())
                    colorsToExclude.append(manager->getColor());
            }
        }

        if ( managers.size()==1 && colorsToExclude.empty() )
        {
            medTagRoiManager *manager = managers.at(0);
            manager->appendRoi();            
        }
        else
        {
            QList<QColor> colors = updateColors(colorsToExclude);
            QMenu * menu = updateLabelMenu(colors);
            if ( !menu->exec(mouseEvent->globalPos()))
                res = true;
            delete menu;
        }
        return res;
    }
}
void polygonEventFilter::updateLabel(int label)
{
    for (medTagRoiManager *manager : managers)
    {
        if (manager->getColor() == colorList.at(label))
        {
            manager->appendRoi();
            return;
        }
    }
    addManagerToList(label);
}

QMenu *polygonEventFilter::updateLabelMenu(QList<QColor> colors)
{
    QMenu *menu = new QMenu;
    for (int i = 0; i<colors.size(); i++)
    {
        QPixmap pixmap(50,50);
        pixmap.fill(colors.at(i));
        QIcon icon(pixmap);
        QAction *action = new QAction(icon, QString::number(colorList.indexOf(colors.at(i))), menu);
        connect(action, &QAction::triggered, [=](){
            updateLabel(colorList.indexOf((colors.at(i))));
        });

        menu->addAction(action);
    }
    return menu;
}

QList<QColor> polygonEventFilter::updateColors(QList<QColor> colorsToExclude)
{
    QList<QColor> colors;
    if (!colorsToExclude.empty())
    {
        colors = getAvailableColors(colorsToExclude);
    }
    else
    {
        colors = colorList;
    }
    if (colors.empty())
    {
        qDebug()<<"No more labels available";
        exit(0);
    }
    return colors;
}

QList<QColor> polygonEventFilter::getAvailableColors(QList<QColor> colorsToExclude)
{
    QList<QColor> colors = colorList;
    for (QColor color : colorsToExclude)
    {
        if (colorList.contains(color))
        {
            colors.removeOne(color);
        }
    }
    return colors;
}

void polygonEventFilter::Off()
{
    removeFromAllViews();
    for (medTagRoiManager *manager : managers)
    {
        manager->setContourEnabled(false);
    }
    activateRepulsor(false);
    cursorState = CURSORSTATE::CS_CONTINUE;
}

void polygonEventFilter::On()
{
    for (medTagRoiManager *manager : managers)
    {
        manager->setContourEnabled(true);
    }
    cursorState = CURSORSTATE::CS_CONTINUE;
}

void polygonEventFilter::setEnableInterpolation(bool state)
{
    for (medTagRoiManager *manager : managers)
    {
        manager->setEnableInterpolation(state);
    }
}

void polygonEventFilter::addAlternativeViews(medAbstractImageView* view)
{
    alternativeViews.append(view);
}

void polygonEventFilter::addRoisInAlternativeViews()
{
    for (medAbstractImageView * v : alternativeViews)
    {
        for (medTagRoiManager *manager : managers)
        {
            manager->addContoursInAlternativeViews(v);
        }
    }
}

void polygonEventFilter::clearAlternativeViews()
{
    alternativeViews.clear();
}

void polygonEventFilter::removeView()
{
    medAbstractView *viewClosed = qobject_cast<medAbstractView*>(QObject::sender());
    for (medAbstractImageView *v : alternativeViews)
    {
        if (viewClosed==v)
        {
            alternativeViews.removeOne(v);
        }
    }
    if ( alternativeViews.size()==0 )
        emit clearLastAlternativeView();
}

void polygonEventFilter::activateRepulsor(bool state)
{
    if ( isRepulsorActivated == state)
        return;

    isRepulsorActivated = state;


    vtkImageView2D *view2D =  static_cast<medVtkViewBackend*>(currentView->backend())->view2D;
    if (state)
    {
        cursorState = CURSORSTATE::CS_REPULSOR;
        vtkInteractorStyleImageView2D *interactorStyle2D = vtkInteractorStyleImageView2D::SafeDownCast(view2D->GetInteractor()->GetInteractorStyle());
        if (!interactorStyleRepulsor)
            interactorStyleRepulsor = vtkInriaInteractorStylePolygonRepulsor::New();
        interactorStyleRepulsor->SetLeftButtonInteraction(interactorStyle2D->GetLeftButtonInteraction());
        view2D->SetInteractorStyle(interactorStyleRepulsor);
        view2D->SetupInteractor(view2D->GetInteractor()); // to reinstall vtkImageView2D pipeline
    }
    else
    {
        cursorState = CURSORSTATE::CS_MOUSE_EVENT;
        vtkInteractorStyleImageView2D *interactorStyle2D = vtkInteractorStyleImageView2D::New();
        interactorStyle2D->SetLeftButtonInteraction(interactorStyleRepulsor->GetLeftButtonInteraction());
        view2D->SetInteractorStyle(interactorStyle2D);
        view2D->SetupInteractor(view2D->GetInteractor()); // to reinstall vtkImageView2D pipeline
        interactorStyle2D->Delete();
    }
    return;
}

void polygonEventFilter::saveMask()
{
    for (medTagRoiManager *manager : managers)
    {
        saveMask(manager);
    }
}

void polygonEventFilter::saveContoursAsMedAbstractData(vtkMetaDataSet *outputDataSet, QVector<medTagContours> contoursData)
{
    contourOutput = medAbstractDataFactory::instance()->createSmartPointer("medContours");

    medAbstractData * input = currentView->layerData(0);
    medAbstractImageData *inputData = qobject_cast<medAbstractImageData*>(input);

    QString desc = QString("contours with label ");
    medUtilities::setDerivedMetaData(contourOutput, inputData, desc);
    contourOutput->setData(outputDataSet);
    contourOutput->setData(&contoursData, 1);
    outputDataSet->Delete();

    medDataManager::instance()->importData(contourOutput, false);
}

void polygonEventFilter::saveAllContours()
{
    QVector<medTagContours> contoursData;
    vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
    append->SetUserManagedInputs(true);
    append->SetNumberOfInputs(managers.size());
    int num = 0;
    for (medTagRoiManager *manager : managers)
    {
        int label = colorList.indexOf(manager->getColor()) + 1;

        auto ctr = manager->getContoursAsNodes();
        medTagContours contoursTag;
        contoursTag.setLabel(qint32(label));
        contoursTag.setContourNodes(ctr);
        contoursData.append(contoursTag);

        auto pd = manager->getContoursAsPolyData(label);
        append->SetInputDataByNumber(num, pd);
        num++;
    }
    append->Update();
    vtkMetaDataSet *outputDataSet = vtkMetaSurfaceMesh::New();
    outputDataSet->SetDataSet(append->GetOutput());


    saveContoursAsMedAbstractData(outputDataSet, contoursData);
    return;

}


void polygonEventFilter::loadContours(medAbstractData *data)
{
    medContours *contours = dynamic_cast<medContours*>(data);
    QVector<medTagContours> &tagContoursSet = contours->getTagContoursSet();

    if ( managers.size()==colorList.size() )
    {
        qDebug()<<"unable to create new manager ";
        return;
    }

    for (medTagContours &tagContours : tagContoursSet)
    {
        if ( managers.size()==colorList.size() )
        {
            qDebug()<<"unable to create new manager ";
            return;
        }
        int label = findAvailableLabel();
        if ( label == -1 )
            return;
        medTagRoiManager *manager = addManagerToList(label);
        manager->loadContours(tagContours.getContourNodes());
    }
}

int polygonEventFilter::findAvailableLabel()
{
    QList<QColor> usedColors;
    for ( medTagRoiManager* manager : managers )
    {
        usedColors.append(manager->getColor());
    }
    for (QColor color : colorList)
    {
        if (!usedColors.contains(color))
        {
            return colorList.indexOf(color);
        }
    }
    return -1;
}

void polygonEventFilter::manageTick()
{
    medIntParameterL *slicingParameter = nullptr;
    for( auto interactor : qobject_cast<medAbstractLayeredView*>(currentView)->layerInteractors(0))
    {
        if ((interactor->identifier() == "medVtkViewItkDataImageInteractor") ||
                (interactor->identifier() == "medVtkViewItkDataImage4DInteractor"))
        {
            for (auto parameter : interactor->linkableParameters())
            {
                if (parameter->name() == "Slicing")
                {
                    slicingParameter = qobject_cast<medIntParameterL*>(parameter);
                    break;
                }
            }
            break;
        }
    }
    if ( slicingParameter )
    {
        slicingParameter->getSlider()->removeAllTicks();
        for (medTagRoiManager *manager : managers)
        {
            manager->manageTick(slicingParameter->getSlider());
        }
    }
    return;
}

void polygonEventFilter::manageRoisVisibility()
{
    for (medTagRoiManager *manager : managers)
    {
        manager->manageVisibility();
    }

}

void polygonEventFilter::setToolboxButtonsState(bool state)
{
    emit enableRepulsor(state);
    emit enableGenerateMask(state);
    emit enableViewChooser(state);
}

void polygonEventFilter::deleteNode(medTagRoiManager *manager, double X, double Y)
{
    manager->deleteNode(X, Y);
    if ( manager->getRois().empty() )
    {
        managers.removeOne(manager);
        delete manager;
    }
    else
    {
        manager->select(false);
    }
    manageTick();
    manageButtonsState();
}

void polygonEventFilter::deleteContour(medTagRoiManager *manager)
{
    manager->deleteContour();
    if ( manager->getRois().empty() )
    {
        managers.removeOne(manager);
        delete manager;
    }
    manageTick();
    manageButtonsState();
}


void polygonEventFilter::deleteLabel(medTagRoiManager *manager)
{
    removeContoursInAlternativeViews(manager);
    managers.removeOne(manager);
    manager->removeAllTick();

    delete manager;
    currentView->render();
    manageButtonsState();
}

void polygonEventFilter::removeContoursInAlternativeViews(medTagRoiManager *manager)
{
    for (medAbstractImageView *v : alternativeViews)
    {
        manager->removeContoursInAlternativeViews(v);
    }
}

void polygonEventFilter::manageButtonsState()
{
    isRepulsorActivated = false;
    cursorState = CURSORSTATE::CS_MOUSE_EVENT;
    if (!isContourInSlice())
    {
        emit toggleRepulsorButton(false);
    }
    if (managers.empty())
    {
        setToolboxButtonsState(false);
    }
}

bool polygonEventFilter::isContourInSlice()
{
    for (medTagRoiManager *manager : managers)
    {
        if (manager->existingRoiInSlice())
        {
            return true;
        }
    }
    return false;
}

void polygonEventFilter::saveMask(medTagRoiManager *manager)
{
    int label = colorList.indexOf(manager->getColor()) + 1;
    manager->createMaskWithLabel(label);
    manager->select(false);
}

void polygonEventFilter::saveContour(medTagRoiManager *manager)
{
    QVector<medTagContours> contoursData;
    vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
    append->SetUserManagedInputs(true);
    append->SetNumberOfInputs(managers.size());

    int label = colorList.indexOf(manager->getColor()) + 1;

    auto ctr = manager->getContoursAsNodes();
    medTagContours contoursTag;
    contoursTag.setLabel(qint32(label));
    contoursTag.setContourNodes(ctr);
    contoursData.append(contoursTag);

    auto pd = manager->getContoursAsPolyData(label);
    append->SetInputDataByNumber(0, pd);

    append->Update();
    vtkMetaDataSet *outputDataSet = vtkMetaSurfaceMesh::New();
    outputDataSet->SetDataSet(append->GetOutput());

    saveContoursAsMedAbstractData(outputDataSet, contoursData);
    manager->select(false);
    return;

}

medTagRoiManager *polygonEventFilter::closestManagerInSlice(double mousePos[2])
{
    double minDistance = DBL_MAX;
    medTagRoiManager * managerInSlice = nullptr;
    for (medTagRoiManager *manager : managers)
    {
        double dist = manager->getMinimumDistanceFromNodesToEventPosition(mousePos);
        if ( dist < minDistance )
        {
            minDistance = dist;
            managerInSlice = manager;
        }
    }
    return managerInSlice;
}

int polygonEventFilter::findClosestSliceFromMouseClick(QVector3D worldMouseCoord)
{
    double minDistance = 1.;
    medTagRoiManager *closestManager = nullptr;
    for (medTagRoiManager *manager : managers)
    {
        double dist = manager->findClosestContourFromPoint(worldMouseCoord);
        if ( dist < minDistance)
        {
            minDistance = dist;
            closestManager = manager;
        }
    }
    return (minDistance<1.)?closestManager->getClosestSliceFromPoint():-1;
}
