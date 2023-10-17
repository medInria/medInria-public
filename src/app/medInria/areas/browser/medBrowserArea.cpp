/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medBrowserArea.h>

#include <medBrowserSourceSelectorToolBox.h>

#include <QtGui>
#include <QtWidgets>

#include <medMessageController.h>
#include <medJobManagerL.h>
#include <medDataManager.h>

#include <medStorage.h>

#include <medDatabaseNonPersistentController.h>
#include <medDatabaseExporter.h>
#include <medDatabaseImporter.h>

#include <medProgressionStack.h>
#include <medToolBox.h>
#include <medToolBoxFactory.h>
#include <medToolBoxContainer.h>
#include <medPacsMover.h>
#include <medPacsWidget.h>
#include <medCompositeDataSetImporterSelectorToolBox.h>
#include <medAbstractDataSource.h>
#include <medDataSourceManager.h>

class medBrowserAreaPrivate
{
public:

    medToolBoxContainer *toolboxContainer;
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

    // Toolbox container
    d->toolboxContainer = new medToolBoxContainer(this);
    d->toolboxContainer->setObjectName("browserContainerToolbox");
    d->toolboxContainer->setFixedWidth(340);
    d->toolboxContainer->addToolBox(d->sourceSelectorToolBox);

    // Layout /////////////////////////////////////////////
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->toolboxContainer);
    layout->addWidget(d->stack);

    // DataSources
    for(medAbstractDataSource *dataSource : medDataSourceManager::instance().dataSources())
    {
        addDataSource(dataSource);
    }

    // Switch to default "File system" tab
    d->sourceSelectorToolBox->setCurrentTab(1);
}

medBrowserArea::~medBrowserArea()
{
    delete d;
    d = nullptr;
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
    for(medToolBox* toolBox : toolBoxes)
    {
        if(toolBox->parentWidget())
        {
            toolBox->setVisible(visible);
        }
        else
        {
            toolBox->setVisible(false);
        }
    }
}

void medBrowserArea::addDataSource( medAbstractDataSource* dataSource )
{
    d->dataSources.push_back(dataSource);
    d->stack->addWidget(dataSource->mainViewWidget());
    d->sourceSelectorToolBox->addTab(dataSource->tabName(),dataSource->sourceSelectorWidget(),dataSource->description());

    QList<medToolBox*> toolBoxes = dataSource->getToolBoxes();
    for(medToolBox* toolBox : toolBoxes)
    {
        d->toolboxContainer->addToolBox(toolBox);
    }
    setToolBoxesVisible(d->stack->count()-1, false);
}
