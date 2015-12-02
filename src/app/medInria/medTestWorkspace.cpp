/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTestWorkspace.h>

#include <medTabbedViewContainers.h>

#include <medCore.h>
#include <medWidgets.h>
#include <medProcessPresenterFactory.h>
#include <medAbstractTractographyProcess.h>
#include <medAbstractTractographyProcessPresenter.h>

#include <medToolBox.h>

class medTestWorkspacePrivate
{
public:
  medAbstractTractographyProcess *process;
  medAbstractTractographyProcessPresenter *presenter;
};

medTestWorkspace::medTestWorkspace(QWidget *parent): medAbstractWorkspaceLegacy (parent), d(new medTestWorkspacePrivate)
{
    d->presenter = NULL;
    d->process = NULL;

    QStringList plugins = medCore::tractography::pluginFactory().keys();
    if(!plugins.isEmpty())
    {
        QString key = plugins.first();
        d->process = medCore::tractography::pluginFactory().create(key);
        d->presenter = medWidgets::tractography::presenterFactory().create(d->process);

        medToolBox* tb = new medToolBox;
        tb->addWidget(d->presenter->buildToolBoxWidget());
        tb->setTitle(d->process->caption());
        this->addToolBox(tb);
    }
}

medTestWorkspace::~medTestWorkspace()
{
    delete d;
    d = NULL;
}

void medTestWorkspace::setupTabbedViewContainer()
{
    if(d->presenter != NULL)
        this->tabbedViewContainers()->setSplitter(0, d->presenter->buildViewContainerSplitter());
}

bool medTestWorkspace::isUsable()
{
    return true;
}
