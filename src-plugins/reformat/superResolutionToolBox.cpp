/*=========================================================================

 Plugin medInria - Super Resolution

 Copyright (c) IHU LIRYC 2013. All rights reserved.

 Author : Mathilde Merle -- mathilde.merle @ ihu-liryc.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "superResolutionToolBox.h"

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractView.h>
#include <medToolBoxFactory.h>
#include <medPluginManager.h>
#include <medDropSite.h>
#include <medAbstractDbController.h>
#include <medAbstractDataFactory.h>
#include <medRunnableProcess.h>
#include <medJobManager.h>
#include <medProgressionStack.h>
#include <medTabbedViewContainers.h>
#include <medViewContainer.h>
#include <medViewContainerSplitter.h>
#include <medUtilities.h>

class superResolutionToolBoxPrivate
{
public:
    QPushButton * run;
    dtkSmartPointer<medAbstractData> inputVol0;
    dtkSmartPointer<medAbstractData> inputVol1;
    dtkSmartPointer<medAbstractData> inputMask0;
    dtkSmartPointer<medAbstractData> inputMask1;

    dtkSmartPointer <medAbstractProcess> process;

    medAbstractData * output;

    medDropSite *dropOrOpenRoi0;
    medDropSite *dropOrOpenRoi1;
    medDropSite *dropOrOpenRoi2;
    medDropSite *dropOrOpenRoi3;

    medProgressionStack * progression_stack;
    medAbstractWorkspace * workspace;
};

superResolutionToolBox::superResolutionToolBox (QWidget *parent) : medSegmentationAbstractToolBox (parent), d(new superResolutionToolBoxPrivate)
{
    this->setTitle("Super Resolution");

    // Global widget
    QWidget *superResolutionToolBoxBody = new QWidget(this);
    QVBoxLayout *superResolutionToolBoxLayout =  new QVBoxLayout(superResolutionToolBoxBody);
    superResolutionToolBoxBody->setLayout(superResolutionToolBoxLayout);

    // Drop box for mask
    QHBoxLayout *dropBoxLayout0 =  new QHBoxLayout(superResolutionToolBoxBody);
    dropBoxLayout0->setContentsMargins(QMargins(0,0,0,0)); //Remove additional spacing
    d->dropOrOpenRoi0 = new medDropSite(superResolutionToolBoxBody);
    d->dropOrOpenRoi0->setToolTip(tr("Drop 1st binary mask"));
    d->dropOrOpenRoi0->setText(tr("Drop 1st mask"));
    d->dropOrOpenRoi0->setCanAutomaticallyChangeAppereance(false);
    dropBoxLayout0->addWidget(d->dropOrOpenRoi0);

    d->dropOrOpenRoi1 = new medDropSite(superResolutionToolBoxBody);
    d->dropOrOpenRoi1->setToolTip(tr("Drop 2nd binary mask"));
    d->dropOrOpenRoi1->setText(tr("Drop 2nd mask"));
    d->dropOrOpenRoi1->setCanAutomaticallyChangeAppereance(false);
    dropBoxLayout0->addWidget(d->dropOrOpenRoi1);

    // Drop box for volumes
    QHBoxLayout *dropBoxLayout1 = new QHBoxLayout(superResolutionToolBoxBody);
    dropBoxLayout1->setContentsMargins(QMargins(0,0,0,0)); //Remove additional spacing
    d->dropOrOpenRoi2 = new medDropSite(superResolutionToolBoxBody);
    d->dropOrOpenRoi2->setToolTip(tr("Drop anatomic volume"));
    d->dropOrOpenRoi2->setText(tr("Drop 1st volume"));
    d->dropOrOpenRoi2->setCanAutomaticallyChangeAppereance(false);
    dropBoxLayout1->addWidget(d->dropOrOpenRoi2);

    d->dropOrOpenRoi3 = new medDropSite(superResolutionToolBoxBody);
    d->dropOrOpenRoi3->setToolTip(tr("Drop 2nd anatomic volume"));
    d->dropOrOpenRoi3->setText(tr("Drop 2nd volume"));
    d->dropOrOpenRoi3->setCanAutomaticallyChangeAppereance(false);
    dropBoxLayout1->addWidget(d->dropOrOpenRoi3);

    superResolutionToolBoxLayout->addLayout(dropBoxLayout0);
    superResolutionToolBoxLayout->addLayout(dropBoxLayout1);

    // Clear Button
    QHBoxLayout *buttonBoxLayout =  new QHBoxLayout(superResolutionToolBoxBody);
    QPushButton *clearRoiButton = new QPushButton("Clear inputs", superResolutionToolBoxBody);
    clearRoiButton->setToolTip(tr("Clear previously loaded mask."));
    buttonBoxLayout->addWidget(clearRoiButton);

    // Button : launch process
    d->run = new QPushButton("Run", superResolutionToolBoxBody);
    d->run->setDisabled(false);
    connect(d->run,             SIGNAL(clicked()), this, SLOT(runProcessRequest()));
    buttonBoxLayout->addWidget(d->run);

    superResolutionToolBoxLayout->addLayout(buttonBoxLayout);
    this->addWidget(superResolutionToolBoxBody);

    // Progression stack
    d->progression_stack = new medProgressionStack(superResolutionToolBoxBody);
    superResolutionToolBoxLayout->addWidget(d->progression_stack);
    this->addWidget(d->progression_stack);

    // Handle
    connect (d->dropOrOpenRoi0, SIGNAL(objectDropped(const medDataIndex&)), this, SLOT(onVol0Imported(const medDataIndex&)));
    connect (d->dropOrOpenRoi1, SIGNAL(objectDropped(const medDataIndex&)), this, SLOT(onVol1Imported(const medDataIndex&)));
    connect (d->dropOrOpenRoi2, SIGNAL(objectDropped(const medDataIndex&)), this, SLOT(onVol2Imported(const medDataIndex&)));
    connect (d->dropOrOpenRoi3, SIGNAL(objectDropped(const medDataIndex&)), this, SLOT(onVol3Imported(const medDataIndex&)));
    connect (clearRoiButton,    SIGNAL(clicked()),                          this, SLOT(onClearRoiButtonClicked()));

    d->output = 0;
}

/* Slot called when external ROI image finishes being imported. */
void superResolutionToolBox::onVol0Imported(const medDataIndex& index){this->onRoiImportedDo(index, 0);}
void superResolutionToolBox::onVol1Imported(const medDataIndex& index){this->onRoiImportedDo(index, 1);}
void superResolutionToolBox::onVol2Imported(const medDataIndex& index){this->onRoiImportedDo(index, 2);}
void superResolutionToolBox::onVol3Imported(const medDataIndex& index){this->onRoiImportedDo(index, 3);}

void superResolutionToolBox::onRoiImportedDo(const medDataIndex& index, int inputNumber)
{
    medAbstractData * data = medDataManager::instance()->retrieveData(index);

    switch (inputNumber)
    {
    case 0:
    {
        d->dropOrOpenRoi0->setPixmap(medDataManager::instance()->thumbnail(index).scaled(d->dropOrOpenRoi0->sizeHint()));
        d->inputMask0 = data;
        break;
    }
    case 1:
    {
        d->dropOrOpenRoi1->setPixmap(medDataManager::instance()->thumbnail(index).scaled(d->dropOrOpenRoi1->sizeHint()));
        d->inputMask1 = data;
        break;
    }
    case 2:
    {
        d->dropOrOpenRoi2->setPixmap(medDataManager::instance()->thumbnail(index).scaled(d->dropOrOpenRoi2->sizeHint()));
        d->inputVol0 = data;
        break;
    }
    case 3:
    {
        d->dropOrOpenRoi3->setPixmap(medDataManager::instance()->thumbnail(index).scaled(d->dropOrOpenRoi3->sizeHint()));
        d->inputVol1 = data;
    }
    }
}

// Sets the image passed as parameter as the @medDropSite image.
void superResolutionToolBox::setImage0(const QImage& thumbnail){d->dropOrOpenRoi0->setPixmap(QPixmap::fromImage(thumbnail));}
void superResolutionToolBox::setImage1(const QImage& thumbnail){d->dropOrOpenRoi1->setPixmap(QPixmap::fromImage(thumbnail));}
void superResolutionToolBox::setImage2(const QImage& thumbnail){d->dropOrOpenRoi2->setPixmap(QPixmap::fromImage(thumbnail));}
void superResolutionToolBox::setImage3(const QImage& thumbnail){d->dropOrOpenRoi3->setPixmap(QPixmap::fromImage(thumbnail));}

void superResolutionToolBox::enableRunButton(const bool enable)
{
    d->run->setEnabled(enable);
}

superResolutionToolBox::~superResolutionToolBox()
{
    delete d;
    d = NULL;
}

bool superResolutionToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<superResolutionToolBox>();
}

dtkPlugin* superResolutionToolBox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "reformatPlugin" );
    return plugin;
}

void superResolutionToolBox::setWorkspace(medAbstractWorkspace * workspace)
{
    d->workspace = workspace;
    medTabbedViewContainers * containers = workspace->stackedViewContainers();
    QObject::connect(containers,SIGNAL(containersSelectedChanged()),this,SLOT(updateView()));
}

void superResolutionToolBox::runProcessRequest()
{
    if ( d->inputVol0 && d->inputVol1 && d->inputMask0 && d->inputMask1)
    {
        // Wait Cursor
        this->setCursor(Qt::WaitCursor);
        QApplication::processEvents();

        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer("superResolutionProcess");
        d->process->setInput(d->inputVol0, 0);
        d->process->setInput(d->inputVol1, 1);
        d->process->setInput(d->inputMask0,2);
        d->process->setInput(d->inputMask1,3);

        medRunnableProcess *runProcess = new medRunnableProcess;
        runProcess->setProcess (d->process);
        d->progression_stack->addJobItem(runProcess, "Progress:");
        d->progression_stack->disableCancel(runProcess);

        connect (runProcess, SIGNAL (success   (QObject*)),    this, SIGNAL (success ()));
        connect (runProcess, SIGNAL (success   (QObject*)),    this, SLOT   (setOutput()));          //after success of process, get output
        connect (runProcess, SIGNAL (failure   (QObject*)),    this, SIGNAL (failure ()));
        connect (runProcess, SIGNAL (cancelled (QObject*)),    this, SIGNAL (failure ()));
        connect (runProcess, SIGNAL (activate(QObject*,bool)), d->progression_stack, SLOT(setActive(QObject*,bool)));

        medJobManager::instance()->registerJobItem(runProcess);
        QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));

        // Arrow Cursor
        this->setCursor(Qt::ArrowCursor);
    }
}

medAbstractData* superResolutionToolBox::processOutput()
{
    return d->output;
}

// Open the process output using the current view container of the workspace
void superResolutionToolBox::setOutput()
{
    d->output = d->process->output();

    if ( d->output )
    {
        medDataManager::instance()->importData(d->output, false);

        medTabbedViewContainers * containers = d->workspace->stackedViewContainers();
        containers->containersInTab(containers->currentIndex()).at(0)->addData(d->output);
    }
}

void superResolutionToolBox::onClearRoiButtonClicked(void)
{
    d->dropOrOpenRoi0->clear();
    d->dropOrOpenRoi0->setText(tr("Drop 1st mask"));

    d->dropOrOpenRoi1->clear();
    d->dropOrOpenRoi1->setText(tr("Drop 2nd mask"));

    d->dropOrOpenRoi2->clear();
    d->dropOrOpenRoi2->setText(tr("Drop 1st volume"));

    d->dropOrOpenRoi3->clear();
    d->dropOrOpenRoi3->setText(tr("Drop 2nd volume"));

    d->inputVol0 = 0;
    d->inputVol1 = 0;
    d->inputMask0 = 0;
    d->inputMask1 = 0;
}
