/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSegmentationSelectorToolBox.h>

#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <medAbstractViewInteractor.h>
#include <dtkLog/dtkLog.h>

#include <medAbstractImageData.h>
#include <medAbstractImageView.h>
#include <medDataManager.h>
#include <medJobManager.h>
#include <medMessageController.h>
#include <medMetaDataKeys.h>
#include <medProgressionStack.h>
#include <medRunnableProcess.h>
#include <medToolBoxFactory.h>
#include <medToolBoxTab.h>
#include <medSegmentationAbstractToolBox.h>
#include <medViewManager.h>
#include <medWorkspace.h>
#include <medViewEventFilter.h>

#include <QtGui>

struct AlgorithmInfo {
    QByteArray algName;
    QString localizedName;
    QString description;
};

class medSegmentationSelectorToolBoxPrivate
{
public:
    medSegmentationSelectorToolBoxPrivate() : progression_stack(NULL), algorithmParameterLayout(NULL),
        toolBoxes(NULL), customToolBox(NULL), workspace(NULL) { }

    medProgressionStack *progression_stack;
    QBoxLayout *algorithmParameterLayout;
    QComboBox *toolBoxes;
    medSegmentationAbstractToolBox * customToolBox;
    medWorkspace * workspace;

    dtkSmartPointer<dtkAbstractProcess> process;

    QString currentAlgorithm;

    typedef QHash< QObject *,  dtkSmartPointer< dtkAbstractProcess > > RunningProcessType;
    RunningProcessType runningProcesses;

    typedef QHash<QByteArray, AlgorithmInfo > AlgInfoContainerType;
    AlgInfoContainerType algInfo;

};

medSegmentationSelectorToolBox::medSegmentationSelectorToolBox( medWorkspace * workspace, QWidget *parent) : medToolBox(parent), d(new medSegmentationSelectorToolBoxPrivate)
{
    d->workspace = workspace;
    QWidget *displayWidget = new QWidget(this);
    d->algorithmParameterLayout = new QBoxLayout( QBoxLayout::LeftToRight );

    QVBoxLayout *displayLayout = new QVBoxLayout(displayWidget);

    this->setTitle("Segmentation");




    // Process section
    // --- Setting up custom toolboxes list ---
    d->toolBoxes = new QComboBox(displayWidget);
    displayLayout->addWidget(d->toolBoxes);
    d->toolBoxes->addItem("Choose algorithm");

    medToolBoxFactory* tbFactory = medToolBoxFactory::instance();
    foreach(QString toolBox, tbFactory->toolBoxesFromCategory("segmentation")) {
        medToolBoxDetails* details = tbFactory->toolBoxDetailsFromId(toolBox);

        QByteArray toolboxIdBA( toolBox.toAscii() );
        d->toolBoxes->addItem(details->name, QVariant(toolboxIdBA));
        d->toolBoxes->setItemData(d->toolBoxes->count() - 1,
            details->description,
            Qt::ToolTipRole);
    }
    // progression stack
    d->progression_stack = new medProgressionStack(displayWidget);
    displayLayout->addLayout( d->algorithmParameterLayout );
    displayLayout->addWidget( d->progression_stack );
    
    this->addWidget(displayWidget);

    connect( d->toolBoxes, SIGNAL( currentIndexChanged(int) ), this, SLOT( onToolBoxChosen( int )) );


    // /////////////////////////////////////////////////////////////////
    // Setup
    // /////////////////////////////////////////////////////////////////


    // ---
    d->toolBoxes->adjustSize();

    //Connect Message Controller:
    connect(this,SIGNAL(showError(const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showError(const QString&,unsigned int)));
    connect(this,SIGNAL(showInfo(const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showInfo(const QString&,unsigned int)));
}

medSegmentationSelectorToolBox::~medSegmentationSelectorToolBox(void)
{
    delete d;

    d = NULL;
}

medProgressionStack * medSegmentationSelectorToolBox::progressionStack()
{
    return d->progression_stack;
}

void medSegmentationSelectorToolBox::onAlgorithmAdded( const QString & algName )
{
    d->toolBoxes->addItem( algName, QVariant(
        QByteArray(algName.toAscii()) ) );
}

void medSegmentationSelectorToolBox::setAlgorithmParameterWidget( QWidget * widget )
{
    while ( ! d->algorithmParameterLayout->isEmpty() ) {
        QScopedPointer<QLayoutItem> item (d->algorithmParameterLayout->itemAt(0));
        d->algorithmParameterLayout->removeItem( item.data() );
    }
    d->algorithmParameterLayout->addWidget( widget );
}

void medSegmentationSelectorToolBox::onToolBoxChosen(int index)
{
    QByteArray algId =  (d->toolBoxes->itemData( index ) ).toByteArray();
    if ( !algId.isEmpty() ) {
        this->onToolBoxChosen( algId );
        emit toolBoxChosen( algId );
    }
}

void medSegmentationSelectorToolBox::onToolBoxChosen(const QByteArray& id)
{
    medSegmentationAbstractToolBox *toolBox = qobject_cast<medSegmentationAbstractToolBox*>(medToolBoxFactory::instance()->createToolBox(QString(id), this));

    if(!toolBox) {
        dtkDebug() << "Unable to instantiate" << id << "toolbox";
        return;
    }

    // toolbox->setParent(this);
    //get rid of old toolBox
    if (d->customToolBox)
    {
        emit removeToolBox(d->customToolBox);
        delete d->customToolBox;
    }
    d->customToolBox = toolBox;
    toolBox->show();
    emit addToolBox(toolBox);
}

void medSegmentationSelectorToolBox::clear(void)
{
    //maybe clear the customtoolbox?
    if (d->customToolBox)
        d->customToolBox->clear();
}

dtkAbstractProcess * medSegmentationSelectorToolBox::process(void)
{
    return d->process;
}

void medSegmentationSelectorToolBox::setProcess(dtkAbstractProcess* proc)
{
    d->process = proc;
}

medAbstractViewCoordinates * medSegmentationSelectorToolBox::viewCoordinates( medAbstractView * view )
{
//    medAbstractImageView * mview = qobject_cast< medAbstractImageView * >( view );
//    if ( ! mview ) {
//        dtkDebug() << "Failed to get a view";
//        return NULL;
//    }

    return NULL;
}

medAbstractData * medSegmentationSelectorToolBox::viewData( medAbstractView * view )
{
    medAbstractImageView * mview = qobject_cast< medAbstractImageView * >( view );
    if ( ! mview ) {
        dtkDebug() << "Failed to get a view";
        return NULL;
    }

    // Why isn't the data of an abstract view a dtkAbstractData????
    return mview->data(0);
}

void medSegmentationSelectorToolBox::onSuccess( QObject * sender )
{
//        alg->update();
    // At this point the sender has already been deleted by the thread pool.
    // Do not attempt to do anything with it (this includes qobject_cast).
    if (! d->runningProcesses.contains(sender) )
        return;
    dtkAbstractProcess * alg = d->runningProcesses.value( sender );

    dtkSmartPointer<medAbstractData> outputData = dynamic_cast<medAbstractData*>(alg->output());

    medDataManager::instance()->importNonPersistent( outputData.data() );

    d->runningProcesses.remove( sender );
}

void medSegmentationSelectorToolBox::onFailure( QObject * sender )
{
    d->runningProcesses.remove( sender );
}

void medSegmentationSelectorToolBox::onCancelled( QObject * sender )
{
    d->runningProcesses.remove( sender );
}

void medSegmentationSelectorToolBox::run( dtkAbstractProcess * alg )
{
    QScopedPointer<medRunnableProcess> runProcessSp (new medRunnableProcess) ;
    medRunnableProcess * runProcess  = runProcessSp.data();

    runProcess->setProcess (alg);

    this->progressionStack()->addJobItem(runProcess, "Progress:");

    connect (runProcess, SIGNAL (success(QObject*)),  this, SLOT (onSuccess(QObject*)));
    connect (runProcess, SIGNAL (failure(QObject*)),  this, SLOT (onFailure(QObject*)));
    connect (runProcess, SIGNAL (cancelled(QObject*)), this, SLOT (onCancelled(QObject*)));

    medJobManager::instance()->registerJobItem(runProcess, tr("Segmenting"));
    d->runningProcesses.insert(runProcess, dtkSmartPointer< dtkAbstractProcess >(alg) );
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcessSp.take()));
}



void medSegmentationSelectorToolBox::initializeAlgorithms()
{
    medToolBoxFactory * factory = medToolBoxFactory::instance();

    QList<QString> algorithmImplementations = factory->toolBoxesFromCategory("segmentation");
    foreach ( QString algName, algorithmImplementations ) {

        AlgorithmInfo itAlgInfo;
        itAlgInfo.algName = algName.toAscii();
        itAlgInfo.localizedName = algName;
        itAlgInfo.description = algName;

        d->algInfo.insert(itAlgInfo.algName, itAlgInfo);
    }
}

QString medSegmentationSelectorToolBox::localizedNameForAlgorithm( const QString & algName ) const
{
    medSegmentationSelectorToolBoxPrivate::AlgInfoContainerType::const_iterator it( d->algInfo.find(algName.toAscii()) );
    if ( it != d->algInfo.end() ) {
        return it->localizedName;
    }
    return QString();
}


void medSegmentationSelectorToolBox::addViewEventFilter( medViewEventFilter * filter )
{
    QList< medAbstractView *> views = d->workspace->currentViewContainer()->views();
    foreach( medAbstractView * view, views ) {
        medAbstractImageView * mview = qobject_cast<medAbstractImageView *>(view);
        filter->installOnView(mview);
    }
}

void medSegmentationSelectorToolBox::removeViewEventFilter( medViewEventFilter * filter )
{
    QList< medAbstractView *> views = d->workspace->currentViewContainer()->views();
    foreach( medAbstractView * view, views ) {
        medAbstractImageView * mview = qobject_cast<medAbstractImageView *>(view);
        filter->removeFromView(mview);
    }
}

void medSegmentationSelectorToolBox::update( medAbstractView *view )
{
    medToolBox::update(view);
}

void medSegmentationSelectorToolBox::setOutputMetadata(const medAbstractData * inputData, medAbstractData * outputData)
{
    Q_ASSERT(outputData && inputData);

    QStringList metaDataToCopy;
    metaDataToCopy // These are just copied from input to output. More can easily be added here.
        << medMetaDataKeys::PatientName.key()
        << medMetaDataKeys::StudyDescription.key();

    foreach( const QString & key, metaDataToCopy ) {
        if ( ! outputData->hasMetaData(key) )
            outputData->setMetaData(key, inputData->metadatas(key));
    }

    if ( ! medMetaDataKeys::SeriesDescription.is_set_in(outputData) ) {
        QString seriesDesc;
        seriesDesc = tr("Segmented from ") + medMetaDataKeys::SeriesDescription.getFirstValue( inputData );

        medMetaDataKeys::SeriesDescription.set(outputData,seriesDesc);
    }
}


