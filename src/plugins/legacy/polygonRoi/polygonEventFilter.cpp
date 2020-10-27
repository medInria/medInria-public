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
#include <QApplication>
#include <QDesktopWidget>
#include <QGuiApplication>
#include <QLineEdit>
#include <QScreen>
#include <QWidgetAction>
#include <medViewContainer.h>

// vtk
#include <QLabel>
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

polygonEventFilter::polygonEventFilter(medAbstractImageView *view) :
        medViewEventFilter(), currentView(view), cursorState(CURSORSTATE::CS_MOUSE_EVENT), isRepulsorActivated(false), activateEventFilter(true), enableInterpolation(true),
        activeManager(nullptr), observer(nullptr)
{
    activeManager = nullptr;
    activeName = QString();
    activeColor = QColor::Invalid;
    scoreState = false;
    pirads.append(QPair<QString, QColor>("PIRADS3", Qt::yellow));
    pirads.append(QPair<QString, QColor>("PIRADS4", QColor(139, 69, 19)));
    pirads.append(QPair<QString, QColor>("PIRADS5", Qt::darkRed));

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
    removeObserver();
    enableOtherViewsVisibility(false);
    otherViews.clear();
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
    activeManager = nullptr;
    activeColor = QColor::Invalid;
    activeName = QString();
    currentView = nullptr;
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
        if ( keyEvent->key() == Qt::Key::Key_A )
        {
            activateContour(savedMousePosition);
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
        if ( keyEvent->key() == Qt::Key::Key_S )
        {
            switchContourColor(savedMousePosition);
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
    int devicePixelRatio = medUtilities::getDevicePixelRatio(mouseEvent);

    savedMousePosition[0] = mouseEvent->x()*devicePixelRatio;
    savedMousePosition[1] = (currentView->viewWidget()->height()-mouseEvent->y()-1)*devicePixelRatio;

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
        if (!activeManager)
        {
            emit sendErrorMessage(QString("Select a label in list to be able to use repulsor tool."));
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

QLineEdit * polygonEventFilter::changeManagerName(medTagRoiManager* closestManager, QMenu *mainMenu)
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
               emit sendErrorMessage("An other contour has already the same name: " + renameManager->text());
               return;
           }
       }
       closestManager->setName(renameManager->text());
       medContourSharedInfo info = medContourSharedInfo(closestManager->getName(), closestManager->getColor());
       info.setChangeName(true);
       emit sendContourInfoToListWidget(info);
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
    QMenu *changeMenu = new QMenu("Change Label");
    for (medTagRoiManager *manager : managers)
    {
        if (manager!= closestManager && !manager->existingRoiInSlice())
        {
            QPixmap pixmap(20,20);
            pixmap.fill(manager->getColor());
            QAction *action = new QAction(pixmap, manager->getName(), changeMenu);
            connect(action, &QAction::triggered, [=](){
                polygonRoi *roi = closestManager->existingRoiInSlice();
                QVector<QVector2D> nodes = roi->copyContour();
                polygonRoi *roiToAdd = manager->appendRoi();
                roiToAdd->pasteContour(nodes);
                deleteContour(closestManager);
                activeManager = manager;
                enableActiveManagerIfExists();
                medContourSharedInfo info = medContourSharedInfo(activeManager->getName(), activeManager->getColor(), true);
                emit sendContourInfoToListWidget(info);
            });
            changeMenu->addAction(action);
        }
    }
    return changeMenu;
}

bool polygonEventFilter::rightButtonBehaviour(medAbstractView *view, QMouseEvent *mouseEvent)
{
    QMenu mainMenu(currentView->viewWidget());

    int devicePixelRatio = medUtilities::getDevicePixelRatio(mouseEvent);

    double mousePos[2];
    mousePos[0] = mouseEvent->x()*devicePixelRatio;
    mousePos[1] = (currentView->viewWidget()->height()-mouseEvent->y()-1)*devicePixelRatio;
    medTagRoiManager * closestManager = getClosestManager(mousePos);

    QMenu *roiManagementMenu = new QMenu("Remove");
    QMenu *saveMenu = new QMenu("Save as");
    QMenu *changeMenu = nullptr;
    QWidgetAction *renameManagerAction = new QWidgetAction(&mainMenu);
    QAction *copyContourAction = new QAction("Copy", &mainMenu);
    QAction *activationAction = nullptr;
    QMenu *scoreMenu = new QMenu("Set Score");
    if (closestManager && (closestManager->getMinimumDistanceFromNodesToMouse(mousePos) < 10.))
    {
        QAction *deleteOneNodeAction = new QAction("Node", roiManagementMenu);
        connect(deleteOneNodeAction, &QAction::triggered, [this, &closestManager, &mousePos](){
            deleteNode(closestManager, mousePos);
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
                enableActiveManagerIfExists();
                medContourSharedInfo info = medContourSharedInfo(activeManager->getName(), activeManager->getColor(), true);
                emit sendContourInfoToListWidget(info);
            });
        }

        changeMenu = changeLabelActions(closestManager);

        renameManagerAction->setDefaultWidget(changeManagerName(closestManager, &mainMenu));


        mainMenu.addAction(renameManagerAction);
        if (!closestManager->getOptName().isEmpty())
        {
            QLabel *name = new QLabel(closestManager->getOptName());
            QFont font = name->font();
            font.setWeight(QFont::Black);
            font.setPointSize(15);
            font.setUnderline(true);
            name->setFont(font);
            name->setAlignment(Qt::AlignCenter);
            QString style = "QLabel { color: rgb(%1, %2, %3); }";
            name->setStyleSheet(style.arg(closestManager->getOptColor().red()).
                                arg(closestManager->getOptColor().green()).
                                arg(closestManager->getOptColor().blue()));
            QWidgetAction *widAct = new QWidgetAction(&mainMenu);
            widAct->setDefaultWidget(name);
            mainMenu.addAction(widAct);
        }
        if ( activationAction )
        {
            mainMenu.addAction(activationAction);
        }
        if (closestManager->hasScore())
        {
            for (QPair<QString, QColor> pirad : pirads)
            {
                scoreMenu->addAction(createScoreAction(closestManager, pirad.first, pirad.second));
            }
            mainMenu.addMenu(scoreMenu);
        }
        mainMenu.addMenu(roiManagementMenu);
        mainMenu.addMenu(saveMenu);
        if ( !changeMenu->actions().isEmpty() )
        {
            mainMenu.addMenu(changeMenu);
        }
        mainMenu.addAction(copyContourAction);

        mainMenu.exec(mouseEvent->globalPos());
    }

    delete roiManagementMenu;
    delete saveMenu;
    delete renameManagerAction;
    delete scoreMenu;

    delete changeMenu;
    delete copyContourAction;
    delete activationAction;
    mainMenu.clear();
    return true;
}


void polygonEventFilter::enableActiveManagerIfExists()
{
    for (medTagRoiManager *manager : managers)
    {
        if ( manager != activeManager )
        {
            manager->activateContours(false);
        }
        else
        {
            manager->activateContours(true);
        }
    }
}

medTagRoiManager *polygonEventFilter::createManager()
{
    medTagRoiManager * manager = new medTagRoiManager(currentView, this, activeColor, activeName);
    manager->setScoreState(scoreState);
    manager->setEnableInterpolation(enableInterpolation);
    managers.append(manager);
    connect(manager, SIGNAL(enableOtherViewsVisibility(bool)), this, SLOT(enableOtherViewsVisibility(bool)), Qt::UniqueConnection);
    connect(manager, SIGNAL(sendErrorMessage(QString)), this, SIGNAL(sendErrorMessage(QString)), Qt::UniqueConnection);
    activeManager = manager;

    enableActiveManagerIfExists();
    return manager;
}

bool polygonEventFilter::addPointInContourWithLabel(QMouseEvent *mouseEvent)
{
    if (activeManager)
    {
        polygonRoi *roi = activeManager->existingRoiInSlice();
        if ( !roi )
        {
            activeManager->appendRoi();
        }
    }
    else
    {
        if (activeColor != QColor::Invalid && activeName != QString())
        {
            createManager();
        }
    }
    return false;
}

QAction *polygonEventFilter::createScoreAction(medTagRoiManager *manager, QString score, QColor color)
{
    QPixmap pixmap(50,50);
    pixmap.fill(color);
    QAction *action  = new QAction(pixmap, score);
    connect(action, &QAction::triggered, [=](){
        manager->setOptionalNameWithColor(score, color);
        manager->switchColor();
        activeManager = manager;
        enableActiveManagerIfExists();
        enableOtherViewsVisibility(true);
        medContourSharedInfo info = medContourSharedInfo(manager->getName(), manager->getColor(), true);
        info.setAdditionalNameAndColor(manager->getOptName(), manager->getOptColor());
        info.setCheckState(true);
        emit sendContourInfoToListWidget(info);
    });
    return action;
}

void polygonEventFilter::removeObserver()
{
    if (currentView)
    {
        vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;
        view2d->RemoveObserver(observer);
        observer = nullptr;
    }
}

void polygonEventFilter::addObserver()
{
    if (!observer)
    {
        observer = vtkSmartPointer<View2DObserver>::New();
        observer->setObject(this);
        vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;
        view2d->AddObserver(vtkImageView2D::SliceChangedEvent,observer,0);
    }
}

void polygonEventFilter::Off()
{
    if (currentView)
    {
        activeColor = QColor::Invalid;
        activeName = QString();
        activateEventFilter = false;
        enableActiveManagerIfExists();
        activateRepulsor(false);
        cursorState = CURSORSTATE::CS_CONTINUE;
    }
}

void polygonEventFilter::On()
{
    activateEventFilter = true;
    enableActiveManagerIfExists();

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
    enableActiveManagerIfExists();
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

void polygonEventFilter::clearAlternativeView()
{
    for (medAbstractImageView *v : otherViews)
    {
        if (v->identifier()=="")
        {
            otherViews.removeOne(v);
        }
    }
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
        int label = managers.indexOf(manager);
        medDisplayPosContours contours = medDisplayPosContours(label, coords);
        copyNodesList.append(contours);
    }
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
                int label =  managers.indexOf(manager);;
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
        medTagRoiManager *mgr = managers.at(contours.getLabel());
        QColor color = mgr->getColor();
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
        enableActiveManagerIfExists();
    }
}

medTagRoiManager *polygonEventFilter::findManagerWithColor(QColor color)
{
    medTagRoiManager *mgr = nullptr;
    for (medTagRoiManager *manager : managers)
    {
        if (manager->getColor() == color)
        {
            mgr = manager;
        }
    }
    return mgr;
}

void polygonEventFilter::receiveDatasFromToolbox(QList<medContourSharedInfo> infos)
{
    for (medContourSharedInfo info: infos)
    {
        medTagRoiManager *mgr = findManagerWithColor(info.getColor());
        if (mgr)
        {
            mgr->setName(info.getName());
            if (mgr==activeManager)
            {
                info.setSelected(true);
                emit sendContourInfoToListWidget(info);
            }
        }
    }
}

void polygonEventFilter::receiveActivationState(medContourSharedInfo info)
{
    activeManager = nullptr;
    medTagRoiManager *manager = findManagerWithColor(info.getColor());
    if (manager)
    {
        activeManager = manager;
    }
    activeColor = info.getColor();
    activeName = info.getName();
    scoreState = info.hasScore();
    enableActiveManagerIfExists();
}

void polygonEventFilter::receiveContourState(medContourSharedInfo info)
{
    activeManager = nullptr;
    medTagRoiManager *manager = findManagerWithColor(info.getColor());
    if (manager)
    {
        manager->setScoreState(info.hasScore());
        manager->setName(info.getName());
        activeManager = manager;
        if (info.getAdditionalColor()!=QColor::Invalid && info.getAdditionalName()==QString())
        {
            manager->setColor(info.getAdditionalColor());
            manager->changeContoursColor(info.getAdditionalColor());
        }
        else if (manager->getOptName()!=QString() && manager->getOptColor()!=QColor::Invalid)
        {
            if (!info.hasScore())
            {
                manager->changeContoursColor(manager->getColor());
                manager->setOptionalNameWithColor(QString(), QColor::Invalid);
            }
            else
            {
                QColor color = (info.checkState())?manager->getOptColor():manager->getColor();
                manager->changeContoursColor(color);
            }
        }
        if (currentView &&
                manager->getName() != "WG (whole gland)" &&
                manager->getName() != "TZ (transitional zone)")
        {
            vtkImageView2D *view2D =  static_cast<medVtkViewBackend*>(currentView->backend())->view2D;

            int newSlice = manager->getClosestSliceFromCurrent2DView();
            if (newSlice!=-1 && newSlice!=view2D->GetSlice())
            {
                view2D->SetSlice(newSlice);
                view2D->Render();
            }
        }
        enableOtherViewsVisibility(true);
    }

    if (info.getAdditionalColor()!=QColor::Invalid && info.getAdditionalName()==QString())
    {
        activeColor = info.getAdditionalColor();
    }
    else
    {
        activeColor = info.getColor();
    }
    activeName = info.getName();
    scoreState = info.hasScore();
    enableActiveManagerIfExists();
}

void polygonEventFilter::receiveContourName(medContourSharedInfo info)
{
    activeColor = info.getColor();
    activeName = info.getName();
    medTagRoiManager *manager = findManagerWithColor(activeColor);
    if (manager)
    {
        manager->setName(info.getName());
        activeManager = manager;
        enableActiveManagerIfExists();
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
        interactorStyleRepulsor->SetCurrentView(currentView);
        interactorStyleRepulsor->SetManager(activeManager);
        interactorStyleRepulsor->SetLeftButtonInteraction(interactorStyle2D->GetLeftButtonInteraction());
        view2D->SetInteractorStyle(interactorStyleRepulsor);
        view2D->SetupInteractor(view2D->GetInteractor()); // to reinstall vtkImageView2D pipeline

        for (medTagRoiManager *manager: managers)
        {
            if (manager != activeManager)
            {
                manager->activateContours(false);
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
        if (manager->getRois().size()!=1 || manager->getRois()[0]->isClosed()==true)
        {
            saveMask(manager);
        }
    }
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
        if (manager->getRois().size()!=1 || manager->getRois()[0]->isClosed()==true)
        {
            int label = managers.indexOf(manager);

            auto ctr = manager->getContoursAsNodes();
            medTagContours contoursTag;

            QString labelName = manager->getName();
            contoursTag.setLabelName(labelName);
            if (manager->hasScore() && manager->getOptName()!=QString())
            {
                contoursTag.setScore(manager->getOptName());
            }
            contoursTag.setContourNodes(ctr);
            contoursData.append(contoursTag);

            auto pd = manager->getContoursAsPolyData(label);
            append->SetInputDataByNumber(num, pd);
            num++;
        }
    }
    append->Update();
    vtkMetaDataSet *outputDataSet = vtkMetaSurfaceMesh::New();
    outputDataSet->SetDataSet(append->GetOutput());

    emit saveContours(currentView, outputDataSet, contoursData);
    return;
}


void polygonEventFilter::loadContours(medTagContours tagContours, QColor color)
{
    if (managers.size()==9)
    {
        emit sendErrorMessage("loadContours - unable to create a new manager.");
        return;
    }

    activeName = tagContours.getLabelName();
    activeColor = color;
    if (findManagerWithColor(color) != nullptr)
    {
        emit sendErrorMessage(QString("loadContours - contour with color %1 already exists").arg(color.toRgb().name()));
        return;
    }
    medTagRoiManager *mgr = createManager();
    mgr->loadContours(tagContours.getContourNodes());
    if (tagContours.getScore() != QString())
    {
        for (QPair<QString, QColor> pirad : pirads)
        {
            if (pirad.first == tagContours.getScore())
            {
                mgr->setOptionalNameWithColor(pirad.first, pirad.second);
                break;
            }
        }
    }
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

void polygonEventFilter::activateContour( double *mousePosition)
{
    medTagRoiManager *closestManager = getClosestManager(mousePosition);
    if (closestManager && closestManager->getMinimumDistanceFromNodesToMouse(mousePosition, true) < 10. )
    {
        activeManager = closestManager;
        enableActiveManagerIfExists();
        medContourSharedInfo info = medContourSharedInfo(activeManager->getName(), activeManager->getColor(), true);
        emit sendContourInfoToListWidget(info);
    }
}

void polygonEventFilter::switchContourColor(double *mousePosition)
{
    medTagRoiManager *closestManager = getClosestManager(mousePosition);
    if (closestManager && closestManager->getMinimumDistanceFromNodesToMouse(mousePosition) < 10. )
    {
        QColor color = closestManager->switchColor();
        if (color != QColor::Invalid)
        {
            activeManager = closestManager;
            enableActiveManagerIfExists();
            medContourSharedInfo info = medContourSharedInfo(closestManager->getName(),
                                                 closestManager->getColor(),
                                                 true);

            bool checkState = (color==closestManager->getOptColor())?true:false;
            info.setAdditionalNameAndColor(closestManager->getOptName(), closestManager->getOptColor());
            info.setCheckState(checkState);
            emit sendContourInfoToListWidget(info);
            enableOtherViewsVisibility(true);
        }
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

void polygonEventFilter::deleteLabel(medContourSharedInfo info)
{
    for (medTagRoiManager *manager : managers)
    {
        if (manager->getName()==info.getName())
        {
            deleteLabel(manager);
            activeManager = nullptr;
            activeName = QString();
            activeColor = QColor::Invalid;
        }
    }
}

void polygonEventFilter::deleteLabel(medTagRoiManager *manager)
{
    for (medAbstractImageView * v : otherViews)
    {
        manager->enableOtherViewVisibility(v, false);
    }
    if (manager->hasScore() && manager->getOptName()!=QString())
    {
        medContourSharedInfo info = medContourSharedInfo(manager->getName(), manager->getColor());
        info.setUpdateDesc(true);
        info.setAdditionalNameAndColor(manager->getName(), QColor::Invalid);
        info.setScoreInfo(false);
        emit sendContourInfoToListWidget(info);
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
    int label = managers.indexOf(manager);
    manager->createMaskWithLabel(label);
}

void polygonEventFilter::saveContour(medTagRoiManager *manager)
{
    QVector<medTagContours> contoursData;
    vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
    append->SetUserManagedInputs(true);
    append->SetNumberOfInputs(managers.size());

    int label = managers.indexOf(manager);

    auto ctr = manager->getContoursAsNodes();
    medTagContours contoursTag;
    QString labelName = manager->getName();
    contoursTag.setLabelName(labelName);
    if (manager->hasScore() && manager->getOptName()!=QString())
    {
        contoursTag.setScore(manager->getOptName());
    }
    contoursTag.setContourNodes(ctr);
    contoursData.append(contoursTag);

    auto pd = manager->getContoursAsPolyData(label);
    append->SetInputDataByNumber(0, pd);

    append->Update();
    vtkMetaDataSet *outputDataSet = vtkMetaSurfaceMesh::New();
    outputDataSet->SetDataSet(append->GetOutput());

    emit saveContours(currentView, outputDataSet, contoursData);
}
