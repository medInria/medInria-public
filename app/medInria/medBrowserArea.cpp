/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medBrowserArea.h>

#include <medBrowserSourceSelectorToolBox.h>

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
#include <medPacsMover.h>
#include <medPacsWidget.h>
#include <medCompositeDataSetImporterSelectorToolBox.h>
#include <medAbstractDataSource.h>
#include <medDataSourceManager.h>
#include <medDatabasePreview.h>

class medBrowserAreaPrivate
{
public:
    medBrowserSourceSelectorToolBox *sourceSelectorToolBox;

    QList <medAbstractDataSource *> dataSources;

    QStackedWidget *mainStack;
    QTabWidget *panelTabWidget;

    QSplitter *splitter;
    QVBoxLayout *leftSideLayout;
};

medBrowserArea::medBrowserArea(QWidget *parent) : QWidget(parent), d(new medBrowserAreaPrivate)
{

    d->splitter = new QSplitter(this);
    d->splitter->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    d->splitter->setHandleWidth(2);

    d->mainStack = new QStackedWidget(this);
    d->panelTabWidget = new QTabWidget(this);

    connect(d->panelTabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeSource(int)));

    // Check if there are already item in the database, otherwise, switch to File system datasource
    // Soooooo ugly :(
    QList<medDataIndex> indexes = medDatabaseNonPersistentController::instance()->availableItems();
    QList<medDataIndex> patients = medDatabaseController::instance()->patients();
    if (indexes.isEmpty() && patients.isEmpty())
    {
        d->sourceSelectorToolBox->setCurrentTab(1);
    }

    //dataSources
    foreach (medAbstractDataSource *dataSource, medDataSourceManager::instance()->dataSources())
        addDataSource(dataSource);

    //Set up main layout
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(d->splitter);
    this->setLayout(layout);
    layout->setContentsMargins(1, 0, 1, 0);

    d->panelTabWidget->setMinimumWidth(340);
    d->panelTabWidget->setMaximumWidth(340);

    d->splitter->addWidget(d->panelTabWidget);
    d->splitter->addWidget(d->mainStack);
 }

medBrowserArea::~medBrowserArea(void)
{
    delete d;
    d = NULL;
}

void medBrowserArea::changeSource(int index)
{
    d->mainStack->setCurrentIndex(index);
}

void medBrowserArea::addDataSource( medAbstractDataSource* dataSource )
{
    d->dataSources.push_back(dataSource);
    d->mainStack->addWidget(dataSource->mainViewWidget());

    medBrowserPanelFrame *panelFrame= new medBrowserPanelFrame;
    QWidget *panelWidget = new QWidget;
    QVBoxLayout *panelLayout = new QVBoxLayout;
    panelLayout->setContentsMargins(0,0,0,0);

    if(dataSource->sourceSelectorWidget())
    {
        panelLayout->addWidget(dataSource->sourceSelectorWidget(), 0);
//        dataSource->sourceSelectorWidget()->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    }

    foreach(QWidget* toolbox, dataSource->getToolBoxes())
    {
        panelLayout->addWidget(toolbox, 0);
//        toolbox->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    }



    panelWidget->setLayout(panelLayout);
    panelFrame->setWidgetAndPreview(panelWidget, dataSource->previewWidget());

    d->panelTabWidget->addTab(panelFrame, dataSource->tabName());
}

class medBrowserPanelFramePrivate
{
public:
    QWidget *preview;
    QWidget *widget;
    QVBoxLayout *layout;
};


medBrowserPanelFrame::medBrowserPanelFrame(QWidget *parent):
    d(new medBrowserPanelFramePrivate)
{
    this->setContentsMargins(0,0,0,0);

    d->layout = new QVBoxLayout(this);
    this->setLayout(d->layout);
    d->layout->setContentsMargins(0,0,0,0);
    d->layout->setSpacing(0);
}


medBrowserPanelFrame::~medBrowserPanelFrame()
{
    delete d;
    d = NULL;
}


void medBrowserPanelFrame::resizeEvent(QResizeEvent *event)
{
    delete d->layout;
    d->layout = new QVBoxLayout(this);
    d->layout->setContentsMargins(0,0,0,0);
    d->layout->setSpacing(0);

    this->setLayout(d->layout);

    d->layout->addWidget(d->widget, 0);
    d->layout->addStretch(1);
    if(medDatabasePreview *p = qobject_cast<medDatabasePreview *>(d->preview))
    {
        d->layout->addWidget(d->preview, 0, Qt::AlignBottom);
        d->layout->addWidget(p->label(), 0);
    }
    if(event)
        QWidget::resizeEvent(event);
}



void
medBrowserPanelFrame::setWidgetAndPreview(QWidget *widget, QWidget *preview)
{
    d->widget = widget;
    d->preview = preview;
    this->resizeEvent(NULL);
}

