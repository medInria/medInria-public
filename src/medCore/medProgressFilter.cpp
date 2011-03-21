#include "medProgressFilter.h"


medProgressFilter::medProgressFilter( QObject * parent, QObject * child, qreal startPercent, qreal endPercent ) 
    :
    _startPercent ( startPercent ),
    _endPercent( endPercent ),
    _currentProgress( -1 )
{
    connect( child, SIGNAL(progressed(int)), this, SLOT(childProgressed(int)) );
    connect( this,  SIGNAL(progressed(int)), parent, SIGNAL(progressed(int)) );
}

medProgressFilter::~medProgressFilter()
{
}

void medProgressFilter::childProgressed( int percent )
{
    int newProgress = percent*0.01*(_endPercent - _startPercent) + _startPercent;
    if ( newProgress != _currentProgress ) {

        _currentProgress = newProgress;
        emit( progressed(_currentProgress) );
    }
}

void medProgressFilter::setRange(qreal startPercent, qreal endPercent ) 
{
    _startPercent = startPercent;
    _endPercent = endPercent;
}
