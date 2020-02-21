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

polygonEventFilter::polygonEventFilter(medAbstractImageView *view) :
        medViewEventFilter(), currentView(view), cursorState(CURSORSTATE::MOUSE_EVENT), isRepulsorActivated(false)
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
}

void polygonEventFilter::removeManagers()
{
    for (medLabelManager *manager: managers)
    {
        delete manager;
    }
    managers.clear();

}

bool polygonEventFilter::mousePressEvent(medAbstractView * view, QMouseEvent *mouseEvent)
{
    currentView = dynamic_cast<medAbstractImageView *> (view);

    if (mouseEvent->button()==Qt::LeftButton)
    {
        return leftButtonBehaviour(view, mouseEvent);
        //manageTick();
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
        cursorState = CURSORSTATE::REPULSOR;

    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(view->backend())->view2D;
    QString state;
    if (cursorState==CURSORSTATE::NONE)
        state = "none";
    else if (cursorState==CURSORSTATE::MOUSE_EVENT)
        state = "slice changed";
    else if (cursorState==CURSORSTATE::CONTINUE)
        state = "continue";
    else if (cursorState==CURSORSTATE::REPULSOR)
        state = "repulsor";
    qDebug()<<"mouse press with state : "<<state;
    qDebug()<<"roi manager size "<<managers.size();

    if (cursorState != CURSORSTATE::NONE)
        setToolboxButtonsState(true);

    switch (cursorState)
    {
    case CURSORSTATE::MOUSE_EVENT:
    {
        return addPointInContourWithLabel(mouseEvent);
    }
    case CURSORSTATE::CONTINUE:
    {
        for (medLabelManager *manager : managers)
        {
            manager->setContourEnabled(true);
            polygonRoi *roi = manager->roiOpenInSlice();
            if ( roi != nullptr)
            {
                roi->getContour()->SetWidgetState(vtkContourWidget::Define);

                roi->getContour()->InvokeEvent(vtkCommand::PlacePointEvent);
                cursorState = CURSORSTATE::NONE;
                return false;
            }
        }
        return addPointInContourWithLabel(mouseEvent);
    }
    case CURSORSTATE::REPULSOR:
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
    case CURSORSTATE::NONE:
    default:
    {
        return false;
    }
    }
    return false;
}

bool polygonEventFilter::addPointInContourWithLabel(QMouseEvent *mouseEvent)
{
    if (managers.size()==0)
    {
        medLabelManager * manager = new medLabelManager(currentView, this, colorList[0]);
        managers.append(manager);
        cursorState = CURSORSTATE::NONE;
        return false;
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
                cursorState = CURSORSTATE::NONE;
                return false;
            }

            if ( manager->existingRoiInSlice() )
            {
                if ( manager->roiClosedInSlice())
                    colorsToExclude.append(manager->getColor());
                else
                {
                    cursorState = CURSORSTATE::NONE;
                    return false;
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
            menu->exec(mouseEvent->globalPos());
            delete menu;
        }
        return false;
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
    medLabelManager *newManager = new medLabelManager(currentView, this, colorList.at(label));
    managers.append(newManager);
    cursorState = CURSORSTATE::NONE;

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
    qDebug()<<"managers size "<<managers.size();
    for (medLabelManager *manager : managers)
    {
        qDebug()<<"mgr "<<manager;
        manager->setContourEnabled(false);
    }
    cursorState = CURSORSTATE::CONTINUE;
    //sendSignals();
}

void polygonEventFilter::On()
{
    for (medLabelManager *manager : managers)
    {
        manager->setContourEnabled(true);
    }
}

void polygonEventFilter::setEnableInterpolation(bool state)
{
    for (medLabelManager *manager : managers)
    {
        manager->setEnableInterpolation(state);
    }
}

void polygonEventFilter::updateAlternativeViews(medAbstractImageView* view, medTableWidgetItem *item)
{
    for (medLabelManager *manager : managers)
    {
        manager->updateAlternativeViews(view, item);
    }
}

void polygonEventFilter::activateRepulsor(bool state)
{
    isRepulsorActivated = state;


    vtkImageView2D *view2D =  static_cast<medVtkViewBackend*>(currentView->backend())->view2D;
    if (state)
    {
        cursorState = CURSORSTATE::REPULSOR;
        vtkInteractorStyleImageView2D *interactorStyle2D = vtkInteractorStyleImageView2D::SafeDownCast(view2D->GetInteractor()->GetInteractorStyle());
        interactorStyleRepulsor->SetLeftButtonInteraction(interactorStyle2D->GetLeftButtonInteraction());
        view2D->SetInteractorStyle(interactorStyleRepulsor);
        view2D->SetupInteractor(view2D->GetInteractor()); // to reinstall vtkImageView2D pipeline
    }
    else
    {
        cursorState = CURSORSTATE::MOUSE_EVENT;
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
    if (managers.empty())
    {
        setToolboxButtonsState(false);
    }
}

void polygonEventFilter::deletedContour(medLabelManager *manager)
{
    manager->deleteContour();
    if ( manager->getRois().empty() )
    {
        managers.removeOne(manager);
        delete manager;
    }
    if (managers.empty())
    {
        setToolboxButtonsState(false);
    }
}

void polygonEventFilter::deletedLabel(medLabelManager *manager)
{
    managers.removeOne(manager);
    manager->removeAllTick();
    delete manager;
    currentView->render();
    if (managers.empty())
    {
        setToolboxButtonsState(false);
    }
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