/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medRegistrationSelectorToolBox.h>

#include <dtkCoreSupport/dtkAbstractViewFactory.h>
#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <dtkCoreSupport/dtkAbstractProcess.h>
#include <dtkCoreSupport/dtkAbstractViewInteractor.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medDataManager.h>
#include <medJobManager.h>
#include <medMessageController.h>
#include <medMetaDataKeys.h>
#include <medAbstractView.h>
#include <medAbstractImageView.h>
#include <medAbstractRegistrationProcess.h>

#include <medAbstractImageData.h>
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

    QVBoxLayout *toolBoxLayout;

    medAbstractData *fixedData;
    medAbstractData *movingData;

    dtkSmartPointer <medAbstractRegistrationProcess> process;
    dtkSmartPointer <medAbstractRegistrationProcess> undoRedoProcess;

    medRegistrationAbstractToolBox * undoRedoToolBox;
    medRegistrationAbstractToolBox * currentToolBox;
    QString nameOfCurrentAlgorithm;
    QString savePath;

};

medRegistrationSelectorToolBox::medRegistrationSelectorToolBox(QWidget *parent) : medToolBox(parent), d(new medRegistrationSelectorToolBoxPrivate)
{
    d->fixedData  = NULL;
    d->movingData = NULL;
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

    foreach(QString toolbox, tbFactory->toolBoxesFromCategory("UndoRedoRegistration"))
    {
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

    connect(d->toolboxes, SIGNAL(activated(int)), this, SLOT(changeCurrentToolBox(int)));

    // ---
    QButtonGroup *layoutButtonGroup = new QButtonGroup(this);
    layoutButtonGroup->addButton(d->saveTransButton);

    QHBoxLayout *layoutButtonLayout = new QHBoxLayout;
    layoutButtonLayout->addWidget(d->saveTransButton);


    QWidget *toolBoxWidget =  new QWidget;
    d->toolBoxLayout = new QVBoxLayout(toolBoxWidget);
    d->toolBoxLayout->addLayout(layoutButtonLayout);
    d->toolBoxLayout->addWidget(d->toolboxes);

    if (d->undoRedoToolBox)
        this->addWidget(d->undoRedoToolBox);

    this->addWidget(toolBoxWidget);


    this->setTitle(tr("Registration"));
    d->currentToolBox = NULL;

    //Connect Message Controller:
    connect(this,SIGNAL(showError(const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showError(const QString&,unsigned int)));
    connect(this,SIGNAL(showInfo(const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showInfo(const QString&,unsigned int)));
//    connect(medJobManager::instance(),SIGNAL(jobRegistered(medJobItem*,QString)),this,SLOT(onJobAdded(medJobItem*,QString)));
}

medRegistrationSelectorToolBox::~medRegistrationSelectorToolBox(void)
{
    delete d;

    d = NULL;
}

//! Gets the fixedData.
medAbstractData *medRegistrationSelectorToolBox::fixedData(void)
{
    return d->fixedData;
}

//! Gets the movingData.
medAbstractData *medRegistrationSelectorToolBox::movingData(void)
{
    return d->movingData;
}


/**
 * Sets up the toolbox chosen and remove the old one.
 *
 * @param index The index of the toolbox that was chosen.
 */
void medRegistrationSelectorToolBox::changeCurrentToolBox(int index)
{
    //get rid of old toolBox
    if (d->currentToolBox)
    {
        d->currentToolBox->hide();
        d->toolBoxLayout->removeWidget(d->currentToolBox);
        d->currentToolBox = NULL;
    }

    //get identifier for toolbox.
    QString id = d->toolboxes->itemData(index).toString();

    medRegistrationAbstractToolBox *toolbox = qobject_cast<medRegistrationAbstractToolBox*>(medToolBoxFactory::instance()->createToolBox(id,this));

    if(!toolbox) {
        qWarning() << "Unable to instantiate" << id << "toolbox";
        return;
    }

    d->nameOfCurrentAlgorithm = medToolBoxFactory::instance()->toolBoxDetailsFromId(id)->name;

    toolbox->setRegistrationToolBox(this);
    d->currentToolBox = toolbox;
    d->currentToolBox->show();
    d->currentToolBox->header()->hide();
    d->toolBoxLayout->addWidget(d->currentToolBox);

    connect (toolbox, SIGNAL (success()), this, SIGNAL (success()));
    connect (toolbox, SIGNAL (failure()), this, SIGNAL (failure()));
    connect (toolbox, SIGNAL (success()),this,SLOT(enableSelectorToolBox()));
    connect (toolbox, SIGNAL (failure()),this,SLOT(enableSelectorToolBox()));

    if (!d->undoRedoProcess && !d->undoRedoToolBox)
    {
        connect(toolbox,SIGNAL(success()),this,SLOT(handleOutput()));
    }
}


//! Clears the toolbox.
void medRegistrationSelectorToolBox::clear(void)
{

    //maybe clear the currentToolBox?
    if (d->currentToolBox)
        d->currentToolBox->clear();
}

//! Gets the process.
medAbstractRegistrationProcess * medRegistrationSelectorToolBox::process(void)
{
    return d->process;
}

/**
 * Sets the process.
 *
 * @param proc The new process.
 */
void medRegistrationSelectorToolBox::setProcess(medAbstractRegistrationProcess* proc)
{
    d->process = proc;
}

medAbstractRegistrationProcess * medRegistrationSelectorToolBox::undoRedoProcess()
{
    return d->undoRedoProcess;
}

void medRegistrationSelectorToolBox::setUndoRedoProcess(medAbstractRegistrationProcess *proc)
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


void medRegistrationSelectorToolBox::handleOutput(typeOfOperation type, QString algoName)
{
    medAbstractData *output(NULL);
    if (type == algorithm)
        if (d->process)
            output = dynamic_cast<medAbstractData*>(d->process->output());
        else return;
    else
        if (d->undoRedoProcess)
            output = dynamic_cast<medAbstractData*>(d->undoRedoProcess->output());
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
            newDescription += "-" + algoName + "\n";
        else
            newDescription += " registered\n-" + algoName+ "\n";
    }
    else if (type == undo)
    {
        newDescription.remove(newDescription.lastIndexOf("-"),newDescription.size()-1);
        if (newDescription.count("-") == 0)
            newDescription.remove(" registered\n");
    }
    else if (type == reset)
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
        medDataManager::instance()->importData(output);

    d->movingData = output;
    emit movingDataRegistered(output);
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
bool medRegistrationSelectorToolBox::setFixedData(medAbstractData* data)
{
    d->fixedData = data;

    return d->undoRedoProcess
            && d->undoRedoProcess->setFixedInput(d->fixedData)
            && d->undoRedoProcess->setMovingInput(d->movingData);
}

bool medRegistrationSelectorToolBox::setMovingData(medAbstractData *data)
{
    d->movingData = data;

    return d->undoRedoProcess
            && d->undoRedoProcess->setFixedInput(d->fixedData)
            && d->undoRedoProcess->setMovingInput(d->movingData);
}
