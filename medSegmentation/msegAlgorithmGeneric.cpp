#include "msegAlgorithmGeneric.h"

#include <dtkCore/dtkAbstractData.h>

#include <dtkCore/dtkLog.h>

namespace mseg {

AlgorithmGeneric::AlgorithmGeneric()
{

}

AlgorithmGeneric::~AlgorithmGeneric()
{
}

void AlgorithmGeneric::addHandler( const QString & typeName, HandlerFunc & func )
{
    m_handlers[ typeName ] = func;
}

int AlgorithmGeneric::callHandler( dtkAbstractData * data )
{
    const QString dataDescription( data->description() );

    HandlerContainerType::iterator it( m_handlers.find( dataDescription ) );
    if ( it == m_handlers.end() ) {
        dtkWarning() << "Unknown data type encountered " << dataDescription;
        return DTK_FAILURE;
    }

    return it->run(data);
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

} // namespace mseg

