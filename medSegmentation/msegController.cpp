#include "msegController.h"

#include "msegView.h"
#include "msegConfiguration.h"
#include "msegToolbox.h"
#include "msegAlgorithmInitializer.h"
#include "msegAlgorithmGeneric.h"
#include "msegAlgorithmParametersWidget.h"
#include "msegAnnotationInitializer.h"
#include "msegParametersWidgetFactory.h"

#include "medAbstractViewScene.h"
#include "medAnnotationGraphicsObject.h"
#include "medAnnotationFactory.h"
#include "medAnnotationData.h"

#include <medCore/medAbstractData.h>
#include <medCore/medAbstractView.h>
#include <medCore/medJobManager.h>
#include <medCore/medRunnableProcess.h>
#include <medCore/medDataManager.h>

#include <medProgressionStack.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkLog.h>
#include <dtkCore/dtkSmartPointer.h>

namespace mseg {

struct AlgorithmInfo {
    QString algName;
    QString localizedName;
};

class ControllerPrivate {
public:
    Configuration * configuration;

    typedef QHash<dtkAbstractView *, dtkSmartPointer<View> > ViewContainerType;
    ViewContainerType views;

    typedef QHash<QString, AlgorithmInfo > AlgInfoContainerType;
    AlgInfoContainerType algInfo;

    QString currentAlgorithm;

    typedef QHash< QObject *,  dtkSmartPointer< AlgorithmGeneric > > RunningProcessType;
    RunningProcessType runningProcesses;

    enum EventFilterStatus { FilterOff, FilterOn, FilterOnUntilClick };
    EventFilterStatus filterStatus;

    medAnnotationFactory * annotationFactory;
};

Controller::Controller(Configuration * configuration) :
    d (new ControllerPrivate)
{
    d->filterStatus = ControllerPrivate::FilterOff;
    d->annotationFactory = new medAnnotationFactory;

    d->configuration = configuration;
    connect(d->configuration,SIGNAL(viewAdded(dtkAbstractView*)),
        this,SLOT(onViewAdded(dtkAbstractView*)));
    connect(d->configuration,SIGNAL(viewRemoved(dtkAbstractView*)),
        this,SLOT(onViewRemoved(dtkAbstractView*)));


    initializeAlgorithms();

    initializeAnnotations();

    d->configuration->buildConfiguration();

    mseg::Toolbox *toolbox( d->configuration->segmentationToobox() );
    toolbox->setController(this);

    connect( this, SIGNAL( algorithmAdded(const QString &) ),
        toolbox, SLOT( onAlgorithmAdded(const QString &)) );

    connect( toolbox, SIGNAL( algorithmSelected(const QString &) ),
        this, SLOT( onAlgorithmSelected(const QString &) ) );

    foreach ( const AlgorithmInfo & alg,  d->algInfo ) {
        emit algorithmAdded( alg.algName );
    }
}

Controller::~Controller()
{
    delete d;
}

medAbstractViewScene * Controller::viewScene( dtkAbstractView * view )
{
    medAbstractView * mview = qobject_cast< medAbstractView * >( view );
    if ( ! mview ) {
        dtkLog::debug() << "Failed to get a view";
        return NULL;
    }

    medAbstractViewScene * ret  = mview->scene();
    return ret;
}

dtkAbstractData * Controller::viewData( dtkAbstractView * view )
{
    medAbstractView * mview = qobject_cast< medAbstractView * >( view );
    if ( ! mview ) {
        dtkLog::debug() << "Failed to get a view";
        return NULL;
    }

    // Why isn't the data of an abstract view a dtkAbstractData????
    return reinterpret_cast< dtkAbstractData * >( mview->data() );
}

void Controller::onSuccess( QObject * sender )
{
//        alg->update();
    // At this point the sender has already been deleted by the thread pool.
    // Do not attempt to do anything with it (this includes qobject_cast).
    if (! d->runningProcesses.contains(sender) ) 
        return;
    AlgorithmGeneric * alg = d->runningProcesses.value( sender );

#pragma message DTK_COMPILER_WARNING("JDS : Need to complete this")
//#error
    /*
    Create a segmentation algorithm, pass it the data and 
        run it. 
        gather the output.
        register the output in the non-persistent db
        Add the output as a layer to the view
    */

    dtkSmartPointer<dtkAbstractData> outputData = alg->output();

    medDataManager::instance()->importNonPersistent( outputData.data() );

    d->runningProcesses.remove( sender );
}

void Controller::onFailure( QObject * sender )
{
    d->runningProcesses.remove( sender );
}

void Controller::onCancelled( QObject * sender )
{
    d->runningProcesses.remove( sender );
}

void Controller::run( mseg::AlgorithmGeneric* alg )
{
    QScopedPointer<medRunnableProcess> runProcessSp (new medRunnableProcess) ;
    medRunnableProcess * runProcess  = runProcessSp.data();

    runProcess->setProcess (alg);

    d->configuration->progressionStack()->addJobItem(runProcess, "Progress:");

    connect (runProcess, SIGNAL (success(QObject*)),  this, SLOT (onSuccess(QObject*)));
    connect (runProcess, SIGNAL (failure(QObject*)),  this, SLOT (onFailure(QObject*)));
    connect (runProcess, SIGNAL (cancelled(QObject*)), this, SLOT (onCancelled(QObject*)));

    medJobManager::instance()->registerJobItem(runProcess, tr("Segmenting"));
    d->runningProcesses.insert(runProcess, dtkSmartPointer< AlgorithmGeneric >(alg) );
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcessSp.take()));
}



void Controller::initializeAlgorithms()
{
    AlgorithmInitializer::initialize();

    ParametersWidgetFactory * factory = ParametersWidgetFactory::instance();
    QList<QString> algorithmImplementations = factory->widgets();
    foreach ( QString algName, algorithmImplementations ) {

        AlgorithmInfo itAlgInfo;
        itAlgInfo.algName = algName;
        itAlgInfo.localizedName =factory->localizedName(algName);

        d->algInfo.insert(algName, itAlgInfo);
    }
}

QString Controller::localizedNameForAlgorithm( const QString & algName ) const
{
    ControllerPrivate::AlgInfoContainerType::const_iterator it( d->algInfo.find(algName) );
    if ( it != d->algInfo.end() ) {
        return it->localizedName;
    }
    return QString();
}

void Controller::onAlgorithmSelected( const QString & algName )
{
    if ( d->currentAlgorithm != algName )
    {
        mseg::Toolbox *toolbox( d->configuration->segmentationToobox() );
        ParametersWidgetFactory * factory = ParametersWidgetFactory::instance();
        AlgorithmParametersWidget * paramWidget = factory->create( algName, this, toolbox);
        toolbox->setAlgorithmParameterWidget( paramWidget );
        d->currentAlgorithm = algName;
    }
}


void Controller::addViewEventFilter( View * filter )
{
    QList< dtkAbstractView *> views = d->configuration->currentViewContainer()->views();
    foreach( dtkAbstractView * view, views ) {
        medAbstractViewScene *vscene = Controller::viewScene(view);
        vscene->installEventFilter( filter );
    }
}

void Controller::removeViewEventFilter( View * filter )
{
    QList< dtkAbstractView *> views = d->configuration->currentViewContainer()->views();
    foreach( dtkAbstractView * view, views ) {
        medAbstractViewScene *vscene = Controller::viewScene(view);
        vscene->removeEventFilter( filter );
    }
}


void Controller::initializeAnnotations()
{
    mseg::AnnotationInitializer::initialize();
}















} // namespace mseg


