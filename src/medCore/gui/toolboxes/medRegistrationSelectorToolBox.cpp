/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medRegistrationSelectorToolBox.h>

#include <dtkCore/dtkAbstractViewFactory.h>
#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewInteractor.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medDataManager.h>
#include <medJobManager.h>
#include <medMessageController.h>

#include <medAbstractView.h>
#include <medAbstractImageView.h>
#include <medAbstractRegistrationProcess.h>

#include <medAbstractImageData.h>
#include <medToolBoxTab.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>
#include <medProgressionStack.h>

#include <medRegistrationAbstractToolBox.h>


#include <QtGui>

class medRegistrationSelectorToolBoxPrivate
{
public:
    QPushButton * saveTransButton;
    QComboBox *processesCombobox;

    QVBoxLayout *toolBoxLayout;

    medToolBox * undoRedoToolBox;
    medToolBox * currentToolBox;

    medProgressionStack* progressionStack;
};

medRegistrationSelectorToolBox::medRegistrationSelectorToolBox(QWidget *parent) : medToolBox(parent), d(new medRegistrationSelectorToolBoxPrivate)
{
    d->undoRedoToolBox = NULL;

    // Process section

    d->saveTransButton = new QPushButton(tr("Export Last Transf."),this);
    d->saveTransButton->setToolTip(
                tr("Export the resulting transformation of the last algorithm to the File System"));

    // --- Setting up custom toolboxes list ---

    d->processesCombobox = new QComboBox(this);
    d->processesCombobox->addItem(tr("Choose algorithm"));
    d->processesCombobox->setToolTip(
                tr( "Choose the registration algorithm"
                    " amongst the loaded plugins" ));

    // ---
    QButtonGroup *layoutButtonGroup = new QButtonGroup(this);
    layoutButtonGroup->addButton(d->saveTransButton);

    QHBoxLayout *layoutButtonLayout = new QHBoxLayout;
    layoutButtonLayout->addWidget(d->saveTransButton);

    QPushButton *runButton = new QPushButton(tr("Run"), this);
    runButton->setToolTip(tr("Start Registration"));

    d->progressionStack = new medProgressionStack;

    QWidget *toolBoxWidget =  new QWidget;
    d->toolBoxLayout = new QVBoxLayout(toolBoxWidget);

    d->toolBoxLayout->addLayout(layoutButtonLayout);
    d->toolBoxLayout->addWidget(d->processesCombobox);
    d->toolBoxLayout->addWidget(runButton);
    d->toolBoxLayout->addWidget(d->progressionStack);

    this->addWidget(toolBoxWidget);

    this->setTitle(tr("Registration"));
    d->currentToolBox = NULL;

    //Connect Message Controller:
//    connect(this,SIGNAL(showError(const QString&,unsigned int)),
//            medMessageController::instance(),SLOT(showError(const QString&,unsigned int)));
//    connect(this,SIGNAL(showInfo(const QString&,unsigned int)),
//            medMessageController::instance(),SLOT(showInfo(const QString&,unsigned int)));

    connect(d->processesCombobox, SIGNAL(activated(QString)), this, SIGNAL(processSelected(QString)));
    connect(runButton, SIGNAL(clicked()), this, SIGNAL(startProcessRequested()));
    connect (d->saveTransButton, SIGNAL(clicked()), this, SIGNAL(saveTransfoRequested()));
}

medRegistrationSelectorToolBox::~medRegistrationSelectorToolBox(void)
{
    delete d;

    d = NULL;
}

void medRegistrationSelectorToolBox::setAvailableProcesses(QStringList processes)
{
    d->processesCombobox->addItems(processes);
}

void medRegistrationSelectorToolBox::setProcessToolbox(medToolBox* tb)
{
    if (d->currentToolBox)
    {
        d->currentToolBox->hide();
        d->toolBoxLayout->removeWidget(d->currentToolBox);
        d->currentToolBox = NULL;
    }

    d->currentToolBox = tb;
    d->currentToolBox->show();
    d->currentToolBox->header()->hide();
    d->toolBoxLayout->addWidget(d->currentToolBox);
}

void medRegistrationSelectorToolBox::setUndoRedoToolbox(medToolBox* tb)
{
    d->undoRedoToolBox = tb;
    d->toolBoxLayout->insertWidget(0, d->undoRedoToolBox);
}


/**
 * Sets up the toolbox chosen and remove the old one.
 *
 * @param index The index of the toolbox that was chosen.
 */
//void medRegistrationSelectorToolBox::changeCurrentToolBox(int index)
//{
//    //get rid of old toolBox
//    if (d->currentToolBox)
//    {
//        d->currentToolBox->hide();
//        d->toolBoxLayout->removeWidget(d->currentToolBox);
//        d->currentToolBox = NULL;
//    }

//    //get identifier for toolbox.
//    QString id = d->processesCombobox->itemData(index).toString();

//    medRegistrationAbstractToolBox *toolbox = qobject_cast<medRegistrationAbstractToolBox*>(medToolBoxFactory::instance()->createToolBox(id,this));

//    if(!toolbox) {
//        qWarning() << "Unable to instantiate" << id << "toolbox";
//        return;
//    }

//    d->nameOfCurrentAlgorithm = medToolBoxFactory::instance()->toolBoxDetailsFromId(id)->name;

//    toolbox->setRegistrationToolBox(this);
//    d->currentToolBox = toolbox;
//    d->currentToolBox->show();
//    d->currentToolBox->header()->hide();
//    d->toolBoxLayout->addWidget(d->currentToolBox);

//    connect (toolbox, SIGNAL (success()), this, SIGNAL (success()));
//    connect (toolbox, SIGNAL (failure()), this, SIGNAL (failure()));
//    connect (toolbox, SIGNAL (success()),this,SLOT(enableSelectorToolBox()));
//    connect (toolbox, SIGNAL (failure()),this,SLOT(enableSelectorToolBox()));

//    if (!d->undoRedoProcess && !d->undoRedoToolBox)
//    {
//        connect(toolbox,SIGNAL(success()),this,SLOT(handleOutput()));
//    }
//}



//! Clears the toolbox.
void medRegistrationSelectorToolBox::clear(void)
{
    if (d->currentToolBox)
        d->currentToolBox->clear();
}



void medRegistrationSelectorToolBox::enableSelectorToolBox(bool enable)
{
    this->setEnabled(enable);
}



//void medRegistrationSelectorToolBox::setFixedData(medAbstractData* data)
//{
//    d->fixedData = data;

//    if(d->undoRedoProcess)
//    {
//        d->undoRedoProcess->setFixedInput(d->fixedData);
//        d->undoRedoProcess->setMovingInput(d->movingData);
//    }
//}

//void medRegistrationSelectorToolBox::setMovingData(medAbstractData *data)
//{
//    d->movingData = data;

//    if(d->undoRedoProcess)
//    {
//        d->undoRedoProcess->setFixedInput(d->fixedData);
//        d->undoRedoProcess->setMovingInput(d->movingData);
//    }
//}
