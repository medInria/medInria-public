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
    medAbstractView * currentView;

    QPushButton                * run;
    QVector<medAbstractData *>   inputs;
    superResolutionProcess     * process;

    medAbstractData * output;

    dtkSmartPointer<medAbstractProcess> threshold;

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
    //this->setAboutPluginVisibility(true); //information icon in toolbox
    //this->setAboutPluginButton(this->plugin());

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

    d->dropOrOpenRoi1 = new medDropSite(superResolutionToolBoxBody);
    d->dropOrOpenRoi1->setToolTip(tr("Drop 2nd binary mask"));
    d->dropOrOpenRoi1->setText(tr("Drop 2nd mask"));
    d->dropOrOpenRoi1->setCanAutomaticallyChangeAppereance(false);

    // Drop box for volumes
    QHBoxLayout *dropBoxLayout1 = new QHBoxLayout(superResolutionToolBoxBody);
    dropBoxLayout1->setContentsMargins(QMargins(0,0,0,0)); //Remove additional spacing
    d->dropOrOpenRoi2 = new medDropSite(superResolutionToolBoxBody);
    d->dropOrOpenRoi2->setToolTip(tr("Drop anatomic volume"));
    d->dropOrOpenRoi2->setText(tr("Drop 1st volume"));
    d->dropOrOpenRoi2->setCanAutomaticallyChangeAppereance(false);

    d->dropOrOpenRoi3 = new medDropSite(superResolutionToolBoxBody);
    d->dropOrOpenRoi3->setToolTip(tr("Drop 2nd anatomic volume"));
    d->dropOrOpenRoi3->setText(tr("Drop 2nd volume"));
    d->dropOrOpenRoi3->setCanAutomaticallyChangeAppereance(false);


    dropBoxLayout0->addWidget(d->dropOrOpenRoi0);
    dropBoxLayout0->addWidget(d->dropOrOpenRoi1);
    dropBoxLayout1->addWidget(d->dropOrOpenRoi2);
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
    //connect (d->dropOrOpenRoi0, SIGNAL(clicked()),                          this, SLOT(onDropSiteClicked()));
    connect (d->dropOrOpenRoi1, SIGNAL(objectDropped(const medDataIndex&)), this, SLOT(onVol1Imported(const medDataIndex&)));
    //connect (d->dropOrOpenRoi1, SIGNAL(clicked()),                          this, SLOT(onDropSiteClicked()));
    connect (d->dropOrOpenRoi2, SIGNAL(objectDropped(const medDataIndex&)), this, SLOT(onVol2Imported(const medDataIndex&)));
    connect (d->dropOrOpenRoi3, SIGNAL(objectDropped(const medDataIndex&)), this, SLOT(onVol3Imported(const medDataIndex&)));
    connect (clearRoiButton,    SIGNAL(clicked()),                          this, SLOT(onClearRoiButtonClicked()));
}

/* Slot called when external ROI image finishes being imported. */
void superResolutionToolBox::onVol0Imported(const medDataIndex& index){this->onRoiImportedDo(index, 0);}
void superResolutionToolBox::onVol1Imported(const medDataIndex& index){this->onRoiImportedDo(index, 1);}
void superResolutionToolBox::onVol2Imported(const medDataIndex& index){this->onRoiImportedDo(index, 2);}
void superResolutionToolBox::onVol3Imported(const medDataIndex& index){this->onRoiImportedDo(index, 3);}

void superResolutionToolBox::onRoiImportedDo(const medDataIndex& index, int inputNumber)
{
    medAbstractData * data = medDataManager::instance()->retrieveData(index);

    // we accept only ROIs (itkDataImageUChar3)
    if (!data){return;}

    // put the thumbnail in the medDropSite as well
    // (code copied from @medDatabasePreviewItem)
    medAbstractDbController* dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());
    QString thumbpath = dbc->metaData(index, medMetaDataKeys::ThumbnailPath);

    bool shouldSkipLoading = false;
    if ( thumbpath.isEmpty() )
    {
        // first try to get it from controller
        QPixmap thumbImage = dbc->thumbnail(index);
        if (!thumbImage.isNull())
        {
            switch (inputNumber)
            {
                case 0: {
                    d->dropOrOpenRoi0->setPixmap(thumbImage);
                    break;
                }
                case 1: {
                    d->dropOrOpenRoi1->setPixmap(thumbImage);
                    break;
                }
                case 2:
                {
                    d->dropOrOpenRoi2->setPixmap(thumbImage);
                    break;
                }
                case 3:
                {
                    d->dropOrOpenRoi3->setPixmap(thumbImage);
                }
           }
            shouldSkipLoading = true;
        }
    }
    if (!shouldSkipLoading) {

        QImageReader reader(thumbpath);

        switch (inputNumber)
        {
            case 0: {
                d->dropOrOpenRoi0->setPixmap(QPixmap::fromImage(reader.read()));
                break;
            }
            case 1: {
                d->dropOrOpenRoi1->setPixmap(QPixmap::fromImage(reader.read()));
                break;
            }
            case 2:
            {
                d->dropOrOpenRoi2->setPixmap(QPixmap::fromImage(reader.read()));
                break;
            }
            case 3:
            {
                d->dropOrOpenRoi3->setPixmap(QPixmap::fromImage(reader.read()));
            }
       }
    }

    d->inputs.push_back(data);
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
    updateView();
}

// Instantiate the superResolutionProcess using the instance of the dtkAbstractProcessFactory.
void superResolutionToolBox::runProcessRequest()
{
    if ( d->inputs.count() != 4 ) return;

    if ( !d->inputs[0] || !d->inputs[1] || !d->inputs[2] || !d->inputs[3])
        return;

    // Wait Cursor
    this->setCursor(Qt::WaitCursor);
    QApplication::processEvents();

    d->process = new superResolutionProcess;
    d->process->setInput(d->inputs[0],0);
    d->process->setInput(d->inputs[1],1);
    d->process->setInput(d->inputs[2],2);
    d->process->setInput(d->inputs[3],3);

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

medAbstractData* superResolutionToolBox::processOutput()
{
    if(!d->output)
        return NULL;

    return d->output;
}

// Open the process output using the current view container of the workspace
void superResolutionToolBox::setOutput()
{
    if(!d->process)
        return;

    d->output = d->process->output();

    if ( !d->output )
        return;

    medUtilities::setDerivedMetaData(d->output, d->inputs[0], "super resolution");
    medDataManager::instance()->importData(d->output, false);

    medTabbedViewContainers * containers = d->workspace->stackedViewContainers();
    containers->containersInTab(containers->currentIndex()).at(0)->addData(d->output); // TODO : find a solution for this. verify if it creates new container or add as a layer

    // Create binary of output
    /*d->threshold = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkThresholdingProcess" );
    if (!d->threshold)
        return;
    d->threshold->setInput(d->output);
    d->threshold->setParameter(2, 0); //thresholdValue
    d->threshold->setParameter(0.0, 1); //outsideValue
    d->threshold->setParameter(0.0, 2); //comparisonOperator (0 : <)
    d->threshold->update();

    d->threshold->setInput(d->threshold->output());
    d->threshold->setParameter(2, 0); //thresholdValue
    d->threshold->setParameter(1.0, 1); //outsideValue
    d->threshold->setParameter(1.0, 2); //comparisonOperator (0 : >)
    d->threshold->update();

    if (!d->threshold->output())
        return;

    // Display binary data in container
    medAbstractData * thresholdedImage = d->threshold->output();
    //thresholdedImage->copyMetaDataFrom(d->inputs[0]);
    setOutputMetadata(d->inputs[0], thresholdedImage);
    thresholdedImage->setMetaData ( medMetaDataKeys::SeriesDescription.key(), "SuperResolutionBinary" );
    medDataManager::instance()->importData(thresholdedImage, false); //save in database

    medViewContainerSplitter *splitter = dynamic_cast<medViewContainerSplitter*>(containers->widget(0));
    splitter->split(splitter->containers()[0])->addData(thresholdedImage);*/
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

    d->inputs.clear();
}

// Save current container when clicked
void superResolutionToolBox::updateView()
{
    medTabbedViewContainers * containers = d->workspace->stackedViewContainers();
    QList<medViewContainer*> containersInTabSelected =  containers->containersInTab(containers->currentIndex());
    medAbstractView *view=NULL;
    for(int i=0;i<containersInTabSelected.length();i++)
    {
        if (containersInTabSelected[i]->isSelected())
        {
            view = containersInTabSelected[i]->view();
            break;
        }
    }

    if (!view)
        return;

    d->currentView = view;
}
