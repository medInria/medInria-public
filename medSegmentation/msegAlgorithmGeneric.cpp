#include "msegAlgorithmGeneric.h"

// Temporary fudge:
#include "msegAlgorithmConnectedThresholdParametersWidget.h"

#include <dtkCore/dtkAbstractData.h>

#include <dtkCore/dtkLog.h>

namespace mseg {

QString AlgorithmGeneric::ms_interfaceName = "mseg::AlgorithmGeneric";

class AlgorithmGenericPrivate 
{
    friend class AlgorithmGeneric;
    typedef QHash< QString, HandlerFunc * > HandlerContainerType;
    HandlerContainerType handlers;

    dtkSmartPointer< dtkAbstractData > inputData;
    dtkSmartPointer< dtkAbstractData > outputData;
};

AlgorithmGeneric::AlgorithmGeneric() :
 d (new AlgorithmGenericPrivate)
{

}

AlgorithmGeneric::~AlgorithmGeneric()
{
    qDeleteAll(d->handlers);
    delete d;
}

void AlgorithmGeneric::addHandler( const QString & typeName, HandlerFunc * func )
{
    typedef HandlerFunc * HandlerFuncPtr;
    // Should return NULL if not already present.
    // Get a reference so we can assign to it later.
    HandlerFuncPtr & oldFunc( d->handlers[ typeName ]);
     if ( oldFunc )
         delete oldFunc;
     oldFunc = func;
}

int AlgorithmGeneric::callHandler( dtkAbstractData * data )
{
    if ( !data  ) {
        dtkWarning() << "Attempt to run segmentation algorithm with no input set.";
        return DTK_FAILURE;
    }

    const QString dataDescription( data->description() );

    AlgorithmGenericPrivate::HandlerContainerType::const_iterator it( d->handlers.find( dataDescription ) );
    if ( it == d->handlers.end() ) {
        dtkWarning() << "Unknown data type encountered " << dataDescription;
        return DTK_FAILURE;
    }

    return (*it)->run(data);
}

bool AlgorithmGeneric::isHandled( const QString & dataDescription ) const
{
    AlgorithmGenericPrivate::HandlerContainerType::const_iterator it( d->handlers.find( dataDescription ) );
    return ( it != d->handlers.end() );
}

void AlgorithmGeneric::setInput( dtkAbstractData * data )
{
    this->d->inputData = data;
}

int AlgorithmGeneric::update()
{
    return this->callHandler( this->input() );
}

void AlgorithmGeneric::setOutput( dtkAbstractData * data )
{
    this->d->outputData = data;
}

void AlgorithmGeneric::reportItkProgress( const itk::Object * caller , float progress )
{
    emit progressed( static_cast<int>( progress * 100.) );
}

bool AlgorithmGeneric::isUndoAble()
{
    return false;
}

void AlgorithmGeneric::undo()
{

}

dtkAbstractData * AlgorithmGeneric::input()
{
    return d->inputData;
}

AlgorithmParametersWidget *AlgorithmGeneric::createParametersWidget(Controller * controller, QWidget *parent)
{
    return new AlgorithmParametersWidget(controller, parent);
}



} // namespace mseg

