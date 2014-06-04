/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <cliSupportWorkspace.h>

#include <medViewContainer.h>
#include <medTabbedViewContainers.h>

#include <cliSupportToolBox.h>

// /////////////////////////////////////////////////////////////////
// cliSupportWorkspacePrivate
// /////////////////////////////////////////////////////////////////

class cliSupportWorkspacePrivate
{
public:
    cliSupportToolBox  * cliToolBox;
};

// /////////////////////////////////////////////////////////////////
// cliSupportWorkspace
// /////////////////////////////////////////////////////////////////


cliSupportWorkspace::cliSupportWorkspace(QWidget *parent) : medAbstractWorkspace(parent), d(new cliSupportWorkspacePrivate)
{
    d->cliToolBox = new cliSupportToolBox(parent, this);
    this->addToolBox( d->cliToolBox );
}


cliSupportWorkspace::~cliSupportWorkspace(void)
{
    delete d;
    d = NULL;
}


void cliSupportWorkspace::setupViewContainerStack()
{
    if (!stackedViewContainers()->count()) {
        this->stackedViewContainers()->addContainerInTab(this->name());
    }
    this->stackedViewContainers()->unlockTabs();
}
