/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.
This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include "medWorkspaceArea.h"
#include "medWorkspaceArea_p.h"


#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <dtkCore/dtkGlobal.h>

#include <dtkVr/dtkVrHeadRecognizer.h>
#include <dtkVr/dtkVrGestureRecognizer.h>


#include <medAbstractDbController.h>
#include <medSettingsManager.h>
#include <medDataIndex.h>
#include <medDataManager.h>
#include <medViewManager.h>
#include <medAbstractView.h>
#include <medMetaDataKeys.h>
#include <medViewFactory.h>
#include <medAbstractView.h>
#include <medAbstractImageView.h>

#include <medDatabaseNonPersistentController.h>
#include <medDatabaseController.h>

#include <medToolBox.h>
#include <medToolBoxContainer.h>
#include <medViewContainer.h>
#include <medWorkspaceFactory.h>
#include <medTabbedViewContainers.h>

#include "medDatabaseDataSource.h"
#include "medDataSourceManager.h"

#include <QtGui>



medWorkspaceArea::medWorkspaceArea(QWidget *parent) : QWidget(parent), d(new medWorkspaceAreaPrivate)
{
    d->selectionToolBox = NULL;

    // -- Internal logic
    d->currentWorkspaceName = "";
    d->currentWorkspace = 0;

    d->splitter = new QSplitter(this);
    // needed to restore state.
    d->splitter->setObjectName("medWorkspaceAreaSplitter");
    d->splitter->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    d->splitter->setHandleWidth(2);

    // -- User interface setup
    d->stack = new QStackedWidget(this);
    d->stack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Setting up toolbox container
    d->toolBoxContainer = new medToolBoxContainer(this);
    d->toolBoxContainer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    d->toolBoxContainer->setMinimumWidth(340);

    // Setting up view container
    d->viewContainer = new QWidget(this);
    QVBoxLayout *viewContainerLayout = new QVBoxLayout(d->viewContainer);
    viewContainerLayout->setContentsMargins(0, 0, 0, 0);
    viewContainerLayout->addWidget(d->stack);

    // Setting up navigator
    d->navigatorContainer = new QWidget(this);
    d->navigatorContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    d->navigatorContainer->setMinimumWidth(186);
    d->navigatorContainer->setMaximumWidth(320);
    d->navigatorContainer->setContentsMargins(0,0,0,0);

    //Set up viewer layout
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(d->splitter);
    setLayout(layout);
    layout->setContentsMargins(1, 0, 1, 0);
    d->splitter->addWidget(d->navigatorContainer);
    d->splitter->addWidget(d->viewContainer);
    d->splitter->addWidget(d->toolBoxContainer);

    //set up all possible workspace:
    medWorkspaceFactory *wFactory = medWorkspaceFactory::instance();
    QHash<QString, medWorkspaceDetails *> wDetails = wFactory->workspaceDetails();
    foreach(QString name, wDetails.keys())
        this->setupWorkspace(name);

    this->addDatabaseView(medDataSourceManager::instance()->databaseDataSource());

    if (!d->splitter->restoreState(medSettingsManager::instance()->value("medWorkspaceArea", "splitterState").toByteArray()))
    {
        d->splitter->setOrientation(Qt::Horizontal);
        //viewcontainer size
        int viewContainerSize = QWIDGETSIZE_MAX -
            d->navigatorContainer->minimumWidth()-
            d->toolBoxContainer->minimumWidth();
        QList<int> sizes;
        sizes.append(d->navigatorContainer->minimumWidth());
        sizes.append(viewContainerSize);
        sizes.append(d->toolBoxContainer->minimumWidth());
        d->splitter->setSizes(sizes);
    }
}

medWorkspaceArea::~medWorkspaceArea(void)
{
    medSettingsManager::instance()->setValue("workspaceAreaSplitter", "state", d->splitter->saveState());
    delete d;
    d = NULL;
}

QPixmap medWorkspaceArea::grabScreenshot()
{
    //TODO - return a list of QPixmap for each container slected ?
//    return QPixmap::grabWidget(d->currentWorkspace->currentViewContainer());
    return QPixmap();
}

void medWorkspaceArea::addToolBox(medToolBox *toolbox)
{
    if(!toolbox)
        return;

    d->toolBoxContainer->addToolBox(toolbox);
    toolbox->show();
}

void medWorkspaceArea::insertToolBox(int index, medToolBox *toolbox)
{
    if(!toolbox)
        return;

    d->toolBoxContainer->insertToolBox(index, toolbox);
    toolbox->show();
}

void medWorkspaceArea::removeToolBox(medToolBox *toolbox)
{
    if(!toolbox)
        return;

    toolbox->hide();
    d->toolBoxContainer->removeToolBox(toolbox);
}

void medWorkspaceArea::setCurrentWorkspace(medAbstractWorkspace *workspace)
{
    if (d->currentWorkspace == workspace)
        return;

    this->disconnect(this, SIGNAL(open(medDataIndex)), d->currentWorkspace, 0);

    d->currentWorkspace = workspace;
    connect(this, SIGNAL(open(medDataIndex)), d->currentWorkspace, SLOT(open(medDataIndex)));

    //clean toolboxes
    d->toolBoxContainer->hide();
    d->toolBoxContainer->clear();
    this->switchToStackedViewContainers(workspace->stackedViewContainers());

    //setup database visibility
    d->navigatorContainer->setVisible(workspace->isDatabaseVisible());

    // add toolboxes
    d->toolBoxContainer->addToolBox(workspace->selectionToolBox());
    workspace->selectionToolBox()->show();

    foreach (medToolBox * toolbox, workspace->toolBoxes())
    {
        d->toolBoxContainer->addToolBox(toolbox);
        toolbox->show();
    }
    d->toolBoxContainer->setVisible(workspace->areToolBoxesVisible());

}

void medWorkspaceArea::setCurrentWorkspace(const QString &name)
{
    this->setCurrentWorkspace(d->workspaces.value(name));
}

medAbstractWorkspace* medWorkspaceArea::currentWorkspace()
{
    return d->currentWorkspace;
}

void medWorkspaceArea::setupWorkspace(const QString &name)
{
    if (d->workspaces.contains(name))
        return;
    medAbstractWorkspace *workspace = NULL;

    workspace = medWorkspaceFactory::instance()->createWorkspace(name, this);
    if (workspace)
        d->workspaces.insert(name, workspace);
    else
    {
        qWarning()<< "Workspace " << name << " couldn't be created";
        return;
    }
    workspace->setupViewContainerStack();
}

void medWorkspaceArea::addDatabaseView(medDatabaseDataSource* dataSource)
{
    QVBoxLayout *databaseViewLayout = new QVBoxLayout;
    databaseViewLayout->setSpacing(0);
    databaseViewLayout->setContentsMargins(0,0,0,0);

    databaseViewLayout->addWidget(dataSource->compactViewWidget());
    d->navigatorContainer->setLayout(databaseViewLayout);

    dataSource->compactViewWidget()->resize(dataSource->compactViewWidget()->width(), dataSource->compactViewWidget()->height());
    //little tricks to force to recompute the stylesheet.
    dataSource->compactViewWidget()->setStyleSheet("/* */");

    connect(dataSource->compactViewWidget(), SIGNAL(open(const medDataIndex&)),
            this, SIGNAL(open(const medDataIndex&)),
            Qt::UniqueConnection);
}

void medWorkspaceArea::removeInternSelectionToolBox()
{
    d->selectionToolBox = NULL;
}

void medWorkspaceArea::switchToStackedViewContainers(medTabbedViewContainers* stack)
{
    if(!stack )
    {
        qWarning() << "No stack to switch to";
        return;
    }

    if (-1 == d->stack->indexOf(stack))
        d->stack->addWidget(stack);

    d->stack->setCurrentWidget(stack);
}
