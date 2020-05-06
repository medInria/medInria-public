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
#include <QLineEdit>
#include <QScreen>
#include <QWidgetAction>

// vtk
#include <vtkAppendPolyData.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

class View2DObserver : public vtkCommand
{
public:
    static View2DObserver* New()
    {
        return new View2DObserver;
    }

    void Execute ( vtkObject *caller, unsigned long event, void *callData );

    void setObject ( polygonEventFilter *obj )
    {
        this->obj = obj;
    }

private:
    polygonEventFilter *obj;
};

void View2DObserver::Execute ( vtkObject *caller, unsigned long event, void *callData )
{

    switch ( event )
    {
        case vtkImageView2D::SliceChangedEvent:
        {
            obj->manageRoisVisibility();
            break;
        }
        default:
        {
            break;
        }
    }
}

polygonEventFilter::polygonEventFilter(medAbstractImageView *view, QList<QColor> colorsList) :
        medViewEventFilter(), currentView(view), cursorState(CURSORSTATE::CS_MOUSE_EVENT), isRepulsorActivated(false), activateEventFilter(true), enableInterpolation(true),
        activeManager(nullptr)
{
    this->colorsList = colorsList;
    defaultColorsList = colorsList;
    // interactors
    interactorStyleRepulsor = vtkInriaInteractorStylePolygonRepulsor::New();
}

polygonEventFilter::~polygonEventFilter()
{
    reset();
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
    isRepulsorActivated = false;
    if (interactorStyleRepulsor)
    {
        interactorStyleRepulsor->Delete();
        interactorStyleRepulsor = nullptr;
    }
    otherViews.clear();
    activeManager = nullptr;
}

bool polygonEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
    if (mouseEvent)
    {
        return medViewEventFilter::eventFilter(obj,event);
    }

    if (!keyEvent)
    {
        return false;
    }

    if ( ! activateEventFilter )
    {
        return false;
    }

    if ( keyEvent->type() == QEvent::ShortcutOverride )
    {
        if ( keyEvent->key() == Qt::Key::Key_C )
        {
            copyContours();
        }
        if ( keyEvent->key() == Qt::Key::Key_V )
        {
            pasteContours();
        }
        if ( keyEvent->key() == Qt::Key::Key_Backspace )
        {
            deleteNode(savedMousePosition);
        }
        if ( keyEvent->key() == Qt::Key::Key_Right )
        {
            if (currentView && currentView->backend())
            {
                vtkImageView2D *view2D =  static_cast<medVtkViewBackend*>(currentView->backend())->view2D;
                view2D->SetSlice(view2D->GetSlice()+1);
                view2D->Render();
            }
        }
        if ( keyEvent->key() == Qt::Key::Key_Left )
        {
            if (currentView && currentView->backend())
            {
                vtkImageView2D *view2D =  static_cast<medVtkViewBackend*>(currentView->backend())->view2D;
                view2D->SetSlice(view2D->GetSlice()-1);
                view2D->Render();
            }
        }
    }
    return event->isAccepted();
}

bool polygonEventFilter::mouseReleaseEvent(medAbstractView *view, QMouseEvent *mouseEvent)
{
    if ( !view )
        return false;
    medAbstractImageView *v = dynamic_cast<medAbstractImageView *> (view);
    if ( !currentView || !v )
    {
        return false;
    }

    if (currentView == v )
    {
        if (isRepulsorActivated && activateEventFilter)
        {
            if ( !isActiveContourInSlice() || (activeManager && activeManager->roiOpenInSlice()))
            {
                return false;
            }

            if (interactorStyleRepulsor != nullptr)
            {
                if (interactorStyleRepulsor->GetManager() != nullptr )
                {
                    interactorStyleRepulsor->GetManager()->SetMasterRoi(true);
                    interactorStyleRepulsor->GetManager()->interpolateIfNeeded();
                }
                enableOtherViewsVisibility(true);
                manageTick();
            }
        }
    }

    return false;
}

bool polygonEventFilter::mouseMoveEvent(medAbstractView *view, QMouseEvent *mouseEvent)
{
    savedMousePosition[0] = mouseEvent->x()*QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();
    savedMousePosition[1] = (currentView->viewWidget()->height()-mouseEvent->y()-1)*QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();

    if (activeManager)
    {
        if ( activeManager->getMinimumDistanceFromNodesToMouse(savedMousePosition, false) < 20.)
        {
            setCustomCursor();
        }
        else
        {
            currentView->viewWidget()->setCursor(Qt::ArrowCursor);
        }
    }
    else
    {
        currentView->viewWidget()->setCursor(Qt::ArrowCursor);
    }
    return false;
}

bool polygonEventFilter::mousePressEvent(medAbstractView * view, QMouseEvent *mouseEvent)
{
    if ( ! activateEventFilter )
        return false;

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

bool polygonEventFilter::leftButtonBehaviour(medAbstractView *view, QMouseEvent *mouseEvent)
{
    if (isRepulsorActivated)
    {
        if (isActiveContourInSlice())
        {
            cursorState = CURSORSTATE::CS_REPULSOR;
            activateRepulsor(true);
        }
        else
        {
            activateRepulsor(false);
            isRepulsorActivated = true;
        }
    }

    if (cursorState != CURSORSTATE::CS_NONE && !managers.empty())
        setToolboxButtonsState(true);

    switch (cursorState)
    {
    case CURSORSTATE::CS_MOUSE_EVENT:
    {
        return addPointInContourWithLabel(mouseEvent);
    }
    case CURSORSTATE::CS_CONTINUE:
    {
        for (medTagRoiManager *manager : managers)
        {
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
        if (!activeManager)
        {
            emit sendErrorMessage("No active contour: Unable to use repulsor tool.");
            return false;
        }
        if (interactorStyleRepulsor == nullptr)
        {
            interactorStyleRepulsor = vtkInriaInteractorStylePolygonRepulsor::New();
        }
        interactorStyleRepulsor->SetCurrentView(currentView);
        interactorStyleRepulsor->SetManager(activeManager);
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

void polygonEventFilter::setCustomCursor()
{
    double radiusSize = 1.;
    // Adapt to scale of view (zoom, crop, etc)
    double radiusSizeDouble = radiusSize * currentView->scale();
    int radiusSizeInt = floor(radiusSizeDouble + 0.5);
    // Create shape of the new cursor
    QPixmap pix(radiusSizeInt, radiusSizeInt);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setBackgroundMode(Qt::TransparentMode);
    painter.setBackground(QColor(255,255,255,255));
    painter.setPen( Qt::red );
    painter.drawEllipse( 0, 0, radiusSizeInt, radiusSizeInt );
    painter.setPen( Qt::red );
    painter.drawPoint(floor(radiusSizeDouble/2.0+0.5),     floor(radiusSizeDouble/2.0+0.5));
    painter.drawPoint(floor(radiusSizeDouble/2.0-1.0+0.5), floor(radiusSizeDouble/2.0+0.5));
    painter.drawPoint(floor(radiusSizeDouble/2.0+1.0+0.5), floor(radiusSizeDouble/2.0+0.5));
    painter.drawPoint(floor(radiusSizeDouble/2.0+0.5),     floor(radiusSizeDouble/2.0-1.0+0.5));
    painter.drawPoint(floor(radiusSizeDouble/2.0+0.5),     floor(radiusSizeDouble/2.0+1.0+0.5));

    // Update the cursor
    currentView->viewWidget()->setCursor(QCursor(pix, -1, -1));
}

QLineEdit * polygonEventFilter::updateNameManager(medTagRoiManager* closestManager, QMenu *mainMenu)
{
    QLineEdit *renameManager = new QLineEdit(closestManager->getName());
    renameManager->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
    QFont font = renameManager->font();
    font.setWeight(QFont::Black);
    font.setPointSize(15);
    font.setUnderline(true);
    renameManager->setFont(font);
    renameManager->setAlignment(Qt::AlignCenter);
    QColor mycolor = closestManager->getColor();
    QString style = "QLineEdit { color: rgb(%1, %2, %3); }";
    renameManager->setStyleSheet(style.arg(mycolor.red()).arg(mycolor.green()).arg(mycolor.blue()));
    connect(renameManager, &QLineEdit::editingFinished, [=](){
       for (medTagRoiManager *manager : managers)
       {
           if (manager->getName()==renameManager->text())
           {
               qDebug()<<"forbidden name";
               emit sendErrorMessage("An other contour has already the same name: " + renameManager->text());
               return;
           }
       }
       closestManager->setName(renameManager->text());
    });
    connect(renameManager, &QLineEdit::returnPressed, [=](){
       mainMenu->close();
    });

    return renameManager;
}

medTagRoiManager *polygonEventFilter::getClosestManager(double *mousePos)
{
    medTagRoiManager *closestManager = nullptr;
    double minDist = DBL_MAX;
    double dist = 0.;
    for (medTagRoiManager *manager : managers)
    {
        if ( manager->existingRoiInSlice() )
        {
            dist = manager->getMinimumDistanceFromNodesToMouse(mousePos);
            if ( dist < minDist )
            {
                minDist = dist;
                closestManager = manager;
            }
        }
    }
    return closestManager;
}

QMenu *polygonEventFilter::changeLabelActions(medTagRoiManager* closestManager)
{
    QMenu *changeMenu = new QMenu("Change Label:");
    for (medTagRoiManager *manager : managers)
    {
        if (manager!= closestManager && !manager->existingRoiInSlice())
        {
            QPixmap pixmap(20,20);
            pixmap.fill(manager->getColor());
            QIcon icon(pixmap);
            QAction *action = new QAction(icon, manager->getName(), changeMenu);
            connect(action, &QAction::triggered, [=](){
                polygonRoi *roi = closestManager->existingRoiInSlice();
                QVector<QVector2D> nodes = roi->copyContour();
                polygonRoi *roiToAdd = manager->appendRoi();
                roiToAdd->pasteContour(nodes);
                deleteContour(closestManager);
                activeManager = manager;
                enableOnlyActiveManager();
            });
            changeMenu->addAction(action);
        }
    }
    return changeMenu;
}

bool polygonEventFilter::rightButtonBehaviour(medAbstractView *view, QMouseEvent *mouseEvent)
{
    QMenu mainMenu(currentView->viewWidget());
    QList<QColor> colorsToExclude;
    QStringList namesToExclude;
    double mousePos[2];
    mousePos[0] = mouseEvent->x()*QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();
    mousePos[1] = (currentView->viewWidget()->height()-mouseEvent->y()-1)*QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();
    medTagRoiManager * closestManager = getClosestManager(mousePos);
    for (medTagRoiManager *manager : managers)
    {
        if ( manager->existingRoiInSlice() )
        {
            colorsToExclude.append(manager->getColor());
            namesToExclude.append(manager->getName());
        }
    }
    QList<QColor> colors = updateColorsList(colorsToExclude);
    QStringList names = predefinedLabels;
    if ( !namesToExclude.empty())
    {
        for (QString name : namesToExclude)
        {
            if (predefinedLabels.contains(name))
            {
                names.removeOne(name);
            }
        }
    }

    QMenu *colorMenu = createColorMenu(colors, names);
    QMenu *roiManagementMenu = new QMenu("Remove: ");
    QMenu *saveMenu = new QMenu("Save as: ");
    QMenu *changeMenu = nullptr;
    QWidgetAction *renameManagerAction = new QWidgetAction(&mainMenu);
    QAction *copyContourAction = new QAction("Copy", &mainMenu);
    QAction *activationAction = nullptr;
    if (closestManager && (closestManager->getMinimumDistanceFromNodesToMouse(mousePos) < 10.))
    {
        QAction *deleteOneNodeAction = new QAction("Node", roiManagementMenu);
        connect(deleteOneNodeAction, &QAction::triggered, [=](){
            deleteNode(closestManager, &mousePos[0]);
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

        QAction *saveMaskAction = new QAction("Mask", saveMenu);
        connect(saveMaskAction, &QAction::triggered, [=](){
            saveMask(closestManager);
        });
        saveMenu->addAction(saveMaskAction);

        QAction *saveContourAction = new QAction("Contour", saveMenu);
        connect(saveContourAction, &QAction::triggered, [=](){
            saveContour(closestManager);
        });
        saveMenu->addAction(saveContourAction);


        connect(copyContourAction, &QAction::triggered, [=](){
            copyContour(closestManager);
        });

        if (closestManager != activeManager)
        {
            activationAction =  new QAction("Activate", &mainMenu);
            connect(activationAction, &QAction::triggered, [=](){
                activeManager = closestManager;
                enableOnlyActiveManager();
            });
        }

        changeMenu = changeLabelActions(closestManager);

        renameManagerAction->setDefaultWidget(updateNameManager(closestManager, &mainMenu));

        mainMenu.addAction(renameManagerAction);
        mainMenu.addAction(activationAction);
        mainMenu.addMenu(roiManagementMenu);
        mainMenu.addMenu(saveMenu);
        if ( !changeMenu->actions().isEmpty() )
        {
            mainMenu.addMenu(changeMenu);
        }
        mainMenu.addAction(copyContourAction);

        mainMenu.exec(mouseEvent->globalPos());
    }
    else
    {
        if (!colorMenu->isEmpty())
        {
            colorMenu->exec(mouseEvent->globalPos());
        }
    }

    delete colorMenu;
    delete roiManagementMenu;
    delete saveMenu;
    delete renameManagerAction;

    delete changeMenu;
    delete copyContourAction;
    delete activationAction;
    mainMenu.clear();
    return true;
}

medTagRoiManager *polygonEventFilter::addManagerToList(int label, QString labelName)
{
    QString name;
    if (predefinedLabels.isEmpty() || predefinedLabels.size()<=label)
    {
        name = labelName;
    }
    else
    {
        name = predefinedLabels.at(label);
    }
    medTagRoiManager * manager = new medTagRoiManager(currentView, this, colorsList[label], name);
    manager->setEnableInterpolation(enableInterpolation);
    managers.append(manager);
    connect(manager, SIGNAL(enableOtherViewsVisibility(bool)), this, SLOT(enableOtherViewsVisibility(bool)), Qt::UniqueConnection);
    connect(manager, SIGNAL(sendErrorMessage(QString)), this, SIGNAL(sendErrorMessage(QString)), Qt::UniqueConnection);
    activeManager = manager;

    enableOnlyActiveManager();
    return manager;
}

void polygonEventFilter::enableOnlyActiveManager()
{
    for (medTagRoiManager *manager : managers)
    {
        if ( manager != activeManager )
        {
            manager->setEnableInteraction(false);
        }
        else
        {
            manager->setEnableInteraction(true);
        }
    }
}

bool polygonEventFilter::addPointInContourWithLabel(QMouseEvent *mouseEvent)
{
    if (managers.size()==0)
    {
        activeManager = addManagerToList(0, QString("label-0"));
    }
    else
    {
        if (activeManager )
        {
            polygonRoi *roi = activeManager->existingRoiInSlice();
            if ( !roi )
            {
                activeManager->appendRoi();
            }
            else
            {
                if (roi->isClosed())
                {
                    double mousePos[2];
                    mousePos[0] = mouseEvent->x()*QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();
                    mousePos[1] = (currentView->viewWidget()->height()-mouseEvent->y()-1)*QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();
                }

            }
        }
    }
    return false;
}

void polygonEventFilter::createNewManager(int label)
{
    for (medTagRoiManager *manager : managers)
    {
        if (manager->getColor() == colorsList.at(label))
        {
            manager->appendRoi();
            activeManager = manager;
            enableOnlyActiveManager();
            return;
        }
    }
    activeManager = addManagerToList(label, QString("label-%1").arg(label));
}

QMenu *polygonEventFilter::createColorMenu(QList<QColor> colors, QStringList names)
{
    QMenu *menu = new QMenu;
    for (int i = 0; i<colors.size(); i++)
    {
        QPixmap pixmap(50,50);
        pixmap.fill(colors.at(i));
        QIcon icon(pixmap);
        QString name;
        if (names.isEmpty() || names.size()<=i)
        {
            name = QString::number(colorsList.indexOf(colors.at(i)));
        }
        else
        {
            name = names.at(i);
        }

        QAction *action = new QAction(icon, name, menu);
        connect(action, &QAction::triggered, [=](){
            createNewManager(colorsList.indexOf((colors.at(i))));
        });

        menu->addAction(action);
    }
    return menu;
}

QList<QColor> polygonEventFilter::updateColorsList(QList<QColor> colorsToExclude)
{
    QList<QColor> colors;
    if (!colorsToExclude.empty())
    {
        colors = getAvailableColors(colorsToExclude);
    }
    else
    {
        colors = colorsList;
    }
    if (colors.empty())
    {
        emit sendErrorMessage("no more labels available");
    }
    return colors;
}

QList<QColor> polygonEventFilter::getAvailableColors(QList<QColor> colorsToExclude)
{
    QList<QColor> colors = colorsList;
    for (QColor color : colorsToExclude)
    {
        if (colorsList.contains(color))
        {
            colors.removeOne(color);
        }
    }
    return colors;
}

void polygonEventFilter::removeObserver()
{
    if (currentView)
    {
        vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;
        view2d->RemoveObserver(observer);
    }
}

void polygonEventFilter::addObserver()
{
    observer = vtkSmartPointer<View2DObserver>::New();
    observer->setObject(this);
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;
    view2d->AddObserver(vtkImageView2D::SliceChangedEvent,observer,0);
}

void polygonEventFilter::Off()
{
    activateEventFilter = false;
    enableOnlyActiveManager();
    activateRepulsor(false);
    cursorState = CURSORSTATE::CS_CONTINUE;
}

void polygonEventFilter::On()
{
    activateEventFilter = true;
    enableOnlyActiveManager();

    cursorState = CURSORSTATE::CS_CONTINUE;
}

void polygonEventFilter::setEnableInterpolation(bool state)
{
    enableInterpolation = state;
    for (medTagRoiManager *manager : managers)
    {
        if (!state)
        {
            for (medAbstractImageView * v : otherViews)
            {
                manager->removeIntermediateContoursOtherView(v);
            }
        }
        manager->setEnableInterpolation(state);
    }
    enableOnlyActiveManager();
    manageTick();
}

void polygonEventFilter::addAlternativeViews(medAbstractImageView* view)
{
    otherViews.append(view);
}

void polygonEventFilter::enableOtherViewsVisibility(bool state)
{
    for (medAbstractImageView * v : otherViews)
    {
        for (medTagRoiManager *manager : managers)
        {
            manager->enableOtherViewVisibility(v, state);
        }
    }
}

void polygonEventFilter::clearAlternativeViews()
{
    otherViews.clear();
}

void polygonEventFilter::removeView()
{
    medAbstractView *viewClosed = qobject_cast<medAbstractView*>(QObject::sender());
    for (medAbstractImageView *v : otherViews)
    {
        if (viewClosed==v)
        {
            otherViews.removeOne(v);
        }
    }
    if ( otherViews.size()==0 )
        emit clearLastAlternativeView();
}

void polygonEventFilter::clearCopiedContours()
{
    copyNodesList.clear();
}

void polygonEventFilter::copyContour(medTagRoiManager *manager)
{
    copyNodesList.clear();
    QVector<QVector2D> coords = manager->copyContour();
    if (!coords.empty())
    {
        int label = colorsList.indexOf(manager->getColor()) + 1;
        medDisplayPosContours contours = medDisplayPosContours(label, coords);
        copyNodesList.append(contours);
    }
    return;
}

void polygonEventFilter::copyContours()
{
    copyNodesList.clear();
    for (medTagRoiManager *manager : managers)
    {
        if (manager->existingRoiInSlice())
        {
            QVector<QVector2D> coords = manager->copyContour();
            if (!coords.empty())
            {
                int label = colorsList.indexOf(manager->getColor()) + 1;
                medDisplayPosContours contours = medDisplayPosContours(label, coords);
                copyNodesList.append(contours);
            }
        }
    }
}

void polygonEventFilter::pasteContours()
{
    for (medDisplayPosContours &contours : copyNodesList)
    {
        QColor color = colorsList.at(contours.getLabel()-1);
        medTagRoiManager *manager = getManagerFromColor(color);
        if ( manager )
        {
            if ( !manager->existingRoiInSlice() )
            {
                manager->pasteContour(contours.getNodes());
            }
            else
            {
                emit sendErrorMessage("contour " + manager->getName() + " already exists in current slice ");
            }
        }
        enableOtherViewsVisibility(true);
        enableOnlyActiveManager();
    }
}

void polygonEventFilter::setPredefinedLabels(QList<QPair<QString, QColor>> labels)
{
    predefinedLabels.clear();
    colorsList = defaultColorsList;
    int idx = 0;
    for (QPair<QString, QColor> label : labels)
    {
        predefinedLabels << label.first;
        colorsList.replace(idx, label.second);
        idx++;
    }

    for ( medTagRoiManager *manager : managers)
    {
        for (int c=0; c<colorsList.size(); c++)
        {
            if ( manager->getColor()==colorsList.at(c) )
            {
                if (predefinedLabels.size()>c)
                {
                    manager->setName(predefinedLabels.at(c));
                }
                else
                {
                    manager->setName(QString("label-%1").arg(c));
                }
            }
        }
    }
}

medTagRoiManager *polygonEventFilter::getManagerFromColor(QColor color)
{
    for (medTagRoiManager *manager : managers)
    {
        if (manager->getColor() == color)
            return manager;
    }
    return nullptr;
}

void polygonEventFilter::activateRepulsor(bool state)
{
    isRepulsorActivated = state;

    vtkImageView2D *view2D =  static_cast<medVtkViewBackend*>(currentView->backend())->view2D;
    if (state)
    {
        cursorState = CURSORSTATE::CS_REPULSOR;
        vtkInteractorStyleImageView2D *interactorStyle2D = vtkInteractorStyleImageView2D::SafeDownCast(view2D->GetInteractor()->GetInteractorStyle());
        if (!interactorStyleRepulsor)
        {
            interactorStyleRepulsor = vtkInriaInteractorStylePolygonRepulsor::New();
        }
        interactorStyleRepulsor->SetLeftButtonInteraction(interactorStyle2D->GetLeftButtonInteraction());
        view2D->SetInteractorStyle(interactorStyleRepulsor);
        view2D->SetupInteractor(view2D->GetInteractor()); // to reinstall vtkImageView2D pipeline

        for (medTagRoiManager *manager: managers)
        {
            if (manager != activeManager)
            {
                manager->setEnableInteraction(false);
                polygonRoi *roi = manager->existingRoiInSlice();
                if ( roi )
                {
                    if (!roi->isClosed())
                    {
                        roi->getContour()->SetWidgetState(vtkContourWidget::Define);
                    }
                }
            }
        }
    }
    else
    {
        cursorState = CURSORSTATE::CS_MOUSE_EVENT;
        vtkInteractorStyleImageView2D *interactorStyle2D = vtkInteractorStyleImageView2D::New();
        if (!interactorStyleRepulsor)
        {
            interactorStyleRepulsor = vtkInriaInteractorStylePolygonRepulsor::New();
        }
        interactorStyle2D->SetLeftButtonInteraction(vtkInteractorStyleImageView2D::InteractionTypeNull);
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
    QString desc;
    if (contoursData.size()==1)
    {
        desc = QString("contour %1").arg(contoursData[0].getLabelName());
    }
    else
    {
        desc = QString("%1 contours").arg(contoursData.size());
    }
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
        int label = colorsList.indexOf(manager->getColor());

        auto ctr = manager->getContoursAsNodes();
        medTagContours contoursTag;

        QString name = QString("label-%1").arg(label);
        QString labelName = ( name == manager->getName() )?"undefined":manager->getName();
        contoursTag.setLabelName(labelName);
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

    if ( managers.size()==colorsList.size() )
    {
        emit sendErrorMessage("loadContours - unable to create a new manager.");
        return;
    }

    for (medTagContours &tagContours : tagContoursSet)
    {
        if ( managers.size()==colorsList.size() )
        {
            emit sendErrorMessage("loadContours - unable to create a new manager.");
            return;
        }
        int label = findAvailableLabel();
        if ( label == -1 )
        {
            emit sendErrorMessage("Unable to load contours. No label available.");
            return;
        }
        QString labelName = (tagContours.getLabelName() == "undefined")?QString("label-%1").arg(label):tagContours.getLabelName();
        for (medTagRoiManager *manager : managers)
        {
            if (manager->getName()==labelName)
            {
                emit sendErrorMessage("loadContours - unable to create a new manager.");
                return;
            }
        }

        medTagRoiManager *manager = addManagerToList(label, labelName);
        manager->loadContours(tagContours.getContourNodes());
        activeManager = manager;
        enableOnlyActiveManager();
    }
}

int polygonEventFilter::findAvailableLabel()
{
    QList<QColor> usedColors;
    for ( medTagRoiManager* manager : managers )
    {
        usedColors.append(manager->getColor());
    }
    for (QColor color : colorsList)
    {
        if (!usedColors.contains(color))
        {
            return colorsList.indexOf(color);
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

void polygonEventFilter::deleteNode(double *mousePosition)
{
    medTagRoiManager *closestManager = getClosestManager(mousePosition);
    if (closestManager && closestManager->getMinimumDistanceFromNodesToMouse(mousePosition, false) < 10. )
    {
        deleteNode(closestManager, mousePosition);
    }

}

void polygonEventFilter::deleteNode(medTagRoiManager *manager, const double *mousePos)
{
    manager->deleteNode(mousePos[0], mousePos[1]);

    if ( manager->getRois().size()==1 && manager->getRois()[0]->getNumberOfNodes()==1)
    {
        for (medAbstractImageView * v : otherViews)
        {
            manager->enableOtherViewVisibility(v, false);
        }
    }
    else if (manager->getRois().isEmpty())
    {
        deleteLabel(manager);
    }
    else if (!manager->existingRoiInSlice())
    {
        deleteContour(manager);
    }
    else
    {
        manageTick();
        manageButtonsState();
    }
}

void polygonEventFilter::deleteContour(medTagRoiManager *manager)
{
    for (medAbstractImageView * v : otherViews)
    {
        manager->removeContourOtherView(v);
    }
    manager->deleteContour();
    if ( manager->getRois().empty() )
    {
        deleteLabel(manager);
    }
    else
    {
        manageTick();
        manageButtonsState();
    }
}


void polygonEventFilter::deleteLabel(medTagRoiManager *manager)
{
    for (medAbstractImageView * v : otherViews)
    {
        manager->enableOtherViewVisibility(v, false);
    }
    managers.removeOne(manager);
    manager->removeAllTick();

    delete manager;
    if (activeManager==manager)
    {
        activeManager = nullptr;
    }
    manageTick();
    manageButtonsState();
    currentView->render();
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

bool polygonEventFilter::isActiveContourInSlice()
{
    bool retVal = false;
    if ( activeManager && activeManager->existingRoiInSlice() )
    {
        retVal = true;
    }
    return retVal;
}

void polygonEventFilter::saveMask(medTagRoiManager *manager)
{
    int label = colorsList.indexOf(manager->getColor()) + 1;
    manager->createMaskWithLabel(label);
}

void polygonEventFilter::saveContour(medTagRoiManager *manager)
{
    QVector<medTagContours> contoursData;
    vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
    append->SetUserManagedInputs(true);
    append->SetNumberOfInputs(managers.size());

    int label = colorsList.indexOf(manager->getColor());

    auto ctr = manager->getContoursAsNodes();
    medTagContours contoursTag;
    QString name = QString("label-%1").arg(label);
    QString labelName = ( name == manager->getName() )?"undefined":manager->getName();
    contoursTag.setLabelName(labelName);
    contoursTag.setContourNodes(ctr);
    contoursData.append(contoursTag);

    auto pd = manager->getContoursAsPolyData(label);
    append->SetInputDataByNumber(0, pd);

    append->Update();
    vtkMetaDataSet *outputDataSet = vtkMetaSurfaceMesh::New();
    outputDataSet->SetDataSet(append->GetOutput());

    saveContoursAsMedAbstractData(outputDataSet, contoursData);
}
