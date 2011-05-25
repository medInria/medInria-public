#include "vtkLogForwarder.h"

#include <vtkObjectFactory.h>

#include <dtkCore/dtkLog.h>

vtkCxxRevisionMacro(vtkLogForwarder, "$Revision: 1 $");
vtkStandardNewMacro(vtkLogForwarder);

void vtkLogForwarder::DisplayErrorText( const char *txt )
{
    dtkError() << txt;
}

void vtkLogForwarder::DisplayWarningText( const char *txt )
{
    dtkWarning() << txt;
}

void vtkLogForwarder::DisplayDebugText( const char *txt )
{
    dtkDebug() << txt;
}

void vtkLogForwarder::PrintSelf( ostream& os, vtkIndent indent )
{
    this->Superclass::PrintSelf(os, indent);  
}

vtkLogForwarder::vtkLogForwarder()
{
    //set the default outputwindow to be our forwarder
    vtkOutputWindow::SetInstance(this);
}

vtkLogForwarder::~vtkLogForwarder()
{

}
