/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseDataSource.h"

#include <dtkCore/dtkSmartPointer>

#include <medDataManager.h>
#include <medDatabaseSearchPanel.h>
#include <medDatabaseView.h>
#include <medDatabaseProxyModel.h>
#include <medDatabaseModel.h>
#include <medDatabaseExporter.h>
#include <medDatabasePreview.h>
#include <medDatabaseCompactWidget.h>
#include <medAbstractDataSourceFactory.h>

#include <medActionsToolBox.h>

class medDatabaseDataSourcePrivate
{
public:
    QPointer<QWidget> mainWidget;
    QPointer<medDatabaseCompactWidget> compactWidget;

    medDatabaseModel *model;
    QPointer<medDatabaseView> largeView;
    medDatabaseView *compactView;

    medDatabasePreview *compactPreview;

    medDatabaseProxyModel *proxy;
    medDatabaseProxyModel *compactProxy;

    QList<medToolBox*> toolBoxes;
    medDatabaseSearchPanel *searchPanel;
    medActionsToolBox* actionsToolBox;

};

medDatabaseDataSource::medDatabaseDataSource( QWidget* parent ): medAbstractDataSource(parent), d(new medDatabaseDataSourcePrivate)
{
    d->model = new medDatabaseModel (this);
    d->proxy = new medDatabaseProxyModel(this);
    d->proxy->setSourceModel(d->model);

    d->compactProxy = new medDatabaseProxyModel(this);
    d->compactProxy->setSourceModel(d->model);
}

medDatabaseDataSource::~medDatabaseDataSource()
{
    delete d;
    d = NULL;
}

QWidget* medDatabaseDataSource::mainViewWidget()
{
    if(d->mainWidget.isNull())
    {
        d->mainWidget = new QWidget;
        d->largeView = new medDatabaseView(d->mainWidget);
        d->largeView->setModel(d->proxy);

        QVBoxLayout *database_layout = new QVBoxLayout(d->mainWidget);
        database_layout->setContentsMargins(0, 0, 0, 0);
        database_layout->setSpacing(0);
        database_layout->addWidget(d->largeView);

        connect(d->largeView, SIGNAL(openRequest(const medDataIndex&)), this, SIGNAL(openRequest(const medDataIndex&)));
        connect(d->largeView, SIGNAL(exportDataRequest(const medDataIndex&)), this, SLOT(exportData(const medDataIndex&)));

        if(!d->toolBoxes.isEmpty())
        {
            connect(d->actionsToolBox, SIGNAL(removeClicked()), d->largeView, SLOT(onRemoveSelectedItemRequested()));
            connect(d->actionsToolBox, SIGNAL(exportClicked()), d->largeView, SLOT(onExportSelectedItemRequested()));
            connect(d->actionsToolBox, SIGNAL(viewClicked()), d->largeView, SLOT(onViewSelectedItemRequested()));
            connect(d->actionsToolBox, SIGNAL(saveClicked()), d->largeView, SLOT(onSaveSelectedItemRequested()));
            connect(d->actionsToolBox, SIGNAL(newPatientClicked()), d->largeView, SLOT(onCreatePatientRequested()));
            connect(d->actionsToolBox, SIGNAL(newStudyClicked()), d->largeView, SLOT(onCreateStudyRequested()));
            connect(d->actionsToolBox, SIGNAL(editClicked()), d->largeView, SLOT(onEditRequested()));

            connect(d->largeView, SIGNAL(patientClicked(const medDataIndex&)), d->actionsToolBox, SLOT(patientSelected(const medDataIndex&)));
            connect(d->largeView, SIGNAL(seriesClicked(const medDataIndex&)), d->actionsToolBox, SLOT(seriesSelected(const medDataIndex&)));
            connect(d->largeView, SIGNAL(noPatientOrSeriesSelected()), d->actionsToolBox, SLOT(noPatientOrSeriesSelected()));
        }

    }

    return d->mainWidget;
}

QWidget* medDatabaseDataSource::dialogWidget()
{
    return this->mainViewWidget();
}

QWidget* medDatabaseDataSource::compactViewWidget()
{
    if(d->compactWidget.isNull())
    {
        d->compactWidget = new medDatabaseCompactWidget;
        d->compactView = new medDatabaseView(d->compactWidget);
        d->compactView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        d->compactView->setModel(d->compactProxy);
        d->compactPreview = new medDatabasePreview(d->compactWidget);

        d->compactWidget->setViewAndPreview(d->compactView, d->compactPreview);


        for(int i =1; i<12; ++i)
            d->compactView->hideColumn(i);

        connect(d->compactView, SIGNAL(patientClicked(const medDataIndex&)), d->compactPreview, SLOT(showPatientPreview(const medDataIndex&)));
        connect(d->compactView, SIGNAL(studyClicked(const medDataIndex&)), d->compactPreview, SLOT(showStudyPreview(const medDataIndex&)));
        connect(d->compactView, SIGNAL(seriesClicked(const medDataIndex&)), d->compactPreview, SLOT(showSeriesPreview(const medDataIndex&)));

        connect(d->compactView, SIGNAL(openRequest(medDataIndex)), this , SIGNAL(openRequest(medDataIndex)));
        connect(d->compactPreview, SIGNAL(openRequest(medDataIndex)), this , SIGNAL(openRequest(medDataIndex)));
        connect(d->compactView, SIGNAL(exportDataRequest(const medDataIndex&)), this, SLOT(exportData(const medDataIndex&)));
    }
    return d->compactWidget;
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
    if(d->toolBoxes.isEmpty())
    {
        d->actionsToolBox = new medActionsToolBox(0, false);
        d->toolBoxes.push_back(d->actionsToolBox);

        d->searchPanel = new medDatabaseSearchPanel();
        d->searchPanel->setColumnNames(d->model->columnNames());
        d->toolBoxes.push_back(d->searchPanel);

        connect(d->searchPanel, SIGNAL(filter(const QString &, int)),this, SLOT(onFilter(const QString &, int)));

        if( !d->largeView.isNull())
        {
            connect(d->actionsToolBox, SIGNAL(removeClicked()), d->largeView, SLOT(onRemoveSelectedItemRequested()));
            connect(d->actionsToolBox, SIGNAL(exportClicked()), d->largeView, SLOT(onExportSelectedItemRequested()));
            connect(d->actionsToolBox, SIGNAL(viewClicked()), d->largeView, SLOT(onViewSelectedItemRequested()));
            connect(d->actionsToolBox, SIGNAL(saveClicked()), d->largeView, SLOT(onSaveSelectedItemRequested()));
            connect(d->actionsToolBox, SIGNAL(newPatientClicked()), d->largeView, SLOT(onCreatePatientRequested()));
            connect(d->actionsToolBox, SIGNAL(newStudyClicked()), d->largeView, SLOT(onCreateStudyRequested()));
            connect(d->actionsToolBox, SIGNAL(editClicked()), d->largeView, SLOT(onEditRequested()));

            connect(d->largeView, SIGNAL(patientClicked(const medDataIndex&)), d->actionsToolBox, SLOT(patientSelected(const medDataIndex&)));
            connect(d->largeView, SIGNAL(seriesClicked(const medDataIndex&)), d->actionsToolBox, SLOT(seriesSelected(const medDataIndex&)));
            connect(d->largeView, SIGNAL(noPatientOrSeriesSelected()), d->actionsToolBox, SLOT(noPatientOrSeriesSelected()));
        }
    }
    return d->toolBoxes;
}

QString medDatabaseDataSource::description(void) const
{
    return tr("Browse the medInria Database");
}

void medDatabaseDataSource::onFilter( const QString &text, int column )
{
    // adding or overriding filter on column
    d->proxy->setFilterRegExpWithColumn(QRegExp(text, Qt::CaseInsensitive, QRegExp::Wildcard), column);
}

void medDatabaseDataSource::onOpeningFailed(const medDataIndex& index, QUuid)
{
    d->largeView->onOpeningFailed(index);
}

void medDatabaseDataSource::exportData(const medDataIndex &index)
{
    medAbstractData *data = medDataManager::instance()->retrieveData(index);
    medDataManager::instance()->exportData(data);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

bool medDatabaseDataSource::registered()
{
    return medAbstractDataSourceFactory::instance()->registerDataSource ( "medDatabaseDataSource", createmedDatabaseDataSource);
}


medAbstractDataSource *createmedDatabaseDataSource (QWidget *)
{
    return new medDatabaseDataSource;
}
