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
    QString key = medCore::morphomathOperation::openingImage::pluginFactory().keys().first();
    d->process = medCore::morphomathOperation::openingImage::pluginFactory().create(key);
    d->presenter = new medAbstractOpeningImageProcessPresenter(d->process);

    medToolBox* tb = new medToolBox;
    tb->addWidget(d->presenter->buildToolBoxWidget());
    tb->setTitle(d->process->caption());
    this->addToolBox(tb);

}

medTestWorkspace::~medTestWorkspace()
{
    delete d;
    d = NULL;
}

void medTestWorkspace::setupTabbedViewContainer()
{
    this->tabbedViewContainers()->setSplitter(0, d->presenter->buildViewContainerSplitter());
}

bool medTestWorkspace::isUsable()
{
    return true;
}
