/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medRegistrationWorkspace.h>

#include <dtkCore/dtkSignalBlocker.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include <medViewFactory.h>
#include <medAbstractView.h>

#include <medRegistrationSelectorToolBox.h>
#include <medViewContainer.h>
#include <medTabbedViewContainers.h>
#include <medRegistrationSelectorToolBox.h>
#include <medAbstractLayeredView.h>
#include <medStringListParameter.h>
#include <medAbstractRegistrationProcess.h>
#include <medJobManager.h>
#include <medMetaDataKeys.h>
#include <medDataManager.h>

#include <medToolBoxFactory.h>

#include <medViewParameterGroup.h>
#include <medLayerParameterGroup.h>

#include <medAbstractUndoRedoProcess.h>


class medRegistrationWorkspacePrivate
{
public:
//    medRegistrationSelectorToolBox * registrationToolBox;

//    medViewContainer *fixedContainer;
//    medViewContainer *movingContainer;
//    medViewContainer *fuseContainer;

//    medViewParameterGroup *viewGroup;
//    medLayerParameterGroup *fixedLayerGroup;
//    medLayerParameterGroup *movingLayerGroup;

//    dtkSmartPointer <medAbstractRegistrationProcess> process;
//    dtkSmartPointer <medAbstractRegistrationProcess> undoRedoRegistrationProcess;
//    medAbstractUndoRedoProcess* undoRedoProcess;

//    medAbstractData *fixedData;
//    medAbstractData *movingData;

//    QString savePath;
//    QString currentAlgorithmName;
};

medRegistrationWorkspace::medRegistrationWorkspace(QWidget *parent) : medAbstractWorkspace(parent), d(new medRegistrationWorkspacePrivate)
{
//    // -- Registration toolbox --

//    d->registrationToolBox = new medRegistrationSelectorToolBox(parent);
//    this->addToolBox(d->registrationToolBox);

//    QStringList implementations = dtkAbstractProcessFactory::instance()->implementations("medAbstractRegistrationProcess");
//    d->registrationToolBox->setAvailableProcesses(implementations);

//    d->viewGroup = new medViewParameterGroup("View Group 1", this, this->identifier());
//    d->fixedLayerGroup =  new medLayerParameterGroup("Fixed Group", this, this->identifier());
//    d->movingLayerGroup = new medLayerParameterGroup("Moving Group", this, this->identifier());

//    d->viewGroup->setLinkAllParameters(true);

//    d->fixedLayerGroup->setLinkAllParameters(true);
//    d->movingLayerGroup->setLinkAllParameters(true);

//    QList<medLayerParameterGroup*> layerGroups;
//    layerGroups.append(d->fixedLayerGroup);
//    layerGroups.append(d->movingLayerGroup);
//    this->setLayerGroups(layerGroups);

//    d->fixedData  = NULL;
//    d->movingData = NULL;

//    d->savePath = QDir::homePath();

//    connect(this->stackedViewContainers(), SIGNAL(currentChanged(int)), this, SLOT(updateUserLayerClosable(int)));

//    connect(d->registrationToolBox, SIGNAL(processSelected(QString)), this, SLOT(setupProcess(QString)));
//    connect(d->registrationToolBox, SIGNAL(startProcessRequested()), this, SLOT(startProcess()));
//    //connect(d->registrationToolBox, SIGNAL(movingDataRegistered(medAbstractData*)), this, SLOT(updateFromRegistrationSuccess(medAbstractData*)));
//    connect(d->registrationToolBox, SIGNAL(destroyed()), this, SLOT(removeSlectorInternToolBox()));


//    // undo redo process
//    QStringList undoRedoImplementations = dtkAbstractProcessFactory::instance()->implementations("itkUndoRedoProcessRegistration");
//    if(!undoRedoImplementations.isEmpty())
//        d->undoRedoRegistrationProcess = dynamic_cast<medAbstractRegistrationProcess*>(dtkAbstractProcessFactory::instance()->create(undoRedoImplementations[0]));

//    if(d->undoRedoRegistrationProcess)
//    {
//        d->registrationToolBox->setUndoRedoToolbox(d->undoRedoRegistrationProcess->toolbox());

//        d->undoRedoProcess = dynamic_cast<medAbstractUndoRedoProcess*>(d->undoRedoRegistrationProcess.data());
//        if(d->undoRedoProcess)
//        {
//            connect(d->undoRedoProcess->wrapper(), SIGNAL(processUndone()), this, SLOT(handleUndo()));
//            connect(d->undoRedoProcess->wrapper(), SIGNAL(processRedone()), this, SLOT(handleRedo()));
//            connect(d->undoRedoProcess->wrapper(), SIGNAL(undoRedoStackReset()), this, SLOT(handleReset()));
//        }
//    }

}

medRegistrationWorkspace::~medRegistrationWorkspace(void)
{
    delete d;
    d = NULL;
}

void medRegistrationWorkspace::setupViewContainerStack()
{

//    //the stack has been instantiated in constructor
//    if (!this->stackedViewContainers()->count())
//    {
//        d->fixedContainer = this->stackedViewContainers()->addContainerInTab(tr("Compare"));
//        QLabel *fixedLabel = new QLabel(tr("FIXED"));
//        fixedLabel->setAlignment(Qt::AlignCenter);
//        d->fixedContainer->setDefaultWidget(fixedLabel);
//        d->fixedContainer->setMultiLayered(false);
//        d->fixedContainer->setUserSplittable(false);
//        d->fixedContainer->setClosingMode(medViewContainer::CLOSE_VIEW);

//        d->movingContainer = d->fixedContainer->splitVertically();
//        QLabel *movingLabel = new QLabel(tr("MOVING"));
//        movingLabel->setAlignment(Qt::AlignCenter);
//        d->movingContainer->setDefaultWidget(movingLabel);
//        d->movingContainer->setUserSplittable(false);
//        d->movingContainer->setMultiLayered(false);
//        d->movingContainer->setClosingMode(medViewContainer::CLOSE_VIEW);


//        d->fuseContainer = this->stackedViewContainers()->addContainerInTab(tr("Fuse"));
//        QLabel *fuseLabel = new QLabel(tr("FUSE"));
//        fuseLabel->setAlignment(Qt::AlignCenter);
//        d->fuseContainer->setDefaultWidget(fuseLabel);
//        d->fuseContainer->setClosingMode(medViewContainer::CLOSE_BUTTON_HIDDEN);
//        d->fuseContainer->setUserSplittable(false);
//        d->fuseContainer->setAcceptDrops(false);

//        connect(d->fixedContainer, SIGNAL(viewContentChanged()),
//                this, SLOT(updateFromFixedContainer()));
//        connect(d->movingContainer,SIGNAL(viewContentChanged()),
//                this, SLOT(updateFromMovingContainer()));

//        connect(d->fixedContainer,SIGNAL(viewRemoved()),
//                this, SLOT(updateFromFixedContainer()));
//        connect(d->movingContainer,SIGNAL(viewRemoved()),
//                this, SLOT(updateFromMovingContainer()));

//        this->stackedViewContainers()->lockTabs();
//        this->stackedViewContainers()->setCurrentIndex(0);
//        d->fixedContainer->setSelected(true);
//        d->movingContainer->setSelected(false);
//    }
}

bool medRegistrationWorkspace::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("registration").size()!=0);
}

void medRegistrationWorkspace::setupProcess(QString process)
{
//    d->process = dynamic_cast<medAbstractRegistrationProcess*>(dtkAbstractProcessFactory::instance()->create(process));
//    if(d->process)
//    {
//        d->registrationToolBox->setProcessToolbox(d->process->toolbox());
//    }

//    d->currentAlgorithmName = process;
}

void medRegistrationWorkspace::updateFromMovingContainer()
{
//    if(!d->registrationToolBox)
//        return;

//    if(!d->movingContainer->view())
//    {
//        medAbstractLayeredView* fuseView  = dynamic_cast<medAbstractLayeredView*>(d->fuseContainer->view());
//        if(fuseView)
//        {
//            if(fuseView->layer(d->movingData) == 0)
//            {
//                d->fuseContainer->removeView();
//                d->fuseContainer->addData(d->fixedData);
//            }
//            else
//                fuseView->removeLayer(1);

//        }

//        return;
//    }

//    medAbstractLayeredView* movingView  = dynamic_cast<medAbstractLayeredView*>(d->movingContainer->view());
//    if(!movingView)
//    {
//        qWarning() << "Non layered view are not suported yet in registration workspace.";
//        return;
//    }

//    d->movingData = movingView->layerData(movingView->currentLayer());

//    medAbstractLayeredView* fuseView  = dynamic_cast<medAbstractLayeredView*>(d->fuseContainer->view());
//    if(fuseView)
//        fuseView->removeData(d->movingData);

//    d->fuseContainer->addData(d->movingData);
//    fuseView  = dynamic_cast<medAbstractLayeredView*>(d->fuseContainer->view());

//    if(d->movingData)
//    {
//        d->viewGroup->addImpactedView(movingView);
//        d->viewGroup->addImpactedView(fuseView);
//        d->viewGroup->removeParameter("DataList");

//        d->movingLayerGroup->addImpactedlayer(movingView, d->movingData);
//        d->movingLayerGroup->addImpactedlayer(fuseView, d->movingData);
//    }

}

void medRegistrationWorkspace::updateFromFixedContainer()
{
//    if(!d->registrationToolBox)
//        return;

//    if(!d->fixedContainer->view())
//    {
//        medAbstractLayeredView* fuseView  = dynamic_cast<medAbstractLayeredView*>(d->fuseContainer->view());
//        if(fuseView)
//        {
//            if(fuseView->layer(d->fixedData) == 0)
//            {
//                d->fuseContainer->removeView();
//                d->fuseContainer->addData(d->movingData);
//            }
//            else
//                fuseView->removeLayer(1);

//        }

//        return;
//    }

//    medAbstractLayeredView* fixedView  = dynamic_cast<medAbstractLayeredView*>(d->fixedContainer->view());
//    if(!fixedView)
//    {
//        qWarning() << "Non layered view are not suported yet in registration workspace.";
//        return;
//    }

//    d->fixedData = fixedView->layerData(fixedView->currentLayer());
//    medAbstractLayeredView* fuseView  = dynamic_cast<medAbstractLayeredView*>(d->fuseContainer->view());
//    if(fuseView)
//        fuseView->removeData(d->fixedData);

//    d->fuseContainer->addData(d->fixedData);
//    fuseView  = dynamic_cast<medAbstractLayeredView*>(d->fuseContainer->view());

//    if(d->fixedData)
//    {
//        d->viewGroup->addImpactedView(fixedView);
//        d->viewGroup->addImpactedView(fuseView);
//        d->viewGroup->removeParameter("DataList");

//        d->fixedLayerGroup->addImpactedlayer(fixedView, d->fixedData);
//        d->fixedLayerGroup->addImpactedlayer(fuseView, d->fixedData);
//    }

}


void medRegistrationWorkspace::updateUserLayerClosable(int tabIndex)
{
    if(tabIndex == 0)
        this->setUserLayerClosable(true);
    else
        this->setUserLayerClosable(false);
}

void medRegistrationWorkspace::updateFromRegistrationSuccess(medAbstractData *output)
{
//    if(!d->registrationToolBox)
//        return;

//    //TODO disconnect because we dont want to change input of the undo redo process.
//    //  find a better way to do it ? - RDE
//    d->movingContainer->disconnect(this);

//    d->movingContainer->removeView();
//    d->movingContainer->addData(output);

//    d->fuseContainer->removeView();
//    d->fuseContainer->addData(d->fixedData);
//    d->fuseContainer->addData(output);


//    // Relink the views...
//    medAbstractLayeredView* movingView  = dynamic_cast<medAbstractLayeredView*>(d->movingContainer->view());
//    if(!movingView)
//    {
//        qWarning() << "Non layered view are not suported yet in registration workspace.";
//        return;
//    }


//    medAbstractLayeredView* fuseView  = dynamic_cast<medAbstractLayeredView*>(d->fuseContainer->view());
//    if(!fuseView)
//    {
//        qWarning() << "Non layered view are not suported yet in registration workspace.";
//        return;
//    }

//    d->viewGroup->addImpactedView(movingView);
//    d->viewGroup->addImpactedView(fuseView);
//    d->viewGroup->removeParameter("DataList");

//    d->movingLayerGroup->addImpactedlayer(movingView, output);
//    d->movingLayerGroup->addImpactedlayer(fuseView, output);

//    connect(d->movingContainer,SIGNAL(viewContentChanged()),
//            this, SLOT(updateFromMovingContainer()));

//    connect(d->movingContainer,SIGNAL(viewRemoved()),
//            this, SLOT(updateFromMovingContainer()));
}

void medRegistrationWorkspace::handleUndo()
{
//    //TODO refactor handleOutput, split it into 3 ( or4 ?) methods
//    handleOutput(undo, "");
}

void medRegistrationWorkspace::handleRedo()
{
//   handleOutput(redo, "");
}

void medRegistrationWorkspace::handleReset()
{
//   handleOutput(reset, "");
}

void medRegistrationWorkspace::handleOutput(typeOfOperation type, QString algoName)
{
//    medAbstractData *output(NULL);
//    if (type == algorithm)
//        if (d->process)
//            output = dynamic_cast<medAbstractData*>(d->process->output());
//        else return;
//    else
//        if (d->undoRedoRegistrationProcess)
//            output = dynamic_cast<medAbstractData*>(d->undoRedoRegistrationProcess->output());
//        else return;

//    // We manage the new description of the image
//    QString newDescription = "";
//    if(d->movingData)
//        newDescription = d->movingData->metadata(medMetaDataKeys::SeriesDescription.key());

//    if (type==algorithm || type==redo)
//    {
//        if (type==algorithm)
//        {
//            algoName = d->currentAlgorithmName.remove(" "); // we remove the spaces to reduce the size of the QString as much as possible
//        }
//        if (newDescription.contains("registered"))
//            newDescription += "-" + algoName + "\n";
//        else
//            newDescription += " registered\n-" + algoName+ "\n";
//    }
//    else if (type == undo)
//    {
//        newDescription.remove(newDescription.lastIndexOf("-"),newDescription.size()-1);
//        if (newDescription.count("-") == 0)
//            newDescription.remove(" registered\n");
//    }
//    else if (type == reset)
//    {
//        if (newDescription.lastIndexOf(" registered") != -1)
//            newDescription.remove(newDescription.lastIndexOf(" registered"),newDescription.size()-1);
//        if(!d->fixedData || !d->movingData)
//            return;
//    }

//    foreach(QString metaData, d->fixedData->metaDataList())
//        output->addMetaData(metaData,d->fixedData->metaDataValues(metaData));

//    foreach(QString property, d->fixedData->propertyList())
//        output->addProperty(property,d->fixedData->propertyValues(property));

//    output->setMetaData(medMetaDataKeys::SeriesDescription.key(), newDescription);

//    QString generatedID = QUuid::createUuid().toString().replace("{","").replace("}","");
//    output->setMetaData ( medMetaDataKeys::SeriesID.key(), generatedID );

//    if (type==algorithm)
//        medDataManager::instance()->importData(output);

//    d->movingData = output;

//    this->updateFromRegistrationSuccess(output);
}

//! Saves the transformation.
void medRegistrationWorkspace::saveTrans()
{
//    if (!d->movingData)
//    {
//        //TODO GPR
//        //emit showError(tr  ("Please Select a moving image before saving"),3000);
//        return;
//    }
//    if (!d->process )
//    {
//        //TODO GPR
//        //emit showError(tr  ("Please run the registration before saving"),3000);
//        return;
//    }

//    //get the transformation type: affine or deformation field.
//    QString fileTypeSuggestion;
//    QString filterSelected;
//    QHash<QString,QString> suffix;
//    if (d->process->hasProperty("transformType"))
//    {
//        if ( d->process->property("transformType") == "rigid")
//            suffix[ tr("Transformation (*.txt)") ] = ".txt";
//        else
//        {
//            suffix[ tr("MetaFile (*.mha)") ] = ".mha";
//            suffix[ tr("MetaFile (*.mhd)") ] = ".mhd";
//            suffix[ tr("Nifti (*.nii)") ] = ".nii";
//            suffix[ tr("Analyse (*.hdr)") ] = ".hdr";
//            suffix[ tr("Nrrd (*.nrrd)") ] = ".nrrd";
//            suffix[ tr("VTK (*.vtk)") ] = ".vtk";
//            suffix[ tr("All supported files "
//                "(*.mha *.mhd *.nii *.hdr *.nrrd *.vtk)") ] = ".mha";
//        }
//        QHashIterator<QString, QString> i(suffix);
//        while (i.hasNext())
//        {
//            i.next();
//            fileTypeSuggestion += i.key();
//            if (i.hasNext())
//                fileTypeSuggestion += ";;";
//        }
//    }
//    QFileInfo file;
//    QString fileName;

//    file = QFileDialog::getSaveFileName(d->registrationToolBox, tr("Save Transformation"),
//        d->savePath,
//        fileTypeSuggestion,&filterSelected,QFileDialog::DontUseNativeDialog);

//    if (!file.filePath().isEmpty())
//    {
//        if (!file.completeSuffix().isEmpty())
//        {
//            if(!suffix.values().contains("."+file.completeSuffix()))
//            {
//                QMessageBox::warning(d->registrationToolBox, tr("Save Transformation"),tr("The save did not occur, you have to choose a format within the types suggested."));
//                d->savePath = file.absolutePath();
//                saveTrans(); // call again the function.
//                return;
//            }
//            fileName = file.filePath();
//        }
//        else
//        {
//            fileName = file.filePath() + suffix[filterSelected];
//            file.setFile(fileName);
//            if (file.exists())
//            {
//                 int res = QMessageBox::warning(d->registrationToolBox, tr("Save Transformation"),
//                                tr("The file ") + file.fileName() + tr(" already exists.\nDo you want to replace it?"),
//                                QMessageBox::Yes | QMessageBox::No);

//                if (res==QMessageBox::No){
//                    d->savePath = file.absolutePath();
//                    saveTrans(); // call again the function.
//                    return;
//                }
//            }
//        }

//        QStringList transformFileName;
//        transformFileName << ""<< fileName;
//        if (d->process->write(transformFileName))
//        {
//            //TODO GPR
//            //emit(showInfo(tr  ("Transformation saved"),3000));
//        }
//        else
//        {
//            //TODO GPR
//            //emit(showError(tr  ("Transformation saving failed, no suitable writer found"),3000));
//        }

//    }
//    d->savePath = QDir::homePath();
}

void medRegistrationWorkspace::removeSlectorInternToolBox()
{
//    d->registrationToolBox = NULL;
}

void medRegistrationWorkspace::startProcess()
{
//    d->process->setFixedInput(d->fixedData);
//    d->process->setMovingInput(d->movingData);

//    medRunnableProcess *runProcess = new medRunnableProcess(d->process, d->process->name());
//    connect (runProcess, SIGNAL (success()),this,SLOT(enableSelectorToolBox()));
//    connect (runProcess, SIGNAL (failure()),this,SLOT(enableSelectorToolBox()));

//    runProcess->start();
}


//TODO GPR: doesn't seem to be used...to check
//void medRegistrationWorkspace::onJobAdded(medAbstractJob* item, QString jobName)
//{
//    if (d->process)
//        if (jobName == d->process->identifier()){
//            dtkAbstractProcess * proc = static_cast<medRunnableProcess*>(item)->getProcess();
//            if (proc==d->process)
//                enableSelectorToolBox(false);
//        }
//}
