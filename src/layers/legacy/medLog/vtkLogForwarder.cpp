/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkLogForwarder.h>

#include <vtkObjectFactory.h>

#include <dtkLog/dtkLog.h>

vtkStandardNewMacro(vtkLogForwarder);

void vtkLogForwarder::DisplayErrorText( const char *txt )
{
    dtkError() << txt;
}

void vtkLogForwarder::DisplayWarningText( const char *txt )
{
    dtkWarn() << txt;
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
