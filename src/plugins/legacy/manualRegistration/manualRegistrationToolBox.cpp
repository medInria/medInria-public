/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2019. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include "manualRegistration.h"
#include "manualRegistrationLandmarkController.h"
#include "manualRegistrationToolBox.h"

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <medAbstractImageView.h>
#include <medAbstractParameterL.h>
#include <medCompositeParameterL.h>
#include <medLayerParameterGroupL.h>
#include <medPluginManager.h>
#include <medRegistrationSelectorToolBox.h>
#include <medRunnableProcess.h>
#include <medStringListParameterL.h>
#include <medToolBoxFactory.h>
#include <medViewContainer.h>
#include <medViewParameterGroupL.h>
#include <medVtkViewBackend.h>

#include <vtkRenderWindowInteractor.h>

class manualRegistrationToolBoxPrivate
{
public:
    QPushButton* b_startManualRegistration;
    QPushButton* b_computeRegistration;
    QPushButton* b_reset;

    QComboBox* transformType;
    QLabel* transformationText;

    medAbstractLayeredView* currentView;
    medViewContainer* leftContainer;
    medViewContainer* rightContainer;
    medViewContainer* bottomContainer;
    manualRegistrationLandmarkController* controller;
    bool regOn;

    QLabel* numberOfLdInLeftContainer;
    QLabel* numberOfLdInRightContainer;
    QLabel* explanation;

    medViewParameterGroupL* viewGroup;
    medLayerParameterGroupL* layerGroup1, *layerGroup2;

    dtkSmartPointer<medAbstractRegistrationProcess> process;
    dtkSmartPointer<medAbstractData> output;
};

manualRegistrationToolBox::manualRegistrationToolBox(QWidget *parent)
    : medAbstractSelectableToolBox(parent), d(new manualRegistrationToolBoxPrivate)
{
    QWidget* widget = new QWidget(this);

    d->b_startManualRegistration = new QPushButton("Start Manual Registration", widget);
    connect(d->b_startManualRegistration,SIGNAL(clicked()),this,SLOT(startManualRegistration()));
    d->b_startManualRegistration->setObjectName("startManualRegistrationButton");

    d->explanation = new QLabel("To add a landmark: \n\tShift + left mouse button\nTo remove a pair of landmarks: \n\tBackspace + left mouse button", widget);

    d->numberOfLdInLeftContainer = new QLabel("Number of landmarks in left container: 0", widget);
    d->numberOfLdInRightContainer = new QLabel("Number of landmarks in right container: 0", widget);

    // Choice between transformations
    QHBoxLayout* transformationLayout = new QHBoxLayout;
    d->transformationText = new QLabel("Transformation");
    transformationLayout->addWidget(d->transformationText);
    d->transformType = new QComboBox(widget);
    d->transformType->setObjectName("transformType");
    d->transformType->addItem("Rigid",  manualRegistration::RIGID);
    d->transformType->addItem("Affine", manualRegistration::AFFINE);
    transformationLayout->addWidget(d->transformType);

    // Action buttons
    d->b_computeRegistration = new QPushButton("Compute Registration", widget);
    connect(d->b_computeRegistration, SIGNAL(clicked()), this, SLOT(computeRegistration()));
    d->b_computeRegistration->setObjectName("computeRegistrationButton");

    d->b_reset = new QPushButton("Reset", widget);
    d->b_reset->setObjectName("resetButton");
    connect(d->b_reset, SIGNAL(clicked()), this, SLOT(reset()));

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(d->b_startManualRegistration);
    mainLayout->addWidget(d->explanation);
    mainLayout->addWidget(d->numberOfLdInLeftContainer);
    mainLayout->addWidget(d->numberOfLdInRightContainer);
    mainLayout->addLayout(transformationLayout);
    mainLayout->addWidget(d->b_computeRegistration);
    mainLayout->addWidget(d->b_reset);
    widget->setLayout(mainLayout);
    this->addWidget(widget);

    d->viewGroup = new medViewParameterGroupL("ManualRegistration", this);
    d->viewGroup->setLinkAllParameters(true);
    d->viewGroup->removeParameter("Position");
    d->viewGroup->removeParameter("DataList");

    d->layerGroup1 = new medLayerParameterGroupL("Fixed", this);
    d->layerGroup1->setLinkAllParameters(true);
    d->layerGroup1->removeParameter("Slicing");

    d->layerGroup2 = new medLayerParameterGroupL("Moving", this);
    d->layerGroup2->setLinkAllParameters(true);
    d->layerGroup2->removeParameter("Slicing");

    d->regOn           = false;
    d->currentView     = nullptr;
    d->leftContainer   = nullptr;
    d->rightContainer  = nullptr;
    d->bottomContainer = nullptr;
    d->controller      = nullptr;
    d->process         = nullptr;
    d->output          = nullptr;

    setDisableComputeResetButtons(true);
    displayButtons(false);
}

manualRegistrationToolBox::~manualRegistrationToolBox()
{
    d->regOn           = false;
    d->currentView     = nullptr;
    d->leftContainer   = nullptr;
    d->rightContainer  = nullptr;
    d->bottomContainer = nullptr;
    d->controller      = nullptr;
    d->process         = nullptr;
    d->output          = nullptr;

    delete d;
    d = nullptr;
}

medAbstractData* manualRegistrationToolBox::processOutput()
{
    if (!d->output)
    {
        computeRegistration();
    }
    return d->output;
}

bool manualRegistrationToolBox::registered()
{
    return medToolBoxFactory::instance()->
            registerToolBox<manualRegistrationToolBox>();
}

dtkPlugin* manualRegistrationToolBox::plugin()
{
    return medPluginManager::instance().plugin("Manual Registration");
}

void manualRegistrationToolBox::updateView()
{
    medAbstractView* view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();

    if (view)
    {
        medAbstractLayeredView* v = qobject_cast<medAbstractLayeredView*>(view);

        if (!d->regOn) // if the registration is activated we do not change the currentView.
        {
            d->currentView = v;
        }
    }
}

void manualRegistrationToolBox::startManualRegistration()
{
    if (d->regOn)
    {
        stopManualRegistration();
        return;
    }

    medTabbedViewContainers *tabContainers = getWorkspace()->tabbedViewContainers();
    d->regOn = true;

    medRegistrationSelectorToolBox *toolbox = dynamic_cast<medRegistrationSelectorToolBox*>(selectorToolBox());
    if (toolbox)
    {
        dtkSmartPointer<medAbstractData> fixedData(toolbox->fixedData());
        dtkSmartPointer<medAbstractData> movingData(toolbox->movingData());

        if (!fixedData || !movingData)
        {
            d->regOn = false;
            return;
        }

        d->leftContainer   = tabContainers->containersInTab(0).at(0);
        d->rightContainer  = tabContainers->containersInTab(0).at(1);
        d->bottomContainer = tabContainers->containersInTab(1).at(0);
    }
    else // not in Reg. workspace, used in Pipelines
    {
        constructContainers(tabContainers);
    }

    d->controller = manualRegistrationLandmarkController::New();
    d->controller->SetViewMoving(qobject_cast<medAbstractLayeredView*>(d->rightContainer->view())); // layer 0 is target
    d->controller->SetViewFixed(qobject_cast<medAbstractLayeredView*>(d->leftContainer->view())); // layer 1 is source
    d->controller->SetViewFuse(qobject_cast<medAbstractLayeredView*>(d->bottomContainer->view()));
    d->controller->SetTbx(this);

    vtkCollection* interactorcollection = vtkCollection::New();
    interactorcollection->AddItem(static_cast<medVtkViewBackend*>(d->controller->GetViewFixed()->backend())->renWin->GetInteractor());
    interactorcollection->AddItem(static_cast<medVtkViewBackend*>(d->controller->GetViewMoving()->backend())->renWin->GetInteractor());
    d->controller->SetInteractorCollection(interactorcollection);
    interactorcollection->Delete();
    d->controller->SetEnabled(1);

    displayButtons(true);
    d->b_startManualRegistration->setText("Stop Manual Registration");

    connect(d->rightContainer->view(), SIGNAL(closed()), this, SLOT(stopManualRegistration()));
    connect(d->leftContainer->view(),  SIGNAL(closed()), this, SLOT(stopManualRegistration()));
}

void manualRegistrationToolBox::stopManualRegistration()
{
    disconnect(d->rightContainer->view(), SIGNAL(closed()), this, SLOT(stopManualRegistration()));
    disconnect(d->leftContainer->view(),  SIGNAL(closed()), this, SLOT(stopManualRegistration()));

    d->b_startManualRegistration->setText("Start Manual Registration");
    reset();
    d->controller->SetEnabled(0);
    d->regOn           = false;
    d->controller->Delete();
    d->controller      = nullptr;
    d->output          = nullptr;
    d->leftContainer   = nullptr;
    d->rightContainer  = nullptr;
    d->bottomContainer = nullptr;
    displayButtons(false);

    if(!this->selectorToolBox())
    {
        medTabbedViewContainers* tabContainers = getWorkspace()->tabbedViewContainers();
        tabContainers->setCurrentIndex(0);
        tabContainers->closeCurrentTab();
    }
}

void manualRegistrationToolBox::synchroniseMovingFuseView()
{
    if(!this->selectorToolBox()) // does not exist in Pipelines
    {
        // This method will resynchronise the lut and window level
        medAbstractImageView* viewMoving = qobject_cast<medAbstractImageView*>(d->rightContainer->view());
        medAbstractImageView* viewFuse   = qobject_cast<medAbstractImageView*>(d->bottomContainer->view());

        d->layerGroup2->addImpactedlayer(viewFuse, viewFuse->layerData(1));

        // Synchronize Lut
        QList<medAbstractInteractor*> interactors = viewMoving->layerInteractors(0);
        QString lutCurrent;
        QList<medAbstractParameterL*> parameters;
        for(int i = 0; i<interactors.size(); i++)
        {
            if (interactors[i]->identifier() == "medVtkViewItkDataImageInteractor")
            {
                parameters = interactors[i]->linkableParameters();
                for(int j = 0; j<parameters.size(); j++)
                {
                    if (parameters[j]->name()=="Lut")
                    {
                        lutCurrent = qobject_cast<medStringListParameterL*>(parameters[j])->value();
                    }
                }
            }
        }

        interactors = viewFuse->layerInteractors(1);

        for(int i = 0; i<interactors.size(); i++)
        {
            if (interactors[i]->identifier() == "medVtkViewItkDataImageInteractor")
            {
                QList<medAbstractParameterL*> parameters = interactors[i]->linkableParameters();
                for(int j = 0; j<parameters.size(); j++)
                {
                    if (parameters[j]->name()=="Lut")
                    {
                        qobject_cast<medStringListParameterL*>(parameters[j])->setValue(lutCurrent);
                    }
                }
            }
        }

        // Synchronize Window/Level
        medCompositeParameterL* windowLevelParameter1 = viewMoving->windowLevelParameter(0);
        medCompositeParameterL* windowLevelParameter2 = viewFuse->windowLevelParameter(1);
        QList<QVariant> windowLevel = windowLevelParameter1->values();
        QHash<QString,QVariant> hash;
        if (windowLevel.size() == 2)
        {
            hash.insert("Level",  windowLevel[0]);
            hash.insert("Window", windowLevel[1]);
        }
        windowLevelParameter2->setValues(hash);
    }
}

void manualRegistrationToolBox::computeRegistration()
{
    if (d->controller)
    {
        if (d->controller->checkLandmarks() == medAbstractProcessLegacy::SUCCESS)
        {
            int transformChosen = d->transformType->itemData(d->transformType->currentIndex()).toInt();

            // Check that Affine process has enough landmarks
            if (transformChosen == manualRegistration::AFFINE)
            {
                if ((d->controller->getPoints_Fixed()->count() < 4) || (d->controller->getPoints_Moving()->count() < 4))
                {
                    displayMessageError("Affine transformation needs 4 landmarks minimum by dataset");
                    return;
                }
            }

            this->setToolBoxOnWaitStatus();

            d->process = dynamic_cast<medAbstractRegistrationProcess*>
                    (dtkAbstractProcessFactory::instance()->create("manualRegistration"));

            medRegistrationSelectorToolBox *toolbox = dynamic_cast<medRegistrationSelectorToolBox*>(selectorToolBox());
            if(toolbox) // toolbox empty in Pipelines and not Registration workspace
            {
                toolbox->setProcess(d->process);
            }

            dtkSmartPointer<manualRegistration> process_Registration = dynamic_cast<manualRegistration* >(d->process.data());
            process_Registration->SetFixedLandmarks(d->controller->getPoints_Fixed());
            process_Registration->SetMovingLandmarks(d->controller->getPoints_Moving());
            process_Registration->setFixedInput(qobject_cast<medAbstractLayeredView*>(d->leftContainer->view())->layerData(0));
            process_Registration->setMovingInput(qobject_cast<medAbstractLayeredView*>(d->rightContainer->view())->layerData(0));
            process_Registration->setParameter(transformChosen);

            medRunnableProcess *runProcess = new medRunnableProcess;
            runProcess->setProcess (d->process);
            connect (runProcess, SIGNAL (success(QObject*)), this, SLOT(retrieveProcessOutputAndUpdateViews()));
            this->addConnectionsAndStartJob(runProcess);
            enableOnProcessSuccessImportOutput(runProcess, false);
        }
    }
}

void manualRegistrationToolBox::retrieveProcessOutputAndUpdateViews()
{
    medAbstractData* newOutput = d->process->output();

    if(newOutput && newOutput->data())
    {
        d->output = newOutput;

        // Pipelines
        if (d->bottomContainer && d->bottomContainer->view())
        {
            qobject_cast<medAbstractImageView*>(d->bottomContainer->view())->removeLayer(1);
            qobject_cast<medAbstractImageView*>(d->bottomContainer->view())->addLayer(d->output);
        }

        synchroniseMovingFuseView();

        d->process = nullptr;
    }
}

void manualRegistrationToolBox::reset()
{
    medAbstractImageView* viewFuse   = qobject_cast<medAbstractImageView*>(d->bottomContainer->view());
    medAbstractImageView* viewMoving = qobject_cast<medAbstractImageView*>(d->rightContainer->view());
    medAbstractImageView* viewFix    = qobject_cast<medAbstractImageView*>(d->leftContainer->view());

    if (d->controller)
    {
        d->controller->Reset(); // Delete every landmark on both view

        // Refresh views except at closing
        if (viewMoving)
        {
            viewMoving->viewWidget()->update();
        }

        if (viewFix)
        {
            viewFix->viewWidget()->update();
        }
    }

    if (viewFuse->layersCount() > 1)
    {
        viewFuse->removeLayer(1);
    }

    if (viewMoving) //not the case if called by viewMoving closed() signal
    {
        viewFuse->addLayer(viewMoving->layerData(0));
    }

    synchroniseMovingFuseView();

    // Disable Transformation/Compute/Reset buttons
    setDisableComputeResetButtons(true);
}

void manualRegistrationToolBox::clear()
{
    if (d->regOn)
    {
        d->b_startManualRegistration->click();
    }
}

void manualRegistrationToolBox::constructContainers(medTabbedViewContainers* tabContainers)
{
    if (d->currentView && getWorkspace())
    {
        if (d->currentView->layersCount() < 2)
        {
            return;
        }

        medCompositeParameterL * windowingParameter0 = qobject_cast<medAbstractImageView*>(d->currentView)->windowLevelParameter(0);
        QHash<QString, QVariant> windowing0;
        windowing0.insert("Level", windowingParameter0->value("Level"));
        windowing0.insert("Window", windowingParameter0->value("Window"));

        medCompositeParameterL * windowingParameter1 = qobject_cast<medAbstractImageView*>(d->currentView)->windowLevelParameter(1);
        QHash<QString, QVariant> windowing1;
        windowing1.insert("Level", windowingParameter1->value("Level"));
        windowing1.insert("Window", windowingParameter1->value("Window"));

        tabContainers->setAcceptDrops(false);

        // deselect container before going to reformat mode, otherwise problem !
        for(QUuid u : tabContainers->containersSelected())
        {
            for(medViewContainer * cont : tabContainers->containersInTab(tabContainers->currentIndex()))
            {
                if (u == cont->uuid())
                {
                    cont->setSelected(false);
                }
            }
        }

        d->leftContainer = tabContainers->insertNewTab(0,"ManualRegistration");
        tabContainers->setCurrentIndex(0);
        d->leftContainer->addData(d->currentView->layerData(0));
        qobject_cast<medAbstractImageView*>(d->leftContainer->view())->windowLevelParameter(0)->setValues(windowing0);

        d->bottomContainer = d->leftContainer->splitHorizontally();
        d->bottomContainer->addData(d->currentView->layerData(0));
        qobject_cast<medAbstractImageView*>(d->bottomContainer->view())->windowLevelParameter(0)->setValues(windowing0);
        d->bottomContainer->addData(d->currentView->layerData(1));
        qobject_cast<medAbstractImageView*>(d->bottomContainer->view())->windowLevelParameter(1)->setValues(windowing1);

        d->rightContainer = d->leftContainer->splitVertically();
        d->rightContainer->addData(d->currentView->layerData(1));
        qobject_cast<medAbstractImageView*>(d->rightContainer->view())->windowLevelParameter(0)->setValues(windowing1);

        tabContainers->adjustContainersSize();

        d->leftContainer->setUserSplittable(false);
        d->rightContainer->setUserSplittable(false);
        d->bottomContainer->setUserSplittable(false);

        //The Fuse view is the view where you see you the 2 volumes which should be registered.
        //By setting one volume's LUT to "Hot Metal", we more easily can differentiate both volumes
        //and then evaluate the registration.
        QList<medAbstractInteractor*> interactors =
                qobject_cast<medAbstractImageView*>(d->rightContainer->view())->layerInteractors(0);
        for(int i=0; i<interactors.size(); i++)
        {
            if (interactors[i]->identifier() == "medVtkViewItkDataImageInteractor")
            {
                QList<medAbstractParameterL*> parameters = interactors[i]->linkableParameters();
                for(int j = 0;j<parameters.size();j++)
                {
                    if (parameters[j]->name()=="Lut")
                    {
                        qobject_cast<medStringListParameterL*>(parameters[j])->setValue("Hot Metal");
                    }
                }
            }
        }

        tabContainers->lockTabs();

        d->viewGroup->addImpactedView(d->rightContainer->view());
        d->viewGroup->addImpactedView(d->leftContainer->view());
        d->viewGroup->addImpactedView(d->bottomContainer->view());
        d->viewGroup->setLinkAllParameters(true);
        d->viewGroup->removeParameter("Position");
        d->viewGroup->removeParameter("DataList");

        d->layerGroup1->addImpactedlayer(qobject_cast<medAbstractLayeredView*>(d->bottomContainer->view()), d->currentView->layerData(0));
        d->layerGroup1->addImpactedlayer(qobject_cast<medAbstractLayeredView*>(d->leftContainer->view()), d->currentView->layerData(0));
        d->layerGroup2->addImpactedlayer(qobject_cast<medAbstractLayeredView*>(d->bottomContainer->view()), d->currentView->layerData(1));
        d->layerGroup2->addImpactedlayer(qobject_cast<medAbstractLayeredView*>(d->rightContainer->view()), d->currentView->layerData(1));

        d->leftContainer->setClosingMode(medViewContainer::CLOSE_BUTTON_HIDDEN);
        d->rightContainer->setClosingMode(medViewContainer::CLOSE_BUTTON_HIDDEN);
        d->bottomContainer->setClosingMode(medViewContainer::CLOSE_BUTTON_HIDDEN);

        d->currentView = nullptr; // we dont need it anymore.

        synchroniseMovingFuseView();
    }
}

void manualRegistrationToolBox::setDisableComputeResetButtons(bool disable)
{
    d->b_computeRegistration->setDisabled(disable);
    setDisableResetButton(disable);
}

void manualRegistrationToolBox::setDisableResetButton(bool disable)
{
    d->b_reset->setDisabled(disable);
}

void manualRegistrationToolBox::displayButtons(bool show)
{
    if(show)
    {
        d->explanation->show();
        d->numberOfLdInLeftContainer->show();
        d->numberOfLdInRightContainer->show();
        d->transformationText->show();
        d->transformType->show();
        d->b_reset->show();
        d->b_computeRegistration->show();
    }
    else
    {
        d->explanation->hide();
        d->numberOfLdInLeftContainer->hide();
        d->numberOfLdInRightContainer->hide();
        d->transformationText->hide();
        d->transformType->hide();
        d->b_reset->hide();
        d->b_computeRegistration->hide();
    }
}

void manualRegistrationToolBox::updateGUI(int left, int right)
{
    d->numberOfLdInLeftContainer->setText( "Number of landmarks in left container: " + QString::number(left));
    d->numberOfLdInRightContainer->setText("Number of landmarks in right container: " + QString::number(right));

    if (left == right)
    {
        if(left > 0)
        {
            setDisableComputeResetButtons(false);
        }
    }

    if ((left > 0) || (right > 0))
    {
        // at least one landmark put
        setDisableResetButton(false);
    }
}
