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
    QWidget* databaseWidget;
    medDatabasePreview *preview;
    medDatabaseModel *model;
    medDatabaseView *view;
    medDatabaseProxyModel *proxy;

    QList<medToolBox*> toolBoxes;
    medDatabaseSearchPanel *searchPanel;
    medActionsToolBox* actionsToolBox;

};

medDatabaseDataSource::medDatabaseDataSource( QWidget* parent /*= 0*/ ): medAbstractDataSource(parent), d(new medDatabaseDataSourcePrivate)
{
    d->databaseWidget = new QWidget(parent);

    d->model = new medDatabaseModel (this);
    d->proxy = new medDatabaseProxyModel(this);

    d->proxy->setSourceModel(d->model);

    d->preview = new medDatabasePreview(d->databaseWidget);
    d->view    = new medDatabaseView(d->databaseWidget);
    d->view->setModel(d->proxy);

    QVBoxLayout *database_layout = new QVBoxLayout(d->databaseWidget);
    database_layout->setContentsMargins(0, 0, 0, 0);
    database_layout->setSpacing(0);
    database_layout->addWidget(d->view);
    database_layout->addWidget(d->preview);

    d->actionsToolBox = new medActionsToolBox(parent);
    d->toolBoxes.push_back(d->actionsToolBox);

    d->searchPanel = new medDatabaseSearchPanel(parent);
    d->searchPanel->setColumnNames(d->model->columnNames());
    d->toolBoxes.push_back(d->searchPanel);

    connect(d->view, SIGNAL(patientClicked(const medDataIndex&)), d->preview, SLOT(onPatientClicked(const medDataIndex&)));
    connect(d->view, SIGNAL(seriesClicked(const medDataIndex&)), d->preview, SLOT(onSeriesClicked(const medDataIndex&)));
    connect(d->view, SIGNAL(patientClicked(const medDataIndex&)), d->actionsToolBox, SLOT(patientSelected(const medDataIndex&)));
    connect(d->view, SIGNAL(seriesClicked(const medDataIndex&)), d->actionsToolBox, SLOT(seriesSelected(const medDataIndex&)));
    connect(d->view, SIGNAL(noPatientOrSeriesSelected()), d->actionsToolBox, SLOT(noPatientOrSeriesSelected()));
    connect(d->view, SIGNAL(open(const medDataIndex&)), this, SIGNAL(open(const medDataIndex&)));
    connect(d->view, SIGNAL(exportData(const medDataIndex&)), this, SIGNAL(exportData(const medDataIndex&)));
    connect(d->view, SIGNAL(dataRemoved(const medDataIndex&)), this, SIGNAL(dataRemoved(const medDataIndex&)));

    connect(d->searchPanel, SIGNAL(filter(const QString &, int)),this, SLOT(onFilter(const QString &, int)));

    connect(d->actionsToolBox, SIGNAL(removeClicked()), d->view, SLOT(onRemoveSelectedItemRequested()));
    connect(d->actionsToolBox, SIGNAL(exportClicked()), d->view, SLOT(onExportSelectedItemRequested()));
    connect(d->actionsToolBox, SIGNAL(viewClicked()), d->view, SLOT(onViewSelectedItemRequested()));
    connect(d->actionsToolBox, SIGNAL(saveClicked()), d->view, SLOT(onSaveSelectedItemRequested()));
    connect(d->actionsToolBox, SIGNAL(newPatientClicked()), d->view, SLOT(onCreatePatientRequested()));
    connect(d->actionsToolBox, SIGNAL(newStudyClicked()), d->view, SLOT(onCreateStudyRequested()));
    connect(d->actionsToolBox, SIGNAL(editClicked()), d->view, SLOT(onEditRequested()));
}

medDatabaseDataSource::~medDatabaseDataSource()
{
    delete d;
    d = NULL;
}

QWidget* medDatabaseDataSource::mainViewWidget()
{
    return d->databaseWidget;
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
    d->view->onOpeningFailed(index);
}
