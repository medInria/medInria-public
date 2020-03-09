/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "polygonEventFilter.h"
#include "vtkContourOverlayRepresentation.h"
#include <QGraphicsSceneMouseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <medIntParameterL.h>
#include <medTableWidgetChooser.h>
#include <medVtkViewBackend.h>
#include <vtkRenderWindowInteractor.h>
#include <float.h>
#include <medDataManager.h>
#include <medAbstractDataFactory.h>
#include <medUtilities.h>
#include <medAbstractImageData.h>
#include <itkJoinImageFilter.h>
#include <itkAddImageFilter.h>
#include <itkComposeImageFilter.h>
#include <medViewContainer.h>

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
    connect(&signalMapper, SIGNAL(mapped(int)), this, SLOT(updateLabel(int)));

    // interactors
    interactorStyleRepulsor = vtkInriaInteractorStylePolygonRepulsor::New();
    connect(this, SIGNAL(nodeToDelete(medLabelManager*,double, double)), this, SLOT(deletedNode(medLabelManager*,double, double)));
    connect(this, SIGNAL(contourToDelete(medLabelManager*)), this, SLOT(deletedContour(medLabelManager*)));
    connect(this, SIGNAL(labelToDelete(medLabelManager*)), this, SLOT(deletedLabel(medLabelManager*)));
    connect(this, SIGNAL(maskToSave(medLabelManager*)), this, SLOT(savedMask(medLabelManager*)));
}

polygonEventFilter::~polygonEventFilter()
{
    interactorStyleRepulsor->Delete();
    for (medLabelManager *manager: managers)
    {
        delete manager;
    }
    managers.clear();
}

void polygonEventFilter::updateView(medAbstractImageView *view)
{
    currentView = view;
}

bool polygonEventFilter::mouseReleaseEvent(medAbstractView *view, QMouseEvent *mouseEvent)
{
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

void polygonEventFilter::reset()
{
    for (medLabelManager *manager: managers)
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

bool polygonEventFilter::mousePressEvent(medAbstractView * view, QMouseEvent *mouseEvent)
{
    currentView = dynamic_cast<medAbstractImageView *> (view);

    if (mouseEvent->button()==Qt::LeftButton)
    {
        return leftButtonBehaviour(view, mouseEvent);
    }
    else if (mouseEvent->button()==Qt::RightButton)
    {
        QMenu mainMenu;
        QList<QColor> colorsToExclude;
        medLabelManager * closestManager = nullptr;
        double minDist = DBL_MAX;
        double dist = 0.;
        double mousePos[2];
        mousePos[0] = mouseEvent->x()*QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();
        mousePos[1] = (currentView->viewWidget()->height()-mouseEvent->y()-1)*QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();
        for (medLabelManager *manager : managers)
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
        QMenu *roiManagementMenu = new QMenu("Delete");
        if (closestManager && closestManager->getMinimumDistanceFromNodesToEventPosition(mousePos) < 10 )
        {
            mainMenu.addMenu(roiManagementMenu);
            QAction *deleteOneNodeAction = new QAction("One node", roiManagementMenu);
            connect(deleteOneNodeAction, &QAction::triggered, [=](){
                emit nodeToDelete(closestManager, mousePos[0], mousePos[1]);
            });
            roiManagementMenu->addAction(deleteOneNodeAction);

            QAction *deleteContour = new QAction("Contour", roiManagementMenu);
            connect(deleteContour, &QAction::triggered, [=](){
                emit contourToDelete(closestManager);
            });
            roiManagementMenu->addAction(deleteContour);

            QAction *deleteLabel = new QAction("Label", roiManagementMenu);
            connect(deleteLabel, &QAction::triggered, [=](){
                emit labelToDelete(closestManager);
            });
            roiManagementMenu->addAction(deleteLabel);

            QAction *saveMask = new QAction("Save Mask", &mainMenu);
            connect(saveMask, &QAction::triggered, [=](){
                emit maskToSave(closestManager);
            });

            mainMenu.addMenu(roiManagementMenu);
            mainMenu.addAction(saveMask);
        }
        else
        {
            labelMenu->setTitle("Label");
            mainMenu.addMenu(labelMenu);
        }


        mainMenu.exec(mouseEvent->globalPos());

        delete labelMenu;
        delete roiManagementMenu;

        return true;
    }
    return false;
}

bool polygonEventFilter::leftButtonBehaviour(medAbstractView *view, QMouseEvent *mouseEvent)
{
    if (isRepulsorActivated)
    {
//        if (cursorState==CURSORSTATE::CS_SLICE_CHANGED)
//        {
//            qDebug()<<"ca devrait...";
//            return false;
//            //activateRepulsor(false);
//        }
//        else
//        {
            cursorState = CURSORSTATE::CS_REPULSOR;
//        }
    }


    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(view->backend())->view2D;
    QString state;
    if (cursorState==CURSORSTATE::CS_NONE)
        state = "none";
    else if (cursorState==CURSORSTATE::CS_MOUSE_EVENT || cursorState== CURSORSTATE::CS_SLICE_CHANGED)
        state = "slice changed or move mouse";
    else if (cursorState==CURSORSTATE::CS_CONTINUE)
        state = "continue";
    else if (cursorState==CURSORSTATE::CS_REPULSOR)
        state = "repulsor";
    qDebug()<<"mouse press with state : "<<state;
    qDebug()<<"roi manager size "<<managers.size();

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
        for (medLabelManager *manager : managers)
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
        medLabelManager * manager = closestManagerInSlice(mousePos);
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

void polygonEventFilter::addManagerToList(int label)
{
    medLabelManager * manager = new medLabelManager(currentView, this, colorList[label]);    
    managers.append(manager);
    connect(manager, SIGNAL(toggleRepulsorButton(bool)), this, SIGNAL(toggleRepulsorButton(bool)), Qt::UniqueConnection);
    connect(manager, SIGNAL(updateRoisInAlternativeViews()), this, SLOT(addRoisInAlternativeViews()), Qt::UniqueConnection);
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
        for (medLabelManager *manager : managers)
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
            medLabelManager *manager = managers.at(0);
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
    for (medLabelManager *manager : managers)
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
        connect(action, SIGNAL(triggered()), &signalMapper, SLOT(map()));
        signalMapper.setMapping(action, colorList.indexOf(colors.at(i)));
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
    for (medLabelManager *manager : managers)
    {
        manager->setContourEnabled(false);
    }
    activateRepulsor(false);
    cursorState = CURSORSTATE::CS_CONTINUE;
}

void polygonEventFilter::On()
{
    for (medLabelManager *manager : managers)
    {
        manager->setContourEnabled(true);
    }
    cursorState = CURSORSTATE::CS_CONTINUE;
}

void polygonEventFilter::setEnableInterpolation(bool state)
{
    for (medLabelManager *manager : managers)
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
        for (medLabelManager *manager : managers)
        {
            manager->addRoisInAlternativeViews(v);
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

void polygonEventFilter::generateMask()
{
    QList<UChar3ImageType::Pointer> masks;
    for (medLabelManager *manager : managers)
    {
        savedMask(manager);
    }
}

void polygonEventFilter::manageTick()
{
    for (medLabelManager *manager : managers)
    {
        manager->manageTick();
    }
}

void polygonEventFilter::manageRoisVisibility()
{
    for (medLabelManager *manager : managers)
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

void polygonEventFilter::deletedNode(medLabelManager *manager, double X, double Y)
{
    manager->deleteNode(X, Y);
    if ( manager->getRois().empty() )
    {
        managers.removeOne(manager);
        delete manager;
    }
    manageButtonsState();
}

void polygonEventFilter::deletedContour(medLabelManager *manager)
{
    manager->deleteContour();
    if ( manager->getRois().empty() )
    {
        managers.removeOne(manager);
        delete manager;
    }
    manageButtonsState();
}


void polygonEventFilter::deletedLabel(medLabelManager *manager)
{
    managers.removeOne(manager);
    manager->removeAllTick();
    delete manager;
    currentView->render();
    manageButtonsState();
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
    for (medLabelManager *manager : managers)
    {
        if (manager->existingRoiInSlice())
        {
            return true;
        }
    }
    return false;
}

void polygonEventFilter::savedMask(medLabelManager *manager)
{
    int label = colorList.indexOf(manager->getColor()) + 1;
    manager->createMaskWithLabel(label);
}

medLabelManager *polygonEventFilter::closestManagerInSlice(double mousePos[2])
{
    double minDistance = DBL_MAX;
    medLabelManager * managerInSlice = nullptr;
    for (medLabelManager *manager : managers)
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
