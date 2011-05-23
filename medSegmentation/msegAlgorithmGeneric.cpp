#include "msegAlgorithmGeneric.h"

#include <dtkCore/dtkAbstractData.h>

#include <dtkCore/dtkLog.h>

namespace mseg {

AlgorithmGeneric::AlgorithmGeneric()
{

}

AlgorithmGeneric::~AlgorithmGeneric()
{
    qDeleteAll(m_handlers);
}

void AlgorithmGeneric::addHandler( const QString & typeName, HandlerFunc * func )
{
    typedef HandlerFunc * HandlerFuncPtr;
    // Should return NULL if not already present.
    // Get a reference so we can assign to it later.
    HandlerFuncPtr & oldFunc( m_handlers[ typeName ]);
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

    HandlerContainerType::const_iterator it( m_handlers.find( dataDescription ) );
    if ( it == m_handlers.end() ) {
        dtkWarning() << "Unknown data type encountered " << dataDescription;
        return DTK_FAILURE;
    }

    return (*it)->run(data);
}

bool AlgorithmGeneric::isHandled( const QString & dataDescription ) const
{
    HandlerContainerType::const_iterator it( m_handlers.find( dataDescription ) );
    return ( it != m_handlers.end() );
}

void AlgorithmGeneric::setInput( dtkAbstractData * data )
{
    this->m_inputData = data;
}

int AlgorithmGeneric::update()
{
    return this->callHandler( this->input() );
}

void AlgorithmGeneric::setOutput( dtkAbstractData * data )
{
    this->m_outputData = data;
}



} // namespace mseg

