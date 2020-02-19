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
#include <medColorEditor.h>
#include <medIntParameterL.h>
#include <medTableWidgetChooser.h>
#include <medVtkViewBackend.h>
#include <vtkRenderWindowInteractor.h>
#include <float.h>

polygonEventFilter::polygonEventFilter(medAbstractImageView *view) :
        medViewEventFilter(), currentView(view), cursorState(CURSORSTATE::FIRST_MOUSE_EVENT), isRepulsorActivated(false)
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
//        QMenu *menu = new QMenu;
//        QMenu *subMenu = new QMenu(QString("define label"));
//        menu->addMenu(subMenu);
//        QPixmap pixmap(100,100);
//        pixmap.fill(QColor("red"));
//        QIcon redIcon(pixmap);
//        subMenu->addAction(new QAction(redIcon, QString("red"), this));
////        menu->addAction(new QAction("New",this));
////        menu->addAction(new QAction("Edit",this));
////        menu->addAction(new QAction("Delete",this));
//        vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(view->backend())->view2D;

//        for (medLabelManager *manager : managers)
//        {
//            if (manager->existingRoiInSlice())
//            {
//                QList<polygonRoi*> rois = manager->getRois();
//                for (polygonRoi *roi : rois)
//                {
//                    for (int i = 0; i< roi->getContour()->GetContourRepresentation()->GetNumberOfNodes(); i++)
//                    {
//                        double pos[2];
//                        roi->getContour()->GetContourRepresentation()->GetNthNodeDisplayPosition(i, pos);
//                        if ( getDistance(mouseEvent->x(), (view->viewWidget()->height()-mouseEvent->y()-1), pos) <= 10 )
//                        {
//                            menu->exec(mouseEvent->globalPos());
//                            break;
//                        }
//                    }
//                }
//            }
//        }

        return true;
    }
    return false;
}

bool polygonEventFilter::leftButtonBehaviour(medAbstractView *view, QMouseEvent *mouseEvent)
{
    if (isRepulsorActivated)
        cursorState = CURSORSTATE::REPULSOR;

    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(view->backend())->view2D;
//    QString state;
//    if (cursorState==CURSORSTATE::NONE)
//        state = "none";
//    else if (cursorState==CURSORSTATE::FIRST_CLICK)
//        state = "first click";
//    else if (cursorState==CURSORSTATE::ROI_CLOSED)
//        state = "roi closed";
//    else if (cursorState==CURSORSTATE::SLICE_CHANGED)
//        state = "slice changed";
//    else if (cursorState==CURSORSTATE::CONTINUE)
//        state = "continue";
//    qDebug()<<"mouse press with state : "<<state<<" and currentLabel : "<<currentLabel;
//    qDebug()<<"roi manager size "<<managers.size();
//    for (medLabelManager *manager : managers)
//    {
//        qDebug()<<"label : "<<manager->getLabel()<<" and number of rois "<<manager->getRois().size();
//        for (polygonRoi *r : manager->getRois())
//        {
//            if (r->isMasterRoi())
//            qDebug()<<"   in slice "<<r->getIdSlice();
//        }
//    }

    switch (cursorState)
    {
    case CURSORSTATE::FIRST_MOUSE_EVENT:
    {
        medLabelManager * manager = new medLabelManager(view, this, colorList[0]);
        managers.append(manager);
        cursorState = CURSORSTATE::NONE;
        break;
    }
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
        mousePos[0] = mouseEvent->x()*QGuiApplication::primaryScreen()->devicePixelRatio();
        mousePos[1] = (currentView->viewWidget()->height()-mouseEvent->y()-1)*QGuiApplication::primaryScreen()->devicePixelRatio();
        medLabelManager * manager = closestManagerInSlice(mousePos);
        if (!manager)
        {
//            if (interactorStyleRepulsor!=nullptr)
//            {
//                interactorStyleRepulsor->Delete();
//                interactorStyleRepulsor = nullptr;
//            }
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
        qDebug()<<"Impossible Case";
        return false;
    }
    else
    {
        QList<QColor> colorsToExclude;
        for (medLabelManager *manager : managers)
        {
            double mousePos[2];
            mousePos[0] = mouseEvent->x()*QGuiApplication::primaryScreen()->devicePixelRatio();
            mousePos[1] = (currentView->viewWidget()->height()-mouseEvent->y()-1)*QGuiApplication::primaryScreen()->devicePixelRatio();
            if ( manager->mouseIsCloseFromContour(mousePos) )
            {
                cursorState = CURSORSTATE::NONE;
                return false;
            }

            if ( manager->existingRoiInSlice() )
            {
                if ( manager->roiClosedInSlice() )
                    colorsToExclude.append(manager->getColor());
                else
                {
                    cursorState = CURSORSTATE::NONE;
                    return false;
                }
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
            chooseLabelMenu(colors, mouseEvent);
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

void polygonEventFilter::chooseLabelMenu(QList<QColor> colors, QMouseEvent *mouseEvent)
{
    QMenu menu;
    for (int i = 0; i<colors.size(); i++)
    {
        QPixmap pixmap(50,50);
        pixmap.fill(colors.at(i));
        QIcon icon(pixmap);
        QAction *action = new QAction(icon, QString::number(colorList.indexOf(colors.at(i))), this);
        connect(action, SIGNAL(triggered()), &signalMapper, SLOT(map()));
        signalMapper.setMapping(action, colorList.indexOf(colors.at(i)));
        menu.addAction(action);
    }
    menu.exec(mouseEvent->globalPos());
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
    //delete viewEventFilter;
    //viewEventFilter = nullptr;
    for (medLabelManager *manager : managers)
    {
        manager->setContourEnabled(false);
    }
    cursorState = CURSORSTATE::CONTINUE;
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
