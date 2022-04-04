/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseDataSource.h"

#include <medActionsToolBox.h>
#include <medDatabaseCompactWidget.h>
#include <medDatabaseExporter.h>
#include <medDatabaseModel.h>
#include <medDatabasePreview.h>
#include <medDatabaseProxyModel.h>
#include <medDatabaseSearchPanel.h>
#include <medDatabaseView.h>
#include <medDataManager.h>

#include <medSourcesLoader.h>
#include <medDataModel.h>
#include <medSourceItemModelPresenter.h>
#include <medSourceModelPresenter.h>
#include <medSourcesWidget.h>

class medDatabaseDataSourcePrivate
{
public:
    QPointer<QWidget> mainWidget;
//    QPointer<medDatabaseCompactWidget> compactWidget;

    medDatabaseModel *model;
    QPointer<medDatabaseView> largeView;
    medSourcesWidget *compactView;

    medDatabasePreview *compactPreview;

    medDatabaseProxyModel *proxy;
    medDatabaseProxyModel *compactProxy;

    QList<medToolBox*> toolBoxes;
    medDatabaseSearchPanel *searchPanel;
    medDatabaseSearchPanel *compactSearchPanel;
    medActionsToolBox* actionsToolBox;
    medSourceModelPresenter *multiSources_tree;
    int currentSource;
};

medDatabaseDataSource::medDatabaseDataSource( QWidget* parent ): medAbstractDataSource(parent), d(new medDatabaseDataSourcePrivate)
{
    d->model = new medDatabaseModel (this);
    d->proxy = new medDatabaseProxyModel(this);
    d->proxy->setSourceModel(d->model);

    d->compactProxy = new medDatabaseProxyModel(this);
    d->compactProxy->setSourceModel(d->model);
    d->multiSources_tree = new medSourceModelPresenter(medDataModel::instance());
    d->currentSource = 0;
}

medDatabaseDataSource::~medDatabaseDataSource()
{
    delete d;
    d = nullptr;
}

//QWidget* medDatabaseDataSource::mainViewWidget()
//{
//    if(d->mainWidget.isNull())
//    {
//        d->mainWidget = new QWidget;
//        d->largeView = new medDatabaseView(d->mainWidget);
//        d->largeView->setModel(d->proxy);
//
//        QVBoxLayout *database_layout = new QVBoxLayout(d->mainWidget);
//        database_layout->setContentsMargins(0, 0, 0, 0);
//        database_layout->setSpacing(0);
//        database_layout->addWidget(d->largeView);
//
//        connect(d->largeView, SIGNAL(open(const medDataIndex&)), this, SIGNAL(open(const medDataIndex&)));
//        connect(d->largeView, SIGNAL(exportData(const medDataIndex&)), this, SIGNAL(exportData(const medDataIndex&)));
//        connect(d->largeView, SIGNAL(dataRemoved(const medDataIndex&)), this, SIGNAL(dataRemoved(const medDataIndex&)));
//
//        if(!d->toolBoxes.isEmpty())
//        {
//            connect(d->actionsToolBox, SIGNAL(removeClicked()), d->largeView, SLOT(onRemoveSelectedItemRequested()));
//            connect(d->actionsToolBox, SIGNAL(exportClicked()), d->largeView, SLOT(onExportSelectedItemRequested()));
//            connect(d->actionsToolBox, SIGNAL(viewClicked()), d->largeView, SLOT(onViewSelectedItemRequested()));
//            connect(d->actionsToolBox, SIGNAL(saveClicked()), d->largeView, SLOT(onSaveSelectedItemRequested()));
//            connect(d->actionsToolBox, SIGNAL(newPatientClicked()), d->largeView, SLOT(onCreatePatientRequested()));
//            connect(d->actionsToolBox, SIGNAL(newStudyClicked()), d->largeView, SLOT(onCreateStudyRequested()));
//            connect(d->actionsToolBox, SIGNAL(editClicked()), d->largeView, SLOT(onEditRequested()));
//
//            connect(d->largeView, SIGNAL(patientClicked(const medDataIndex&)), d->actionsToolBox, SLOT(patientSelected(const medDataIndex&)));
//            connect(d->largeView, SIGNAL(studyClicked(const medDataIndex&)), d->actionsToolBox, SLOT(studySelected(const medDataIndex&)));
//            connect(d->largeView, SIGNAL(seriesClicked(const medDataIndex&)), d->actionsToolBox, SLOT(seriesSelected(const medDataIndex&)));
//            connect(d->largeView, SIGNAL(noPatientOrSeriesSelected()), d->actionsToolBox, SLOT(noPatientOrSeriesSelected()));
//        }
//
//    }
//
//    return d->mainWidget;
//}

QWidget* medDatabaseDataSource::mainViewWidget()
{
    auto *widget = new QWidget();

    auto *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QStackedWidget *mainViewWidgetRes = d->multiSources_tree->buildBrowser();
    connect(this, SIGNAL(changeSource(int)), mainViewWidgetRes, SLOT(setCurrentIndex(int)));

    layout->addWidget(mainViewWidgetRes);


    widget->setLayout(layout);

    return widget;
}

QWidget* medDatabaseDataSource::compactViewWidget()
{
    QWidget *compactViewWidgetRes = new QWidget();

    QVBoxLayout *layout = new QVBoxLayout();

    auto filterLabel = new QLabel("Filter ");
    auto filterLineEdit = new QLineEdit();

    medDataManager::instance()->setIndexV2Handler([](medDataIndex const & dataIndex) -> medAbstractData* {return medDataModel::instance()->getData(dataIndex); },
                                                  [](medAbstractData & data, bool originSrc) -> QUuid {return medDataModel::instance()->writeResultsHackV3(data, originSrc);});//TODO Remove ok c'est le truc le moins classe du monde (Part3)
    d->compactView = d->multiSources_tree->buildTree();

    connect(filterLineEdit, SIGNAL(textChanged(const QString &)), d->multiSources_tree, SIGNAL(filterProxy(const QString &)));

    d->compactView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    d->compactPreview = new medDatabasePreview(compactViewWidgetRes);

    QVBoxLayout *filterLayout = new QVBoxLayout;
    filterLayout->addWidget(filterLabel, 0);
    filterLayout->addWidget(filterLineEdit, 1);

    layout->addLayout(filterLayout, 0);
    layout->addWidget(d->compactView, 1);
    layout->addWidget(d->compactPreview);

    compactViewWidgetRes->setLayout(layout);

    return compactViewWidgetRes;
}

QWidget* medDatabaseDataSource::sourceSelectorWidget()
{
    auto *widget = new QWidget();

    auto *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QListWidget *listSources = d->multiSources_tree->buildSourceList();
    connect(listSources, SIGNAL(currentRowChanged(int)), this, SLOT(currentSourceChanged(int)));

    layout->addWidget(listSources);

    widget->setLayout(layout);

    return widget;
}

void medDatabaseDataSource::currentSourceChanged(int current)
{
    emit changeSource(current);
}

QString medDatabaseDataSource::tabName()
{
    return tr("Data Sources");
}

QList<medToolBox*> medDatabaseDataSource::getToolBoxes()
{


    if(d->toolBoxes.isEmpty())
    {
        
        auto * pToolBox = new medToolBox();
        auto * pFiltersStackBySource = d->multiSources_tree->buildFilters();
        connect(this, SIGNAL(changeSource(int)), pFiltersStackBySource, SLOT(setCurrentIndex(int)));
        pToolBox->addWidget(pFiltersStackBySource);
        d->toolBoxes.push_back(pToolBox);

//        d->actionsToolBox = new medActionsToolBox(0, false);
//        d->toolBoxes.push_back(d->actionsToolBox);
//
//        d->searchPanel = new medDatabaseSearchPanel();
//        d->searchPanel->setColumnNames(d->model->columnNames());
//        d->toolBoxes.push_back(d->searchPanel);
//
//        connect(d->searchPanel, SIGNAL(filter(const QString &, int)),this, SLOT(onFilter(const QString &, int)),
//                Qt::UniqueConnection);
//
//        if( !d->largeView.isNull())
//        {
//            connect(d->actionsToolBox, SIGNAL(removeClicked()), d->largeView, SLOT(onRemoveSelectedItemRequested()));
//            connect(d->actionsToolBox, SIGNAL(exportClicked()), d->largeView, SLOT(onExportSelectedItemRequested()));
//            connect(d->actionsToolBox, SIGNAL(viewClicked()), d->largeView, SLOT(onViewSelectedItemRequested()));
//            connect(d->actionsToolBox, SIGNAL(saveClicked()), d->largeView, SLOT(onSaveSelectedItemRequested()));
//            connect(d->actionsToolBox, SIGNAL(newPatientClicked()), d->largeView, SLOT(onCreatePatientRequested()));
//            connect(d->actionsToolBox, SIGNAL(newStudyClicked()), d->largeView, SLOT(onCreateStudyRequested()));
//            connect(d->actionsToolBox, SIGNAL(editClicked()), d->largeView, SLOT(onEditRequested()));
//
//            connect(d->largeView, SIGNAL(patientClicked(const medDataIndex&)), d->actionsToolBox, SLOT(patientSelected(const medDataIndex&)));
//            connect(d->largeView, SIGNAL(studyClicked(const medDataIndex&)), d->actionsToolBox, SLOT(studySelected(const medDataIndex&)));
//            connect(d->largeView, SIGNAL(seriesClicked(const medDataIndex&)), d->actionsToolBox, SLOT(seriesSelected(const medDataIndex&)));
//            connect(d->largeView, SIGNAL(noPatientOrSeriesSelected()), d->actionsToolBox, SLOT(noPatientOrSeriesSelected()));
//        }
    }
    return d->toolBoxes;
}

QString medDatabaseDataSource::description(void) const
{
    return tr("Browse the Data Sources");
}

void medDatabaseDataSource::onFilter( const QString &text, int column )
{
    // adding or overriding filter on column
    d->proxy->setFilterRegExpWithColumn(QRegExp(text, Qt::CaseInsensitive, QRegExp::Wildcard), column);
}

void medDatabaseDataSource::compactFilter( const QString &text, int column )
{
    // adding or overriding filter on column
    d->compactProxy->setFilterRegExpWithColumn(QRegExp(text, Qt::CaseInsensitive, QRegExp::Wildcard), column);
}

void medDatabaseDataSource::onOpeningFailed(const medDataIndex& index, QUuid)
{
    d->largeView->onOpeningFailed(index);
}

