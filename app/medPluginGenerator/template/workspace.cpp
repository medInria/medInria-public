/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <%1.h>

#include <medWorkspaceFactory.h>


// /////////////////////////////////////////////////////////////////
// %1WorkspacePrivate
// /////////////////////////////////////////////////////////////////

class %1Private
{
public:
};

// /////////////////////////////////////////////////////////////////
// %1
// /////////////////////////////////////////////////////////////////

%1::%1(QWidget *parent) : medAbstractWorkspace(parent), d(new %1Private)
{
    
}

%1::~%1()
{
    
}


void %1::setupViewContainerStack()
{
    // TODO: setup your view container here: e.g.

    //if (!this->stackedViewContainers()->count())
    //{
    //    medSingleViewContainer *singleViewContainer = new medSingleViewContainer ();
    //    this->stackedViewContainers()->addContainer (identifier(), singleViewContainer);
    //}
}

bool %1::isUsable()
{
    // TODO: you can add some conditions here to check if your workspace is ready to be used
    // (successfully initialized, ...)
    return true;
}

bool %1::registered()
{
    return medWorkspaceFactory::instance()->registerWorkspace<%1>();
}


