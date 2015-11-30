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
#include <medAbstractOpeningImageProcess.h>
#include <medAbstractOpeningImageProcessPresenter.h>

#include <medToolBox.h>

class medTestWorkspacePrivate
{
public:
  medAbstractOpeningImageProcess *process;
  medAbstractOpeningImageProcessPresenter *presenter;

};

medTestWorkspace::medTestWorkspace(QWidget *parent): medAbstractWorkspaceLegacy (parent), d(new medTestWorkspacePrivate)
{
    d->presenter = NULL;
    d->process = NULL;

    QStringList openingImagePlugins = medCore::morphomathOperation::openingImage::pluginFactory().keys();
    if(!openingImagePlugins.isEmpty())
    {
        QString key = openingImagePlugins.first();
        d->process = medCore::morphomathOperation::openingImage::pluginFactory().create(key);
        d->presenter = medWidgets::morphomathOperation::openingImage::presenterFactory().create(d->process);

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
