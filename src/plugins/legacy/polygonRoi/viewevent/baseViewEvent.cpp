/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "baseViewEvent.h"


// medInria

#include <medUtilities.h>
#include <medVtkViewBackend.h>

// Qt

// vtk
#include <vtkAppendPolyData.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <medViewParameterGroupL.h>
#include <medParameterGroupManagerL.h>
#include <medAbstractDataFactory.h>
#include <vtkMetaSurfaceMesh.h>
#include <medMetaDataKeys.h>
#include <medDataManager.h>

baseViewEvent::baseViewEvent(medAbstractImageView *iView, polygonRoiToolBox *toolBox) :
        medViewEventFilter(),
        currentLabel(nullptr), enableInterpolation(true),
        pToolBox(toolBox), cursorState(CURSORSTATE::CS_DEFAULT), isRepulsorActivated(false)
{
    currentView = iView;

    currentLabel = nullptr;

    // interactors
    interactorStyleRepulsor = vtkInriaInteractorStylePolygonRepulsor::New();
}

baseViewEvent::~baseViewEvent()
{
    removeViewInteractor();
    manageTickVisibility(false);
    for (polygonLabel *label: labelList)
    {
        delete label;
    }
    labelList.clear();
    cursorState = CURSORSTATE::CS_DEFAULT;
    isRepulsorActivated = false;
    if (interactorStyleRepulsor)
    {
        interactorStyleRepulsor->Delete();
        interactorStyleRepulsor = nullptr;
    }
    currentLabel = nullptr;
    currentView = nullptr;
}

bool baseViewEvent::eventFilter(QObject *obj, QEvent *event)
{
    if (!currentView)
        return false;

    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;

    auto keyEvent = dynamic_cast<QKeyEvent*>(event);
    auto mouseEvent = dynamic_cast<QMouseEvent*>(event);

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
            if (currentView)
            {
                view2d->SetSlice(view2d->GetSlice() + 1);
                view2d->Render();
            }
        }
        if ( keyEvent->key() == Qt::Key::Key_Left )
        {
            if (currentView)
            {
                view2d->SetSlice(view2d->GetSlice()-1);
                view2d->Render();
            }
        }
        if ( keyEvent->key() == Qt::Key::Key_S )
        {
            switchContourColor(savedMousePosition);
        }
    }
    return event->isAccepted();
}

bool baseViewEvent::mouseReleaseEvent(medAbstractView *view, QMouseEvent *mouseEvent)
{
    if ( !view || ! currentView )
        return false;

    if (currentView != view)
        return false;


    if (isRepulsorActivated )
    {
        if ( !isActiveContourInSlice() || (currentLabel && currentLabel->roiOpenInSlice()))
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
            manageTickVisibility(true);
        }
    }

    return false;
}

bool baseViewEvent::mouseMoveEvent(medAbstractView *view, QMouseEvent *mouseEvent)
{
    if ( !view )
        return false;
    if (view != currentView)
    {
        return true;
    }
#if QT_VERSION > QT_VERSION_CHECK(5, 10, 0)
    qreal devicePixelRatio = QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();
#else
    int screenNumber = QApplication::desktop()->screenNumber(mouseEvent->globalPos());
    int devicePixelRatio = QGuiApplication::screens().at(screenNumber)->devicePixelRatio();
#endif

    savedMousePosition[0] = mouseEvent->x()*devicePixelRatio;
    savedMousePosition[1] = (view->viewWidget()->height()-mouseEvent->y()-1)*devicePixelRatio;

    if (currentLabel)
    {
        if (currentLabel->getMinimumDistanceFromNodesToMouse(savedMousePosition, false) < 20.)
        {
            setCustomCursor(view);
        }
        else
        {
            view->viewWidget()->setCursor(Qt::ArrowCursor);
        }
    }
    else
    {
        view->viewWidget()->setCursor(Qt::ArrowCursor);
    }
    return false;
}

bool baseViewEvent::mousePressEvent(medAbstractView * view, QMouseEvent *mouseEvent)
{
    if ( !view || ! currentView )
        return false;

    if (currentView != view)
        return false;

    if (mouseEvent->button()==Qt::LeftButton)
    {
        leftButtonBehaviour(view, mouseEvent);
        return false;
    }
    else if (mouseEvent->button()==Qt::RightButton)
    {
        return rightButtonBehaviour(view, mouseEvent);
    }
    return false;
}

void baseViewEvent::leftButtonBehaviour(medAbstractView *view, QMouseEvent *mouseEvent)
{
    if ( !view || ! currentView )
        return;

    if (currentView != view)
        return;

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

    switch (cursorState)
    {
        case CURSORSTATE::CS_DEFAULT:
        {
            bool isRoiOpen = false;
            for (polygonLabel *label : labelList)
            {
                polygonRoi *roi = label->roiOpenInSlice();
                if (roi)
                {
                    roi->getContour()->SetWidgetState(vtkContourWidget::Define);

                    roi->getContour()->InvokeEvent(vtkCommand::PlacePointEvent);
                    cursorState = CURSORSTATE::CS_NONE;
                    isRoiOpen = true;
                    break;
                }
            }
            if (!isRoiOpen)
            {
                addPointInContourWithLabel();
            }
            break;
        }
        case CURSORSTATE::CS_REPULSOR:
        {
            if (!currentLabel)
            {
                pToolBox->displayMessageError(QString("Select a label in list to be able to use repulsor tool."));
            }
            if (interactorStyleRepulsor == nullptr)
            {
                interactorStyleRepulsor = vtkInriaInteractorStylePolygonRepulsor::New();
            }
            interactorStyleRepulsor->SetCurrentView(view);
            interactorStyleRepulsor->SetManager(currentLabel);
            break;
        }
        case CURSORSTATE::CS_NONE:
            break;
    }
}

void baseViewEvent::setCustomCursor(medAbstractView *view)
{
    if (!view)
        return;
    auto imageView = dynamic_cast<medAbstractImageView *>(view);
    if (!imageView)
        return;
    double radiusSize = 1.;
    // Adapt to scale of view (zoom, crop, etc)
    double radiusSizeDouble = radiusSize * imageView->scale();
    int radiusSizeInt = (int)floor(radiusSizeDouble + 0.5);
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
    view->viewWidget()->setCursor(QCursor(pix, -1, -1));
}

QLineEdit * baseViewEvent::changeManagerNameBase(polygonLabel* closestManager, QMenu *mainMenu)
{
    auto renameManager = new QLineEdit(closestManager->getName());
    renameManager->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
    QFont font = renameManager->font();
    font.setWeight(QFont::Black);
    font.setPointSize(15);
    font.setUnderline(true);
    renameManager->setFont(font);
    renameManager->setAlignment(Qt::AlignCenter);
    QColor color = closestManager->getColor();
    QString style = "QLineEdit { color: rgb(%1, %2, %3); }";
    renameManager->setStyleSheet(style.arg(color.red()).arg(color.green()).arg(color.blue()));

    connect(renameManager, &QLineEdit::returnPressed, [=](){
        mainMenu->close();
    });

    return renameManager;
}


void baseViewEvent::updateLabelToolBoxStateBase()
{
    currentLabel = nullptr;
    for (polygonLabel *pLabel : labelList)
    {
        if (pLabel->getState().selected)
        {
            currentLabel = pLabel;
        }
    }

}

polygonLabel *baseViewEvent::getClosestPLabel(double *mousePos)
{
    polygonLabel *closestManager = nullptr;
    double minDist = DBL_MAX;
    double dist = 0.;
    for (polygonLabel *manager : labelList)
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

QMenu *baseViewEvent::changeLabelActions(polygonLabel* closestLabel)
{
    auto changeMenu = new QMenu("Change Label");
    for (polygonLabel *manager : labelList)
    {
        if (manager != closestLabel && !manager->existingRoiInSlice())
        {
            QPixmap pixmap(20,20);
            pixmap.fill(manager->getColor());
            auto action = new QAction(pixmap, manager->getName(), changeMenu);
            connect(action, &QAction::triggered, [=](){
                polygonRoi *roi = closestLabel->existingRoiInSlice();
                QVector<QVector2D> nodes = roi->copyContour();
                polygonRoi *roiToAdd = manager->appendRoi();
                roiToAdd->pasteContour(nodes);
                setLabelActivationState();
                deleteContour(closestLabel);
            });
            changeMenu->addAction(action);
        }
    }
    return changeMenu;
}

void baseViewEvent::setLabelActivationState()
{
    for (polygonLabel *manager : labelList)
    {
        manager->setRoisSelectedState();
    }
}


bool baseViewEvent::rightButtonBehaviour(medAbstractView *view, QMouseEvent *mouseEvent)
{
    if (!view)
        return false;

    QMenu mainMenu(view->viewWidget());

#if QT_VERSION > QT_VERSION_CHECK(5, 10, 0)
    qreal devicePixelRatio = QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();
#else
    int screenNumber = QApplication::desktop()->screenNumber(mouseEvent->globalPos());
    int devicePixelRatio = QGuiApplication::screens().at(screenNumber)->devicePixelRatio();
#endif

    double mousePos[2];
    mousePos[0] = mouseEvent->x()*devicePixelRatio;
    mousePos[1] = (view->viewWidget()->height()-mouseEvent->y()-1)*devicePixelRatio;
    polygonLabel * closestManager = getClosestPLabel(mousePos);

    auto roiManagementMenu = new QMenu("Remove");
    auto saveMenu = new QMenu("Save as");
    QMenu *changeMenu = nullptr;
    auto renameManagerAction = new QWidgetAction(&mainMenu);
    auto copyContourAction = new QAction("Copy", &mainMenu);
    QAction *activationAction = nullptr;


    if (closestManager && (closestManager->getMinimumDistanceFromNodesToMouse(mousePos) < 10.))
    {
        auto deleteOneNodeAction = new QAction("Node", roiManagementMenu);
        connect(deleteOneNodeAction, &QAction::triggered, [this, &closestManager, &mousePos](){
            deleteNode(closestManager, mousePos);
        });
        roiManagementMenu->addAction(deleteOneNodeAction);

        auto deleteContourAction = new QAction("Contour", roiManagementMenu);
        connect(deleteContourAction, &QAction::triggered, [=](){
            deleteContour(closestManager);
        });
        roiManagementMenu->addAction(deleteContourAction);

        auto deleteLabelAction = new QAction("Label", roiManagementMenu);
        connect(deleteLabelAction, &QAction::triggered, [=](){
            deleteLabel(closestManager);
        });
        roiManagementMenu->addAction(deleteLabelAction);

        auto saveMaskAction = new QAction("Mask", saveMenu);
        connect(saveMaskAction, &QAction::triggered, [=](){
            saveMask(closestManager);
        });
        saveMenu->addAction(saveMaskAction);

        auto saveContourAction = new QAction("Contour", saveMenu);
        connect(saveContourAction, &QAction::triggered, [=](){
            saveContour(closestManager);
        });
        saveMenu->addAction(saveContourAction);


        connect(copyContourAction, &QAction::triggered, [=](){
            copyContour(closestManager);
        });

        if (closestManager != currentLabel)
        {
            activationAction =  new QAction("Activate", &mainMenu);
            connect(activationAction, &QAction::triggered, [=](){
                currentLabel = closestManager;
                activateContour(savedMousePosition);
            });
        }

        changeMenu = changeLabelActions(closestManager);

        renameManagerAction->setDefaultWidget(changeManagerName(closestManager, &mainMenu));


        mainMenu.addAction(renameManagerAction);
        if (!closestManager->getOptName().isEmpty())
        {
            auto name = new QLabel(closestManager->getOptName());
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
            auto widAct = new QWidgetAction(&mainMenu);
            widAct->setDefaultWidget(name);
            mainMenu.addAction(widAct);
        }
        if ( activationAction )
        {
            mainMenu.addAction(activationAction);
        }

        auto scoreMenu = createScoreMenu(closestManager);
        if (scoreMenu)
        {
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
    delete changeMenu;
    delete copyContourAction;
    delete activationAction;
    mainMenu.clear();
    return true;
}

void baseViewEvent::addPointInContourWithLabel()
{
    if (currentLabel)
    {
        if (!currentLabel->existingRoiInSlice())
        {
            currentLabel->appendRoi();
        }
    }
}

void baseViewEvent::setEnableInterpolation(bool state)
{
    enableInterpolation = state;
    for (polygonLabel *manager : labelList)
    {
        manager->setEnableInterpolation(state);
    }
    setLabelActivationState();
    manageTickVisibility(true);
}

void baseViewEvent::copyContour(polygonLabel *manager)
{
    copyNodesList.clear();
    QVector<QVector2D> coords = manager->copyContour();
    if (!coords.empty())
    {
        int label = labelList.indexOf(manager);
        medDisplayPosContours contours = medDisplayPosContours(label, coords);
        copyNodesList.append(contours);
    }
}

void baseViewEvent::copyContours()
{
    copyNodesList.clear();
    for (polygonLabel *manager : labelList)
    {
        if (manager->existingRoiInSlice())
        {
            QVector<QVector2D> coords = manager->copyContour();
            if (!coords.empty())
            {
                int label =  labelList.indexOf(manager);;
                medDisplayPosContours contours = medDisplayPosContours(label, coords);
                copyNodesList.append(contours);
            }
        }
    }
}

void baseViewEvent::pasteContours()
{
    for (medDisplayPosContours &contours : copyNodesList)
    {
        polygonLabel *fromLabel = labelList.at(contours.getLabel());
        auto label = findManager(fromLabel->getPosition());
        if ( label )
        {
            if ( !label->existingRoiInSlice() )
            {
                label->pasteContour(contours.getNodes());
            }
            else
            {
                pToolBox->displayMessageError("contour " + label->getName() + " already exists in current slice ");
            }
        }
        setLabelActivationState();
    }
}

polygonLabel *baseViewEvent::findManagerWithColor(QColor &color)
{
    polygonLabel *mgr = nullptr;
    for (polygonLabel *manager : labelList)
    {
        if (manager->getColor() == color)
        {
            mgr = manager;
        }
    }
    return mgr;
}

polygonLabel *baseViewEvent::findManager(int position)
{
    polygonLabel *label = nullptr;
    for (polygonLabel *label1 : labelList)
    {
        if (label1->getPosition() == position)
        {
            label = label1;
        }
    }
    return label;
}

void baseViewEvent::updateContourProperty(QString &name, QColor &color, int position, bool isSelected, bool hasScore,
                                          bool checkState)
{
    qDebug()<<"updateContourProperty "<<name;
    if (!currentView)
        return;

    // 1/ unselect all labels. only one can be selected and it will be set just after
    for (auto label : labelList)
    {
        label->getState().selected = false;
        label->setRoisSelectedState();
    }

    currentLabel = findManager(position);
    if (currentLabel)
    {
        currentLabel->updateContourProperty(name, color, isSelected, hasScore, checkState);
        if (hasScore && currentLabel->getState().secondColor!=QColor::Invalid) //this label can have a score
        {
            QColor newColor = (checkState)?currentLabel->getState().secondColor:currentLabel->getState().mainColor;
            currentLabel->changeContoursColor(newColor);
        }
    }
    else
    {
        currentLabel = new polygonLabel(currentView, this, color, name, position, isSelected, enableInterpolation);
        currentLabel->setScoreState(hasScore);
        labelList.append(currentLabel);
        connect(currentLabel, SIGNAL(sendErrorMessage(QString)), this, SIGNAL(sendErrorMessage(QString)), Qt::UniqueConnection);
    }
    currentLabel->setRoisSelectedState();
    currentView->render();

}

void baseViewEvent::activateRepulsor(bool state)
{
    if (!currentView)
        return;

    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(currentView->backend())->view2D;

    isRepulsorActivated = state;

    if (state)
    {
        cursorState = CURSORSTATE::CS_REPULSOR;
        vtkInteractorStyleImageView2D *interactorStyle2D = vtkInteractorStyleImageView2D::SafeDownCast(view2d->GetInteractor()->GetInteractorStyle());
        if (!interactorStyleRepulsor)
        {
            interactorStyleRepulsor = vtkInriaInteractorStylePolygonRepulsor::New();
        }
        interactorStyleRepulsor->SetCurrentView(currentView);
        interactorStyleRepulsor->SetManager(currentLabel);
        interactorStyleRepulsor->SetLeftButtonInteraction(interactorStyle2D->GetLeftButtonInteraction());
        view2d->SetInteractorStyle(interactorStyleRepulsor);
        view2d->SetupInteractor(view2d->GetInteractor()); // to reinstall vtkImageView2D pipeline

        for (auto label: labelList)
        {
            if (label != currentLabel)
            {
                label->setRoisSelectedState();
                polygonRoi *roi = label->existingRoiInSlice();
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
        cursorState = CURSORSTATE::CS_DEFAULT;
        vtkInteractorStyleImageView2D *interactorStyle2D = vtkInteractorStyleImageView2D::New();
        if (!interactorStyleRepulsor)
        {
            interactorStyleRepulsor = vtkInriaInteractorStylePolygonRepulsor::New();
        }
        interactorStyle2D->SetLeftButtonInteraction(vtkInteractorStyleImageView2D::InteractionTypeNull);
        view2d->SetInteractorStyle(interactorStyle2D);
        view2d->SetupInteractor(view2d->GetInteractor()); // to reinstall vtkImageView2D pipeline
        interactorStyle2D->Delete();
    }
}

void baseViewEvent::saveMask()
{
    for (polygonLabel *label : labelList)
    {
        if (!label->getRois().empty())
        {
            saveMask(label);
        }
    }
}

void baseViewEvent::saveAllContours()
{
    if (!currentView)
        return;

    contourOutput = medAbstractDataFactory::instance()->createSmartPointer("medContours");

    QVector<medTagContours> contoursData;
    vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
    append->SetUserManagedInputs(true);
    append->SetNumberOfInputs(labelList.size());
    int num = 0;
    for (polygonLabel *pLabel : labelList)
    {
        if (!pLabel->getRois().empty())
        {
            int position = pLabel->getPosition();

            auto ctr = pLabel->getContoursAsNodes();
            medTagContours contoursTag;

            contoursTag.setSpecialityIndex(speciality());

            contoursTag.setLabelName(pLabel->getName());

            contoursTag.setTarget(pLabel->hasScore());

            if (pLabel->hasScore() && pLabel->getOptName() != QString())
            {
                contoursTag.setScore(pLabel->getOptName());
            }
            contoursTag.setContourNodes(ctr);
            contoursData.append(contoursTag);

            auto pd = pLabel->getContoursAsPolyData(position);
            append->SetInputDataByNumber(num, pd);
            num++;
        }
    }
    if (!contoursData.empty())
    {
        append->Update();
        vtkMetaDataSet *outputDataSet = vtkMetaSurfaceMesh::New();
        outputDataSet->SetDataSet(append->GetOutput());

        QString description;
        if (contoursData.size() == 1)
        {
            description = QString("contour %1").arg(contoursData[0].getLabelName());

        }
        else
        {
            description = QString("%1 contours").arg(contoursData.size());
        }
        medAbstractData * data = currentView->layerData(0);
        auto *originalData = qobject_cast<medAbstractImageData*>(data);
        medUtilities::copyMetaDataIfEmpty(contourOutput, originalData, medUtilities::metaDataKeysToCopyForDerivedData(contourOutput));
        QString newSeriesDescription = description + " (" + originalData->metadata(medMetaDataKeys::SeriesDescription.key()) + ")";
        contourOutput->setMetaData(medMetaDataKeys::SeriesDescription.key(), newSeriesDescription);

        contourOutput->setData(outputDataSet);
        contourOutput->setData(&contoursData, 1);
        outputDataSet->Delete();

        medDataManager::instance()->importData(contourOutput, false);
    }
}

void baseViewEvent::manageTickVisibility(bool show)
{
    medIntParameterL *slicingParameter = getSlicingParameter(currentView);
    if ( slicingParameter )
    {
        slicingParameter->getSlider()->removeAllTicks();
        if (show)
        {
            for (polygonLabel *label : labelList)
            {
                label->manageTick(slicingParameter->getSlider());
            }
        }
    }
}

medIntParameterL *baseViewEvent::getSlicingParameter(medAbstractImageView *iView)
{
    medIntParameterL *slicingParameter = nullptr;
    for( auto interactor : qobject_cast<medAbstractLayeredView*>(iView)->layerInteractors(0))
    {
        if ((interactor->identifier() == "medVtkViewItkDataImageInteractor") ||
            (interactor->identifier() == "medVtkViewItkDataImage4DInteractor"))
        {
            for (auto parameter : interactor->linkableParameters())
            {
                if (parameter->name() == "Slicing")
                {
                    slicingParameter = qobject_cast<medIntParameterL*>(parameter);
                }
            }
        }
    }
    return slicingParameter;
}

void baseViewEvent::deleteNode(double *mousePosition)
{
    polygonLabel *closestManager = getClosestPLabel(mousePosition);
    if (closestManager && closestManager->getMinimumDistanceFromNodesToMouse(mousePosition, false) < 10. )
    {
        deleteNode(closestManager, mousePosition);
    }
}

int baseViewEvent::activateContourBase(double *mousePosition)
{
    // 1/ unselect all labels. only one can be selected and it will be set just after
    unselectLabels();

    int position = -1;
    polygonLabel *pLabel = getClosestPLabel(mousePosition);
    if (pLabel && pLabel->getMinimumDistanceFromNodesToMouse(mousePosition, true) < 10. )
    {
        currentLabel = pLabel;
        pLabel->getState().selected = true;
        pLabel->setRoisSelectedState();
        position = pLabel->getPosition();
    }
    return position;
}

void baseViewEvent::unselectLabels()
{
    for (auto label : labelList)
    {
        label->getState().selected = false;
        label->setRoisSelectedState();
    }
}

void baseViewEvent::deleteNode(polygonLabel *manager, const double *mousePos)
{
    manager->deleteNode(mousePos[0], mousePos[1]);

    if (manager->getRois().isEmpty())
    {
        deleteLabel(manager);
    }
    else if (!manager->existingRoiInSlice())
    {
        deleteContour(manager);
    }
    else
    {
        manageTickVisibility(true);
    }
}

void baseViewEvent::deleteContour(polygonLabel *manager)
{
    manager->deleteContour();
    if ( manager->getRois().empty() )
    {
        deleteLabel(manager);
    }
    else
    {
        manageTickVisibility(true);
    }
}

void baseViewEvent::deleteLabel(int position)
{
    polygonLabel *label = findManager(position);
    if (label)
    {
        deleteLabel(label);
    }
}

int baseViewEvent::deleteLabelBase(polygonLabel *manager)
{
    int position = -1;
    labelList.removeOne(manager);
    manager->removeAllTick();

    if (currentLabel == manager && !labelList.empty())
    {
        unselectLabels();
        currentLabel = labelList.at(0);
        currentLabel->getState().selected = true;
        currentLabel->setRoisSelectedState();
        position = currentLabel->getPosition();
        if (isRepulsorActivated)
        {
            interactorStyleRepulsor->SetManager(currentLabel);
        }
    }
    delete manager;

    if (labelList.empty())
    {
        currentLabel = nullptr;
        position = -1;
        if (pToolBox->repulsorTool->isChecked())
        {
            pToolBox->repulsorTool->click();
        }
    }
    manageTickVisibility(true);
    return position;
}

bool baseViewEvent::isActiveContourInSlice()
{
    bool retVal = false;
    if (currentLabel && currentLabel->existingRoiInSlice() )
    {
        retVal = true;
    }
    return retVal;
}

void baseViewEvent::saveMask(polygonLabel *manager)
{
    int label = labelList.indexOf(manager);
    manager->createMaskWithLabel(label);
}

void baseViewEvent::saveContour(polygonLabel *label)
{
    contourOutput = medAbstractDataFactory::instance()->createSmartPointer("medContours");

    QVector<medTagContours> contoursData;
    vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
    append->SetUserManagedInputs(true);
    append->SetNumberOfInputs(labelList.size());

    int position = label->getPosition();

    QVector<medWorldPosContours> contour = label->getContoursAsNodes();
    medTagContours contoursTag;

    contoursTag.setSpecialityIndex(speciality());

    contoursTag.setLabelName(label->getName());

    contoursTag.setTarget(label->hasScore());

    if (label->hasScore() && label->getOptName() != QString())
    {
        contoursTag.setScore(label->getOptName());
    }
    contoursTag.setContourNodes(contour);
    contoursData.append(contoursTag);

    auto pd = label->getContoursAsPolyData(position);
    append->SetInputDataByNumber(0, pd);

    append->Update();
    vtkMetaDataSet *outputDataSet = vtkMetaSurfaceMesh::New();
    outputDataSet->SetDataSet(append->GetOutput());

    QString description;
    if (contoursData.size() == 1)
    {
        description = QString("contour %1").arg(contoursData[0].getLabelName());

    }
    else
    {
        description = QString("%1 contours").arg(contoursData.size());
    }
    medAbstractData * data = currentView->layerData(0);
    auto *originalData = qobject_cast<medAbstractImageData*>(data);
    medUtilities::copyMetaDataIfEmpty(contourOutput, originalData, medUtilities::metaDataKeysToCopyForDerivedData(contourOutput));
    QString newSeriesDescription = description + " (" + originalData->metadata(medMetaDataKeys::SeriesDescription.key()) + ")";
    contourOutput->setMetaData(medMetaDataKeys::SeriesDescription.key(), newSeriesDescription);

    contourOutput->setData(outputDataSet);
    contourOutput->setData(&contoursData, 1);
    outputDataSet->Delete();

    medDataManager::instance()->importData(contourOutput, false);

}

void baseViewEvent::onContourFinished(CURSORSTATE state)
{
    cursorState = state;
    for (polygonLabel *label : labelList)
    {
        label->interpolateIfNeeded();
        currentView->render();
    }
    manageTickVisibility(true);
}

medAbstractView *baseViewEvent::getCurrentView()
{
    return currentView;
}

void baseViewEvent::showOnDifferentOrientation()
{
    auto view = dynamic_cast<medAbstractImageView *>(sender());
    for (polygonLabel *label : labelList)
    {
        label->updateRoiOnOrientedView(view, true);
    }
}

void baseViewEvent::removeViewFromList(medAbstractImageView *iView)
{
    for (polygonLabel *label : labelList)
    {
        label->updateRoiOnOrientedView(iView, false);
    }
}

void baseViewEvent::rename(int position, QString newName)
{
    polygonLabel *label = findManager(position);
    if (label)
    {
        label->setName(newName);
    }
}

void baseViewEvent::onSelectContainer()
{
    updateLabelToolBoxState(identifier);
    installOnView(currentView);
    for (auto label : labelList)
    {
        label->addViewObservers();
    }
}

void baseViewEvent::removeViewInteractor()
{
    removeFromAllViews();
    for (auto label : labelList)
    {
        label->removeViewObservers();
    }
}

bool baseViewEvent::isContourLoadable(QString &labelName)
{
    if (labelList.size() == 9)
    {
        // TODO : send message to toolbox (signal or return ?)
        qDebug()<<"isContourLoadable - unable to create new label (max size)";
        return false;
    }

    for (polygonLabel *label : labelList)
    {
        // label is already created (click on label in tb) and segmentation has be done in view (create roi in view)
        if (label->getName() == labelName && !(label->getRois().empty()))
        {
            // TODO : send message to toolbox (signal or return ?)
            qDebug()<<QString("isContourLoadable - contour with name %1 already exists").arg(label->getName());
            return false;
        }
    }
    return true;
}

