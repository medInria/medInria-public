/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSegmentationSelectorToolBox.h>





#include <medToolBoxFactory.h>
#include <medToolBoxTab.h>
#include <medSegmentationAbstractToolBox.h>
#include <medToolBoxHeader.h>
#include <medViewEventFilter.h>

#include <QtGui>


class medSegmentationSelectorToolBoxPrivate
{
public:
    QComboBox *chooseSegmentationComboBox;
    medSegmentationAbstractToolBox * currentSegmentationToolBox;
    QHash<QString, medSegmentationAbstractToolBox*> segmentationToolBoxes;
    QVBoxLayout *mainLayout;
};

medSegmentationSelectorToolBox::medSegmentationSelectorToolBox(QWidget *parent) :
    medToolBox(parent),
    d(new medSegmentationSelectorToolBoxPrivate)
{
    d->currentSegmentationToolBox = NULL;


    d->chooseSegmentationComboBox = new QComboBox;
    //TODO algorithm is not the best IMO - RDE
    d->chooseSegmentationComboBox->addItem("Choose algorithm");
    d->chooseSegmentationComboBox->setToolTip(tr("Browse through the list of available segmentation algorithm"));

    medToolBoxFactory* tbFactory = medToolBoxFactory::instance();
    int i = 1; //account for the choose Filter item
    foreach(QString toolboxName, tbFactory->toolBoxesFromCategory("segmentation"))
    {
        medToolBoxDetails* details = tbFactory->toolBoxDetailsFromId(toolboxName);
        d->chooseSegmentationComboBox->addItem(details->name, toolboxName);
        d->chooseSegmentationComboBox->setItemData(i,
                                                   details->description,
                                                   Qt::ToolTipRole);
        i++;
    }

    connect(d->chooseSegmentationComboBox, SIGNAL(activated(int)), this, SLOT(changeCurrentToolBox(int)));

    QWidget *mainWidget = new QWidget;
    d->mainLayout = new QVBoxLayout;

    d->chooseSegmentationComboBox->adjustSize();
    d->mainLayout->addWidget(d->chooseSegmentationComboBox);
    mainWidget->setLayout(d->mainLayout);
    this->addWidget(mainWidget);
    this->setTitle("Segmentation");
}

medSegmentationSelectorToolBox::~medSegmentationSelectorToolBox(void)
{
    delete d;
    d = NULL;
}

medSegmentationAbstractToolBox* medSegmentationSelectorToolBox::currentToolBox()
{
    return d->currentSegmentationToolBox;
}


void medSegmentationSelectorToolBox::changeCurrentToolBox(int index)
{
    medSegmentationAbstractToolBox *toolbox = NULL;
    //get identifier for toolbox.
    QString identifier = d->chooseSegmentationComboBox->itemData(index).toString();
    if (d->segmentationToolBoxes.contains (identifier))
        toolbox = d->segmentationToolBoxes[identifier];
    else
    {
        medToolBox* tb = medToolBoxFactory::instance()->createToolBox(identifier, this);
        toolbox = qobject_cast<medSegmentationAbstractToolBox*>(tb);
        if (toolbox)
        {
            toolbox->setStyleSheet("medToolBoxBody {border:none}");
            d->segmentationToolBoxes[identifier] = toolbox;
            connect(toolbox, SIGNAL(installEventFilterRequest(medViewEventFilter*)),
                    this, SIGNAL(installEventFilterRequest(medViewEventFilter*)),
                    Qt::UniqueConnection);
        }
    }

    if(d->currentSegmentationToolBox)
    {
        d->currentSegmentationToolBox->hide();
        d->mainLayout->removeWidget(d->currentSegmentationToolBox);
        d->currentSegmentationToolBox = NULL;
    }

    if(!toolbox)
    {
        this->setAboutPluginVisibility(false);
        return;
    }


    d->currentSegmentationToolBox = toolbox;
    d->currentSegmentationToolBox->header()->hide();

    dtkPlugin* plugin = d->currentSegmentationToolBox->plugin();
    this->setAboutPluginButton(plugin);
    this->setAboutPluginVisibility(true);

    d->currentSegmentationToolBox->show();
    d->mainLayout->addWidget(d->currentSegmentationToolBox);
}


//void medSegmentationSelectorToolBox::changeCurrentToolBox(const QByteArray& id)
//{
//    medSegmentationAbstractToolBox *toolBox = qobject_cast<medSegmentationAbstractToolBox*>(medToolBoxFactory::instance()->createToolBox(QString(id), this));

//    if(!toolBox) {
//        dtkDebug() << "Unable to instantiate" << id << "toolbox";
//        return;
//    }

//    // toolbox->setParent(this);
//    //get rid of old toolBox
//    if (d->currentSegmentationToolBox)
//    {
//        emit removeToolBox(d->currentSegmentationToolBox);
//        delete d->currentSegmentationToolBox;
//    }
//    d->currentSegmentationToolBox = toolBox;
//    toolBox->show();
//    emit addToolBox(toolBox);
//}


//void medSegmentationSelectorToolBox::onSuccess( QObject * sender )
//{
////        alg->update();
//    // At this point the sender has already been deleted by the thread pool.
//    // Do not attempt to do anything with it (this includes qobject_cast).
//    if (! d->runningProcesses.contains(sender))
//        return;

//    dtkAbstractProcess * alg = d->runningProcesses.value( sender );
//    dtkSmartPointer<medAbstractData> outputData = dynamic_cast<medAbstractData*>(alg->output());

//    medDataManager::instance()->importNonPersistent( outputData.data() );
//    d->runningProcesses.remove( sender );
//}

//void medSegmentationSelectorToolBox::onFailure( QObject * sender )
//{
//    d->runningProcesses.remove( sender );
//}

//void medSegmentationSelectorToolBox::onCancelled( QObject * sender )
//{
//    d->runningProcesses.remove( sender );
//}

//void medSegmentationSelectorToolBox::run( dtkAbstractProcess * alg )
//{
//    QScopedPointer<medRunnableProcess> runProcessSp (new medRunnableProcess) ;
//    medRunnableProcess * runProcess  = runProcessSp.data();

//    runProcess->setProcess (alg);

//    this->progressionStack()->addJobItem(runProcess, "Progress:");

//    connect (runProcess, SIGNAL (success(QObject*)),  this, SLOT (onSuccess(QObject*)));
//    connect (runProcess, SIGNAL (failure(QObject*)),  this, SLOT (onFailure(QObject*)));
//    connect (runProcess, SIGNAL (cancelled(QObject*)), this, SLOT (onCancelled(QObject*)));

//    medJobManager::instance()->registerJobItem(runProcess, tr("Segmenting"));
//    d->runningProcesses.insert(runProcess, dtkSmartPointer< dtkAbstractProcess >(alg) );
//    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcessSp.take()));
//}



//void medSegmentationSelectorToolBox::addViewEventFilter( medViewEventFilter * filter )
//{
//    //make it fit with new containers - RDE
//    QList< medAbstractView *> views = d->workspace->currentViewContainer()->views();
//    foreach( medAbstractView * view, views )
//    {
//        medAbstractImageView * mview = qobject_cast<medAbstractImageView *>(view);
//        filter->installOnView(mview);
//    }
//}

//void medSegmentationSelectorToolBox::removeViewEventFilter( medViewEventFilter * filter )
//{
//    //make it fit with new containers - RDE
//    QList< medAbstractView *> views = d->workspace->currentViewContainer()->views();
//    foreach( medAbstractView * view, views )
//    {
//        medAbstractImageView * mview = qobject_cast<medAbstractImageView *>(view);
//        filter->removeFromView(mview);
//    }
//}


//void medSegmentationSelectorToolBox::setOutputMetadata(const medAbstractData * inputData, medAbstractData * outputData)
//{
//    Q_ASSERT(outputData && inputData);

//    QStringList metaDataToCopy;
//    metaDataToCopy // These are just copied from input to output. More can easily be added here.
//        << medMetaDataKeys::PatientName.key()
//        << medMetaDataKeys::StudyDescription.key();

//    foreach( const QString & key, metaDataToCopy ) {
//        if ( ! outputData->hasMetaData(key) )
//            outputData->setMetaData(key, inputData->metadatas(key));
//    }

//    if ( ! medMetaDataKeys::SeriesDescription.is_set_in(outputData) ) {
//        QString seriesDesc;
//        seriesDesc = tr("Segmented from ") + medMetaDataKeys::SeriesDescription.getFirstValue( inputData );

//        medMetaDataKeys::SeriesDescription.set(outputData,seriesDesc);
//    }
//}


