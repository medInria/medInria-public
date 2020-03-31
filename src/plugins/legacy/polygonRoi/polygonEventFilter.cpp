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

// vtk
#include <QWidgetAction>
#include <vtkAppendPolyData.h>
#include <vtkRenderWindowInteractor.h>

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
        activeManager(nullptr)
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

    vtkSmartPointer<View2DObserver> observer = vtkSmartPointer<View2DObserver>::New();
    observer->setObject(this);
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;
    view2d->AddObserver(vtkImageView2D::SliceChangedEvent,observer,0);
    // interactors
    interactorStyleRepulsor = vtkInriaInteractorStylePolygonRepulsor::New();
}

polygonEventFilter::~polygonEventFilter()
{
    if (interactorStyleRepulsor)
    {
        interactorStyleRepulsor->Delete();
    }
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

    if (currentView != v )
    {
        return updateMainViewOnChosenSlice(v, mouseEvent);
    }
    else
    {
        if (isRepulsorActivated && activateEventFilter)
        {
            if ( !isContourInSlice() || isOnlyOneNodeInSlice())
            {
                return false;
            }

            if (interactorStyleRepulsor != nullptr)
            {
                interactorStyleRepulsor->GetManager()->SetMasterRoi(true);
                interactorStyleRepulsor->GetManager()->interpolateIfNeeded();
                enableOtherViewsVisibility(true);
                manageTick();
            }
        }
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
        if (isContourInSlice())
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


QWidgetAction * polygonEventFilter::updateNameManager(medTagRoiManager* closestManager, QMenu *mainMenu)
{
    QLineEdit *renameManager = new QLineEdit(closestManager->getName());
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
       closestManager->setName(renameManager->text());
    });
    QWidgetAction *renameManagerAction = new QWidgetAction(mainMenu);
    renameManagerAction->setDefaultWidget(renameManager);

    return renameManagerAction;
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

    QList<QColor> colors = updateColorsList(colorsToExclude);
    QMenu *colorMenu = createColorMenu(colors);
    QMenu *roiManagementMenu = new QMenu("Remove: ");
    QMenu *saveMenu = new QMenu("Save as: ");
    if (closestManager && closestManager->getMinimumDistanceFromNodesToEventPosition(mousePos) < 10 )
    {
        closestManager->select(true);
        mainMenu.addMenu(roiManagementMenu);
        QAction *deleteOneNodeAction = new QAction("Node", roiManagementMenu);
        connect(deleteOneNodeAction, &QAction::triggered, [=](){
            deleteNode(closestManager, mouseEvent);
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

        QAction *copyContourAction = new QAction("Copy", &mainMenu);
        connect(copyContourAction, &QAction::triggered, [=](){
            copyContour(closestManager);
        });

        QWidgetAction *renameManagerAction = updateNameManager(closestManager, &mainMenu);

        mainMenu.addAction(renameManagerAction);
        mainMenu.addMenu(roiManagementMenu);
        mainMenu.addMenu(saveMenu);
        mainMenu.addAction(copyContourAction);

    }
    else
    {
        colorMenu->setTitle("Label");
        mainMenu.addMenu(colorMenu);
    }


    QAction *action = mainMenu.exec(mouseEvent->globalPos());
    if ( !action)
    {
        if ( closestManager)
            closestManager->select(false);
    }

    delete colorMenu;
    delete roiManagementMenu;
    delete saveMenu;

    return true;
}

medTagRoiManager *polygonEventFilter::addManagerToList(int label, QString labelName)
{
    medTagRoiManager * manager = new medTagRoiManager(currentView, this, colorList[label], labelName);
    manager->setEnableInterpolation(enableInterpolation);
    managers.append(manager);
    connect(manager, SIGNAL(enableOtherViewsVisibility(bool)), this, SLOT(enableOtherViewsVisibility(bool)), Qt::UniqueConnection);
    return manager;
}

bool polygonEventFilter::addPointInContourWithLabel(QMouseEvent *mouseEvent)
{
    bool res = false;
    if (managers.size()==0)
    {
        activeManager = addManagerToList(0, QString("label-0"));
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
                {
                    colorsToExclude.append(manager->getColor());
                }
                else
                {
                    cursorState = CURSORSTATE::CS_NONE;
                    return res;
                }
            }
            else
            {
                if (!manager->isSameSliceOrientation())
                {
                    colorsToExclude.append(manager->getColor());
                }
            }
        }


        if (activeManager )
        {
            if (!activeManager->existingRoiInSlice())
            {
                activeManager->appendRoi();
            }
            else
            {
                QList<QColor> colors = updateColorsList(colorsToExclude);
                QMenu * menu = createColorMenu(colors);
                if ( !menu->exec(mouseEvent->globalPos()))
                {
                    res = true;
                }
                delete menu;
            }
        }
        else
        {
            QList<QColor> colors = updateColorsList(colorsToExclude);
            QMenu * menu = createColorMenu(colors);
            if ( !menu->exec(mouseEvent->globalPos()))
            {
                res = true;
            }
            delete menu;
        }

        return res;
    }
}

void polygonEventFilter::createNewManager(int label)
{
    for (medTagRoiManager *manager : managers)
    {
        if (manager->getColor() == colorList.at(label))
        {
            manager->appendRoi();
            activeManager = manager;
            return;
        }
    }
    activeManager = addManagerToList(label, QString("label-%1").arg(label));
}

QMenu *polygonEventFilter::createColorMenu(QList<QColor> colors)
{
    QMenu *menu = new QMenu;
    for (int i = 0; i<colors.size(); i++)
    {
        QPixmap pixmap(50,50);
        pixmap.fill(colors.at(i));
        QIcon icon(pixmap);
        QAction *action = new QAction(icon, QString::number(colorList.indexOf(colors.at(i))), menu);
        connect(action, &QAction::triggered, [=](){
            createNewManager(colorList.indexOf((colors.at(i))));
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
    activateEventFilter = false;
    for (medTagRoiManager *manager : managers)
    {
        manager->setContourEnabled(false);
    }
    activateRepulsor(false);
    cursorState = CURSORSTATE::CS_CONTINUE;
}

void polygonEventFilter::On()
{
    activateEventFilter = true;
    for (medTagRoiManager *manager : managers)
    {
        manager->setContourEnabled(true);
    }
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
        int label = colorList.indexOf(manager->getColor()) + 1;
        medDisplayPosContours contours = medDisplayPosContours(label, coords);
        copyNodesList.append(contours);
    }
    manager->select(false);
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
                int label = colorList.indexOf(manager->getColor()) + 1;
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
        QColor color = colorList.at(contours.getLabel()-1);
        medTagRoiManager *manager = getManagerFromColor(color);
        if ( manager )
        {
            if ( !manager->existingRoiInSlice() )
            {
                manager->pasteContour(contours.getNodes());
            }
            else
            {
                qDebug()<<"contour with label "<< contours.getLabel() << " already exist in slice ";
            }
        }
        enableOtherViewsVisibility(true);
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
    QString desc = QString("contours: ");
    int num = 0;
    for (medTagContours contours : contoursData)
    {
        if (contours.getLabelName()=="undefined")
        {
            desc.append(QString("%1 ").arg(num));
            num++;
        }
        else
        {
            desc.append(QString("%1 ").arg(contours.getLabelName()));
        }
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
        int label = colorList.indexOf(manager->getColor());

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

    if ( managers.size()==colorList.size() )
    {
        qDebug()<<metaObject()->className()<<":: loadContours - unable to create a new manager.";
        return;
    }

    for (medTagContours &tagContours : tagContoursSet)
    {
        if ( managers.size()==colorList.size() )
        {
            qDebug()<<metaObject()->className()<<":: loadContours - unable to create a new manager.";
            return;
        }
        int label = findAvailableLabel();
        if ( label == -1 )
        {
            return;
        }
        for (medTagRoiManager *manager : managers)
        {
            if (manager->getName()==tagContours.getLabelName())
            {
                qDebug()<<metaObject()->className()<<":: loadContours - unable to create a new manager.";
                return;
            }
        }
        QString labelName = (tagContours.getLabelName() == "undefined")?QString("label-%1").arg(label):tagContours.getLabelName();

        medTagRoiManager *manager = addManagerToList(label, labelName);
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

void polygonEventFilter::deleteNode(medTagRoiManager *manager, QMouseEvent *mouseEvent)
{
    double X = mouseEvent->x()*QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();
    double Y = (currentView->viewWidget()->height()-mouseEvent->y()-1)*QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();

    manager->deleteNode(X, Y);

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
        manager->select(false);
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
    for (medTagRoiManager *manager : managers)
    {
        if (manager->existingRoiInSlice())
        {
            return true;
        }
    }
    return false;
}

bool polygonEventFilter::isOnlyOneNodeInSlice()
{
    bool res = false;
    for (medTagRoiManager *manager : managers)
    {
        polygonRoi *roi = manager->existingRoiInSlice();
        if (roi)
        {
            if (roi->getNumberOfNodes()==1)
            {
                res = true;
            }
            else if ( roi->getNumberOfNodes()>1)
            {
                return false;
            }
        }
    }
    return res;
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

    int label = colorList.indexOf(manager->getColor());

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
    manager->select(false);
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
