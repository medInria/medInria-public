#include "msegController.h"

#include "msegView.h"
#include "msegConfiguration.h"
#include "msegToolbox.h"
#include "msegAlgorithmConnectedThreshold.h"
// Until I have AlgorithmParametersWidget
#include "msegAlgorithmConnectedThresholdParametersWidget.h"

#include "medAbstractViewScene.h"

#include <medCore/medAbstractView.h>
#include <medCore/medJobManager.h>
#include <medCore/medRunnableProcess.h>
#include <medCore/medDataManager.h>

#include <medGui/medProgressionStack.h>

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

    dtkSmartPointer< AlgorithmGeneric > currentAlgorithm;

    typedef QHash< QObject *,  dtkSmartPointer< AlgorithmGeneric > > RunningProcessType;
    RunningProcessType runningProcesses;

    enum EventFilterStatus { FilterOff, FilterOn, FilterOnUntilClick };
    EventFilterStatus filterStatus;
};

Controller::Controller(Configuration * configuration) :
    d (new ControllerPrivate)
{
    d->filterStatus = ControllerPrivate::FilterOff;

    d->configuration = configuration;
    connect(d->configuration,SIGNAL(viewAdded(dtkAbstractView*)),
        this,SLOT(onViewAdded(dtkAbstractView*)));
    connect(d->configuration,SIGNAL(viewRemoved(dtkAbstractView*)),
        this,SLOT(onViewRemoved(dtkAbstractView*)));


    initializeAlgorithms();

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

    QGraphicsView * qview = qobject_cast < QGraphicsView * >( mview->receiverWidget() );
    medAbstractViewScene * ret  = qobject_cast < medAbstractViewScene * >( qview->scene() );
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
    medRunnableProcess * runProcess = qobject_cast< medRunnableProcess *>(sender);

    if (! d->runningProcesses.contains(sender) ) 
        return;
    AlgorithmGeneric * alg = d->runningProcesses.value( sender );

#pragma message DTK_COMPILER_WARNING("JDS : Need to complete this")
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
    QScopedPointer<medRunnableProcess> runProcess (new medRunnableProcess) ;

    runProcess->setProcess (alg);

    d->configuration->progressionStack()->addJobItem(runProcess.data(), "Progress:");

    connect (runProcess.data(), SIGNAL (onSuccess  (QObject*)),  this, SLOT (onSuccess (QObject*)));
    connect (runProcess.data(), SIGNAL (onFailure  (QObject*)),  this, SLOT (onFailure (QObject*)));
    connect (runProcess.data(), SIGNAL (onCancelled (QObject*)), this, SLOT (onCancelled (QObject*)));

    medJobManager::instance()->registerJobItem(runProcess.data(), tr("Segmenting"));
    d->runningProcesses.insert(runProcess.data(), dtkSmartPointer< AlgorithmGeneric >(alg) );
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess.take()));
}

void Controller::onViewAdded( dtkAbstractView* view )
{

}

void Controller::onViewRemoved( dtkAbstractView* view )
{
    medAbstractViewScene *vscene = Controller::viewScene(view);
    // Filters?
}

void Controller::initializeAlgorithms()
{
    dtkAbstractProcessFactory *factory = dtkAbstractProcessFactory::instance();
    AlgorithmConnectedThreshold::registerAlgorithm(factory);


    QStringList algorithmImplementations = factory->implementations( AlgorithmGeneric::ms_interfaceName );
    foreach ( QString algName, algorithmImplementations ) {
        QScopedPointer< dtkAbstractProcess > procInstance( factory->create(algName) );
        AlgorithmGeneric *algInstance = qobject_cast< AlgorithmGeneric *>( procInstance.data() );
        AlgorithmInfo itAlgInfo;
        itAlgInfo.algName = algName;
        itAlgInfo.localizedName = algInstance->localizedName();

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
    if ( (!d->currentAlgorithm) ||
        (d->currentAlgorithm->description() != algName ) ) 
    {

        dtkAbstractProcessFactory *factory = dtkAbstractProcessFactory::instance();
        dtkSmartPointer< dtkAbstractProcess > procInstance;
        procInstance.takePointer( factory->create(algName) );
        dtkSmartPointer< AlgorithmGeneric > algInstance = dynamic_cast< AlgorithmGeneric *>(procInstance.data());
        d->currentAlgorithm = algInstance;

        mseg::Toolbox *toolbox( d->configuration->segmentationToobox() );
        QWidget * paramWidget = algInstance->createParametersWidget(this, toolbox);
        toolbox->setAlgorithmParameterWidget( paramWidget );

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










} // namespace mseg


