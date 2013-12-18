/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medRegistrationSelectorToolBox.h"

#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewInteractor.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medDataManager.h>
#include <medViewManager.h>
#include <medJobManager.h>
#include <medMessageController.h>
#include <medMetaDataKeys.h>
#include <medAbstractView.h>

#include <medAbstractDataImage.h>
#include <medRunnableProcess.h>
#include <medToolBoxTab.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>

#include <medRegistrationAbstractToolBox.h>


#include <QtGui>

class medRegistrationSelectorToolBoxPrivate
{
public:
    QPushButton * saveTransButton;

    QComboBox *toolboxes;
    dtkSmartPointer<medAbstractView> fixedView;
    dtkSmartPointer<medAbstractView> movingView;
    dtkSmartPointer<medAbstractView> fuseView;

    dtkSmartPointer<medAbstractDataImage> fixedData;
    dtkSmartPointer<medAbstractDataImage> movingData;
    
    dtkSmartPointer<dtkAbstractProcess> process;
    dtkSmartPointer<dtkAbstractProcess> undoRedoProcess;

    medRegistrationAbstractToolBox * undoRedoToolBox;
    medRegistrationAbstractToolBox * customToolBox;
    QString nameOfCurrentAlgorithm;
    QString savePath;

};

medRegistrationSelectorToolBox::medRegistrationSelectorToolBox(QWidget *parent) : medToolBox(parent), d(new medRegistrationSelectorToolBoxPrivate)
{
    d->fuseView = NULL;
    d->fixedData  = NULL;
    d->movingData = NULL;
    d->fixedView  = NULL;
    d->movingView = NULL;
    d->process = NULL;
    d->undoRedoProcess = NULL;
    d->undoRedoToolBox = NULL;
    d->nameOfCurrentAlgorithm = "";
    d->savePath = QDir::homePath();
    // Process section
    
    d->saveTransButton = new QPushButton(tr("Export Last Transf."),this);
    d->saveTransButton->setToolTip(
                tr("Export the resulting transformation of the last algorithm to the File System"));
    connect (d->saveTransButton, SIGNAL(clicked()), this, SLOT(onSaveTrans()));


    // --- Setting up custom toolboxes list ---

    d->toolboxes = new QComboBox(this);
    d->toolboxes->addItem(tr("Choose algorithm"));
    d->toolboxes->setToolTip(
                tr( "Choose the registration algorithm"
                    " amongst the loaded plugins" ));
    medToolBoxFactory* tbFactory =medToolBoxFactory::instance();
    
    foreach(QString toolbox, tbFactory->toolBoxesFromCategory("UndoRedoRegistration")){
        medToolBoxDetails* details = tbFactory->toolBoxDetailsFromId(toolbox);
        medRegistrationAbstractToolBox * tb = qobject_cast<medRegistrationAbstractToolBox*>(medToolBoxFactory::instance()->createToolBox(toolbox));
        if(!tb)
            qWarning() << "Unable to instantiate" << details->name << "toolbox";
        else
        {
            tb->header()->hide();
            d->undoRedoToolBox = tb;
            d->undoRedoToolBox->setRegistrationToolBox(this);
        }
    }
    int i=1;
    foreach(QString toolbox, tbFactory->toolBoxesFromCategory("registration"))
    {
        medToolBoxDetails* details = tbFactory->toolBoxDetailsFromId(toolbox);
        d->toolboxes->addItem(details->name, toolbox);
        d->toolboxes->setItemData(i,
                                  details->description,
                                  Qt::ToolTipRole);
        i++;
    }  
    
    connect(d->toolboxes, SIGNAL(activated(int)), this, SLOT(onToolBoxChosen(int)));


    // /////////////////////////////////////////////////////////////////
    // Setup
    // /////////////////////////////////////////////////////////////////


    // ---
    QButtonGroup *layoutButtonGroup = new QButtonGroup(this);
    layoutButtonGroup->addButton(d->saveTransButton);

    QHBoxLayout *layoutButtonLayout = new QHBoxLayout;
    layoutButtonLayout->addWidget(d->saveTransButton);

    QVBoxLayout *layoutLayout = new QVBoxLayout;
    layoutLayout->addLayout(layoutButtonLayout);
    layoutLayout->addWidget(d->toolboxes);

    QWidget * layoutSection = new QWidget(this);
    layoutSection->setLayout(layoutLayout);

    if (d->undoRedoToolBox)
        addWidget(d->undoRedoToolBox);
    addWidget(d->toolboxes);
    addWidget(layoutSection);


    this->setTitle(tr("Registration"));
    d->customToolBox = NULL;

    //Connect Message Controller:
    connect(this,SIGNAL(showError(const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showError(const QString&,unsigned int)));
    connect(this,SIGNAL(showInfo(const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showInfo(const QString&,unsigned int)));
    connect(medJobManager::instance(),SIGNAL(jobRegistered(medJobItem*,QString)),this,SLOT(onJobAdded(medJobItem*,QString)));
}

medRegistrationSelectorToolBox::~medRegistrationSelectorToolBox(void)
{
    delete d;

    d = NULL;
}

//! Gets the fixedView.
dtkAbstractView *medRegistrationSelectorToolBox::fixedView(void)
{
    return d->fixedView;
}

//! Gets the movingView.
dtkAbstractView *medRegistrationSelectorToolBox::movingView(void)
{
    return d->movingView;
}

//! Gets the fuseView.
dtkAbstractView *medRegistrationSelectorToolBox::fuseView(void)
{
    return d->fuseView;
}

//! Gets the fixedData.
medAbstractDataImage *medRegistrationSelectorToolBox::fixedData(void)
{
    return d->fixedData;
}

//! Gets the movingData.
medAbstractDataImage *medRegistrationSelectorToolBox::movingData(void)
{
    return d->movingData;
}

/** 
 * Sets up the fixedView and the layer 0 of the fuseView.
 *
 * @param index The index that was imported.
 */
void medRegistrationSelectorToolBox::onFixedImageDropped (const medDataIndex& index)
{

    if(!d->fuseView)
    {
        dtkSmartPointer<medAbstractView> newView = dtkAbstractViewFactory::instance()->createSmartPointer("medVtkView");
        setFuseView(newView);
        d->fuseView->setProperty("Closable","false");
        emit newFuseView( d->fuseView);
    }

    if (!index.isValid())
        return;

    d->fixedData = medDataManager::instance()->data(index);

    if (!d->fixedData)
        return;

    d->fixedView = dynamic_cast<medAbstractView*>
                   (medViewManager::instance()->views(index).first());

    if(!d->fixedView) {
        qDebug() << "Unable to retrieve fixed view";
	return;
    }

    if (d->fuseView)
    {
        d->fuseView->blockSignals(true);

        d->fuseView->show();
        d->fuseView->removeLayerAt(1);
        d->fuseView->removeLayerAt(0);

        if (d->movingView && d->fuseView->layersCount()==1)
        {

            //only the moving view has been set: shift it to layer 1
            d->fuseView->setData(d->fixedData,0);
            d->fuseView->setData(d->movingData,1);
            if(d->undoRedoProcess)
            {
                d->undoRedoProcess->setInput(d->fixedData,  0);
                d->undoRedoProcess->setInput(d->movingData, 1);
            }
        }
        else
        {
            //either both views are set, or only the fixed view
            d->fuseView->setData(d->fixedData,0);
        }
        d->fuseView->reset();
        d->fuseView->update();
        this->synchroniseWindowLevel(d->movingView); // This line will synchronise the windowlvl between the movingView and fuseView.
        this->synchroniseWindowLevel(d->fixedView); // This line will synchronise the windowlvl between the fixedView and fuseView.
        d->fuseView->blockSignals(false);
    }
    connect(d->fixedView,SIGNAL(positionChanged(QVector3D,bool)),this,SLOT(synchronisePosition(QVector3D)));
    connect(d->fixedView,SIGNAL(windowingChanged(double,double,bool)),this,SLOT(synchroniseWindowLevel(void)));
    if (!d->movingView)
    {
        connect(d->fuseView,SIGNAL(positionChanged(QVector3D,bool)),this,SLOT(synchronisePosition(QVector3D)));
        connect(d->fuseView,SIGNAL(windowingChanged(double,double,bool)),this,SLOT(synchroniseWindowLevel(void)));
    }

}

/** 
 * Sets up the movingView and the layer 1 of the fuseView.
 *
 * @param index The index that was imported.
 */
void medRegistrationSelectorToolBox::onMovingImageDropped (const medDataIndex& index)
{

    if(!d->fuseView)
    {
        dtkSmartPointer<medAbstractView> newView = dtkAbstractViewFactory::instance()->createSmartPointer("medVtkView");
        setFuseView(newView);
        d->fuseView->setProperty("Closable","false");
        emit newFuseView( d->fuseView);
    }

    if (!index.isValid())
        return;

    d->movingData = medDataManager::instance()->data(index);

    if (!d->movingData)
        return;

    d->movingView = dynamic_cast<medAbstractView*>
                    (medViewManager::instance()->views
                     (index).first());

    if(!d->movingView)
    {
        qDebug() << "Unable to retrieve moving view";
        return;
    }

    if (d->fixedView) {
        d->movingView->update();
    }

    d->fuseView->blockSignals(true);

    d->fuseView->show();
    d->fuseView->removeLayerAt(1);
    d->fuseView->removeLayerAt(0);

    if (d->fixedData)
    {
        //already one layer present
        d->fuseView->setData(d->fixedData,0);
        d->fuseView->setData(d->movingData,1);
        if(d->undoRedoProcess)
        { 
            d->undoRedoProcess->setInput(d->fixedData,  0);
            d->undoRedoProcess->setInput(d->movingData, 1);
        }
    }
    else
    {
        //only the moving view is set
        d->fuseView->setData(d->movingData,0);
    }
    d->fuseView->reset();
    d->fuseView->update();
    this->synchroniseWindowLevel(d->movingView); // This line will synchronise the windowlvl between the movingView and fuseView.
    this->synchroniseWindowLevel(d->fixedView); // This line will synchronise the windowlvl between the fixedView and fuseView.
    d->fuseView->blockSignals(false);

    
    connect(d->movingView,SIGNAL(positionChanged(QVector3D,bool)),this,SLOT(synchronisePosition(QVector3D)));
    connect(d->movingView,SIGNAL(windowingChanged(double,double,bool)),this,SLOT(synchroniseWindowLevel(void)));
    if (!d->fixedView)
    {
        connect(d->fuseView,SIGNAL(positionChanged(QVector3D,bool)),this,SLOT(synchronisePosition(QVector3D)));
        connect(d->fuseView,SIGNAL(windowingChanged(double,double,bool)),this,SLOT(synchroniseWindowLevel(void)));
    }
}

/** 
 * Sets up the toolbox chosen and remove the old one.
 *
 * @param index The index of the toolbox that was chosen.
 */
void medRegistrationSelectorToolBox::onToolBoxChosen(int index)
{
    //get identifier for toolbox.
    QString id = d->toolboxes->itemData(index).toString();

    medRegistrationAbstractToolBox *toolbox = qobject_cast<medRegistrationAbstractToolBox*>(medToolBoxFactory::instance()->createToolBox(id,this));
    
    if(!toolbox) {
        qWarning() << "Unable to instantiate" << id << "toolbox";
        return;
    }

    d->nameOfCurrentAlgorithm = medToolBoxFactory::instance()->toolBoxDetailsFromId(id)->name;

    toolbox->setRegistrationToolBox(this);
    //get rid of old toolBox
    if (d->customToolBox)
    {
        emit removeToolBox(d->customToolBox);
        delete d->customToolBox;
    }
    d->customToolBox = toolbox;
    toolbox->show();
    emit addToolBox(toolbox);

    connect (toolbox, SIGNAL (success()), this, SIGNAL (success()));
    connect (toolbox, SIGNAL (failure()), this, SIGNAL (failure()));
    connect (toolbox, SIGNAL (success()),this,SLOT(enableSelectorToolBox()));
    connect (toolbox, SIGNAL (failure()),this,SLOT(enableSelectorToolBox()));

    if (!d->undoRedoProcess && !d->undoRedoToolBox)
    {
        connect(toolbox,SIGNAL(success()),this,SLOT(handleOutput()));
    }
}

/** 
 * Sets the fuseView.
 *
 * @param view The new fuseView.
 */
void medRegistrationSelectorToolBox::setFuseView(dtkAbstractView *view)
{
    if (!view)
        return;

    if( d->fuseView )
    {
        disconnect(d->fuseView, SIGNAL(dataRemoved(int)), this, SLOT(closeCompareView(int)));
    }

    d->fuseView = dynamic_cast <medAbstractView*> (view);
    connect(d->fuseView, SIGNAL(dataRemoved(int)), this, SLOT(closeCompareView(int)));
}

//! Clears the toolbox.
void medRegistrationSelectorToolBox::clear(void)
{

    //maybe clear the customtoolbox?
    if (d->customToolBox)
        d->customToolBox->clear();
}

//! Gets the process.
dtkAbstractProcess * medRegistrationSelectorToolBox::process(void)
{
    return d->process;
}

/** 
 * Sets the process.
 *
 * @param proc The new process.
 */
void medRegistrationSelectorToolBox::setProcess(dtkAbstractProcess* proc)
{
    d->process = proc;
}

dtkAbstractProcess * medRegistrationSelectorToolBox::undoRedoProcess()
{
    return d->undoRedoProcess;
}

void medRegistrationSelectorToolBox::setUndoRedoProcess(dtkAbstractProcess *proc)
{
    d->undoRedoProcess = proc;
}

QString medRegistrationSelectorToolBox::getNameOfCurrentAlgorithm()
{
    return d->nameOfCurrentAlgorithm;
}

//! Saves the transformation.
void medRegistrationSelectorToolBox::onSaveTrans()
{
    if (!d->movingData)
    {
        emit showError(tr  ("Please Select a moving image before saving"),3000);
        return;
    }
    if (!d->process )
    {
        emit showError(tr  ("Please run the registration before saving"),3000);
        return;
    }

    //get the transformation type: affine or deformation field.
    QString fileTypeSuggestion;
    QString filterSelected;
    QHash<QString,QString> suffix;
    if (d->process->hasProperty("transformType"))
    {
        if ( d->process->property("transformType") == "rigid")
            suffix[ tr("Transformation (*.txt)") ] = ".txt";
        else
        {
            suffix[ tr("MetaFile (*.mha)") ] = ".mha";
            suffix[ tr("MetaFile (*.mhd)") ] = ".mhd";
            suffix[ tr("Nifti (*.nii)") ] = ".nii";
            suffix[ tr("Analyse (*.hdr)") ] = ".hdr";
            suffix[ tr("Nrrd (*.nrrd)") ] = ".nrrd";
            suffix[ tr("VTK (*.vtk)") ] = ".vtk";
            suffix[ tr("All supported files "
                "(*.mha *.mhd *.nii *.hdr *.nrrd *.vtk)") ] = ".mha";
        }
        QHashIterator<QString, QString> i(suffix);
        while (i.hasNext()) 
        {
            i.next();
            fileTypeSuggestion += i.key();
            if (i.hasNext()) 
                fileTypeSuggestion += ";;";
        }
    }
    QFileInfo file;
    QString fileName;
    
    file = QFileDialog::getSaveFileName(this,tr("Save Transformation"),
        d->savePath,
        fileTypeSuggestion,&filterSelected,QFileDialog::DontUseNativeDialog);

    if (!file.filePath().isEmpty())
    {   
        if (!file.completeSuffix().isEmpty())
        {
            if(!suffix.values().contains("."+file.completeSuffix()))
            {
                QMessageBox::warning(this,tr("Save Transformation"),tr("The save did not occur, you have to choose a format within the types suggested."));
                d->savePath = file.absolutePath();
                onSaveTrans(); // call again the function.
                return;
            }
            fileName = file.filePath();
        }
        else
        {
            fileName = file.filePath() + suffix[filterSelected];
            file.setFile(fileName);
            if (file.exists())
            {
                 int res = QMessageBox::warning(this, tr("Save Transformation"),
                                tr("The file ") + file.fileName() + tr(" already exists.\nDo you want to replace it?"),
                                QMessageBox::Yes | QMessageBox::No);
                                
                if (res==QMessageBox::No){
                    d->savePath = file.absolutePath();
                    onSaveTrans(); // call again the function.
                    return;
                }
            }
        }

        QStringList transformFileName;
        transformFileName << ""<< fileName;
        if (d->process->write(transformFileName))
        {
            emit(showInfo(tr  ("Transformation saved"),3000));
        }
        else
        {
            emit(showError(tr  ("Transformation saving failed, no suitable writer found"),3000));
        }

    }
    d->savePath = QDir::homePath();
}

// TODO CHANGE COMMENTARY
//! If the registration has ended well, it sets the output's metaData and reset the movingView and fuseView with the registered image.
void medRegistrationSelectorToolBox::handleOutput(typeOfOperation type, QString algoName)
{   
    dtkSmartPointer<dtkAbstractData> output(NULL); //initialisation : UGLY but necessary
    
    if (type==algorithm)
        if (d->process)
            output = d->process->output();
        else return;
    else
        if (d->undoRedoProcess)
            output = d->undoRedoProcess->output();
        else return;

    // We manage the new description of the image
    QString newDescription = "";
    if(d->movingData)
        newDescription = d->movingData->metadata(medMetaDataKeys::SeriesDescription.key());

    if (type==algorithm || type==redo)
    {
        if (type==algorithm)
        {
            algoName = d->nameOfCurrentAlgorithm.remove(" "); // we remove the spaces to reduce the size of the QString as much as possible
        }
        if (newDescription.contains("registered"))
            newDescription += "-"+ algoName + "\n";
        else
            newDescription += " registered\n-" + algoName+ "\n";
    }
    else if (type==undo)
    {
        newDescription.remove(newDescription.lastIndexOf("-"),newDescription.size()-1);
        if (newDescription.count("-") == 0)
            newDescription.remove(" registered\n");
    }
    else if (type==reset)
    {
        if (newDescription.lastIndexOf(" registered") != -1)
            newDescription.remove(newDescription.lastIndexOf(" registered"),newDescription.size()-1);
        if(!d->fixedData || !d->movingData)
            return;
    }

    foreach(QString metaData, d->fixedData->metaDataList())
        output->addMetaData(metaData,d->fixedData->metaDataValues(metaData));

    foreach(QString property, d->fixedData->propertyList())
        output->addProperty(property,d->fixedData->propertyValues(property));

    output->setMetaData(medMetaDataKeys::SeriesDescription.key(), newDescription);
    
    QString generatedID = QUuid::createUuid().toString().replace("{","").replace("}","");
    output->setMetaData ( medMetaDataKeys::SeriesID.key(), generatedID );

    if (type==algorithm)
        medDataManager::instance()->importNonPersistent(output);

    if(output)
    {   
        d->movingData = output;

        d->movingView->setData(output, 0);
        // calling reset() will reset all the view parameters (position - zoom - window/level) to default
        d->movingView->reset();
        d->movingView->update();
        
        d->fuseView->setData(output,1);
        d->fuseView->update();
                       
        d->fixedView->setLinkPosition(true);
        d->fixedView->setLinkCamera(true);
        d->movingView->setLinkPosition(true);
        d->movingView->setLinkCamera(true);
     }
}

void medRegistrationSelectorToolBox::enableSelectorToolBox(bool enable){
    this->setEnabled(enable);
}

void medRegistrationSelectorToolBox::onJobAdded(medJobItem* item, QString jobName){
    if (d->process)
        if (jobName == d->process->identifier()){
            dtkAbstractProcess * proc = static_cast<medRunnableProcess*>(item)->getProcess();
            if (proc==d->process)
                enableSelectorToolBox(false);
        }
}

//! Synchronises the window/level of the layer 0 of the fixedView with the layer 0 of the fuseView, and the layer 0 of the movingView with the layer 1 of the fuseView. 
void medRegistrationSelectorToolBox::synchroniseWindowLevel(QObject * sender){ 
			
    double window,level;
    QObject * senderView;
    if (sender)
        senderView = sender;
    else
        senderView = QObject::sender();

    if (d->fixedView==senderView)
    {
        d->fixedView->windowLevel(level,window);
        d->fuseView->setCurrentLayer(0);
        d->fuseView->setWindowLevel(level,window);
    }
    else if (d->movingView==senderView)
    {
        d->movingView->windowLevel(level,window);
        d->fuseView->setCurrentLayer(1);
        d->fuseView->setWindowLevel(level,window);
    }
    else
    {		
        d->fuseView->windowLevel(level,window);
        if (d->fixedView && d->fixedView->windowingLinked() && d->movingView && d->movingView->windowingLinked())
        {
            d->fixedView->setWindowLevel(level,window);
            d->movingView->setWindowLevel(level,window);
            if (d->fuseView->currentLayer()==0)// Since the fixed view and moving view are linked we must assure that the two layers of the fuse view are changed.
            { 
                d->fuseView->setCurrentLayer(1);
                d->fuseView->setWindowLevel(level,window);
            }
            else if (d->fuseView->currentLayer()==1)
            {
                d->fuseView->setCurrentLayer(0);
                d->fuseView->setWindowLevel(level,window);
            }
        }
        else if (d->fixedView && d->fuseView->currentLayer()==0)
            d->fixedView->setWindowLevel(level,window);
        else if (d->movingView && (d->fuseView->currentLayer()==1 || d->fuseView->layersCount()==1))
            d->movingView->setWindowLevel(level,window);
        // In the case that the currentLayer>1 we do nothing.
    }
}

//! Synchronises the position between the compare and the fuse mode.
void medRegistrationSelectorToolBox::synchronisePosition(const QVector3D &position){ 
   
    if (d->fixedView==QObject::sender() || d->movingView==QObject::sender())
    {
        if (d->fixedView && d->fixedView->positionLinked() && d->movingView &&  d->movingView->positionLinked()) // If the fixedView and movingView are linked in position then the changes also appear in fuseView.
            d->fuseView->setPosition(position);
    }
    else // the changes in fuseView are propagated to the fixedView and movingView.
    {		
        if (d->fixedView)
            d->fixedView->setPosition(position);
        if (d->movingView)
            d->movingView->setPosition(position);
    }
}


void medRegistrationSelectorToolBox::onViewRemoved(dtkAbstractView* view)
{
    medAbstractView* closedView = dynamic_cast <medAbstractView*> (view);

    d->fuseView->blockSignals(true);

    if(closedView == d->movingView)
    {
        d->fuseView->removeLayerAt(1);
        d->movingData = NULL;

        if(!d->fixedData)
        {
            d->fuseView->close();
        }
    }
    else if(closedView == d->fixedView)
    {
        //Reset the Undo Redo stack if needed.
        d->fixedData = NULL;

        if(d->movingData)
        {
            d->fuseView->removeLayerAt(1);
            double window, level;
            d->movingView->windowLevel(window, level);
            d->fuseView->removeLayerAt(0);
            d->fuseView->setData(d->movingData, 0);
            d->fuseView->setCurrentLayer(0);
            d->fuseView->setWindowLevel(window, level);
        }
        else
        {
            d->fuseView->removeLayerAt(0);
            d->fuseView->close();
        }
    }

    emit viewRemoved();
    d->fuseView->blockSignals(false);
}


void medRegistrationSelectorToolBox::closeCompareView(int layer)
{
    if(layer == 0)
    {
        d->fixedView->close();
        d->fixedData = NULL;
    }
    else if(layer == 1)
    {
        d->movingView->close();
        d->movingData = NULL;
    }
}
