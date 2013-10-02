/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medBrowserArea.h"

#include "medBrowserSourceSelectorToolBox.h"

#include <QtGui>

#include <dtkCore/dtkGlobal.h>
#include <dtkGui/dtkFinder.h>

#include <medMessageController.h>
#include <medJobManager.h>
#include <medDataManager.h>

#include <medStorage.h>

#include <medDatabaseController.h>
#include <medDatabaseNonPersistentController.h>
#include <medDatabaseExporter.h>
#include <medDatabaseImporter.h>

#include <medProgressionStack.h>
#include <medToolBox.h>
#include <medToolBoxFactory.h>
#include <medToolBoxContainer.h>
#include <medBrowserJobsToolBox.h>
#include <medPacsMover.h>
#include <medPacsWidget.h>
#include <medCompositeDataSetImporterSelectorToolBox.h>
#include <medAbstractDataSource.h>
#include "medDataSourceManager.h"

class medBrowserAreaPrivate
{
public:
    
    medToolBoxContainer *toolboxContainer;
    medBrowserJobsToolBox *jobsToolBox;
    medBrowserSourceSelectorToolBox *sourceSelectorToolBox;

    QList <medAbstractDataSource *> dataSources;
    
    
    QStackedWidget *stack;
};

// /////////////////////////////////////////////////////////////////
// medBrowserArea
// /////////////////////////////////////////////////////////////////

medBrowserArea::medBrowserArea(QWidget *parent) : QWidget(parent), d(new medBrowserAreaPrivate)
{

    d->stack = new QStackedWidget(this);

    // Source toolbox

    d->sourceSelectorToolBox = new medBrowserSourceSelectorToolBox(this);
    connect(d->sourceSelectorToolBox, SIGNAL(indexChanged(int)), this, SLOT(onSourceIndexChanged(int)));

    // Jobs

    d->jobsToolBox = new medBrowserJobsToolBox(this);
    d->jobsToolBox->setVisible(false);
    // connect the job-manager with the visual representation
    connect(medJobManager::instance(), SIGNAL(jobRegistered(medJobItem*, QString)),
    d->jobsToolBox->stack(),SLOT(addJobItem(medJobItem*, QString)));

    // Toolbox container

    d->toolboxContainer = new medToolBoxContainer(this);
    d->toolboxContainer->setObjectName("browserContainerToolbox");
    d->toolboxContainer->setFixedWidth(340);
    d->toolboxContainer->addToolBox(d->sourceSelectorToolBox);
    
    connect(medDataSourceManager::instance(), SIGNAL(registered(medAbstractDataSource*)),
            this, SLOT(addDataSource(medAbstractDataSource*)));
    
    //TODO: DatabaseController  call ?
    connect(medDatabaseController::instance(), SIGNAL(jobStarted(medJobItem*,QString)),
            this, SLOT(displayJobItem(medJobItem *, QString)));
    
    connect(medDatabaseNonPersistentController::instance(), SIGNAL(jobStarted(medJobItem*,QString)),
            this, SLOT(displayJobItem(medJobItem *, QString)));

    // Jobs should be added as the last item so that they appear at the bottom
    d->toolboxContainer->addToolBox(d->jobsToolBox);

    // Layout /////////////////////////////////////////////
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->toolboxContainer);
    layout->addWidget(d->stack);

    // make toolboxes visible
    onSourceIndexChanged(d->stack->currentIndex());

    medDatabaseControllerImpl* dbcont = medDatabaseController::instance().data();
    connect(dbcont, SIGNAL(partialImportAttempted ( const QString& )), this, SLOT(onPartialImportAttempted ( const QString& )));

    //Check if there are already item in the database, otherwise, switch to File system datasource
    QList<medDataIndex> indexes = medDatabaseNonPersistentController::instance()->availableItems();
    QList<medDataIndex> patients = medDatabaseController::instance()->patients();
    if (indexes.isEmpty() && patients.isEmpty())
    {
        d->sourceSelectorToolBox->setCurrentTab(1);
    }
 }

medBrowserArea::~medBrowserArea(void)
{
    delete d;
    d = NULL;
}

void medBrowserArea::onPartialImportAttempted(const QString& message)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Import warning");
    msgBox.setText(message);
    msgBox.exec();
}

void medBrowserArea::displayJobItem(medJobItem *importer, QString infoBaseName)
{
    d->jobsToolBox->stack()->addJobItem(importer, infoBaseName);
}

void medBrowserArea::onSourceIndexChanged(int index)
{
    setToolBoxesVisible(d->stack->currentIndex(),false);
    setToolBoxesVisible(index, true);
    d->stack->setCurrentIndex(index);
}

void medBrowserArea::setToolBoxesVisible(int index, bool visible )
{
    if(d->dataSources.isEmpty())
        return;

    QList<medToolBox*> toolBoxes = d->dataSources[index]->getToolBoxes();
    foreach(medToolBox* toolBox, toolBoxes)
        toolBox->setVisible(visible);
}

void medBrowserArea::addDataSource( medAbstractDataSource* dataSource )
{
    d->dataSources.push_back(dataSource);
    d->stack->addWidget(dataSource->mainWidget());
    d->sourceSelectorToolBox->addTab(dataSource->tabName(),dataSource->sourceSelectorWidget(),dataSource->description());
    
    QList<medToolBox*> toolBoxes = dataSource->getToolBoxes();
    foreach(medToolBox* toolBox, toolBoxes) 
    {
        toolBox->setVisible(false);
        d->toolboxContainer->addToolBox(toolBox);
    }
}

void medBrowserArea::addToolBox(medToolBox *toolbox)
{
    d->toolboxContainer->addToolBox(toolbox);
}

void medBrowserArea::removeToolBox(medToolBox *toolbox)
{
    d->toolboxContainer->removeToolBox(toolbox);
}

