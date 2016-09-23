/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <manualRegistration.h>
#include <manualRegistrationLandmarkController.h>
#include <manualRegistrationToolBox.h>

#include <medAbstractImageView.h>
#include <medAbstractLayeredView.h>
#include <medCompositeParameter.h>
#include <medDataManager.h>
#include <medLayerParameterGroup.h>
#include <medPluginManager.h>
#include <medRegistrationSelectorToolBox.h>
#include <medStringListParameter.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medUtilities.h>
#include <medViewContainer.h>
#include <medViewParameterGroup.h>
#include <medVtkViewBackend.h>

#include <vtkRenderWindowInteractor.h>

class manualRegistrationToolBoxPrivate
{
public:
    QPushButton * b_startManualRegistration;  
    QPushButton * b_computeRegistration;
    QPushButton * b_reset;
    QPushButton * b_save;
    QPushButton * b_exportTransformation;
    medAbstractLayeredView * currentView;
    medViewContainer * leftContainer;
    medViewContainer * rightContainer;
    medViewContainer * bottomContainer;
    manualRegistrationLandmarkController * controller;
    bool regOn;
    QLabel * numberOfLdInLeftContainer;
    QLabel * numberOfLdInRightContainer;

    medViewParameterGroup* viewGroup;
    medLayerParameterGroup* layerGroup1,*layerGroup2;

    dtkSmartPointer<manualRegistration> process;
    dtkSmartPointer<medAbstractData> output;
};

manualRegistrationToolBox::manualRegistrationToolBox(QWidget *parent) : medAbstractSelectableToolBox(parent), d(new manualRegistrationToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    d->b_startManualRegistration = new QPushButton("Start Manual Registration",widget);
    connect(d->b_startManualRegistration,SIGNAL(clicked()),this,SLOT(startManualRegistration()));
    d->b_startManualRegistration->setObjectName("startManualRegistrationButton");

    QLabel* explanation = new QLabel("To add a landmark: \n\tShift + left mouse button\n", widget);

    d->numberOfLdInLeftContainer = new QLabel("Number of landmarks in left container: 0",widget);
    d->numberOfLdInRightContainer = new QLabel("Number of landmarks in right container: 0",widget);

    d->b_computeRegistration = new QPushButton("Compute Registration",widget);
    connect(d->b_computeRegistration,SIGNAL(clicked()),this,SLOT(computeRegistration()));
    d->b_computeRegistration->setObjectName("computeRegistrationButton");

    d->b_reset = new QPushButton("Reset",widget);
    d->b_reset->setObjectName("resetButton");
    connect(d->b_reset,SIGNAL(clicked()),this,SLOT(reset()));

    d->b_save = new QPushButton("Save Image",widget);
    d->b_save->setToolTip(tr("Save registered image"));
    d->b_save->setObjectName("saveButton");
    connect(d->b_save,SIGNAL(clicked()),this,SLOT(save()));

    d->b_exportTransformation = new QPushButton("Export Transformation",widget);
    d->b_exportTransformation->setObjectName("Export Transformation");
    d->b_exportTransformation->setToolTip(tr("Export the transformation in a tfm or txt file"));
    connect(d->b_exportTransformation,SIGNAL(clicked()),this,SLOT(exportTransformation()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(d->b_startManualRegistration);
    mainLayout->addWidget(explanation);
    mainLayout->addWidget(d->numberOfLdInLeftContainer);
    mainLayout->addWidget(d->numberOfLdInRightContainer);
    mainLayout->addWidget(d->b_computeRegistration);
    mainLayout->addWidget(d->b_reset);
    mainLayout->addWidget(d->b_save);
    mainLayout->addWidget(d->b_exportTransformation);
    widget->setLayout(mainLayout);
    this->addWidget(widget);

    d->viewGroup = new medViewParameterGroup("ManualRegistration", this);
    d->viewGroup->setLinkAllParameters(true);
    d->viewGroup->removeParameter("Position");
    d->viewGroup->removeParameter("DataList");

    d->layerGroup1 = new medLayerParameterGroup("Fixed", this);
    d->layerGroup1->setLinkAllParameters(true);
    d->layerGroup1->removeParameter("Slicing");

    d->layerGroup2 = new medLayerParameterGroup("Moving", this);
    d->layerGroup2->setLinkAllParameters(true);
    d->layerGroup2->removeParameter("Slicing");


    d->regOn           = false;
    d->currentView     = 0;
    d->leftContainer   = 0;
    d->rightContainer  = 0;
    d->bottomContainer = 0;
    d->controller      = 0;
    d->process         = 0;
    d->output          = 0;

    setDisableSaveButtons(true);
    setDisableComputeResetButtons(true);
    displayButtons(false);
}

manualRegistrationToolBox::~manualRegistrationToolBox()
{
    d->regOn           = false;
    d->currentView     = 0;
    d->leftContainer   = 0;
    d->rightContainer  = 0;
    d->bottomContainer = 0;
    d->controller      = 0;
    d->process         = 0;
    d->output          = 0;

    delete d;
    d = NULL;
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
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "manualRegistrationPlugin" );
    return plugin;
}

void manualRegistrationToolBox::updateView()
{
    medTabbedViewContainers * containers = this->getWorkspace()->stackedViewContainers();
    QList<medViewContainer*> containersInTabSelected = containers->containersInTab(containers->currentIndex());
    medAbstractView *view=NULL;
    for(int i=0;i<containersInTabSelected.length();i++)
    {
        if (containersInTabSelected[i]->isSelected())
        {
            view = containersInTabSelected[i]->view();
            break;
        }
    }

    medAbstractLayeredView * v = qobject_cast<medAbstractLayeredView*>(view);

    if (!v)
        return;

    if (!d->regOn) // if the registration is activated we do not change the currentView.
        d->currentView=v;

}

void manualRegistrationToolBox::startManualRegistration()
{
    if(d->regOn)
    {
        stopManualRegistration();
        return;
    }
    medTabbedViewContainers * tabContainers = getWorkspace()->stackedViewContainers();
    d->regOn = true;

    medRegistrationSelectorToolBox* toolbox = dynamic_cast<medRegistrationSelectorToolBox*>(selectorToolBox());

    if(toolbox)
    {
        dtkSmartPointer<medAbstractData> fixedData(toolbox->fixedData());
        dtkSmartPointer<medAbstractData> movingData(toolbox->movingData());

        if (!fixedData || !movingData)
        {
            d->regOn = false;
            return;
        }

        d->leftContainer = tabContainers->containersInTab(0).at(0);
        d->rightContainer = tabContainers->containersInTab(0).at(1);
        d->bottomContainer  = tabContainers->containersInTab(1).at(0);
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

    connect(d->rightContainer->view(),SIGNAL(closed()),this,SLOT(stopManualRegistration()));
    connect(d->leftContainer->view(),SIGNAL(closed()),this,SLOT(stopManualRegistration()));
}

void manualRegistrationToolBox::stopManualRegistration()
{
    disconnect(d->rightContainer->view(),SIGNAL(closed()),this,SLOT(stopManualRegistration()));
    disconnect(d->leftContainer->view(),SIGNAL(closed()),this,SLOT(stopManualRegistration()));

    d->b_startManualRegistration->setText("Start Manual Registration");
    reset();
    d->controller->SetEnabled(0);
    d->regOn           = false;
    d->controller->Delete();
    d->controller      = 0;
    d->output          = 0;
    d->leftContainer   = 0;
    d->rightContainer  = 0;
    d->bottomContainer = 0;
    displayButtons(false);

    if(!selectorToolBox())
    {
        medTabbedViewContainers * tabContainers = getWorkspace()->stackedViewContainers();
        tabContainers->setCurrentIndex(0);
        tabContainers->closeCurrentTab();
    }
}

void manualRegistrationToolBox::synchroniseMovingFuseView()
{
    if(!selectorToolBox()) // selectorToolBox does not exist in Pipelines
    {
        // This method will resynchronise the lut and window level
        medAbstractImageView * viewMoving = qobject_cast<medAbstractImageView*>(d->rightContainer->view());
        medAbstractImageView * viewFuse = qobject_cast<medAbstractImageView*>(d->bottomContainer->view());

        d->layerGroup2->addImpactedlayer(viewFuse,viewFuse->layerData(1));

        // Synchronize Lut
        QList<medAbstractInteractor*> interactors = viewMoving->layerInteractors(0);
        QString lutCurrent;
        QList<medAbstractParameter*> parameters;
        for(int i = 0;i<interactors.size();i++)
        {
            if (interactors[i]->identifier() == "medVtkViewItkDataImageInteractor")
            {
                parameters = interactors[i]->linkableParameters();
                for(int j = 0;j<parameters.size();j++)
                {
                    if (parameters[j]->name()=="Lut")
                    {
                        lutCurrent = qobject_cast<medStringListParameter*>(parameters[j])->value();
                    }
                }
            }
        }

        interactors = viewFuse->layerInteractors(1);

        for(int i = 0;i<interactors.size();i++)
        {
            if (interactors[i]->identifier() == "medVtkViewItkDataImageInteractor")
            {
                QList<medAbstractParameter*> parameters = interactors[i]->linkableParameters();
                for(int j = 0;j<parameters.size();j++)
                {
                    if (parameters[j]->name()=="Lut")
                    {
                        qobject_cast<medStringListParameter*>(parameters[j])->setValue(lutCurrent);
                    }
                }
            }
        }

        // Synchronize Window/Level
        medCompositeParameter * windowLevelParameter1 = viewMoving->windowLevelParameter(0);
        medCompositeParameter * windowLevelParameter2 = viewFuse->windowLevelParameter(1);
        QList<QVariant> windowLevel = windowLevelParameter1->values();
        QHash<QString,QVariant> hash;
        if (windowLevel.size()==2)
        {
            hash.insert("Level",windowLevel[0]);
            hash.insert("Window",windowLevel[1]);
        }
        windowLevelParameter2->setValues(hash);
    }
}

void manualRegistrationToolBox::computeRegistration()
{
    if (!d->controller)
        return;

    if (d->controller->checkLandmarks() == DTK_FAILURE)
        return;

    d->process = new manualRegistration();
    d->process->SetFixedLandmarks(d->controller->getPoints_Fixed());
    d->process->SetMovingLandmarks(d->controller->getPoints_Moving());
    d->process->setFixedInput(qobject_cast<medAbstractLayeredView*>(d->leftContainer->view())->layerData(0));
    d->process->setMovingInput(qobject_cast<medAbstractLayeredView*>(d->rightContainer->view())->layerData(0));
    d->process->update(itkProcessRegistration::FLOAT);

    medRegistrationSelectorToolBox* toolbox = dynamic_cast<medRegistrationSelectorToolBox*>(selectorToolBox());

    if(toolbox) // toolbox empty in Pipelines and not Registration workspace
    {
        toolbox->setProcess(d->process);
    }

    medAbstractData * newOutput = d->process->output();

    if(!newOutput)
    {
        return;
    }

    d->output = newOutput;

    medUtilities::setDerivedMetaData(d->output, (qobject_cast<medAbstractLayeredView*>(d->rightContainer->view())->layerData(0)), "registered");

    qobject_cast<medAbstractImageView*>(d->bottomContainer->view())->removeLayer(1);
    qobject_cast<medAbstractImageView*>(d->bottomContainer->view())->addLayer(d->output);
    synchroniseMovingFuseView();

    setDisableSaveButtons(false);
}

void manualRegistrationToolBox::reset()
{
    medAbstractImageView * viewFuse   = qobject_cast<medAbstractImageView*>(d->bottomContainer->view());
    medAbstractImageView * viewMoving = qobject_cast<medAbstractImageView*>(d->rightContainer->view());
    medAbstractImageView * viewFix    = qobject_cast<medAbstractImageView*>(d->leftContainer->view());

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

    if(viewMoving) //not the case if called by viewMoving closed() signal
    {
        viewFuse->addLayer(viewMoving->layerData(0));
    }

    synchroniseMovingFuseView();

    // Disable Save/Transformation/Compute/Reset buttons
    setDisableSaveButtons(true);
    setDisableComputeResetButtons(true);
}

void manualRegistrationToolBox::save()
{
    if (d->controller && d->output)
    {
        medDataManager::instance()->importData(d->output, false);
    }
}

void manualRegistrationToolBox::exportTransformation()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save transformation in tfm (default) or txt."),
                               "/home/transformation.tfm",
                               tr("Text files (*.tfm *.txt)"));
    if(fileName != "")
    {
        d->process->writeTransform(fileName);
    }
}


void manualRegistrationToolBox::constructContainers(medTabbedViewContainers * tabContainers)
{
    if (d->currentView && getWorkspace())
    {
        if (d->currentView->layersCount()<2)
        {
            return;
        }

        medCompositeParameter * windowingParameter0 = qobject_cast<medAbstractImageView*>(d->currentView)->windowLevelParameter(0);
        QHash<QString, QVariant> windowing0;
        windowing0.insert("Level", windowingParameter0->value("Level"));
        windowing0.insert("Window", windowingParameter0->value("Window"));

        medCompositeParameter * windowingParameter1 = qobject_cast<medAbstractImageView*>(d->currentView)->windowLevelParameter(1);
        QHash<QString, QVariant> windowing1;
        windowing1.insert("Level", windowingParameter1->value("Level"));
        windowing1.insert("Window", windowingParameter1->value("Window"));

        tabContainers->setAcceptDrops(false);

        // deselect container before going to reformat mode, otherwise problem !
        foreach(QUuid u,tabContainers->containersSelected())
            foreach(medViewContainer * cont,tabContainers->containersInTab(tabContainers->currentIndex()))
                if (u == cont->uuid())
                    cont->setSelected(false);

        d->leftContainer = tabContainers->insertContainerInTab(0,"ManualRegistration");
        tabContainers->setCurrentIndex(0);
        d->leftContainer->addData(d->currentView->layerData(0));//
        qobject_cast<medAbstractImageView*>(d->leftContainer->view())->windowLevelParameter(0)->setValues(windowing0);

        d->bottomContainer = d->leftContainer->splitHorizontally();
        d->bottomContainer->addData(d->currentView->layerData(0));
        qobject_cast<medAbstractImageView*>(d->bottomContainer->view())->windowLevelParameter(0)->setValues(windowing0);
        d->bottomContainer->addData(d->currentView->layerData(1));
        qobject_cast<medAbstractImageView*>(d->bottomContainer->view())->windowLevelParameter(1)->setValues(windowing1);
        tabContainers->containersInTab(0);

        d->rightContainer = d->leftContainer->splitVertically();
        d->rightContainer->addData(d->currentView->layerData(1));
        qobject_cast<medAbstractImageView*>(d->rightContainer->view())->windowLevelParameter(0)->setValues(windowing1);

        //The Fuse view is the view where you see you the 2 volumes which should be registered.
        //By setting one volume's LUT to "Hot Metal", we more easily can differentiate both volumes
        //and then evaluate the registration.
        QList<medAbstractInteractor*> interactors =
                qobject_cast<medAbstractImageView*>(d->rightContainer->view())->layerInteractors(0);
        for(int i=0; i<interactors.size(); i++)
        {
            if (interactors[i]->identifier() == "medVtkViewItkDataImageInteractor")
            {
                QList<medAbstractParameter*> parameters = interactors[i]->linkableParameters();
                for(int j = 0;j<parameters.size();j++)
                {
                    if (parameters[j]->name()=="Lut")
                    {
                        qobject_cast<medStringListParameter*>(parameters[j])->setValue("Hot Metal");
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
        d->currentView = 0; // we dont need it anymore.

        synchroniseMovingFuseView();
    }
}

void manualRegistrationToolBox::setDisableSaveButtons(bool disable)
{
    d->b_save->setDisabled(disable);
    d->b_exportTransformation->setDisabled(disable);
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
        d->b_reset->show();
        d->b_computeRegistration->show();
        d->b_save->show();
        d->b_exportTransformation->show();
    }
    else
    {
        d->b_reset->hide();
        d->b_computeRegistration->hide();
        d->b_save->hide();
        d->b_exportTransformation->hide();
    }
}

void manualRegistrationToolBox::updateGUI(int left,int right)
{
    d->numberOfLdInLeftContainer->setText( "Number of landmarks in left container: " + QString::number(left));
    d->numberOfLdInRightContainer->setText("Number of landmarks in right container: " + QString::number(right));

    if (left==right)
    {
        if(left>0)
        {
            setDisableComputeResetButtons(false);
        }
    }

    if ((left>0) || (right>0))
    {
        // at least one landmark put
        setDisableResetButton(false);
    }
}
