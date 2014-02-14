/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <%1Workspace.h>

#include <medWorkspaceFactory.h>


static QString s_identifier()
{
   return "%1Workspace";
}

static QString s_description()
{
   return "MyDescription";
}

// /////////////////////////////////////////////////////////////////
// %1WorkspacePrivate
// /////////////////////////////////////////////////////////////////

class %1WorkspacePrivate
{
public:
};

// /////////////////////////////////////////////////////////////////
// %1
// /////////////////////////////////////////////////////////////////

%1Workspace::%1Workspace(QWidget *parent) : medWorkspace(parent), d(new %1WorkspacePrivate)
{
    
}

%1Workspace::~%1Workspace()
{
    
}

QString %1Workspace::identifier() const
{
    return s_identifier();
}

QString %1Workspace::description() const
{
    return s_description();
}

void %1Workspace::setupViewContainerStack()
{
    // TODO: setup your view container here: e.g.

    //if (!this->stackedViewContainers()->count())
    //{
    //    medSingleViewContainer *singleViewContainer = new medSingleViewContainer ();
    //    this->stackedViewContainers()->addContainer (identifier(), singleViewContainer);
    //}
}

bool %1Workspace::isUsable()
{
    // TODO: you can add some conditions here to check if your workspace is ready to be used
    // (successfully initialized, ...)
    return true;
}

bool %1Workspace::registered()
{
    return medWorkspaceFactory::instance()->registerWorkspace <%1Workspace>
            (s_identifier(), "%1Workspace", s_description());
}


