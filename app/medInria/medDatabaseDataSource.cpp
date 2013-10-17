/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseDataSource.h"

#include <medDataManager.h>

#include <medDatabaseSearchPanel.h>
#include <medDatabaseView.h>
#include <medDatabasePreview.h>

#include <medDatabaseProxyModel.h>
#include <medDatabaseModel.h>
#include <medDatabaseExporter.h>

#include <medActionsToolBox.h>

class medDatabaseDataSourcePrivate
{
public:
    QWidget* mainWidget;
    QWidget* compactWidget;
    
    medDatabasePreview *preview;
    medDatabaseModel *model;
    medDatabaseView *largeView;
    medDatabaseView *compactView;
    
    medDatabaseProxyModel *proxy;
    medDatabaseProxyModel *compactProxy;

    QList<medToolBox*> toolBoxes;
    medDatabaseSearchPanel *searchPanel;
    medActionsToolBox* actionsToolBox;

};

medDatabaseDataSource::medDatabaseDataSource( QWidget* parent ): medAbstractDataSource(parent), d(new medDatabaseDataSourcePrivate)
{
    d->mainWidget = new QWidget(parent);

    d->model = new medDatabaseModel (this);
    d->proxy = new medDatabaseProxyModel(this);
    d->proxy->setSourceModel(d->model);

    d->compactProxy = new medDatabaseProxyModel(this);
    d->compactProxy->setSourceModel(d->model);
    
    d->preview = new medDatabasePreview(d->mainWidget);
    d->largeView = new medDatabaseView(d->mainWidget);
    d->largeView->setModel(d->proxy);
    
    d->compactView = new medDatabaseView(parent);
    d->compactView->setModel(d->compactProxy);
    
    for(int i =1; i<12; ++i)
        d->compactView->hideColumn(i);

    QVBoxLayout *database_layout = new QVBoxLayout(d->mainWidget);
    database_layout->setContentsMargins(0, 0, 0, 0);
    database_layout->setSpacing(0);
    database_layout->addWidget(d->largeView);
    database_layout->addWidget(d->preview);

    d->actionsToolBox = new medActionsToolBox(0, false);
    d->toolBoxes.push_back(d->actionsToolBox);

    d->searchPanel = new medDatabaseSearchPanel();
    d->searchPanel->setColumnNames(d->model->columnNames());
    d->toolBoxes.push_back(d->searchPanel);

    connect(d->largeView, SIGNAL(patientClicked(const medDataIndex&)), d->preview, SLOT(onPatientClicked(const medDataIndex&)));
    connect(d->largeView, SIGNAL(studyClicked(const medDataIndex&)), d->preview, SLOT(onStudyClicked(const medDataIndex&)));
    connect(d->largeView, SIGNAL(seriesClicked(const medDataIndex&)), d->preview, SLOT(onSeriesClicked(const medDataIndex&)));
    connect(d->largeView, SIGNAL(patientClicked(const medDataIndex&)), d->actionsToolBox, SLOT(patientSelected(const medDataIndex&)));
    connect(d->largeView, SIGNAL(seriesClicked(const medDataIndex&)), d->actionsToolBox, SLOT(seriesSelected(const medDataIndex&)));
    connect(d->largeView, SIGNAL(noPatientOrSeriesSelected()), d->actionsToolBox, SLOT(noPatientOrSeriesSelected()));
    connect(d->largeView, SIGNAL(open(const medDataIndex&)), this, SIGNAL(open(const medDataIndex&)));
    connect(d->largeView, SIGNAL(exportData(const medDataIndex&)), this, SIGNAL(exportData(const medDataIndex&)));
    connect(d->largeView, SIGNAL(dataRemoved(const medDataIndex&)), this, SIGNAL(dataRemoved(const medDataIndex&)));

    connect(d->searchPanel, SIGNAL(filter(const QString &, int)),this, SLOT(onFilter(const QString &, int)));

    connect(d->actionsToolBox, SIGNAL(removeClicked()), d->largeView, SLOT(onRemoveSelectedItemRequested()));
    connect(d->actionsToolBox, SIGNAL(exportClicked()), d->largeView, SLOT(onExportSelectedItemRequested()));
    connect(d->actionsToolBox, SIGNAL(viewClicked()), d->largeView, SLOT(onViewSelectedItemRequested()));
    connect(d->actionsToolBox, SIGNAL(saveClicked()), d->largeView, SLOT(onSaveSelectedItemRequested()));
    connect(d->actionsToolBox, SIGNAL(newPatientClicked()), d->largeView, SLOT(onCreatePatientRequested()));
    connect(d->actionsToolBox, SIGNAL(newStudyClicked()), d->largeView, SLOT(onCreateStudyRequested()));
    connect(d->actionsToolBox, SIGNAL(editClicked()), d->largeView, SLOT(onEditRequested()));
    
    connect(d->compactView, SIGNAL(exportData(const medDataIndex&)), this, SIGNAL(exportData(const medDataIndex&)));
    connect(d->compactView, SIGNAL(dataRemoved(const medDataIndex&)), this, SIGNAL(dataRemoved(const medDataIndex&)));
}

medDatabaseDataSource::~medDatabaseDataSource()
{
    delete d;
    d = NULL;
}

QWidget* medDatabaseDataSource::mainViewWidget()
{
    return d->mainWidget;
}

QWidget* medDatabaseDataSource::compactViewWidget()
{
    return d->compactView;
}

QWidget* medDatabaseDataSource::sourceSelectorWidget()
{
    return new QWidget();
}

QString medDatabaseDataSource::tabName()
{
    return tr("Database");
}

QList<medToolBox*> medDatabaseDataSource::getToolBoxes()
{
    return d->toolBoxes;
}

QString medDatabaseDataSource::description(void) const
{
	return tr("Browse the medInria Database");
}

void medDatabaseDataSource::update(const medDataIndex &index)
{
    Q_UNUSED(index);
    d->preview->reset();
    d->preview->init();
    d->preview->update();
}

void medDatabaseDataSource::onFilter( const QString &text, int column )
{
    // adding or overriding filter on column
    d->proxy->setFilterRegExpWithColumn(QRegExp(text, Qt::CaseInsensitive, QRegExp::Wildcard), column);
}

void medDatabaseDataSource::onOpeningFailed(const medDataIndex& index)
{
    d->largeView->onOpeningFailed(index);
}
