#include "itkLogForwarder.h"



#include <dtkCore/dtkLog.h>

namespace itk {


void LogForwarder::DisplayText( const char* t)
{
    dtkDebug() << t;
}

void LogForwarder::DisplayDebugText( const char *t )
{
    dtkDebug() << t;
}

void LogForwarder::DisplayGenericOutputText( const char *t )
{
    dtkDebug() << t;
}

void LogForwarder::DisplayWarningText( const char *t )
{
    dtkWarning() << t;
}

void LogForwarder::DisplayErrorText( const char *t )
{
    dtkError() << t;
}

LogForwarder::LogForwarder()
{
    //set the default outputwindow to be our forwarder
    itk::OutputWindow::SetInstance(this);
}

LogForwarder::~LogForwarder()
{

}

void LogForwarder::PrintSelf( std::ostream& os, Indent indent ) const
{
    Superclass::PrintSelf(os, indent);
}

} // end namespace itk
