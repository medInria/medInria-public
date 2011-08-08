#include "medDatabaseDataSource.h"

#include <medDataManager.h>

#include <medDatabaseSearchPanel.h>
#include <medDatabaseView.h>
#include <medDatabasePreview.h>

#include <medDatabaseProxyModel.h>
#include <medDatabaseModel.h>
#include <medDatabaseExporter.h>

class medDatabaseDataSourcePrivate
{
public:
    QWidget* database_widget;
    medDatabasePreview *preview;
    medDatabaseModel *model;
    medDatabaseView *view;
    medDatabaseProxyModel *proxy;

    QList<medToolBox*> toolboxes;
    medDatabaseSearchPanel *searchPanel;

};

medDatabaseDataSource::medDatabaseDataSource( QWidget* parent /*= 0*/ ): medAbstractDataSource(parent), d(new medDatabaseDataSourcePrivate)
{
    d->preview = new medDatabasePreview(parent);

    d->model = new medDatabaseModel;

    d->proxy = new medDatabaseProxyModel(parent);
    d->proxy->setSourceModel(d->model);

    d->view = new medDatabaseView(parent);
    d->view->setModel(d->proxy);

    d->database_widget = new QWidget(parent);

    QVBoxLayout *database_layout = new QVBoxLayout(d->database_widget);
    database_layout->setContentsMargins(0, 0, 0, 0);
    database_layout->setSpacing(0);
    database_layout->addWidget(d->view);
    database_layout->addWidget(d->preview);

    d->searchPanel = new medDatabaseSearchPanel(parent);
    d->searchPanel->setColumnNames(d->model->columnNames());
    d->toolboxes.push_back(d->searchPanel);

    connect(d->view, SIGNAL(patientClicked(const medDataIndex&)), d->preview, SLOT(onPatientClicked(const medDataIndex&)));
    connect(d->view, SIGNAL(seriesClicked(const medDataIndex&)), d->preview, SLOT(onSeriesClicked(const medDataIndex&)));
    connect(d->view, SIGNAL(open(const medDataIndex&)), this, SIGNAL(open(const medDataIndex&)));
    connect(d->view, SIGNAL(exportData(const medDataIndex&)), this, SIGNAL(exportData(const medDataIndex&)));
    connect(d->view, SIGNAL(dataRemoved(const medDataIndex&)), this, SIGNAL(dataRemoved(const medDataIndex&)));

    connect(d->searchPanel, SIGNAL(filter(const QString &, int)),this, SLOT(onFilter(const QString &, int)));

}

medDatabaseDataSource::~medDatabaseDataSource()
{
    delete d->model;
    d->model = NULL;
    delete d;
    d = NULL;
}

QWidget* medDatabaseDataSource::mainViewWidget()
{
    return d->database_widget;
}

QWidget* medDatabaseDataSource::sourceSelectorWidget()
{
    return new QWidget();
}

QString medDatabaseDataSource::tabName()
{
    return tr("Db");
}

QList<medToolBox*> medDatabaseDataSource::getToolboxes()
{
    return d->toolboxes;
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
